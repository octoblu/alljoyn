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

#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32
#include <Windows.h>
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_cond_t CONDITION_VARIABLE
#else
#include <pthread.h>
#endif
#include <errno.h>
#include <alljoyn/Status.h>

namespace ajn {
namespace services {

/**
 * a wrapper class to mutex
 */
class Mutex {
  public:

    /**
     * Mutex constructor
     */
    Mutex();

    /**
     * Mutex destructor
     */
    ~Mutex();

    /**
     * Lock mutex
     */
    QStatus Lock();

    /**
     * Unlock mutex
     */
    QStatus Unlock();

    /**
     * Get mutex object
     */
    pthread_mutex_t* GetMutex() { return &mutex; }

  private:

    pthread_mutex_t mutex;

};

} /* services */
} /* ajn */

#endif /* _MUTEX_H_ */
