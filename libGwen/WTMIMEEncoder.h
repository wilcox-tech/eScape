/*
 * WTMIMEEncoder.h - interface of C++ MIME encoder
 * libGwen - Social networking functionality
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2012 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBINK_WTMIMEENCODER_H__
#define __LIBINK_WTMIMEENCODER_H__


#include <libGwen/MIME.h>	// Common MIME routines
#include <libAmy/connect.h>	// WTConnection
#include <Utility.h>		// libAPI, nonfatal_error


class WTMIMEEncoder
{
public:
	libAPI static char *encode_single(WTMIMEAttachment *attachment);
	libAPI static char *encode_multiple(vector<WTMIMEAttachment *> attachments);
	
	libAPI static void *encode_single_to_url(WTMIMEAttachment *attachment,
						 WTConnection *connection,
						 uint64_t *length);
	libAPI static void *encode_multiple_to_url(vector<WTMIMEAttachment *>attachments,
						   WTConnection *connection,
						   uint64_t *length);
	
	libAPI static size_t encode_single_to_file(WTMIMEAttachment *attachment,
						   FILE *file);
	libAPI static size_t encode_multiple_to_file(vector<WTMIMEAttachment *> attachments,
						     FILE *file);
private:
	static char *_mimeify_data(const void *data, size_t length);
	static char *_mimeify_file(FILE *file);
	static void _do_iteration(vector<WTMIMEAttachment *> attachments,
				  char *boundary,
				  char **result,
				  uint64_t *result_len);
};


#endif /*!__LIBINK_WTMIMEENCODER_H__*/
