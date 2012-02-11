/*
 * WTMIMEEncoder.cpp - implementation of C++ MIME encoder
 * libGwen - Social networking functionality
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2012 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#include "WTMIMEEncoder.h"	// Self
#include <b64/encode.h>		// Encode to base64
#include <errno.h>		// errno

#ifndef _WIN32
#	include <unistd.h>	// getpid
#	include <sys/utsname.h>	// uname
#else
#	include <process.h>	// getpid
#	define getpid _getpid
#	warning hostname code will probably not be functional
#endif


// "BUT WAIT, awilcox@," you say.  "THERE'S A STATIC VAR, THIS METHOD IS
// NOT THREAD-SAFE!"  Actually, curr_message is only used to generate a
// unique ID.  If two threads increment it, we'll lose that number and
// potentially roll over sooner, but hell, if someone writes out that
// many MIME messages they can have a roll over.
static uint64_t curr_message = 0;


char *hostname(void)
{
	// XXX - Unfortunately, I don't think this will work on Windows.
	// Actually, it would with MSL, but I don't think we still use CW7 for
	// win32 builds.  So, this will need to be worked on later for Windows.
	struct utsname name_info;
	if(uname(&name_info) == -1)
	{
		nonfatal_error(strerror(errno));
		return strdup("");
	}
	
	// On Macintosh systems (really only pre-X), there can be spaces in the
	// name.  This isn't acceptable for our user (generating message/content
	// IDs), so we replace them with dashes (which is, AFAIK, what MacOS X
	// does for DNS).
	char *space;
	while((space = strchr(name_info.nodename, static_cast<int>(' '))) != NULL)
		space[0] = '-';
	
	return strdup(name_info.nodename);
}


char *WTMIMEEncoder::_mimeify_data(const void *data, size_t len)
{
	size_t written = 0;
	// We want to be absolutely safe with the length.  This is the max.
	char *result = static_cast<char *>(calloc(len*2, sizeof(char)));
	if(result == NULL) alloc_error("MIME attachment encoding buffer", len*2);
	base64::encoder b64encoder;
	
	// Encode len bytes of data to result using base64, and return result.
	written = b64encoder.encode(static_cast<const char *>(data), len, result);
	b64encoder.encode_end(result+written);
	return result;
}

char *WTMIMEEncoder::_mimeify_file(FILE *file)
{
	return NULL;
}


void WTMIMEEncoder::_do_iteration(vector<WTMIMEAttachment *> attachments,
				  char *boundary,
				  char **result,
				  uint64_t *result_len)
{
	size_t bound_len = strlen(boundary);
	
	for(unsigned int next = 0;
	    next < attachments.size();
	    next++)
	{
		// Retrieve current attachments
		WTMIMEAttachment *attach = attachments.at(next);
		
		// Create the header for this attachment
		char *header = NULL;
		asprintf(&header, "Content-type: %s\n"
			 "Content-transfer-encoding: base64\n\n",
			 (attach->type ? attach->type : "application/octet-stream"));
		if(header == NULL) alloc_error("attachment headers", 1);
		
		// Encode this attachment
		char *encoded_attach = _mimeify_data(attach->data.buffer, attach->length);
		
		// Concatenate everything
		size_t next_attach_len = bound_len + 6 + strlen(header) + strlen(encoded_attach);
		char *next_attach = static_cast<char *> (calloc(next_attach_len, sizeof(char)));
		snprintf(next_attach, next_attach_len, "--%s\n%s%s\n\n",
			 boundary, header, encoded_attach);
		
		free(header);
		free(encoded_attach);
		
		// Stick this attachment on the end of the result buffer
		size_t old_len = *result_len;
		if(old_len == 0) old_len = 1;
		size_t attach_len = strlen(next_attach);
		*result_len += attach_len;
		*result = static_cast<char *> (realloc(*result, *result_len));
		
		strncpy(*result+old_len-1, next_attach, attach_len);
		
		free(next_attach);
	}
	
	// At the end of the message, put the ending boundary
	char *result_end = *result + *result_len - 1;
	result_len += bound_len + 5;
	
	snprintf(result_end, bound_len + 6, "--%s--\n", boundary);
}


libAPI char *WTMIMEEncoder::encode_single(WTMIMEAttachment *attachment)
{
	// We're lazy.  Just shove it in a vector and pretend there's multiple.
	
	vector<WTMIMEAttachment *> single;
	single.push_back(attachment);
	
	return encode_multiple(single);
}

libAPI char *WTMIMEEncoder::encode_multiple(vector<WTMIMEAttachment *> attachments)
{
	// We need to ensure that we actually have at least one attachment.
	if(attachments.size() <= 0)
	{
		nonfatal_error("No attachments");
		return NULL;
	}
	
	
	
	// This is where we generate our shiny unique ID.
	char *boundary = NULL;
	
	// Grab the hostname
	char *host = hostname();
	// Set it to something witty and NetBSD-like if we don't have one
	if(host == NULL) host = strdup("amnesiac");
	
	asprintf(&boundary, "%llu,%u,%lu@%s",
		 ++curr_message, getpid(), time(NULL), host);
	
	free(host);
	if(boundary == NULL) alloc_error("Unique message ID", 1);
	
	
	
	// Start off the result with the header.
	char *result = strdup("MIME-Version: 1.0\n");
	if(result == NULL) alloc_error("MIME encoding buffer", 18);
	char *result_moved = result;
	
	
	
	// If we have more than one, we need to make a mixed message.
	
	// XXX - Note that at this time, we don't support multipart/alternative
	// or multipart/parallel.  I don't think anyone uses them anyway, but
	// for completeness, I felt this 'limitation' should be documented.
	if(attachments.size() > 1)
	{
		// boundary+(MIME-Version+Content-type headers = 61)+message+\n\n\0
		size_t bound_len = strlen(boundary);
		uint64_t result_len = bound_len + 64 + 124;
		result = static_cast<char *>(realloc(result, result_len));
		if(result == NULL) alloc_error("MIME encoding buffer", result_len);
		
		// Add multipart header with boundary description
		snprintf(result, result_len,
			 "MIME-Version: 1.0\nContent-type: multipart/mixed; \n"
			 "  boundary=%s\n\nThis message was created by eScape in "
			 "multi-part MIME format.\nUse a MIME 1.0-compliant "
			 "reader, like MailScape, to read it.\n\n", boundary);
		
		// Handle attachments
		_do_iteration(attachments, boundary, &result, &result_len);
	} else {
		char *header = NULL;
		WTMIMEAttachment *attach = attachments.at(0);
		char *encoded_attach;
		size_t result_len;
		
		// Create the MIME 1.0-compliant header for a single attachment
		asprintf(&header, "Content-type: %s\n"
			 "Content-transfer-encoding: base64\n",
			 (attach->type ? attach->type : "application/octet-stream"));
		if(header == NULL) alloc_error("attachment headers", 1);
		
		result_len = 18 + strlen(header);
		result = static_cast<char *> (realloc(result, result_len));
		
		// We're now pointing at the NUL byte of the initial result
		result_moved = result + 18;
		
		// Copy the headers to the end of the result
		strncpy(result_moved, header, strlen(header));
		
		// Encode the attachment
		encoded_attach = _mimeify_data(attach->data.buffer, attach->length);
		
		// Move up the result and resize it
		result_len += strlen(encoded_attach);
		result = static_cast<char *> (realloc(result, result_len));
		
		// Move up the result to the end of the headers, and free the
		// temp var that contained them.
		result_moved = result + 18 + strlen(header);
		free(header);
		
		// Copy the attachment into the result
		strncpy(result_moved, encoded_attach, strlen(encoded_attach));
		
		// Set the NUL terminator, to be sure
		result_moved[result_len - 1] = '\0';
		
		// The attachment buffer is no longer needed
		free(encoded_attach);
	}
	
	
	
	// We're finished, return the result
	free(boundary);
	return result;
}


libAPI void *WTMIMEEncoder::encode_single_to_url(WTMIMEAttachment *attachment,
						 WTConnection *connection,
						 uint64_t *result_len)
{
	return NULL;
}

libAPI void *WTMIMEEncoder::encode_multiple_to_url(vector<WTMIMEAttachment *>attachments,
						   WTConnection *connection,
						   uint64_t *result_len)
{
	void *response = NULL;
	
	// We need to ensure that we actually have at least one attachment.
	if(attachments.size() <= 0)
	{
		nonfatal_error("No attachments");
		return NULL;
	}
	
	
	
	// This is where we generate our shiny unique ID.
	char *boundary = NULL;
	
	// Grab the hostname
	char *host = hostname();
	// Set it to something witty and NetBSD-like if we don't have one
	if(host == NULL) host = strdup("amnesiac");
	
	asprintf(&boundary, "%llu,%u,%lu@%s",
		 ++curr_message, getpid(), time(NULL), host);
	
	free(host);
	if(boundary == NULL) alloc_error("Unique message ID", 1);
	
	
	
	// Since headers are HTTP, we just set the result to NULL
	// realloc can handle that
	char *result = NULL;
	*result_len = 0;
	
	
	
	// If we have more than one, we need to make a mixed message.
	if(attachments.size() > 1)
	{
		char *type = NULL;
		asprintf(&type, "multipart/mixed; boundary=%s", boundary);
		if(type == NULL) alloc_error("MIME Content-type header", 25);
		
		connection->http_header("MIME-Version", strdup("1.0"));
		connection->http_header("Content-type", type);
		
		// Handle attachments
		_do_iteration(attachments, boundary, &result, result_len);
	} else {
		return NULL;
	}
	
	response = connection->upload(reinterpret_cast<const void *>(result), result_len);
	
	free(result);
}


libAPI size_t WTMIMEEncoder::encode_single_to_file(WTMIMEAttachment *attachment,
						   FILE *file)
{
	return 0;
}

libAPI size_t WTMIMEEncoder::encode_multiple_to_file(vector<WTMIMEAttachment *> attachments,
						     FILE *file)
{
	return 0;
}
