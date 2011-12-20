/*
 * WTFormParser.cpp - implementation of common routines to parse form output
 * libInk, the glue holding together
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2011 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#include "WTFormParser.h"	// Self
#include "WTDictionary.h"	// WTDictionary
#include <Utility.h>		// nonfatal_error
#include <uriparser/Uri.h>	// Used for decoding urlencoded forms
#include <string.h>			// strdup

libAPI WTDictionary * WTFormParser::get_params(const char *urlencoded_form)
{
	if(urlencoded_form == NULL) return NULL;
	
	WTDictionary *dict = new WTDictionary;
	char *form;
	char *next_param = form = strdup(urlencoded_form);
	
	while(next_param != NULL)
	{
		char *name, *value, *next;
		
		name = next_param;
		
		next = strchr(next_param, static_cast<int>('&'));
		if(next != NULL)
		{
			next[0] = '\0';	// Terminate the value (or name)
			next++;
		}
		
		value = strchr(next_param, static_cast<int>('='));
		if(value != NULL)
		{
			value[0] = '\0';	// Replace = with \0 for name
			value++;
			uriUnescapeInPlaceA(value);
		}
		
		dict->set(strdup(name), (value == NULL ? strdup("") : strdup(value)));
		if(value != NULL) value[-1] = '=';
		if(next_param != NULL) next_param[-1] = '&';
		
		next_param = next;
	}
	
	return dict;
}
