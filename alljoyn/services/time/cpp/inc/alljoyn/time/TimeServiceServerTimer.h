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

#ifndef TIMESERVICESERVERTIMER_H_
#define TIMESERVICESERVERTIMER_H_

#include <alljoyn/time/TimeServicePeriod.h>
#include <qcc/String.h>

namespace ajn {
namespace services {

class TimeServiceServer;
class TimeServiceTimerBusObj;

/**
 * Inherit from this class to receive events related to this Timer object
 * and send Timer signals.
 */
class TimeServiceServerTimer {

    friend class TimeServiceServer;
    friend class TimeServiceTimerFactoryBusObj;

  public:

    /**
     * Constructor
     */
    TimeServiceServerTimer();

    /**
     * Destructor
     */
    virtual ~TimeServiceServerTimer();

    /**
     * Returns TimeServiceSchedule of this Timer
     *
     * @return interval
     */
    virtual const TimeServicePeriod& getInterval() = 0;

    /**
     * Set interval of this Timer object
     *
     * @param interval TimeServiceSchedule
     *
     * @return Status ER_OK If succeeded to set the Schedule, otherwise, if this Timer does not support
     * one of the interval values, return an appropriate failure status.
     */
    virtual QStatus setInterval(const TimeServicePeriod& interval) = 0;

    /**
     * Returns time left for this Timer
     *
     * @return time left
     */
    virtual const TimeServicePeriod& getTimeLeft() = 0;

    /**
     * Returns Timer title.
     *
     * @return Optional textual description of what this Timer is
     */
    virtual const qcc::String& getTitle() = 0;

    /**
     * Set title of this Timer.
     *
     * @param title Timer title.
     * Title is an optional textual description of what this Timer is set for.
     */
    virtual void setTitle(const qcc::String& title) = 0;

    /**
     * Returns whether or not this Timer is running
     *
     * @return Is Timer running
     */
    virtual const bool isRunning() = 0;

    /**
     * Returns number of repeats for this Timer
     *
     * @return repeats
     */
    virtual const uint16_t getRepeat() = 0;

    /**
     * Sets number of repeats for this Timer
     *
     * @param repeat Number of repeats for this Timer
     */
    virtual void setRepeat(uint16_t repeat) = 0;

    /**
     * Start Timer
     */
    virtual void start() = 0;

    /**
     * Reset Timer
     */
    virtual void reset() = 0;

    /**
     * Pause
     */
    virtual void pause() = 0;

    /**
     * Return the object path of this Timer object
     *
     * @return Object path
     */
    qcc::String const& getObjectPath() const;

    /**
     * Emit the signal when the Timer is reached
     *
     * @return status ER_OK if succeeded to emit the signal, otherwise an appropriate error is returned
     */
    QStatus timerEvent();

    /**
     * Emit the signal when the Timer changes its running state as a result of
     * start() or pause() invocation.
     *
     * @param isRunning TRUE if this Timer is currently running
     *
     * @return status ER_OK if succeeded to emit the signal, otherwise an appropriate error is returned.
     * Status of ER_BUS_NO_SESSION means that the signal wasn't emitted because, currently there is no
     * session established with the TimeServiceServer
     */
    QStatus runStateChanged(bool isRunning);

    /**
     * Releases object resources
     */
    void release();

  protected:

    /**
     * Returns Timer BusObject
     */
    const TimeServiceTimerBusObj* getTimerBusObj();

  private:

    /**
     * Private Copy constructor of TimeServiceServerTimer.
     * TimeServiceServerTimer is not copy-able
     *
     * @param serverTimer TimeServiceServerTimer to copy
     */
    TimeServiceServerTimer(const TimeServiceServerTimer& serverTimer);

    /**
     * Private assignment operator of TimeServiceServerTimer.
     * TimeServiceServerTimer is not assignable
     *
     * @param serverTimer
     *
     * @return TimeServiceServerTimer
     */
    TimeServiceServerTimer& operator=(const TimeServiceServerTimer& serverTimer);

    /**
     * TimeServiceTimer TimeServiceTimerBusObj implementing AllJoyn functionality
     */
    TimeServiceTimerBusObj* m_TimerBusObj;

    /**
     * Associate this Timer with the TimeServiceTimerBusObj which implements AllJoyn functionality
     *
     * @param TimerBusObj TimerBusObj
     * @return status if succeeded to set TimerBusObj
     */
    QStatus setTimerBusObj(TimeServiceTimerBusObj* TimerBusObj);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVERTIMER_H_ */
