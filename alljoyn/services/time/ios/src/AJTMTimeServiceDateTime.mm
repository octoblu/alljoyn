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

#import "AJTMTimeServiceDateTime.h"
#import "alljoyn/time/TimeServiceDateTime.h"

@interface AJTMTimeServiceDateTime ()
@property ajn::services::TimeServiceDateTime* handle;
@end

@implementation AJTMTimeServiceDateTime

-(id)init
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::TimeServiceDateTime();
    }
    return self;
}

-(id)initWithHandle:(const ajn::services::TimeServiceDateTime*) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::TimeServiceDateTime*)handle;
    }
    return self;
}

-(QStatus)populateWithDate:(AJTMTimeServiceDate*) date time:(AJTMTimeServiceTime*) time offsetMinutes:(int16_t) offsetMinutes
{
    return self.handle->init([date getHandle], [time getHandle], offsetMinutes);
}

-(bool)isValid
{
    return self.handle->isValid();
}

-(AJTMTimeServiceDate*)date
{
   
    return [[AJTMTimeServiceDate alloc] initWithHandle:(ajn::services::TimeServiceDate*)&self.handle->getDate()];
}

-(AJTMTimeServiceTime*)time
{
    return [[AJTMTimeServiceTime alloc] initWithHandle:(ajn::services::TimeServiceTime*)&self.handle->getTime()];
}

-(int16_t)offsetMinutes
{
    return self.handle->getOffsetMinutes();
}

-(ajn::services::TimeServiceDateTime*)getHandle
{
    return _handle;
}

@end
