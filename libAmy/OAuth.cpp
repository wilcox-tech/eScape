/*
 * OAuth.cpp - implementation of OAuth routines
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#include "OAuth.h"		// self
#include <b64/encode.h>		// base64-encoding
#include <uriparser/Uri.h>	// Used for decoding urlencoded forms
#include <Utility.h>		// alloc_error, url<encode/decode>
#include <string.h>		// strcspn, strlen, strncpy
#include <stdlib.h>		// calloc, realloc, free
#include <stdio.h>		// fprintf, stderr
#include <ctype.h>		// tolower
#include <errno.h>		// errnos
#include <limits.h>		// ULLONG_MAX

const char *sigmeth_enum_to_str(WTOAuthSigMethod method)
{
	switch(method)
	{
		case OAUTH_SIG_PLAINTEXT:
			return "PLAINTEXT";
		case OAUTH_SIG_HMAC_SHA1:
			return "HMAC-SHA1";
		default:
			return "UNKNOWN";
	}
}

bool WTOAuthConnection::gen_sigbase_and_auth(const char *req_type, const void *data)
{
	char *base_uri; size_t uri_length; char *stripped_uri; size_t stripped_len;
	WTDictionary *param_dict; WTSizedBuffer *params; char *next_param; char *enc_params;
	char nonce[8], *timestamp; size_t timestamp_len;
	char *encoded_uri; size_t sig_base_len;
	char *key; unsigned char *signature; size_t key_len; unsigned int signature_len;
	char *b64_sig;  size_t b64_sig_len = 28; char *enc_b64_sig;
	char *auth_header; size_t auth_header_len;
	
	//
	// Ensure we are needed/wanted
	if(consumer_key == NULL && token == NULL) return false;
	
	
	// 
	// Calculate the length of the URI string
	stripped_uri = strchr(uri, static_cast<int>('?'));
	if(stripped_uri != NULL)
	{
		stripped_len = abs(uri - stripped_uri);
		stripped_uri = static_cast<char *>(calloc(stripped_len + 1, sizeof(char)));
		strncpy(stripped_uri, uri, stripped_len);
		stripped_uri[stripped_len] = '\0';
	} else {
		stripped_uri = strdup(uri);
		stripped_len = strlen(uri);
	}
	
	uri_length  = strlen(protocol) + 3 + strlen(domain); // 3 is for "://"
	uri_length += stripped_len;	// add the URI
	uri_length++;			// add NUL byte
	
	
	
	// 
	// Create the Base String URI
	base_uri = static_cast<char *>(calloc(uri_length, sizeof(char)));
	if(base_uri == NULL) alloc_error("OAuth base string URI buffer", uri_length);
	snprintf(base_uri, uri_length, "%s://%s%s", protocol, domain, stripped_uri);
	
	
	
	// 
	// Generate nonce and timestamp
	timestamp_len = snprintf(NULL, 0, "%ld", time(NULL)) + 1;
	timestamp = static_cast<char *>(calloc(timestamp_len, sizeof(char)));
	if(timestamp == NULL) alloc_error("OAuth timestamp buffer", timestamp_len);
	snprintf(timestamp, timestamp_len, "%ld", time(NULL));
	
	srand(time(NULL));
	snprintf(nonce, sizeof(nonce), "%02x%02x%02x%02x",
		 rand() % 128, rand() % 128, rand() % 128, rand() % 128);
	
	
	
	// 
	// Parameters
	param_dict = new WTDictionary;
	
	next_param = (this->query_string == NULL ? NULL : this->query_string+1);
	
	while (next_param != NULL)
	{
		char *name, *value;
		
		value = strchr(next_param, static_cast<int>('='));
		if(value != NULL)
		{
			value[0] = '\0';	// Replace = with \0 for name
			value++;
		}
		
		name = next_param;
		
		next_param = strchr(value, static_cast<int>('&'));
		if(next_param != NULL)
		{
			next_param[0] = '\0';	// Terminate the value (or name)
			next_param++;
		}
		
		param_dict->set(name, (value == NULL ? strdup("") : strdup(value)));
		if(value != NULL) value[-1] = '=';
		if(next_param != NULL) next_param[-1] = '&';
	}
	
	if(consumer_key != NULL)
		param_dict->set("oauth_consumer_key", strdup(consumer_key));
	if(token != NULL)
		param_dict->set("oauth_token", strdup(token));
	
	param_dict->set("oauth_nonce", strdup(nonce));
	param_dict->set("oauth_signature_method", strdup(sigmeth_enum_to_str(sig_method)));
	param_dict->set("oauth_timestamp", strdup(timestamp));
	param_dict->set("oauth_version", strdup("1.0"));
	
	params = param_dict->all("%s=%s&");
	
	
	
	// 
	// Signature Base String generation
	params->buffer[params->buffer_len - 2] = '\0';	// remove trailing &
	encoded_uri = URLEncode(base_uri);
	enc_params = URLEncode(params->buffer);
	free(params->buffer);
	free(params);
	params = NULL;
	sig_base_len = snprintf(NULL, 0, "%s&%s&%s",
				req_type, encoded_uri, enc_params);
	sig_base_len++;
	sig_base = static_cast<char *>(calloc(sig_base_len, sizeof(char)));
	if(sig_base == NULL) alloc_error("OAuth signature base string buffer", sig_base_len);
	snprintf(sig_base, sig_base_len, "%s&%s&%s",
		 req_type, encoded_uri, enc_params);
	//fprintf(stderr, "OAUTH DEBUG: Signature base string is %s\n", sig_base);
	fflush(stderr);
	
	
	// 
	// Generate the real signature now.
	key_len = snprintf(NULL, 0, "%s&%s", (consumer_secret == NULL ? "" : consumer_secret), (token_secret == NULL ? "" : token_secret));
	key_len++;
	key = static_cast<char *>(calloc(key_len, sizeof(char)));
	if(key == NULL) alloc_error("OAuth private key", key_len);
	snprintf(key, key_len, "%s&%s", (consumer_secret == NULL ? "" : consumer_secret), (token_secret == NULL ? "" : token_secret));
	signature = static_cast<unsigned char *>(calloc(EVP_MAX_MD_SIZE, sizeof(char)));
	if(signature == NULL) alloc_error("OAuth HMAC signature buffer", EVP_MAX_MD_SIZE);
	HMAC(EVP_sha1(), static_cast<const void *>(const_cast<const char *>(key)), key_len - 1, reinterpret_cast<const unsigned char *>(const_cast<const char *>(sig_base)), sig_base_len - 1, signature, &signature_len);
	
	
	
	// 
	// base64-encode it!  I love OAuth!
	// You think this is bad?  Wait til you see the auth header code below.
	b64_sig = static_cast<char *>(calloc(b64_sig_len, sizeof(char)));
	if(b64_sig == NULL) alloc_error("base64 buffer", b64_sig_len);
	base64::encoder b64encoder;
	b64_sig_len = b64encoder.encode(reinterpret_cast<const char *>(signature), signature_len, b64_sig);
	b64encoder.encode_end(b64_sig + b64_sig_len);
	
	
	
	// 
	// Authorization[sic.] header
	enc_b64_sig = URLEncode(b64_sig);
	free(b64_sig);
	auth_header_len = snprintf(NULL, 0, "OAuth Realm=\"\",%s%s%s oauth_nonce=\"%s\", oauth_signature_method=\"%s\", oauth_timestamp=\"%s\",%s%s%s oauth_version=\"1.0\", oauth_signature=\"%s\"",
				   (consumer_key == NULL ? "" : " oauth_consumer_key=\""), (consumer_key == NULL ? "" : consumer_key), (consumer_key == NULL ? "" : "\","),
				   nonce, sigmeth_enum_to_str(sig_method), timestamp,
				   (token == NULL ? "" : " oauth_token=\""), (token == NULL ? "" : token), (token == NULL ? "" : "\","),
				   enc_b64_sig);
	auth_header_len++;
	auth_header = static_cast<char *>(calloc(auth_header_len, sizeof(char)));
	if(auth_header == NULL) alloc_error("HTTP Authorization buffer", auth_header_len);
	snprintf(auth_header, auth_header_len, "OAuth Realm=\"\",%s%s%s oauth_nonce=\"%s\", oauth_signature_method=\"%s\", oauth_timestamp=\"%s\",%s%s%s oauth_version=\"1.0\", oauth_signature=\"%s\"",
		 (consumer_key == NULL ? "" : " oauth_consumer_key=\""), (consumer_key == NULL ? "" : consumer_key), (consumer_key == NULL ? "" : "\","),
		 nonce, sigmeth_enum_to_str(sig_method), timestamp,
		 (token == NULL ? "" : " oauth_token=\""), (token == NULL ? "" : token), (token == NULL ? "" : "\","),
		 enc_b64_sig);
	
	http_header("Authorization", strdup(auth_header));
	
	free(auth_header);
	free(signature);
	free(key);
	free(sig_base);
	free(timestamp);
	free(base_uri);
	free(enc_params);
	free(encoded_uri);
	free(enc_b64_sig);
	free(stripped_uri);
	
	delete param_dict;
	
	sig_base = NULL;
	
	return true;
}

void * WTOAuthConnection::download(uint64_t *length)
{
	gen_sigbase_and_auth("GET");
	return WTConnection::download(length);
}

void * WTOAuthConnection::upload(const void *data, uint64_t *length)
{
	gen_sigbase_and_auth("POST", data);
	return WTConnection::upload(data, length);
}

libAPI bool WTOAuthConnection::oauth_set_params(const char *_consumer_key,
						const char *_consumer_secret,
						const char *_token,
						const char *_token_secret,
						WTOAuthSigMethod _sig_method)
{
	free(const_cast<char *>(consumer_key));
	free(const_cast<char *>(consumer_secret));
	free(const_cast<char *>(token));
	free(const_cast<char *>(token_secret));
	this->consumer_key = (_consumer_key ? strdup(_consumer_key) : NULL);
	this->consumer_secret = (_consumer_secret ? strdup(_consumer_secret) : NULL);
	this->token = (_token ? strdup(_token) : NULL);
	this->token_secret = (_token_secret ? strdup(_token_secret) : NULL);
	this->sig_method = _sig_method;
	
	return true;
}

WTOAuthConnection::WTOAuthConnection(WTConnDelegate *_delegate) : WTConnection(_delegate)
{
	consumer_key = consumer_secret = token = token_secret = NULL;
}

WTOAuthConnection::~WTOAuthConnection()
{
	free(const_cast<char *>(this->consumer_key));
	free(const_cast<char *>(this->consumer_secret));
	free(const_cast<char *>(this->token));
	free(const_cast<char *>(this->token_secret));
}
