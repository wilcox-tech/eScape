/*
 * WTDictionary.h - interface for C++ dictionary
 * General utility
 * Wilcox Technologies
 * 
 * Copyright (C) 2011 Wilcox Technologies.  Some rights reserved.
 * License: BSD
 */

#ifndef __WTDICTIONARY_H_
#define __WTDICTIONARY_H_

#include <libmowgli/mowgli.h> // duh.

#ifndef MOWGLI_PATRICIA_ALLOWS_NULL_CANONIZE
#	error Your libmowgli is too old.  Upgrade to 0.9.50 or higher.
#endif

/*!
	@brief		sized buffer
	@details	a buffer with its current size
 */
typedef struct buf_struct
{
	char *buffer;
	size_t buffer_len;
	const char *fmt;
} WTSizedBuffer;

/*!
	@brief		C++ sorted dictionary data structure.
	@details	This class implements a C++ interface to a dictionary
			library with alphabetical sorting using the standard C
			strcasecmp (or platform equivalent).
 */
class WTDictionary
{
public:
	WTDictionary(bool manage_memory = true);
	~WTDictionary();
	
	const void *get(const char *);
	void set(const char *, const void *);
	
	WTSizedBuffer *all(const char *fmt = NULL);
	
	size_t count;
	
protected:
	bool manager;
	mowgli_patricia_t *dict;
	
	mowgli_mutex_t access_mutex;
};

#endif/*!__WTDICTIONARY_H_*/
