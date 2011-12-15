/*
 * WTDictionary.h - interface for C++ dictionary
 * libInk, the glue holding together
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2011 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBINK_WTDICTIONARY_H__
#define __LIBINK_WTDICTIONARY_H__

#include <libmowgli/mowgli.h> // the mowgli_patricia backend.
#include <Utility.h> //libAPI
#include <vector>

#ifndef MOWGLI_PATRICIA_ALLOWS_NULL_CANONIZE
#	error Your libmowgli is too old.  Please upgrade to 0.9.50 or higher.
#endif

using std::vector;

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
	libAPI WTDictionary(bool manage_memory = true);
	libAPI ~WTDictionary();
	
	libAPI const void *get(const char *);
	libAPI void set(const char *, const void *);
	
	libAPI const char **allKeys(void);
	libAPI const void **allValues(void);
	
	libAPI WTSizedBuffer *all(const char *fmt = NULL);
	
	libAPI const size_t count();
	
protected:
	size_t _count;
	bool manager;
	bool vectors_valid;
	mowgli_patricia_t *dict;
	vector<const char *> keys;
	vector<const void *> values;
	
	mowgli_mutex_t access_mutex;
	friend int fill_my_vector(const char *,void*,void*);
	
	void reloadVectors();
};

#endif/*!__LIBINK_WTDICTIONARY_H__*/
