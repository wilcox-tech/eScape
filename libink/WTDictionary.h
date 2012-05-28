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

enum WTDictionaryMemoryPolicy
{
	/*! Manage this key's memory. */
	WTDICT_KEY_MANAGED,
	/*! Do not manage this key's memory. */
	WTDICT_KEY_UNMANAGED,
	/*! Honour the dictionary's policy. */
	WTDICT_KEY_DEFAULT
};

/*!
	@brief		sized buffer
	@details	a buffer with its current size
 */
typedef struct buf_struct
{
	/*! The string buffer */
	char *buffer;
	/*! The length of the string buffer, not including the \0 terminator */
	size_t buffer_len;
	/*! The format string used to create the string buffer. */
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
	/*!
	@brief		Construct a new, empty dictionary.
	@param		manage_memory	If this is set, all values will be
					free()d upon deletion of the dictionary.
	@result		A new, empty WTDictionary object.
	 */
	libAPI WTDictionary(bool manage_memory = true);
	/*!
	@brief		Construct a new dictionary, copying the contents of an
			old dictionary.
	@param		old		The old dictionary to copy keys and
					values from.
	@result		A new dictionary with the same keys/values as the old
			dictionary.  Note that this dictionary will
			automatically inherit the old dictionary's memory
			management flag.
	 */
	libAPI WTDictionary(const WTDictionary &old);
	libAPI ~WTDictionary();
	
	/*!
	@brief		Retrieve a value for a specified key.
	@param		key		The key to retrieve the value for.
	@result		The value associated with key, or NULL if the key does
			not exist.
	 */
	libAPI const void *get(const char *key);
	/*!
	@brief		Set the value of a specified key.
	@param		key		The key to set the value for.
	@param		data		The new value of the key.
	@param		managed		The management policy of this key, if
					different from the dictionary's policy.
	@details	This method sets the value of the key specified to the
			value specified in data.  If the key already existed,
			the old value is discarded (and free()d if the memory
			management flag is set).
	 */
	libAPI void set(const char *key, const void *data,
			WTDictionaryMemoryPolicy managed = WTDICT_KEY_DEFAULT);
	
	/*!
	@brief		Remove all keys from the dictionary.
	@result		All keys are removed from the dictionary.  If this
			dictionary is memory-managed, all values are freed.
	 */
	libAPI void clear(void);
	
	/*!
	@brief		Retrieve all keys in the dictionary.
	@result		An array of the keys from this dictionary.  Use the
			::count method to determine the number of keys in the
			dictionary for iteration.
	 @note		The allKeys and allValues methods will cache their
			results for future calls to avoid overhead.  Subsequent
			calls will return immediately as long as the dictionary
			is not modified.
	 */
	libAPI const char **allKeys(void);
	/*!
	@brief		Retrieve all values in the dictionary.
	@result		An array of the values from this dictionary.  As long as
			no other thread modifies the dictionary between calls,
			these values will match the keys found in ::allKeys.
	@note		The allKeys and allValues methods will cache their
			results for future calls to avoid overhead.  Subsequent
			calls will return immediately as long as the dictionary
			is not modified.
	 */
	libAPI const void **allValues(void);
	
	/*!
	@brief		Retrieve a string concatenation of all keys and values
			in the dictionary.
	@param		fmt		An optional format string to use for
					concatenation.  If none is specified,
					the default ("\r\n%s: %s") is used.
	@result		A sized buffer structure containing the string
			concatenation of the keys and values in this dictionary.
			Note that the buffer returned must be free()d to avoid
			a memory leak.
	 */
	libAPI WTSizedBuffer *all(const char *fmt = NULL);
	
	/*!
	@brief		Retrieve the number of key/value pairs in the dictionary.
	@result		The number of key/value pairs in the dictionary.
	 */
	libAPI const size_t count();
	
protected:
	size_t _count;
	bool manager;
	bool vectors_valid;
	mowgli_patricia_t *dict;
	vector<const char *> keys;
	vector<const char *> special_keys;
	vector<const void *> values;
	const char **key_array;
	const void **value_array;
	
	mowgli_mutex_t access_mutex;
	friend int fill_my_vector(const char *,void*,void*);
	friend void tear_down(const char *, void *, void *);
	
	void reloadVectors();
};

#endif/*!__LIBINK_WTDICTIONARY_H__*/
