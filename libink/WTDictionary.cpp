/*
 * WTDictionary.cpp - implementation of C++ mowgli_dictionary wrapper
 * libInk, the glue holding together
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2011 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#include "WTDictionary.h"
#include <Utility.h>
#include <stdlib.h> // NULL, calloc, realloc, free
#include <string.h> // strlen
#include <assert.h> // assert

void tear_down(const char *, void *, void *);
int add_to_buffer(const char *, void *, void *);

#define mtex_do_or_die(x) if(x != 0) fatal_error("mutex operation error")

libAPI WTDictionary::WTDictionary(bool manage_memory)
{
	mowgli_mutex_create(&(this->access_mutex));
	mtex_do_or_die(mowgli_mutex_lock(&(this->access_mutex)));
	this->dict = mowgli_patricia_create(NULL);
	mtex_do_or_die(mowgli_mutex_unlock(&(this->access_mutex)));
	this->_count = 0;
	this->manager = manage_memory;
}

void tear_down(const char *key UNUSED, void *data, void *privdata UNUSED)
{
	free(data);
}

libAPI WTDictionary::~WTDictionary()
{
	mowgli_patricia_destroy(this->dict, (manager ? tear_down : NULL), NULL);
	mowgli_mutex_destroy(&(this->access_mutex));
}

libAPI void WTDictionary::set(const char *key, const void *value)
{
	if(key == NULL)
		return;
	
	if(get(key) == NULL)
	{
		// we can't delete something that doesn't exist
		if(value == NULL) return;

		/* do not allow reads while adding to the dict */
		mtex_do_or_die(mowgli_mutex_lock(&(this->access_mutex)));
		mowgli_patricia_add(this->dict, key, const_cast<void *>(value));
		++(this->_count);
		mtex_do_or_die(mowgli_mutex_unlock(&(this->access_mutex)));
		return;
	}
	 else
	{
		/* XXX This makes corgi sad.  We're in a critical section for
		   a LONG ARSE TIME with this.  We have to retrieve the old,
		   free it, delete it, decrement, and then add and increment
		   (if applicable).  But it MUST be locked, because otherwise
		   Very Bad Things happen. */
		mtex_do_or_die(mowgli_mutex_lock(&(this->access_mutex)));
		if(manager)
		{
			free(mowgli_patricia_retrieve(this->dict, key));
		};
		mowgli_patricia_delete(this->dict, key);
		--(this->_count);
		if(value != NULL)
		{
			mowgli_patricia_add(this->dict, key, const_cast<void *>(value));
			++(this->_count);
		};
		mtex_do_or_die(mowgli_mutex_unlock(&(this->access_mutex)));
		
		return;
	};
}

libAPI const void *WTDictionary::get(const char *key)
{
	void *value;
	
	if(key == NULL)
		return NULL;
	
	/* Only one thread shall read at a time */
	mtex_do_or_die(mowgli_mutex_lock(&(this->access_mutex)));
	value = mowgli_patricia_retrieve(this->dict, key);
	mtex_do_or_die(mowgli_mutex_unlock(&(this->access_mutex)));
	
	return value;
}

libAPI const size_t WTDictionary::count(void)
{
	return this->_count;
}

int add_to_buffer(const char *key, void *data, void *privdata)
{
	assert(key != NULL);
	assert(data != NULL);
	
	if(key == NULL)
	{
		nonfatal_error("NULL key in add_to_buffer");
		return -1;
	};
	
	if(data == NULL)
	{
		nonfatal_error("NULL data in add_to_buffer");
		return -1;
	};
	
	WTSizedBuffer *dict = static_cast<WTSizedBuffer *>(privdata);
	const char *next = static_cast<char *>(data);
	if(next == NULL) next = "";
	char *temp_buffer = NULL;
	const char *fmt = dict->fmt;
	size_t next_len = strlen(key) + strlen(next) + strlen(fmt) - 4; /*%s%s*/
	size_t old_len = dict->buffer_len;

	dict->buffer_len += next_len;
	
	if(old_len == 0)
	{
		++(dict->buffer_len);
		dict->buffer = static_cast<char *>(calloc(dict->buffer_len, sizeof(char)));
		snprintf(dict->buffer, dict->buffer_len, fmt, key, next);
	}
	 else
	{
		dict->buffer = static_cast<char *>(realloc(dict->buffer, dict->buffer_len * sizeof(char)));
		++next_len; // for the nul byte
		temp_buffer = static_cast<char *>(calloc(next_len, sizeof(char)));
		if(temp_buffer == NULL) alloc_error("Temporary dictionary buffer", next_len);
		snprintf(temp_buffer, next_len, fmt, key, next);
		strncat(dict->buffer, const_cast<const char *>(temp_buffer), dict->buffer_len);
		free(temp_buffer);
	};
		
	return 0;
}

libAPI WTSizedBuffer *WTDictionary::all(const char *fmt)
{
	WTSizedBuffer *all_buffer;
	bool fre = false;
	all_buffer = static_cast<WTSizedBuffer *>(calloc(1, sizeof(WTSizedBuffer)));
	if(all_buffer == NULL) alloc_error("WTSizedBuffer", sizeof(WTSizedBuffer));
	if(fmt == NULL)
	{
		fre = true;
		all_buffer->fmt = strdup("\r\n%s: %s"); 
	}
	else
		all_buffer->fmt = fmt;
	
	mtex_do_or_die(mowgli_mutex_lock(&(this->access_mutex)));
	mowgli_patricia_foreach(this->dict, &add_to_buffer, all_buffer);
	mtex_do_or_die(mowgli_mutex_unlock(&(this->access_mutex)));
	
	if(fre) { free(const_cast<char *>(all_buffer->fmt)); all_buffer->fmt = NULL; };
	
	return all_buffer;
}

