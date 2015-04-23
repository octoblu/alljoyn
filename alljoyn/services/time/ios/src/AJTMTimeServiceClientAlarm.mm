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
#import "AJTMTimeServiceClientAlarm.h"
#import "alljoyn/time/TimeServiceClientAlarm.h"


@interface AJTMTimeServiceClientAlarm()

@property ajn::services::TimeServiceClientAlarm* handle;

@end


@implementation AJTMTimeServiceClientAlarm
/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientAlarm*)handle
{
    self = [super initWithHandle:handle];
    if (self) {
        self.handle = (ajn::services::TimeServiceClientAlarm *)handle;
    }
    return self;
}

-(void)releaseObject
{
    self.handle->release();
}

-(QStatus)retrieveSchedule:(AJTMTimeServiceSchedule**) schedule
{
    ajn::services::TimeServiceSchedule nativeSchedule;
    QStatus status = self.handle->retrieveSchedule(&nativeSchedule);

    if (status == ER_OK) {
        *schedule = [[AJTMTimeServiceSchedule alloc]init];

        ajn::services::TimeServiceTime *time = new ajn::services::TimeServiceTime();
        time->init(nativeSchedule.getTime().getHour(),nativeSchedule.getTime().getMinute(), nativeSchedule.getTime().getSecond(), nativeSchedule.getTime().getMillisecond());
        AJTMTimeServiceTime *objtime = [[AJTMTimeServiceTime alloc]initWithHandle:time];
        [*schedule populateWithTime:objtime weekDays:nativeSchedule.getWeekDays()];
    }

    return status;
}


-(QStatus)setSchedule:(AJTMTimeServiceSchedule*) schedule
{
    return self.handle->setSchedule([schedule getHandle]);
}


-(QStatus)retrieveTitle:(NSString**)title
{
    qcc::String nativeTitle;
    QStatus status = self.handle->retrieveTitle(&nativeTitle);

    if (status == ER_OK) {
        *title = [NSString stringWithUTF8String:nativeTitle.c_str()];
    }

    return status;
}


-(QStatus)setTitle:(NSString *)title
{
    return self.handle->setTitle([title UTF8String]);
}


-(QStatus)retrieveIsEnabled:(bool*)isEnabled
{
    return self.handle->retrieveIsEnabled(isEnabled);
}


-(QStatus)setEnabled:(bool)isEnabled
{
    return self.handle->setEnabled(isEnabled);
}


-(QStatus)registerAlarmHandler:(id<AJTMAlarmHandler>)handler
{
    AlarmHandlerAdapter *adapter = new AlarmHandlerAdapter(handler);
    return self.handle->registerAlarmHandler(adapter);

}


-(void)unRegisterAlarmHandler
{
    self.handle->unRegisterAlarmHandler();
}


-(id<AJTMAlarmHandler>)getAlarmHandler
{
    return ((AlarmHandlerAdapter *)self.handle->getAlarmHandler())->handle;

}


@end