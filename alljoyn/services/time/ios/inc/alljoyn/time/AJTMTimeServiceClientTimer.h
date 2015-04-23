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

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import <Foundation/Foundation.h>
#import "alljoyn/time/TimeServiceClientTimer.h"
#import "AJTMTimeServiceObjectIntrospector.h"
#import "AJTMTimeServicePeriod.h"

@class AJTMTimeServiceClientTimer;

@protocol AJTMTimerHandler <NSObject>

/**
 * Handle Timer Event event, which was sent from the given TimeServiceClientTimer
 *
 * @param timer The TimeServiceClientTimer that received the event.
 */
-(void)handleTimerEvent:(AJTMTimeServiceClientTimer*) timer;

/**
 * Handle run state changed event, which was sent from the given TimeServiceClientTimer
 *
 * @param timer The TimeServiceClientTimer that received the event.
 * @param runState The state of the run
 */
-(void)handleRunStateChanged:(AJTMTimeServiceClientTimer*) timer runState:(bool)runState;


@end

@interface AJTMTimeServiceClientTimer : AJTMTimeServiceObjectIntrospector

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientTimer*)handle;

/**
 * Release object resources
 */
-(void)releaseObject;

/**
 * Retrieve timer interval from the TimeServiceServer Timer object.
 *
 * @param interval TimeServicePeriod interval.
 *
 * @return status
 */
-(QStatus)retrieveInterval:(AJTMTimeServicePeriod**) interval;

/**
 * Set Timer interval to the TimeServiceServer Timer object.
 *
 * @param period TimeServicePeriod period
 *
 * @return status
 */
-(QStatus)setInterval:(AJTMTimeServicePeriod*)interval;

/**
 * Retrieve amount of Time Left until the TimeServiceServer fires.
 *
 * @param period TimeServicePeriod of Time Left
 *
 * @return status
 */
-(QStatus)retrieveTimeLeft:(AJTMTimeServicePeriod**)period;

/**
 * Retrieve whether the TimeServiceClientTimer is currently running
 *
 * @param isRunning TRUE if the TimeServiceClientTimer is running
 *
 * @return status
 */
-(QStatus)retrieveIsRunning:(bool *)isRunning;

/**
 * Retrieve how many times this TimeServiceClientTimer should repeat itself.
 * The value of TIMER_REPEAT_FOREVER means to repeat the timer forever
 *
 * @param repeat How many times this TimeServiceClientTimer should repeat itself.
 *
 * @return status
 */
-(QStatus)retrieveRepeat:(uint16_t *) repeat;

/**
 * Set how many times this TimeServiceClientTimer should repeat itself.
 * The value of TIMER_REPEAT_FOREVER means to repeat the timer forever.
 *
 * @param repeat How many times this TimeServiceClientTimer should repeat itself
 *
 * @return status
 */
-(QStatus)setRepeat:(short) repeat;

/**
 * Retrieve this TimeServiceClientTimer title.

 * @param title TimeServiceClientTimer title
 *
 * @return status
 */
-(QStatus)retrieveTitle:(NSString**) title;

/**
 * Set title to the TimeServiceServer Timer object.
 *
 * @param title Timer title.
 * Title is an optional textual description of what this Timer is set for.
 *
 * @return status
 */
-(QStatus)setTitle:(NSString *)title;

/**
 * Start the TimeServiceClientTimer
 *
 * @return status
 */
-(QStatus)start;

/**
 * Pause the TimeServiceClientTimer execution
 *
 * @return status
 */
-(QStatus)pause;

/**
 * Reset the TimeServiceClientTimer so that TimeLeft is equal to Interval
 *
 * @return status
 */
-(QStatus)reset;

/*
 * Register TimerHandler to receive Timer related events.
 *
 * @param handler handler
 *
 * @return status
 */
-(QStatus)registerTimerHandler:(id<AJTMTimerHandler>) handler;

/**
 * Unregister TimerHandler to stop receiving Timer related events
 * events.
 */
-(void) unRegisterTimerHandler;

/**
 * @return Timer handler that has been registered or NULL
 */
-(id<AJTMTimerHandler>) getTimerHandler;

@end


class TimerHandlerAdapter: public ajn::services::TimerHandler {

public:

    id<AJTMTimerHandler> handle;

    TimerHandlerAdapter(id<AJTMTimerHandler> timerHandler):handle(timerHandler){}

    /**
     * Handle Timer Event event, which was sent from the given TimeServiceClientTimer
     *
     * @param timer The TimeServiceClientTimer that received the event.
     */

    void handleTimerEvent(ajn::services::TimeServiceClientTimer*timer)
    {[handle handleTimerEvent:[[AJTMTimeServiceClientTimer alloc]initWithHandle:timer]];}

    /**
     * Handle run state changed event, which was sent from the given TimeServiceClientTimer
     *
     * @param timer The TimeServiceClientTimer that received the event.
     * @param runState The state of the run
     */
    void handleRunStateChanged(ajn::services::TimeServiceClientTimer*timer, bool runState)
    {[handle handleRunStateChanged:[[AJTMTimeServiceClientTimer alloc]initWithHandle:timer] runState:runState];}
};
