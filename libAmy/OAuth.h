/*
 * OAuth.h - adds OAuth functionality to WTConnection
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBAMY_OAUTH_H__
#define __LIBAMY_OAUTH_H__

#include "connect.h"

enum WTOAuthSigMethod {
	OAUTH_SIG_PLAINTEXT,
	OAUTH_SIG_HMAC_SHA1
};

/*!
	@class		WTOAuthConnection
	@brief		Represents an OAuth authorised connection to the network.
	@details	This class abstracts away most of the OAuth process,
			leaving you to focus on the actual requests.
			
			It extends WTConnection, adding OAuth-specific methods and
			overriding some existing methods.
			
			Note that only HTTP(S) is supported by this class.  You can't
			authenticate FTP or Gopher with OAuth anyway, but if you try,
			just watch the pretty explosion.
 */
class WTOAuthConnection : public WTConnection
{
public:
	/*!
	@brief		Initialise the OAuth connection object.
	@param		delegate	The optional delegate object for this
					connection.  Receives status updates.
	@result		The connection object is initalised.
	@note		Currently you cannot change the delegate of a connection.
	 */
	libAPI WTOAuthConnection(WTConnDelegate *delegate = NULL);

	/*!
	@brief		Set up the OAuth parameters of the request.
	@param		consumer_key	The "consumer key" of the requestor.
	@param		consumer_secret	Shared-secret used by the consumer
	@param		token		The public key of the requestor, or
					NULL if there is no token.
	@param		token_secret	The private key of the requestor, or
					NULL if there is no token.
	@param		sig_method	The signature method used for this request.
	@result		true if the parameters are set; false for invalid params.
	 */
	libAPI bool oauth_set_params(const char *consumer_key,
				     const char *consumer_secret,
				     const char *token,
				     const char *token_secret,
				     WTOAuthSigMethod sig_method);

	/*!
	@brief		Download from the URL connected to.
	@param		length	The length of the result. (Out)
	@result		NULL if there is an error, there is no data, or the
			object is not connected.  Otherwise, a pointer to
			the data.
	 */
	libAPI void * download(uint64_t *length);
	/*!
	@brief		Upload data to the URL connected to.
	@param		data	The data to upload.
	@param		length	The length of the data and result. (In/Out)
	 */
	libAPI void * upload(const void *data, uint64_t *length);
	libAPI void * store(const void *data, uint64_t *length);

	libAPI virtual ~WTOAuthConnection();
private:
	/*! The client credential ID */
	const char *consumer_key;
	/*! The client credential privkey */
	const char *consumer_secret;
	/*! The requestor public key (if available) */
	const char *token;
	/*! The requestor private key (if available) */
	const char *token_secret;
	/*! The signature method */
	WTOAuthSigMethod sig_method;
	
	/*! The signature base string */
	char *sig_base;
	
	bool gen_sigbase_and_auth(const char *req_type = "GET", const void *data = NULL);
};

#endif /*!__LIBAMY_OAUTH_H__*/
