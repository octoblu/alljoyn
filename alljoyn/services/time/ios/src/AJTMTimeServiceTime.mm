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

#import "AJTMTimeServiceTime.h"

@interface AJTMTimeServiceTime ()
@property ajn::services::TimeServiceTime* handle;
@end

@implementation AJTMTimeServiceTime

-(id)init
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::TimeServiceTime();
    }
    return self;
}

- (id)initWithHandle:(ajn::services::TimeServiceTime*)handle;
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::TimeServiceTime *)handle;
    }
    return self;
}

-(QStatus)populateWithHour:(uint8_t) hour minute:(uint8_t) minute second:(uint8_t) second millisecond:(uint16_t) millisecond
{
    return self.handle->init(hour, minute, second, millisecond);
}

-(bool)isValid
{
    return self.handle->isValid();
}

-(uint8_t)hour
{
    return self.handle->getHour();
}

-(uint8_t)minute
{
    return self.handle->getMinute();
}

-(uint8_t)second
{
    return self.handle->getSecond();
}

-(uint8_t)millisecond
{
    return self.handle->getMillisecond();
}

-(const ajn::services::TimeServiceTime&)getHandle
{
    return *self.handle;
}

@end
