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

#import "AJCPSCPSTime.h"

@interface AJCPSCPSTime ()
@property (nonatomic) ajn::services::CPSTime *handle;
@end

@implementation AJCPSCPSTime

- (id)initWithHour:(uint16_t) hour minute:(uint16_t) minute second:(uint16_t) second
{
    self = [super init];
	if (self) {
		self.handle = new  ajn::services::CPSTime(hour, minute, second);
	}
	return self;
}

- (id)initWithHandle:(ajn::services::CPSTime *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

/**
 * Get the hour value of the date
 * @return hour value
 */
- (uint16_t)getHour
{
	return self.handle->getHour();
}

/**
 * Set the hour Value of the date
 * @param hour value
 */
- (void)setHour:(uint16_t)hour
{
	self.handle->setHour(hour);
}

/**
 * Get the Minute value of the date
 * @return minute value
 */
- (uint16_t)getMinute
{
	return self.handle->getMinute();
}

/**
 * Set the Minute value of the date
 * @param minute value
 */
- (void)setMinute:(uint16_t)minute
{
	return self.handle->setMinute(minute);
}

/**
 * Get the Second value of the date
 * @return second value
 */
- (uint16_t)getSecond
{
	return self.handle->getSecond();
}

/**
 * Set the Second value of the date
 * @param second value
 */
- (void)setSecond:(uint16_t)second
{
	return self.handle->setSecond(second);
}

@end