/*
 * MIME.h - common interface of the C++ MIME module
 * libInk, the glue holding together
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2012 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBINK_MIME_H__
#define __LIBINK_MIME_H__


#include <cstdlib>	// free
#include <cstdio>	// FILE


#define MIME_DATATYPE_BUFFER	0x0
#define MIME_DATATYPE_FILE	0x1
#define	MIME_DATATYPE_SOCKET	0x2


struct WTMIMEAttachment
{
	/*! The data of the attachment (required) */
	union
	{
		FILE *file;
		char *buffer;
		int socket;
	} data;
	
	/*! The type of the data used in the union
	    (required; default: buffer) */
	char datatype;
	
	/*! The filename of the attachment
	    (optional; default: "Attachment [n]") */
	char *filename;
	
	/*! The length of the attachment (REQUIRED if using a memory buffer;
	    optional otherwise) */
	size_t length;
	
	/*! The content-type of the attachment
	    (optional; default: "application/octet-stream") */
	char *type;
	
	WTMIMEAttachment()
	{
		filename = type = NULL;
		datatype = MIME_DATATYPE_BUFFER;
		
		data.file = NULL;
		data.buffer = NULL;
		data.socket = 0;
		
		length = 0;
	}
	
	~WTMIMEAttachment()
	{
		free(filename);
	}
};


#endif /*!__LIBINK_MIME_H__ */
