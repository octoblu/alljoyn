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

#ifndef TIMESERVICECLIENTTIMER_H_
#define TIMESERVICECLIENTTIMER_H_

#include <alljoyn/Status.h>
#include <alljoyn/time/TimeServiceObjectIntrospector.h>
#include <alljoyn/time/TimeServicePeriod.h>

namespace ajn {
namespace services {

class TimeServiceClientTimer;

/**
 * Inherit from this class to handle timer events.
 */
class TimerHandler {

  public:

    /**
     * Destructor
     */
    virtual ~TimerHandler()
    {
    }

    /**
     * Handle Timer Event event, which was sent to the given TimeServiceClientTimer
     *
     * @param timer The TimeServiceClientTimer that received the event.
     */

    virtual void handleTimerEvent(TimeServiceClientTimer*timer) = 0;

    /**
     * Handle run state changed event, which was sent to the given TimeServiceClientTimer
     *
     * @param timer The TimeServiceClientTimer that received the event.
     * @param runState The state of the run
     */
    virtual void handleRunStateChanged(TimeServiceClientTimer*timer, bool runState) = 0;
};

/**
 * Time Service Timer.
 * Use this class to communication with the server Timer object.
 */
class TimeServiceClientTimer : public TimeServiceObjectIntrospector {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this TimeServiceClientTimer object
     * @param objectPath Object path of the server side Timer object
     */
    TimeServiceClientTimer(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientTimer();

    /**
     * Release object resources
     */
    void release();

    /**
     * Register TimerHandler to receive Timer related events.
     *
     * @param handler handler
     *
     * @return status
     */
    QStatus registerTimerHandler(TimerHandler* handler);

    /**
     * Unregister TimerHandler to stop receiving Timer related events
     * events.
     */
    void unRegisterTimerHandler();


    /**
     * Returns Timer handler that has been registered or NULL
     *
     * @return Timer handler that has been registered or NULL
     */
    TimerHandler* getTimerHandler();

    /**
     * Retrieve timer interval from the TimeServiceServer Timer object.
     *
     * @param interval TimeServicePeriod interval.
     *
     * @return status
     */
    QStatus retrieveInterval(TimeServicePeriod* interval);

    /**
     * Set Timer interval to the TimeServiceServer Timer object.
     *
     * @param interval TimeServicePeriod period
     *
     * @return status
     */
    QStatus setInterval(const TimeServicePeriod& interval);

    /**
     * Retrieve amount of Time Left until the TimeServiceServer fires.
     *
     * @param period TimeServicePeriod of Time Left
     *
     * @return status
     */
    QStatus retrieveTimeLeft(TimeServicePeriod* period);

    /**
     * Retrieve whether the TimeServiceClientTimer is currently running
     *
     * @param isRunning TRUE if the TimeServiceClientTimer is running
     *
     * @return status
     */
    QStatus retrieveIsRunning(bool& isRunning);

    /**
     * Retrieve how many times this TimeServiceClientTimer should repeat itself.
     * The value of TIMER_REPEAT_FOREVER means to repeat the timer forever
     *
     * @param repeat How many times this TimeServiceClientTimer should repeat itself.
     *
     * @return status
     */
    QStatus retrieveRepeat(uint16_t& repeat);

    /**
     * Set how many times this TimeServiceClientTimer should repeat itself.
     * The value of TIMER_REPEAT_FOREVER means to repeat the timer forever.
     *
     * @param repeat How many times this TimeServiceClientTimer should repeat itself
     *
     * @return status
     */
    QStatus setRepeat(short repeat);

    /**
     * Retrieve this TimeServiceClientTimer title.
     * @param title TimeServiceClientTimer title
     *
     * @return status
     */
    QStatus retrieveTitle(qcc::String* title);

    /**
     * Set title to the TimeServiceServer Timer object.
     *
     * @param title Timer title.
     * Title is an optional textual description of what this Timer is set for.
     *
     * @return status
     */
    QStatus setTitle(const qcc::String& title);

    /**
     * Start the TimeServiceClientTimer
     *
     * @return status
     */
    QStatus start();

    /**
     * Pause the TimeServiceClientTimer execution
     *
     * @return status
     */
    QStatus pause();

    /**
     * Reset the TimeServiceClientTimer so that TimeLeft is equal to Interval
     *
     * @return status
     */
    QStatus reset();

  private:

    /**
     * Handler of the Timer related events
     */
    TimerHandler* m_TimerHandler;

    /**
     * Interface description of the Timer interface
     */
    InterfaceDescription* m_TimerIface;

};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTTIMER_H_ */
