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

#ifndef TIMESERVICESESSIONLISTENER_H_
#define TIMESERVICESESSIONLISTENER_H_

#include <alljoyn/SessionListener.h>
#include <alljoyn/time/TimeServiceClient.h>

namespace ajn {
namespace services {

/**
 * Inherit from this class to receive AllJoyn session related events
 */
class TimeServiceSessionListener {

  public:

    /**
     * Constructor
     */
    TimeServiceSessionListener();

    /**
     * Destructor
     */
    virtual ~TimeServiceSessionListener();

    /**
     * The method is called when the existing session is lost.
     * Avoid blocking this thread with long running tasks.
     *
     * @param timeServiceClient TimeServiceClient that was used to create the session
     * @param reason Session lost reason
     */
    virtual void sessionLost(TimeServiceClient* timeServiceClient, SessionListener::SessionLostReason reason) = 0;

    /**
     * The method is called when a session was established following the call to
     * TimeServiceClient.joinSessionAsync().
     * Avoid blocking this thread with long running tasks.
     *
     * @param timeServiceClient TimeServiceClient that was used to create the session
     * @param status Check this QStatus to ensure that the session was created successfully
     */
    virtual void sessionJoined(TimeServiceClient* timeServiceClient, QStatus status) = 0;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESESSIONLISTENER_H_ */
