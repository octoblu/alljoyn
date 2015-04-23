/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include "Mutex.h"

using namespace ajn;
using namespace services;

#define QCC_MODULE "MUTEX"

#ifdef _WIN32
#include <process.h>
#endif

Mutex::Mutex()
{
#ifdef _WIN32
    InitializeCriticalSection(&mutex);
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&mutex);
#else
    pthread_mutex_destroy(&mutex);
#endif
}

QStatus Mutex::Lock()
{
#ifdef _WIN32
    EnterCriticalSection(&mutex);
    return ER_OK;
#else
    int ret = pthread_mutex_lock(&mutex);

    if (ret == 0) {
        return ER_OK;
    } else if ((ret == EBUSY) || (ret == EAGAIN)) {
        return ER_WOULDBLOCK;
    } else {
        return ER_FAIL;
    }
#endif
}

QStatus Mutex::Unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&mutex);
    return ER_OK;
#else
    int ret = pthread_mutex_unlock(&mutex);

    if (ret == 0) {
        return ER_OK;
    } else {
        return ER_FAIL;
    }
#endif
}