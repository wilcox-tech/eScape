/*
 * amy_init.c - Initialisation routines
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#ifdef WIN32
#	include <winsock2.h>
#endif /*WIN32*/

#ifndef NO_SSL
#	include <openssl/ssl.h>
#	include <openssl/err.h>
#	include <libmowgli/mowgli.h>
#endif /*!NO_SSL*/

#ifndef NO_THREADSAFE
	static mowgli_mutex_t *ssl_lock_group;
#endif /*!NO_THREADSAFE*/

#include <Utility.h>

void amy_ssl_lockback(int mode, int n, const char *file, int line)
{
#ifndef NO_SSL
	if(mode & CRYPTO_LOCK)
		mowgli_mutex_lock(&(ssl_lock_group[n]));
	else
		mowgli_mutex_unlock(&(ssl_lock_group[n]));
#endif
}
	
libAPI void amy_init()
{
#ifndef NO_SSL
	int i;
	
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
#endif /*!NO_SSL*/
#ifdef WIN32
	WSADATA wsadata;
	
	if(WSAStartup(MAKEWORD(2,0), &wsadata) != 0) abort(); // XXX
#endif /*WIN32*/
	
	mowgli_init();

#if !defined(NO_THREADSAFE) && !defined(NO_SSL)
	
	ssl_lock_group = (mowgli_mutex_t *)(calloc(CRYPTO_num_locks(), sizeof(mowgli_mutex_t)));
	if(ssl_lock_group == NULL) alloc_error("OpenSSL lock group", CRYPTO_num_locks() * sizeof(mowgli_mutex_t));
	for(i = 0; i < CRYPTO_num_locks(); i++)
		mowgli_mutex_create(&(ssl_lock_group[i]));

	CRYPTO_set_locking_callback(amy_ssl_lockback);

#endif
}

libAPI void amy_clean()
{
#ifndef NO_SSL
	int i;
	
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
#endif
#ifdef WIN32
	WSACleanup();
#endif /*WIN32*/

#if !defined(NO_THREADSAFE) && !defined(NO_SSL)

	for(i = 0; i < CRYPTO_num_locks(); i++)
		mowgli_mutex_destroy(&(ssl_lock_group[i]));

	free(ssl_lock_group);
#endif
}
