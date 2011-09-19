//
//  AmyThread.h
//  libAmy
//
//  Created by Andrew Wilcox on 7/26/11.
//  Copyright 2011 Wilcox Technologies LLC. All rights reserved.
//

#ifndef __AMY_THREAD_H_
#define __AMY_THREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef _WIN32


typedef HANDLE mutex_t;


#else /*!_WIN32*/


#include <pthread.h>
typedef pthread_mutex_t mutex_t;


#endif /*_WIN32*/


int amy_mutexcreate(mutex_t *mutex);
int amy_mutexlock(mutex_t mutex);
int amy_mutextrylock(mutex_t mutex);
int amy_mutexunlock(mutex_t mutex);
int amy_mutexdestroy(mutex_t mutex);


#ifdef __cplusplus
}
#endif


#endif /*!__AMY_THREAD_H_ */
