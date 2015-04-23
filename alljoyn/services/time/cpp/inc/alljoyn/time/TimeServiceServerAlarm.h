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

#ifndef TIMESERVICESERVERALARM_H_
#define TIMESERVICESERVERALARM_H_

#include <alljoyn/time/TimeServiceSchedule.h>
#include <qcc/String.h>

namespace ajn {
namespace services {


class TimeServiceServer;
class TimeServiceAlarmBusObj;
class TimeServiceAlarmFactoryBusObj;

/**
 * Extend this class to receive events related to this Alarm object.
 */
class TimeServiceServerAlarm {

    /**
     * This Alarm object is a Friend of the TimeServiceServer
     */
    friend class TimeServiceServer;
    friend class TimeServiceAlarmFactoryBusObj;

  public:

    /**
     * Constructor
     */
    TimeServiceServerAlarm();

    /**
     * Destructor
     */
    virtual ~TimeServiceServerAlarm();

    /**
     * Returns TimeServiceSchedule of this Alarm
     *
     * @return Alarm
     */
    virtual const TimeServiceSchedule& getSchedule() = 0;

    /**
     * Set TimeServiceSchedule to this Alarm object
     *
     * @param schedule TimeServiceSchedule
     * @return Status ER_OK If succeeded to set the Schedule, otherwise, if this Alarm does not support
     * one of the Schedule values, return an appropriate failure status.
     */
    virtual QStatus setSchedule(const TimeServiceSchedule& schedule) = 0;

    /**
     * Returns Alarm title.
     *
     * @return Optional textual description of what this Alarm is
     */
    virtual const qcc::String& getTitle() = 0;

    /**
     * Set title to this Alarm.
     *
     * @param title Alarm title.
     * Title is an optional textual description of what this Alarm is set for.
     */
    virtual void setTitle(const qcc::String& title) = 0;

    /**
     * Returns whether or not this Alarm is enabled
     *
     * @return Is Alarm enabled
     */
    virtual const bool isEnabled() = 0;

    /**
     * Set whether or not this Alarm is enabled
     *
     * @param enabled Is Alarm enabled
     */
    virtual void setEnabled(bool enabled) = 0;

    /**
     * Return the object path of this Alarm object
     *
     * @return Object path
     */
    qcc::String const& getObjectPath() const;

    /**
     * Emit the signal when the Alarm is reached
     */
    QStatus alarmReached();

    /**
     * Releases object resources
     */
    void release();

  protected:

    /**
     * Returns Alarm BusObject
     */
    const TimeServiceAlarmBusObj* getAlarmBusObj();

  private:

    /**
     * TimeServiceAlarm TimeServiceAlarmBusObj implementing AllJoyn functionality
     */
    TimeServiceAlarmBusObj* m_AlarmBusObj;

    /**
     * Private Copy constructor of TimeServiceServerAlarm.
     * TimeServiceServerAlarm is not copy-able
     *
     * @param alarm TimeServiceServerAlarm to copy
     */
    TimeServiceServerAlarm(const TimeServiceServerAlarm& alarm);

    /**
     * Private assignment operator of TimeServiceServerAlarm.
     * TimeServiceServerAlarm is not assignable
     *
     * @param alarm
     *
     * @return TimeServiceServerAlarm
     */
    TimeServiceServerAlarm& operator=(const TimeServiceServerAlarm& alarm);

    /**
     * Associate this Alarm with the TimeServiceAlarmBusObj which implements AllJoyn functionality
     *
     * @param alarmBusObj AlarmBusObj
     * @return status if succeeded to set alarmBusObj
     */
    QStatus setAlarmBusObj(TimeServiceAlarmBusObj* alarmBusObj);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVERALARM_H_ */
