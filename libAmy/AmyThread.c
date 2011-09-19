//
//  AmyThread.c
//  libAmy
//
//  Created by Andrew Wilcox on 7/26/11.
//  Copyright 2011 Wilcox Technologies LLC. All rights reserved.
//

#include "AmyThread.h"

#ifdef __cplusplus
extern "C"
{
#endif

int amy_mutexcreate(mutex_t *mutex)
{
#ifdef _WIN32
	*mutex = CreateMutex(NULL,false,NULL);
	if(*mutex == NULL)
		return -1;
	return 0;
#else
	return pthread_mutex_init(mutex, NULL);
#endif
}


int amy_mutexlock(mutex_t mutex)
{
#ifdef _WIN32
	if(WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0)
		return -1;
	return 0;
#else
	return pthread_mutex_lock(&mutex);
#endif
}

	
int amy_mutextrylock(mutex_t mutex)
{
#ifdef _WIN32
	if(WaitForSingleObject(mutex, 0) != WAIT_OBJECT_0)
		return -1;
	return 0;
#else
	return pthread_mutex_trylock(&mutex);
#endif
}


int amy_mutexunlock(mutex_t mutex)
{
#ifdef _WIN32
	ReleaseMutex(mutex);
	return 0;
#else
	return pthread_mutex_unlock(&mutex);
#endif
}


int amy_mutexdestroy(mutex_t mutex)
{
#ifdef _WIN32
	CloseHandle(mutex);
#else
	pthread_mutex_destroy(&mutex);
#endif	
	return 0;
}

#ifdef __cplusplus
}
#endif
