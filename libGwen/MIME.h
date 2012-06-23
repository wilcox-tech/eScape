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


#include <stdlib.h>	// free
#include <stdio.h>	// FILE


#define MIME_DATATYPE_BUFFER	0x0
#define MIME_DATATYPE_FILE	0x1
#define	MIME_DATATYPE_SOCKET	0x2


#define MIME_TRANSFER_BASE64	0x0
#define	MIME_TRANSFER_BINARY	0x1
#define MIME_TRANSFER_7BIT	0x2


#define MIME_DISPOSITION_INLINE	0x0
#define MIME_DISPOSITION_ATTACH	0x1
#define MIME_DISPOSITION_FORM	0x2


struct WTMIMEAttachment
{
	/*! The data of the attachment (required) */
	union
	{
		FILE *file;
		const void *buffer;
		int socket;
	} data;
	
	/*! The type of the data used in the union
	    (required; default: buffer) */
	char datatype;
	
	/*! The disposition of the attachment
	    (optional; default: inline) */
	char disposition;
	
	/*! Extra fields for content-disposition header
	    (optional; default: NULL) */
	const char *extra_disposition;
	
	/*! The filename of the attachment
	    (optional; default: "Attachment [n]") */
	char *filename;
	
	/*! The length of the attachment (REQUIRED if using a memory buffer;
	    optional otherwise) */
	size_t length;
	
	/*! The transfer encoding of the attachment
	    (optional; default: base64) */
	char transfer_enc;
	
	/*! The content-type of the attachment
	    (optional; default: "application/octet-stream") */
	const char *type;
	
	WTMIMEAttachment()
	{
		extra_disposition = NULL;
		filename = NULL;
		type = NULL;
		
		datatype = MIME_DATATYPE_BUFFER;
		disposition = MIME_DISPOSITION_INLINE;
		transfer_enc = MIME_TRANSFER_BASE64;
		
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
