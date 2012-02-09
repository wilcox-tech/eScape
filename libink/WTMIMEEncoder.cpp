/*
 * WTMIMEEncoder.cpp - implementation of C++ MIME encoder
 * libInk, the glue holding together
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


const char *hostname(void)
{
	// XXX - Unfortunately, I don't think this will work on Windows.
	// Actually, it would with MSL, but I don't think we still use CW7 for
	// win32 builds.  So, this will need to be worked on later for Windows.
	struct utsname name_info;
	if(uname(&name_info) == -1)
	{
		nonfatal_error(strerror(errno));
		return "";
	}
	
	// On Macintosh systems (really only pre-X), there can be spaces in the
	// name.  This isn't acceptable for our user (generating message/content
	// IDs), so we replace them with dashes (which is, AFAIK, what MacOS X
	// does for DNS).
	char *space;
	while((space = strchr(name_info.nodename, static_cast<int>(' '))) != NULL)
		space[0] = '-';
	
	return name_info.nodename;
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
	// "BUT WAIT, awilcox@," you say.  "THERE'S A STATIC VAR, THIS METHOD IS
	// NOT THREAD-SAFE!"  Actually, curr_message is only used to generate a
	// unique ID.  If two threads increment it, we'll lose that number and
	// potentially roll over sooner, but hell, if someone writes out that
	// many MIME messages they can have a roll over.
	static uint64_t curr_message = 0;
	
	// We need to ensure that we actually have at least one attachment.
	if(attachments.size() <= 0)
	{
		nonfatal_error("No attachments");
		return NULL;
	}
	
	// This is where we generate our shiny unique ID.
	char *boundary = NULL;
	asprintf(&boundary, "%d.%d.%ld@%s",
		 ++curr_message, getpid(), time(NULL), hostname());
	if(boundary == NULL) alloc_error("Unique message ID", 1);
	
	// Start off the result with the header.
	char *result = strdup("MIME-Version: 1.0\n");
	if(result == NULL) alloc_error("MIME encoding buffer", 19);
	
	// If we have more than one, we need to make a mixed message.
	// XXX - Note that at this time, we don't support multipart/alternative
	// or multipart/parallel.  I don't think anyone uses them anyway, but
	// for completeness, I felt this 'limitation' should be documented.
	if(attachments.size() > 1)
	{
		// boundary + (MIME-Version + Content-type headers = 62) + \0
		result = static_cast<char *>(realloc(result, strlen(boundary) + 63));
		if(result == NULL) alloc_error("MIME encoding buffer",
					       strlen(boundary) + 63);
		
		snprintf(result, strlen(boundary) + 63,
			 "MIME-Version: 1.0\nContent-type: multipart/mixed; \n"
			 "  boundary=%s\n", boundary);
	}
	
	free(boundary);
	return result;
}


libAPI void *WTMIMEEncoder::encode_single_to_url(WTMIMEAttachment *attachment,
						 WTConnection *connection)
{
	return NULL;
}

libAPI void *WTMIMEEncoder::encode_multiple_to_url(vector<WTMIMEAttachment *>attachments,
						   WTConnection *connection)
{
	return NULL;
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
