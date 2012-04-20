/*
 * connect.h - interface for Internet socket / connection functionality
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBAMY_CONN_H__
#define __LIBAMY_CONN_H__

#ifndef NO_SSL
#	include <openssl/ssl.h>
#	include <openssl/bio.h>
#	include <openssl/err.h>
#endif

#if defined(__MWERKS__) && defined(macintosh)
#	define OSNAME "Mac OS (PPC)"

struct addrinfo
{
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	size_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_canonname;
	struct addrinfo *ai_next;
};
char *gai_strerror(int);
void freeaddrinfo(struct addrinfo *);
int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **); 

#endif

#ifndef EXTRA_UA
#	define EXTRA_UA ""
#endif

#include "WTConnDelegate.h"
#include <libink/WTDictionary.h>
#include <Utility.h>

#ifndef WIN32
#	include <stdint.h>
#endif

#define delegate_status(status) \
	if(this->delegate != NULL)\
	{\
		this->delegate->update_status(this,status);\
	};

/*!
	@class		WTConnection
	@brief		Represents a connection to the network.
	@details	This class abstracts away pretty much all the nasty,
			disgusting intricacies of both BSD sockets and the
			cross-platform hate they bring with them.

			The most important abstraction however is the fact that
			this class can parse URL strings in almost any format
			and, using them, gain information about whether to use
			SSL or not and what port number to use.  You can just
			call connect with any URL string you could type into
			Midori and it will connect to it.

			It comes with HTTP and FTP for "free".  More protocols
			may be added later.
 */
class WTConnection
{
public:
	/*!
	@brief		Initialise the connection object.
	@param		delegate	The delegate object for this connection
					object.  (Optional.)
	@result		The connection object is initialised.
	@note		If "delegate" is NULL, the object will run in the
			delegateless mode.
	 */
	libAPI WTConnection(WTConnDelegate *delegate);

	/*!
	@brief		Connect to a URL.
	@param		url		The URL to connect to.
	@result		true if the connection succeeded; false otherwise.
	 */
	libAPI bool connect(const char *url);
	/*!
	@brief		Disconnect from present server, if connected.
	@note		If this object isn't connected, this is a no-op.
	 */
	libAPI void disconnect(void);

	/*!
	@brief		Download from the URL connected to.
	@param		length	The length of the result. (Out)
	@result		NULL if there is an error, there is no data, or the
			object is not connected.  Otherwise, a pointer to
			the data.
	 */
	libAPI virtual void * download(uint64_t *length);
	/*!
	@brief		Download from the connected URL to a file.
	@param		filename	The name of the file to write. (In)
	@result		The number of bytes written to the file.
	 */
	libAPI virtual size_t download_to(const char *filename);
	/*!
	@brief		Upload data to the URL connected to.
	@param		data	The data to upload.
	@param		length	The length of the result. (In/Out)
	 */
	libAPI virtual void * upload(const void *data, uint64_t *length);
	/*!
	@brief		Store data to the URL connected to.
	@param		data	The data to put. (In)
	@param		lenth	The length of the result. (In/Out)
	@notes		This is the same as upload() for FTP.  For HTTP, this
			uses the PUT verb instead of the POST verb.
	 */
	libAPI virtual void * store(const void *data, uint64_t *length);

	/*!
	@brief		Set a header (HTTP only).
	@param		header	The name of the header to set.
	@param		data	The contents of the header to set.  If data is
				NULL, the header will be cleared / unset.
	 */
	libAPI void http_header(const char *header, char *data);

	libAPI virtual ~WTConnection();
	
	/*!
	@brief		Retrieve the last error.
	@result		The last error that occurred, or NULL if no errors
			have occurred during this object's lifetime.
	 */
	libAPI const char *get_last_error(void);
protected:
	/*! Whether the connection is active */
	bool connected;
	/*! Whether the connection is being established */
	bool connecting;
	/*! The delegate object for this connection */
	WTConnDelegate *delegate;
	/*! The domain name */
	char *domain;
	/*! HTTP headers, if any */
	WTDictionary *headers;
	/*! The port number to connect to */
	uint16_t port;
	/*! Protocol name */
	char *protocol;
	/*! The socket */
	int socket;
#ifndef NO_SSL
	/*! SSL context */
	SSL_CTX *ssl_ctx;
	/*! SSL socket */
	BIO *ssl_socket;
	/*! SSL */
	SSL *ssl;
#endif
	/*! The address information of the domain */
	struct addrinfo *addr_info;
	/*! The URI to act on */
	char *uri;
	/*! The query string (HTTP-like only) */
	char *query_string;
	/*! The internal storage for errors */
	const char *last_error;
private:
	/*!
	@brief		Parse a URL string into its respective bits.
	@param		url		The URL string to parse.
	@result		The class params are set.
	 */
	bool parse_url(const char *url);
	
	bool connect_https(void);
	
	void *upload_http(const void *data, uint64_t *length);
	void *download_http(uint64_t *length);
	void *put_http(const void *data, uint64_t *length);
	void *upload_internal_http(const char *verb, const void *data, uint64_t *length);
};

#endif /*!__LIBAMY_CONNECT_H__*/
