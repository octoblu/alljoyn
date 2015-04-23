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

#ifndef TIMESERVICECLIENTCLOCK_H_
#define TIMESERVICECLIENTCLOCK_H_

#include <alljoyn/Status.h>
#include <alljoyn/time/TimeServiceClientBase.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceDateTime.h>

namespace ajn {
namespace services {

class TimeServiceClientClock;

/**
 * Inherit from this class to handle timeSync() events.
 * This event is sent only from the Time Authority Clocks
 */
class TimeAuthorityHandler {

  public:

    /**
     * Handle Time Sync event, which was sent from the given TimeServiceClientClock
     * @param clock The TimeServiceClientClock that received the TimeSync event.
     */
    virtual void handleTimeSync(TimeServiceClientClock* clock) = 0;
    virtual ~TimeAuthorityHandler() { }
};

/**
 * Time Service Clock.
 * Use this class to communication with the server side Clock object.
 */
class TimeServiceClientClock : public TimeServiceClientBase {

  public:

    /**
     * Constructor
     * @param tsClient TimeServiceClient managing this TimeServiceClientClock object
     * @param objectPath Object path of the server side Clock object
     */
    TimeServiceClientClock(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientClock();

    /**
     * Release object resources
     */
    void release();

    /**
     * This method to be called by the TimeServiceClient after finding out
     * whether this Clock is a reliable source of time.
     */
    void setAuthority(bool isAuthority);

    /**
     * Returns whether the server side Clock is a reliable source of time.
     *
     * @return TRUE if server side Clock is a reliable source of time.
     */
    bool isAuthority() const;

    /**
     * Retrieve DateTime from the server Clock object.
     *
     * @param dateTime Out variable. Is populated with the retrieved DateTime.
     *
     * @return status ER_OK, or a failure status if failed to retrieve DateTime
     */
    QStatus retrieveDateTime(TimeServiceDateTime* dateTime);

    /**
     * Set DateTime to the server Clock object
     *
     * @param dateTime TimeServiceDateTime to set
     *
     * @return status
     */
    QStatus setDateTime(const TimeServiceDateTime& dateTime);

    /**
     * Retrieve IsSet from the server Clock object.
     *
     * @param isSet Out variable.
     *
     * @return status ER_OK, or a failure status if failed to retrieve IsSet
     */
    QStatus retrieveIsSet(bool* isSet);

    /**
     * Retrieves Authority Type of the server clock.
     * This method is only valid for the Time Authority clock object.
     *
     * @param authType Out variable.
     *
     * @return status ER_OK, or a failure status if failed to retrieve authority type
     */
    QStatus retrieveAuthorityType(tsConsts::ClockAuthorityType* authType);

    /**
     * Register TimeAuthorityHandler to receive TimeSync
     * events. This method is only valid for the Time Authority clocks.
     *
     * @param handler TimeAuthorityHandler
     *
     * @return status
     */
    QStatus registerTimeAuthorityHandler(TimeAuthorityHandler* handler);

    /**
     * Unregister TimeAuthorityHandler to stop receiving TimeSync
     * events. This method is only valid for objects for the Time Authority clocks.
     */
    void unRegisterTimeAuthorityHandler();

    /**
     * Returns TimeAuthorityHandler that has been registered or NULL
     *
     * @return TimeAuthorityHandler that has been registered or NULL
     */
    TimeAuthorityHandler* getTimeAuthorityHandler();

  private:

    /**
     * Whether the server Clock is an Authority Clock
     */
    bool m_IsAuthority;

    /**
     * Interface description of the Clock interface
     */
    InterfaceDescription* m_ClockIface;

    /**
     * Interface description of the TimeAuthoroty interface
     */
    InterfaceDescription* m_TimeAuthIface;

    /**
     * Handler of the TimeSync events
     */
    TimeAuthorityHandler* m_TimeAuthorityHandler;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTCLOCK_H_ */
