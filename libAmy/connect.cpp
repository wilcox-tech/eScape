/*
 * connect.cpp - implementation of Internet socket / connection functionality
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#ifdef _WIN32
// Required to be included BEFORE mowgli due to a bug in VC10 and/or Win SDK 7
#	include <winsock2.h>	// not required on win7, but is on xp
#	include <ws2tcpip.h>	// getaddrinfo, freeaddri, gai_strerror
#endif

#include "connect.h"

#include <Utility.h> // alloc_error
#include <string.h> // strcspn, strlen, strncpy
#include <stdlib.h> // calloc, realloc, free
#include <stdio.h>  // fprintf, stderr
#include <ctype.h>  // tolower
#include <errno.h>
#include <limits.h> // ULLONG_MAX

// socket crap!
#ifndef _WIN32
#	include <sys/types.h>	// XXX the docs made me do it
#	include <sys/socket.h>	// getaddrinfo, freeaddri, gai_strerror
#	include <netdb.h>	// struct sockaddr and friends
#	include <arpa/inet.h>	// inet_ntop
#	include <unistd.h>	// close
#	define	close_portable	close
#else
#	define	snprintf sprintf_s
#	define	close_portable	closesocket
#endif

extern "C"
{
	bool sendall(int sock, const char *buf, size_t *len)
	{
		size_t sent = 0;
		size_t left = *len;
		int n = 0;
		
		while(sent < *len)
		{
			n = send(sock, buf + sent, left, 0);
			if(n == -1) break;
			sent += n;
			left -= n;
		};
		
		*len = sent;
		
		return n == -1 ? false : true;
	}
	
#ifndef NO_SSL
	bool sendall_ssl(BIO *sock, const char *buf, size_t *len)
	{
		size_t sent = 0;
		size_t left = *len;
		int n = 0;
		
		while(sent < *len)
		{
			n = BIO_write(sock, buf + sent, left);
			if(n == -1)
			{
				if(!BIO_should_retry(sock))
					break;
				continue;
			};
			sent += n;
			left -= n;
		};
		
		*len = sent;
		
		return n == -1 ? false : true;
	}
#endif
};

bool WTConnection::parse_url(const char *url)
{
	unsigned int proto_length;
	char *port_str;
	int domain_length;

	// Get the protocol
	protocol = static_cast<char *>(calloc(5, sizeof(char)));
	if(protocol == NULL)
		alloc_error("protocol", 5);
	proto_length = strcspn(url, "://");
	if(proto_length == strlen(url))
	{
		// No protocol, assume HTTP
		strncpy(this->protocol, "http\0", 5);
	}
	 else
	{
		this->protocol = static_cast<char *>(
				 realloc(this->protocol,
					 sizeof(char) * (proto_length + 1)));
		strncpy(this->protocol, url, proto_length);
		this->protocol[proto_length] = '\0';
		url += (proto_length + 3);  // skip proto and ://
	};

	for(unsigned int nextchar = 0; nextchar < proto_length; nextchar++)
		this->protocol[nextchar] = (char)tolower(this->protocol[nextchar]);

	if(strcmp("https", this->protocol) == 0) {
		port = 443;
	} else if(strcmp("http", this->protocol) == 0) {
		port = 80;
	} else if(strcmp("ftp", this->protocol) == 0) {
		port = 21;
	} else if(strcmp("gopher", this->protocol) == 0) {
		port = 70;
	} else {
		fprintf(stderr, "unrecognised protocol %s\n", protocol);
		last_error = "unrecognised protocol";
		free(this->protocol);
		return false;
	};

	domain_length = strcspn(url, "/");
	this->domain = static_cast<char *>(calloc(domain_length + 1, sizeof(char)));
	if(this->domain == NULL)
		alloc_error("domain", domain_length);
	strncpy(domain, url, domain_length);
	this->domain[domain_length] = '\0';
	if(domain[0] == '[')
	{
		//IPv6 address.
		port_str = strrchr(this->domain, (int)':');
		//ensure this isn't the last oct of a v6 address
		if(strchr(port_str, (int)']') == NULL)
		{
			int temp_port;
			temp_port = static_cast<int>(strtol(static_cast<char *>(port_str + 1), NULL, 10));
			if(temp_port > 65535)
			{
				fprintf(stderr, "warning: port %d > 65535; "
					"defaulting to %d\n", temp_port, port);
			}
			 else
			{
				port = temp_port;
			};
			port_str[0] = '\0';
		};
	}
	 else
	{
		port_str = strchr(this->domain, static_cast<int>(':'));
		if(port_str != NULL)
		{
			int temp_port;
			temp_port = atoi(static_cast<char *>((port_str + 1)));
			if(temp_port > 65535)
			{
				fprintf(stderr, "warning: port %d > 65535; "
					"defaulting to %d\n", temp_port, port);
			}
			 else
			{
				port = temp_port;
			};
			port_str[0] = '\0';
		};
	};

	if(static_cast<unsigned int>(domain_length) == strlen(url))
	{
		this->uri = static_cast<char *>(calloc(2, sizeof(char)));
		if(this->uri == NULL)
			alloc_error("uri", 2);
		this->uri[0] = '/'; this->uri[1] = '\0';
	}
	 else
	{
		size_t length = strlen(url) - domain_length;
		this->uri = static_cast<char *>(calloc(length + 1, sizeof(char)));
		if(this->uri == NULL)
			alloc_error("uri", length);
		strncpy(uri, (url + domain_length), length);
		this->uri[length] = '\0';
		this->query_string = strchr(uri, static_cast<int>('?'));
	};
	
	return true;
}

bool WTConnection::connect(const char *url)
{
	int addr_result;
	struct addrinfo hint;
	struct timeval tv;
	char ports[5];
	
	if(this->connecting)
	{
		return false;
	};

	if(this->connected)
	{
		this->disconnect();
	};
	
	this->connecting = true;

	if(!this->parse_url(url))
	{
		fprintf(stderr, "can't parse URL %s\n", url);
		last_error = "unparsable URL";
		delegate_status(WTHTTP_Error);
		return false;
	};

	if(strcmp("https", this->protocol) == 0)
	{
		return connect_https();
	};

	/* Ensure cleanliness */
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	delegate_status(WTHTTP_Resolving);
	snprintf(ports, 5, "%d", this->port);
	if( (addr_result = getaddrinfo(this->domain,
					ports,
					&hint,
					&(this->addr_info))) != 0)
	{
		last_error = gai_strerror(addr_result);
		fprintf(stderr, "can't resolve %s: %s\n",
			this->domain,
			last_error);
		delegate_status(WTHTTP_Error);
		free(this->uri);
		free(this->domain);
		free(this->protocol);
		return false;
	};

	delegate_status(WTHTTP_Connecting);
	this->socket = ::socket(this->addr_info->ai_family,
				this->addr_info->ai_socktype,
				this->addr_info->ai_protocol);
	
	tv.tv_sec = 30;
	tv.tv_usec = 0;
	
	if(::setsockopt(this->socket, SOL_SOCKET, SO_RCVTIMEO,
			reinterpret_cast<char *>(&tv), sizeof(tv)) == -1)
	{
		warning_error("couldn't set recv timeout -- expect delays");
	}
	
	if(::connect(this->socket,
			this->addr_info->ai_addr,
			this->addr_info->ai_addrlen) == -1)
	{
		last_error = strerror(errno);
		fprintf(stderr, "can't connect to %s: %s\n",
			this->domain,
			last_error);
		delegate_status(WTHTTP_Error);
		close_portable(this->socket);
		freeaddrinfo(this->addr_info);
		free(this->uri);
		free(this->domain);
		free(this->protocol);
		return false;
	};

	this->connected = true;
	this->connecting = false;
	delegate_status(WTHTTP_Connected);

	return true;
}

void * WTConnection::download(uint64_t *length)
{
	if(strcmp("http", this->protocol) == 0 || strcmp("https", this->protocol) == 0)
	{
		return download_http(length);
	} else {
		last_error = "Unimplemented upload for selected protocol";
		delegate_status(WTHTTP_Error);
		return NULL;
	};
}

size_t WTConnection::download_to(const char *filename)
{
	return 0;
}

void *WTConnection::upload(const void *data, uint64_t *length)
{
	if(strcmp("http", this->protocol) == 0 || strcmp("https", this->protocol) == 0)
	{
		return upload_http(data, length);
	} else {
		last_error = "Unimplemented upload for selected protocol";
		delegate_status(WTHTTP_Error);
		return NULL;
	};
}

void WTConnection::disconnect(void)
{
	if(!this->connecting && !this->connected)
	{
		return;
	};
	
	this->connecting = false;
	this->connected = false;

	if(this->socket != 0)
		close_portable(this->socket);
#ifndef NO_SSL
	if(this->ssl_socket != NULL)
	{
		if(BIO_reset(this->ssl_socket) != 0)
			warning_error("failed to reset socket");
		BIO_free_all(this->ssl_socket);
		this->ssl_socket = NULL;
		this->ssl = NULL;
	};
	if(this->ssl_ctx != NULL)
	{
		SSL_CTX_free(this->ssl_ctx);
		this->ssl_ctx = NULL;
	};
#endif
	delegate_status(WTHTTP_Closed);

	if(this->addr_info != NULL)
	{
		freeaddrinfo(this->addr_info);
		this->addr_info = NULL;
	};

	if(this->uri != NULL)
	{
		free(this->uri);
		this->uri = NULL;
	};

	if(this->domain != NULL)
	{
		free(this->domain);
		this->domain = NULL;
	};

	if(this->protocol != NULL)
	{
		free(this->protocol);
		this->protocol = NULL;
	};
}

const char *WTConnection::get_last_error(void)
{
	return this->last_error;
}

WTConnection::WTConnection(WTConnDelegate *_delegate)
{
	this->connected = this->connecting = false;
	this->delegate = _delegate;

	domain	= NULL;
	headers	= NULL;
	port	= 0;
	protocol = NULL;
	socket	= 0;
	addr_info = NULL;
	uri	= NULL;
	last_error = NULL;
	query_string = NULL;

#ifndef NO_SSL
	ssl_ctx = NULL;
	ssl_socket = NULL;
#endif
}

WTConnection::~WTConnection()
{
	if(this->connecting || this->connected)
	{
		this->disconnect();
	};

	if(this->headers != NULL)
	{
		delete this->headers;
	};

#ifndef NO_SSL
	ERR_remove_state(0);
#endif

	// This solves a corner case where object deletion leaks -- could happen
	// if a connection is deleted in the middle of a connect operation and
	// disconnect code isn't run

	if(this->addr_info != NULL)
	{
		freeaddrinfo(this->addr_info);
		this->addr_info = NULL;
	};

	if(this->uri != NULL)
	{
		free(this->uri);
		this->uri = NULL;
	};

	if(this->domain != NULL)
	{
		free(this->domain);
		this->domain = NULL;
	};

	if(this->protocol != NULL)
	{
		free(this->protocol);
		this->protocol = NULL;
	};
}
