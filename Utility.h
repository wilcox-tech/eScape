/*
 * Utility.h - Useful functions
 * General utility
 * Wilcox Technologies
 *
 * Copyright (c) 2011 Wilcox Technologies. All rights reserved.
 */

#ifndef __WT_GENERAL_UTILITY_H_
#define __WT_GENERAL_UTILITY_H_

#include <stdio.h> // fprintf, stdout
#include <string.h> // strlen
#include <ctype.h> // isalnum, isspace
#include <stdlib.h> // calloc
#include <time.h>

#define alloc_error(name,len) { fprintf(stdout, "PANIC: couldn't allocate %ld bytes for %s\n", static_cast<long int>(len), name); abort(); }
#define fatal_error(errstr) { fprintf(stdout, "PANIC: %s\n", errstr); abort(); }
#define nonfatal_error(errstr) { fprintf(stdout, "ERROR: %s\n", errstr); }
#define warning_error(errstr) { fprintf(stdout, "WARNING: %s\n", errstr); }

#ifdef __GNUC__
#	define UNUSED __attribute__((unused))
#	define UNIMPLEMENTED fprintf(stderr, "WARNING: function %s is unimplemented! (file %s line %d)\n", __func__, __FILE__, __LINE__);
#else
#	define UNUSED
#	define UNIMPLEMENTED fprintf(stderr, "WARNING: function is unimplemented! (file %s line %d)\n", __FILE__, __LINE__);
#endif

#ifdef _WIN32
#	define libAPI __declspec(dllexport)
#	define strcasecmp			_stricmp
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
#else
#	define libAPI
#endif

#if defined(__MWERKS__)
#include <stdarg.h>
static int asprintf(char **strp, const char *fmt, ...)
{
	va_list ap;
	
	/* Initialise, like BSD */
	*strp = NULL;
	
	va_start(ap, fmt);
	int len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	
	if(len > -1)
	{
		*strp = static_cast<char *>(calloc(len + 1, sizeof(char)));
		if(*strp == NULL)
			alloc_error("asprintf buffer", len + 1);
		
		va_start(ap, fmt);
		len = vsnprintf(*strp, len + 1, fmt, ap);
		va_end(ap);
		
		if(len < 0)
		{
			free(*strp);
			*strp = NULL;
			return -1;
		}
	}
	
	return len;
}
#endif

#if defined(_WIN32) || defined(__sun) || defined(__MWERKS__)
inline time_t timegm(struct tm *stupid_time)
{
	time_t secs, gmt_secs;
	
	time(&secs);
	
	//local_secs = mktime(stupid_time);
	stupid_time = gmtime(&secs);
	gmt_secs = mktime(stupid_time);
	
	return gmt_secs;
}
#endif

// originally from: http://www.codeproject.com/KB/string/urlencode.aspx
// Written horribly MFC-specific by rkrakowiak in 2001
// made portable to excess by awilcox in 2011
inline char toHex(const char &x)
{
	return x > 9 ? x + 55: x + 48;
}
inline char *URLEncode(const char *sIn, const char *charsToIgnore = "\x5f\x2e\x2d")
{
	char *sOut;
	
	if(sIn == NULL) return NULL;

	int k;
	const int nLen = strlen(sIn) + 1;
	
	register char *pOutTmp = NULL;
	register const char *pInTmp = NULL;
	
	//count not alphanumeric characters
	
	k = 0;
	
	pInTmp = sIn;
	while(*pInTmp)
	{
		if (!isalnum(*pInTmp) && (!charsToIgnore || strchr(charsToIgnore, *pInTmp) == NULL))
			k++;
		pInTmp++;
	}
	
	//alloc out buffer
	
	sOut = static_cast<char *> (calloc(nLen + (2 * k), sizeof(char)));
	
	
	if(sOut)
	{
		pInTmp	= sIn;
		pOutTmp = sOut;
		
		// do encoding
		
		while (*pInTmp)
		{
			if(isalnum(*pInTmp) || (charsToIgnore && strchr(charsToIgnore, *pInTmp) != NULL))
				*pOutTmp++ = *pInTmp;
			else
			{
				*pOutTmp++ = '%';
				*pOutTmp++ = toHex(*pInTmp>>4);
				*pOutTmp++ = toHex(*pInTmp%16);
			}
			pInTmp++;
		}
		
		*pOutTmp = '\0';
	}
	return sOut;
}

#endif/*!__WT_GENERAL_UTILITY_H_*/
