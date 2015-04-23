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
#import "alljoyn/time/TimeServiceClientAlarm.h"
#import "AJTMTimeServiceSchedule.h"
#import "AJTMTimeServiceObjectIntrospector.h"

@class AJTMTimeServiceClientAlarm;


@protocol AJTMAlarmHandler <NSObject>

/**
 * Handle Alarm reached event, which was sent from the given TimeServiceClientAlarm
 *
 * @param alarm The TimeServiceClientAlarm that received the AlarmReached event.
 */
-(void)handleAlarmReached:(AJTMTimeServiceClientAlarm*) alarm;

@end

@interface AJTMTimeServiceClientAlarm : AJTMTimeServiceObjectIntrospector

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientAlarm*)handle;

/**
 * Release object resources
 */
-(void)releaseObject;

/**
 * Retrieve Schedule from the server Alarm object.
 *
 * @param schedule Out variable. Is populated with the retrieved Schedule.
 *
 * @return status ER_OK, or a failure status if failed to retrieve Schedule
 */
-(QStatus)retrieveSchedule:(AJTMTimeServiceSchedule**) schedule;

/**
 * Set Schedule to the server Alarm object.
 *
 * @param schedule Schedule to be set
 *
 * @return status ER_OK, or a failure status if failed to set the Schedule
 */
-(QStatus)setSchedule:(AJTMTimeServiceSchedule*) schedule;

/**
 * Retrieve Title from the server Alarm object.
 *
 * @param title Out variable. Is populated with the retrieved Alarm title.
 *
 * @return status ER_OK, or a failure status if failed to retrieve the Alarm title
 */
-(QStatus)retrieveTitle:(NSString**)title;

/**
 * Set Title to the server Alarm object.
 *
 * @param title Title to be set
 *
 * @return status ER_OK, or a failure status if failed to set the Alarm title
 */
-(QStatus)setTitle:(NSString *)title;

/**
 * Retrieve IsEnabled from the server Alarm object.
 *
 * @param isEnabled Out variable.
 *
 * @return status ER_OK, or a failure status if failed to retrieve IsEnabled property
 */
-(QStatus)retrieveIsEnabled:(bool*)isEnabled;

/**
 * Set IsEnabled status to the server Alarm object.
 *
 * @param isEnabled IsEnabled status to be set
 *
 * @return status ER_OK, or a failure status if failed to set IsEnabled property
 */
-(QStatus)setEnabled:(bool)isEnabled;

/*
 * Register AlarmHandler to receive AlarmReached events.
 *
 * @param handler AlarmHandler
 *
 * @return status
 */
-(QStatus)registerAlarmHandler:(id<AJTMAlarmHandler>)handler;

/**
 * Unregister AlarmHandler to stop receiving AlarmReached
 * events.
 */
-(void)unRegisterAlarmHandler;

/**
 * @return AlarmHandler that has been registered or NULL
 */
-(id<AJTMAlarmHandler>)getAlarmHandler;

@end

class AlarmHandlerAdapter : public ajn::services::AlarmHandler
{
public:
    id<AJTMAlarmHandler> handle;

    /**
     * Constructor
     */
    AlarmHandlerAdapter(id<AJTMAlarmHandler> alarmHandler):handle(alarmHandler){}

    void handleAlarmReached(ajn::services::TimeServiceClientAlarm* alarm) {[handle handleAlarmReached:[[AJTMTimeServiceClientAlarm alloc]initWithHandle:alarm]];}
};
