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
#else
#	define libAPI
#endif

#if defined(_WIN32) || defined(__sun)
inline time_t timegm(struct tm *stupid_time)
{
	time_t secs, local_secs, gmt_secs;
	
	time(&secs);
	
	local_secs = mktime(stupid_time);
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
inline char *URLEncode(const char *sIn)
{
	char *sOut;
	
	int k;
	const int nLen = strlen(sIn) + 1;
	
	register char *pOutTmp = NULL;
	register const char *pInTmp = NULL;
	
	//count not alphanumeric characters
	
	k = 0;
	
	pInTmp = sIn;
	while(*pInTmp)
	{
		if (!isalnum(*pInTmp) && *pInTmp != 0x5f && *pInTmp != 0x2e && *pInTmp != 0x2d)
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
			if(isalnum(*pInTmp) || *pInTmp == 0x5f || *pInTmp == 0x2e || *pInTmp == 0x2d)
				*pOutTmp++ = *pInTmp;
			else
				if(isspace(*pInTmp))
					*pOutTmp++ = '+';
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
