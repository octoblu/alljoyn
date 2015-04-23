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

#import "AJCPSConstraintRange.h"

@interface AJCPSConstraintRange ()
@property (nonatomic) ajn::services::ConstraintRange *handle;
@end

@implementation AJCPSConstraintRange

- (id)initWithHandle:(ajn::services::ConstraintRange *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

- (uint16_t)getIncrementValue
{
	return self.handle->getIncrementValue().int16Value;
}

- (uint16_t)getMaxValue
{
	return self.handle->getMaxValue().int16Value;
}

- (uint16_t)getMinValue
{
	return self.handle->getMinValue().int16Value;
}

@end