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

#ifndef TIMESERVICESERVERAUTHORITYCLOCK_H_
#define TIMESERVICESERVERAUTHORITYCLOCK_H_

#include <alljoyn/time/TimeServiceServerClock.h>

namespace ajn {
namespace services {

/**
 * Extend this class to receive events related to this TimeAuthority Clock
 * and send TimeAuthorityClock.timeSync() signal.
 */
class TimeServiceServerAuthorityClock : public TimeServiceServerClock {

    /**
     * This AuthorityClock object is a Friend of the TimeServiceServer
     */
    friend class TimeServiceServer;

  public:

    /**
     * Constructor
     */
    TimeServiceServerAuthorityClock();

    /**
     * Destructor
     */
    ~TimeServiceServerAuthorityClock();

    /**
     * Send TimeSync signal to suggest clients of this TimeAuthority Clock to synchronize their time
     *
     * @return Status of sending the signal
     */
    QStatus timeSync();
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVERAUTHORITYCLOCK_H_ */
