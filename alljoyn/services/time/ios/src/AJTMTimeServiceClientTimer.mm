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
#import "AJTMTimeServiceClientTimer.h"
#import "alljoyn/time/TimeServiceClientTimer.h"


@interface AJTMTimeServiceClientTimer()

@property ajn::services::TimeServiceClientTimer* handle;

@end


@implementation AJTMTimeServiceClientTimer
/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientTimer*)handle
{
    self = [super initWithHandle:handle];
    if (self) {
        self.handle = (ajn::services::TimeServiceClientTimer *)handle;
    }
    return self;
}

-(void)releaseObject
{
    self.handle->release();
}

-(QStatus)retrieveInterval:(AJTMTimeServicePeriod**) interval
{
    ajn::services::TimeServicePeriod nativeInterval;
    QStatus status = self.handle->retrieveInterval(&nativeInterval);

    if (status == ER_OK) {
        *interval = [[AJTMTimeServicePeriod alloc]init];
        status = [*interval populateWithHour:nativeInterval.getHour() minute:nativeInterval.getMinute() second:nativeInterval.getSecond() millisecond:nativeInterval.getMillisecond()];
    }

    return status;

}


-(QStatus)setInterval:(AJTMTimeServicePeriod*)interval
{
    return self.handle->setInterval([interval getHandle]);
}


-(QStatus)retrieveTimeLeft:(AJTMTimeServicePeriod**)period
{
    ajn::services::TimeServicePeriod nativePeriod;
    QStatus status = self.handle->retrieveTimeLeft(&nativePeriod);

    if (status == ER_OK) {
        *period = [[AJTMTimeServicePeriod alloc]init ];

        status = [*period populateWithHour:nativePeriod.getHour() minute:nativePeriod.getMinute() second:nativePeriod.getSecond() millisecond:nativePeriod.getMillisecond()];
    }

    return status;
}


-(QStatus)retrieveIsRunning:(bool *)isRunning
{
    return self.handle->retrieveIsRunning(*isRunning);
}


-(QStatus)retrieveRepeat:(uint16_t *) repeat
{
    return self.handle->retrieveRepeat(*repeat);
}


-(QStatus)setRepeat:(short) repeat
{
    return self.handle->setRepeat(repeat);
}


-(QStatus)retrieveTitle:(NSString**) title
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


-(QStatus)start
{
    return self.handle->start();
}


-(QStatus)pause
{
    return self.handle->pause();
}


-(QStatus)reset
{
    return self.handle->reset();
}


-(QStatus)registerTimerHandler:(id<AJTMTimerHandler>) handler
{
    TimerHandlerAdapter *adapter = new TimerHandlerAdapter(handler);
    return self.handle->registerTimerHandler(adapter);
}


-(void) unRegisterTimerHandler
{
    self.handle->unRegisterTimerHandler();
}


-(id<AJTMTimerHandler>) getTimerHandler
{
    return ((TimerHandlerAdapter *)self.handle->getTimerHandler())->handle;
}

@end