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

#ifndef TIMESERVICECLIENTALARM_H_
#define TIMESERVICECLIENTALARM_H_

#include <alljoyn/Status.h>
#include <alljoyn/time/TimeServiceObjectIntrospector.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceSchedule.h>

namespace ajn {
namespace services {

class TimeServiceClientAlarm;

/**
 * Inherit from this class to handle alarmReached() events.
 */
class AlarmHandler {

  public:

    /**
     * Handle Alarm reached event, which was sent from the given TimeServiceClientAlarm
     *
     * @param alarm The TimeServiceClientAlarm that received the AlarmReached event.
     */
    virtual void handleAlarmReached(TimeServiceClientAlarm* alarm) = 0;
    virtual ~AlarmHandler() { }
};

/**
 * Time Service Alarm.
 * Use this class to communication with the server side Alarm object.
 */
class TimeServiceClientAlarm : public TimeServiceObjectIntrospector {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this TimeServiceClientAlarm object
     * @param objectPath Object path of the server side Alarm object
     */
    TimeServiceClientAlarm(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientAlarm();

    /**
     * Release object resources
     */
    void release();

    /**
     * Retrieve Schedule from the server Alarm object.
     *
     * @param schedule Out variable. Is populated with the retrieved Schedule.
     *
     * @return status ER_OK, or a failure status if failed to retrieve Schedule
     */
    QStatus retrieveSchedule(TimeServiceSchedule* schedule);

    /**
     * Set Schedule to the server Alarm object.
     *
     * @param schedule Schedule to be set
     *
     * @return status ER_OK, or a failure status if failed to set the Schedule
     */
    QStatus setSchedule(const TimeServiceSchedule& schedule);

    /**
     * Retrieve Title from the server Alarm object.
     *
     * @param title Out variable. Is populated with the retrieved Alarm title.
     *
     * @return status ER_OK, or a failure status if failed to retrieve the Alarm title
     */
    QStatus retrieveTitle(qcc::String* title);

    /**
     * Set Title to the server Alarm object.
     *
     * @param title Title to be set
     *
     * @return status ER_OK, or a failure status if failed to set the Alarm title
     */
    QStatus setTitle(const qcc::String& title);

    /**
     * Retrieve IsEnabled from the server Alarm object.
     *
     * @param isEnabled Out variable.
     *
     * @return status ER_OK, or a failure status if failed to retrieve IsEnabled property
     */
    QStatus retrieveIsEnabled(bool* isEnabled);

    /**
     * Set IsEnabled status to the server Alarm object.
     *
     * @param isEnabled IsEnabled status to be set
     *
     * @return status ER_OK, or a failure status if failed to set IsEnabled property
     */
    QStatus setEnabled(bool isEnabled);

    /**
     * Register AlarmHandler to receive AlarmReached events.
     *
     * @param handler AlarmHandler
     *
     * @return status
     */
    QStatus registerAlarmHandler(AlarmHandler* handler);

    /**
     * Unregister AlarmHandler to stop receiving AlarmReached
     * events.
     */
    void unRegisterAlarmHandler();

    /**
     * Returns AlarmHandler that has been registered or NULL
     *
     * @return AlarmHandler that has been registered or NULL
     */
    AlarmHandler* getAlarmHandler();

  private:

    /**
     * Interface description of the Alarm interface
     */
    InterfaceDescription* m_AlarmIface;

    /**
     * Handler of the AlarmReached events
     */
    AlarmHandler* m_AlarmHandler;

};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTALARM_H_ */
