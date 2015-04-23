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

#ifndef TIMESERVICEGLOBALSTRINGSEQUENCER_H_
#define TIMESERVICEGLOBALSTRINGSEQUENCER_H_

#include <alljoyn/Status.h>
#include <qcc/String.h>
#include "../common/Mutex.h"

namespace ajn {
namespace services {

/**
 * This is utility singleton class that is used to append a unique number to received string.
 */
class TimeServiceGlobalStringSequencer {

  public:

    /**
     * Get instance of the TimeServiceGlobalStringSequencer
     */
    static TimeServiceGlobalStringSequencer* getInstance();

    /**
     * Initialize the sequencer
     */
    QStatus init();

    /**
     * Appends to the given prefix the current number.
     * @param objPath contains the prefix as input, receives the object path in return
     */
    QStatus append(qcc::String* objPath);

    /**
     * Destructor
     */
    ~TimeServiceGlobalStringSequencer();

  private:

    /**
     * GlobalStringSequencer is a singleton
     */
    static TimeServiceGlobalStringSequencer* s_Instance;

    /**
     * Current mutex
     */
    Mutex m_Mutex;

    /**
     * The current incremented value
     */
    uint32_t m_Current;

    /**
     * Private Constructor - the class is singleton
     */
    TimeServiceGlobalStringSequencer();

    /**
     * Converts to string the received number
     */
    void numToString(uint32_t num, qcc::String* convString);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICEGLOBALSTRINGSEQUENCER_H_ */
