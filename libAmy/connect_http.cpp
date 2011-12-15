/*
 * connect_http.cpp - implementation of HTTP-specific connection routines
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#include "connect.h"
#include <libink/WTDictionary.h>
#include <Utility.h>
#include <assert.h>

#define HTTP_BLOCK_SIZE 512

#ifndef NO_SSL
#	define SET_THE_ERROR \
if (is_ssl)\
{\
	last_error = ERR_error_string(ERR_get_error(), NULL);\
	fprintf(stderr, "socket_op: %s\n", last_error);\
} else {\
	perror("socket_op");\
	last_error = strerror(errno);\
}
#else
#	define SET_THE_ERROR \
perror("socket_op");\
last_error = strerror(errno);
#endif

extern "C"
{
	bool sendall(int sock, const char *buf, size_t *len);
	bool sendall_ssl(BIO *sock, const char *buf, size_t *len);
};

void parse_http_headers(const char *http_response,
			WTDictionary **header_container,
			uint16_t *http_code,
			int *data)
{
	const char *buffer = http_response;
	
	while(1)
	{
		char *name = NULL;
		char *value = NULL;
		const char *sep = NULL;
		const char *space = NULL;
		size_t name_size, value_size;
		
		int header_length = strcspn(buffer, "\n\0");
		++header_length; // include the \n or \0
		
		// Special-case the HTTP response code
		if(strncmp("HTTP/", buffer, 5) == 0)
		{
			char proto[4];
			char status[4];
			
			strncpy(proto, buffer + 5, 3);
			proto[3] = '\0';
			strncpy(status, buffer + 9, 3);
			status[3] = '\0';
			
			if(http_code != NULL)
				*http_code = strtol(status, NULL, 10);
			
			buffer += header_length;
			continue;
		};
		
		// A blank newline after a header indicates payload follows
		if(*buffer == '\r' || *buffer == '\n')
		{
			buffer += header_length;
			*data = (buffer - http_response);
			break;
		};
		if(*(buffer + header_length) == '\0') break;
		
		sep = strchr(buffer, ':');
		if(sep == NULL)
		{
			buffer += header_length;
			continue;
		};
		space = strchr(sep, ' ');
		// The space after the colon is not *explicitly* required by HTTP/1.1 standards
		if(space == NULL) space = sep + 1;
		
		name_size = (sep - buffer);
		name = static_cast<char *>(calloc(name_size + 1, 1));
		if(name == NULL)
			alloc_error("Header name", (name_size));
		value_size = (header_length - (space + 1 /* no space */ - buffer));
		value = static_cast<char *>(calloc(value_size + 1, 1));
		if(value == NULL)
			alloc_error("Header value", (value_size));
		
		strncpy(name, buffer, name_size);
		name[name_size] = '\0';
		strncpy(value, space + 1 /* no space */, value_size);
		value[value_size] = '\0';
		
		(*(header_container))->set(name, value);
		
		free(name);
		
		buffer += header_length;
	};
	
	return;
}

void *parse_http_response(const char *resp, uint16_t *http_code, uint64_t *length)
{
	WTDictionary *headers;
	int start_of_data = 0;
	char *return_buffer = NULL;
	const char *content_length = NULL;
	
	if(resp == NULL)
	{
		return NULL;
	};
	
	headers = new WTDictionary;
	parse_http_headers(resp, &headers, http_code, &start_of_data);
	content_length = static_cast<const char *>(headers->get("Content-Length"));
	if(content_length == NULL)
	{
		const char *encoding = static_cast<const char *>(headers->get("Transfer-Encoding"));
		if(encoding != NULL && strcmp(encoding, "chunked\r\n") == 0)
		{
			uint64_t total_size = 0;
			
			resp += start_of_data;
			
			while(true)
			{
				uint32_t octet_off = strcspn(resp, "\n\0");
				const_cast<char *>(resp)[octet_off] = '\0';
				uint64_t octets = strtol(resp, NULL, 16);
				if(octets == 0) break; // End of transfer
				
				// XXX Platform Dependent
				// Yes Elizabeth, I really am that anal.
				uint64_t bytes = octets;
				
				total_size += bytes;
				
				return_buffer = static_cast<char *>(realloc(return_buffer, sizeof(char) * total_size));
				if(return_buffer == NULL)
				{
					free(return_buffer);
					alloc_error("HTTP chunked buffer", total_size);
				}
				
				memcpy(return_buffer + (total_size - bytes), resp + octet_off + 1, bytes);
				
				resp += (octet_off /* offset */ + 1 /*padding*/ + bytes/* skip what we copied */ + 2/* \r\n to separate */);
			};
			
			return_buffer = static_cast<char *>(realloc(return_buffer, sizeof(char) * ++total_size));
			if(return_buffer == NULL) alloc_error("HTTP response buffer", total_size);
			return_buffer[total_size - 1] = '\0';
			
			*length = total_size;
		}
		else
		{
			resp += start_of_data;
			*length = strlen(resp) + 1;
			return_buffer = static_cast<char *>(calloc(*length, sizeof(char)));
			if(return_buffer == NULL) alloc_error("HTTP response buffer", *length);
			memcpy(return_buffer, resp, *length);
		};
	}
	else
	{
		*length = strtol(content_length, NULL, 10);
		return_buffer = static_cast<char *>(calloc(*length + 1, sizeof(char)));
		if(return_buffer == NULL) alloc_error("HTTP response buffer", *length);
		memcpy(return_buffer, resp + start_of_data, *length);
		return_buffer[*length] = '\0';
	};
	
	delete headers;
	return static_cast<void *>(return_buffer);
}

void WTConnection::http_header(const char *header, char *data)
{
	if(this->headers == NULL)
	{
		this->headers = new WTDictionary;
	};
	
	this->headers->set(header, data);
	
	return;
}

bool WTConnection::connect_https(void)
{
#ifndef NO_SSL
	this->ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	this->ssl_socket = BIO_new_ssl_connect(this->ssl_ctx);
	BIO_get_ssl(this->ssl_socket, &ssl);
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(this->ssl_socket, this->domain);
	BIO_set_conn_int_port(this->ssl_socket, &(this->port));
	if(this->ssl_socket == NULL)
	{
		if(!this->connecting) return false;
		
		last_error = ERR_error_string(ERR_get_error(), NULL);
		fprintf(stderr, "SSL error: %s\n", last_error);
		delegate_status(WTHTTP_Error);
		return false;
	};
	if(BIO_do_connect(this->ssl_socket) <= 0)
	{
		if(!this->connecting) return false;
		
		last_error = ERR_error_string(ERR_get_error(), NULL);
		fprintf(stderr, "Couldn't connect to %s %d: %s\n",
			this->domain, this->port, last_error);
		delegate_status(WTHTTP_Error);
		return false;
	};
	if(BIO_do_handshake(this->ssl_socket) != 1)
	{
		if(!this->connecting) return false;
		
		last_error = ERR_error_string(ERR_get_error(), NULL);
		fprintf(stderr, "Handshake failed: %s\n", last_error);
		delegate_status(WTHTTP_Error);
		return false;
	};
	
	if(SSL_get_verify_result(ssl) != X509_V_OK)
	{
		//fprintf(stderr, "BUG: Invalid certificate; CONTINUING ANYWAY\n");
		//	BIO_free_all(this->ssl_socket);
		//	SSL_CTX_free(this->ssl_ctx);
		//	delegate_status(WTHTTP_Error);
		//	return false;
	};
	
	this->connecting = false;
	this->connected = true;
	delegate_status(WTHTTP_Connected);
	
	return true;
#else
	fprintf(stderr, "SSL/TLS disabled.  Can't connect to %s.\n", url);
	free(this->uri); this->uri = NULL;
	free(this->domain); this->domain = NULL;
	free(this->protocol); this->protocol = NULL;
	last_error = "SSL/TLS is disabled.  Your version of libamy can't use https.";
	delegate_status(WTHTTP_Error);
	return false;
#endif
}

void *WTConnection::download_http(uint64_t *length)
{
	char *request = NULL, *header_str = NULL, *response = NULL;
	size_t size_of_req = 0, req_sent = 0;
	WTSizedBuffer *header_buff;
	bool is_ssl, did_send;
	
	if(strcmp("https", this->protocol) == 0) is_ssl = true;
	else is_ssl = false;
	
#ifdef NO_SSL
	if(is_ssl)
	{
		fprintf(stderr, "BUG: SSL/TLS disabled (you shouldn't even be connected).\n");
		return NULL;
	}
#endif
	
	if(this->headers == NULL)
	{
		this->headers = new WTDictionary;
	};
		if(this->headers->get("User-Agent") == NULL)
	{
		this->headers->set("User-Agent", strdup("Mozilla/4.0 (compatible; "
							OSNAME
							"; U; en-GB) eScapeCore/0.1.0"
							EXTRA_UA));
	};
	if(this->headers->get("Connection") == NULL)
	{
		this->headers->set("Connection", strdup("Close"));
	};
	this->headers->set("Host", strdup(this->domain));
	// we don't HAVE a content-type for GET requests
	this->headers->set("Content-type", NULL);
	
	if(!this->connected)
	{
		fprintf(stderr, "WTConnection: download before connect!  (order error)\n");
		last_error = "You must be connected to download data.";
		delegate_status(WTHTTP_Error);
		return NULL;
	};
	
	header_buff = this->headers->all();
	header_str = header_buff->buffer;
	
	size_of_req = ( (15 /* GET  HTTP/1.1\r\n */
			 + strlen(this->uri)
			 + (header_buff->buffer_len) /* All headers */
			 + (this->headers->count() * 2) /* \r\n for each header */
			 + 4 /* \r\n for end of headers */) * sizeof(char));
	
	request = static_cast<char *>(malloc(size_of_req));
	if(request == NULL)
		alloc_error("request buffer", size_of_req);
	
	req_sent = snprintf(request, size_of_req, "GET %s HTTP/1.1%s\r\n\r\n", this->uri, header_str);
	free(header_str);
	free(header_buff);
	
	delegate_status(WTHTTP_Transferring);
#ifndef NO_SSL
	if (is_ssl)
	{
		did_send = sendall_ssl(this->ssl_socket, request, &req_sent);
	} else {
#endif
		did_send = sendall(this->socket, request, &req_sent);
#ifndef NO_SSL
	}
#endif
	
	free(request);
	
	if(!did_send)
	{
		SET_THE_ERROR
		
		delegate_status(WTHTTP_Error);
		return NULL;
	};
	
	uint64_t total = 0;
	
	while(1)
	{
		response = static_cast<char *>(realloc(response, total+HTTP_BLOCK_SIZE));
		if(response == NULL) alloc_error("HTTP response buffer", total+HTTP_BLOCK_SIZE);
		int read;
		
#ifndef NO_SSL
		if (is_ssl)
		{
			read = BIO_read(this->ssl_socket, (response+total), HTTP_BLOCK_SIZE);
		} else {
#endif
			read = recv(this->socket, (response+total), HTTP_BLOCK_SIZE, 0);
#ifndef NO_SSL
		}
#endif
		
		if(read < 0)
		{
			SET_THE_ERROR
			
			free(response);
			
			delegate_status(WTHTTP_Error);
			return NULL;
		};
		if(read == 0)
		{
			// closed
			response[total] = '\0';
			break;
		};
		total += read;
	};
	uint16_t http_code = 0;
	void *ret = parse_http_response(response, &http_code, length);
	// TODO: Deal with 3xx codes
	if(http_code >= 400)
	{
		last_error = "Please try again later.";
		delegate_status(WTHTTP_Error);
	}
	else
	{
		delegate_status(WTHTTP_Finished);
	};
	free(response);
	return ret;
}

void *WTConnection::upload_http(const void *data, uint64_t *length)
{
	size_t size_of_initial;
	char str_size_of_data[64];		// XXX magic number
	size_t data_sent, initial_sent;
	char *initial_crap, *response = NULL;
	char *header_str;
	WTSizedBuffer *header_buff;
	bool sent_initial, sent_data;
	bool is_ssl;
	
	if(strcmp("https", this->protocol) == 0) is_ssl = true;
	else is_ssl = false;
	
#ifdef NO_SSL
	if(is_ssl)
	{
		fprintf(stderr, "BUG: SSL/TLS disabled (you shouldn't even be connected).\n");
		return NULL;
	}
#endif
	
	if(this->headers == NULL)
	{
		this->headers = new WTDictionary;
	};
	
	if(this->headers->get("User-Agent") == NULL)
	{
		this->headers->set("User-Agent", strdup("Mozilla/4.0 (compatible; "
							OSNAME
							"; U; en-GB) eScapeCore/0.1.0"
							EXTRA_UA));
	};
	this->headers->set("Host", strdup(this->domain));
	if(this->headers->get("Connection") == NULL)
	{
		this->headers->set("Connection", strdup("Close"));
	};
	if(this->headers->get("Content-type") == NULL)
	{
		this->headers->set("Content-type", strdup("application/x-www-form-urlencoded"));
	}
	
	if(!this->connected)
	{
		fprintf(stderr, "WTConnection: upload before connect!  (order error)\n");
		last_error = "You must be connected to upload data.";
		delegate_status(WTHTTP_Error);
		return NULL;
	};
	
	data_sent = *length;
	
	snprintf(str_size_of_data, sizeof(str_size_of_data) - 1,
		 "%llu", *length);
	
	header_buff = this->headers->all();
	header_str = header_buff->buffer;
	
	size_of_initial = ( (16 /* POST  HTTP/1.1\r\n */
			     + strlen(this->uri)
			     + 16 /*strlen("Content-length: ") */
			     + strlen(str_size_of_data)
			     + header_buff->buffer_len /* All headers */
			     + (this->headers->count() * 2) /* \r\n for each header */
			     + 2 /* \r\n for end of headers */
			     + 2 /* \r\n for beginning of data */) * sizeof(char));
	
	initial_crap = static_cast<char *>(malloc(size_of_initial));
	if(initial_crap == NULL)
		alloc_error("initial headers", size_of_initial);
	
	initial_sent = snprintf(initial_crap, size_of_initial,
		 "POST %s HTTP/1.1%s\r\nContent-Length: %s\r\n\r\n",
		 this->uri, header_str, str_size_of_data);
		
	free(header_str);
	free(header_buff);
	
	delegate_status(WTHTTP_Transferring);
	
#ifndef NO_SSL
	if(is_ssl)
	{
		sent_initial = sendall_ssl(this->ssl_socket, initial_crap, &initial_sent);
		if(sent_initial > 0) sent_data = sendall_ssl(this->ssl_socket, static_cast<const char *>(data), &data_sent);
	} else {
#endif
		sent_initial = sendall(this->socket, initial_crap, &initial_sent);
		if(sent_initial > 0) sent_data = sendall(this->socket, static_cast<const char *>(data), &data_sent);
#ifndef NO_SSL
	}
#endif
	
	free(initial_crap);
	
	if(!sent_initial || !sent_data)
	{
		fprintf(stderr, "Sent %lu of %lu initial bytes, and %lu of %llu payload\n",
			static_cast<long int>(initial_sent), static_cast<long int>(size_of_initial),
			static_cast<long int>(data_sent), *length);
		
		free(response);
		
		SET_THE_ERROR
		
		delegate_status(WTHTTP_Error);
		return NULL;
	};
	
	uint64_t total = 0;
	
	while(1)
	{
		response = static_cast<char *>(realloc(response, total+HTTP_BLOCK_SIZE));
		int read;
		
#ifndef NO_SSL
		if(is_ssl)
		{
			read = BIO_read(this->ssl_socket, (response+total), HTTP_BLOCK_SIZE);
		}
		 else
		{
#endif
			read = recv(this->socket, (response+total), HTTP_BLOCK_SIZE, 0);
#ifndef NO_SSL
		}
#endif
		
		if(read < 0)
		{
			free(response);
			
			if(!this->connected)
			{
				// cancelled, this is normal
				last_error = "User cancelled operation.";
				delegate_status(WTHTTP_Cancelled);
				return NULL;
			};
			
			SET_THE_ERROR
			
			delegate_status(WTHTTP_Error);
			return NULL;
		};
		if(read == 0)
		{
			// closed
			response[total] = '\0';
			break;
		};
		total += read;
	};
	
	uint16_t http_code = 0;
	void *ret = parse_http_response(response, &http_code, length);
	// TODO: Deal with 3xx codes
	if(http_code >= 400)
	{
		last_error = "Please try again later.";
		delegate_status(WTHTTP_Error);
	}
	else
	{
		delegate_status(WTHTTP_Finished);
	};
	free(response);
	return ret;
}
