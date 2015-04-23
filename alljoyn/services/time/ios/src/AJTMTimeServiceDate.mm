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

#import "AJTMTimeServiceDate.h"

@interface AJTMTimeServiceDate ()
@property ajn::services::TimeServiceDate* handle;
@end

@implementation AJTMTimeServiceDate

-(id)init
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::TimeServiceDate();
    }
    return self;
}

- (id)initWithHandle:(ajn::services::TimeServiceDate *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::TimeServiceDate *)handle;
    }
    return self;
}

-(QStatus)populateWithYear:(uint16_t) year month:(uint8_t) month day:(uint8_t) day
{
    QStatus status;
    status = self.handle->init(year, month, day);
    return status;
}

-(bool)isValid
{
    return self.handle->isValid();
}

-(uint16_t)year
{
    return self.handle->getYear();
}

-(uint8_t)month
{
    return self.handle->getMonth();
}

-(uint8_t)day
{
    return self.handle->getDay();
}

-(const ajn::services::TimeServiceDate&)getHandle
{
    return *self.handle;
}

@end
