/*
 * WTFormParser.cpp - inteface for common routines to parse form output
 * libInk, the glue holding together
 * eScape
 * Wilcox Technologies, LLC
 * 
 * Copyright (C) 2011 Wilcox Technologies, LLC.  Some rights reserved.
 * License: NCSA-WT
 */

#ifndef __WTFORMPARSER_H__
#define __WTFORMPARSER_H__

#include "WTDictionary.h"

class WTFormParser
{
public:
	libAPI static WTDictionary *get_params(const char *urlencoded_form);
	
};

#endif /*!__WTFORMPARSER_H__*/
