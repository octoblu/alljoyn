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

#import <Foundation/Foundation.h>
#import "alljoyn/controlpanel/ConstraintRange.h"
#import "AJCPSControlPanelEnums.h"

typedef union {
	/**
	 * Value of type uint16_t
	 */
	uint16_t uint16Value;
	/**
	 * Value of type int16_t
	 */
	int16_t int16Value;
	/**
	 * Value of type uint32_t
	 */
	uint32_t uint32Value;
	/**
	 * Value of type int32_t
	 */
	int32_t int32Value;
	/**
	 * Value of type uint64_t
	 */
	uint64_t uint64Value;
	/**
	 * Value of type int64_t
	 */
	int64_t int64Value;
	/**
	 * Value of type double
	 */
	double doubleValue;
} AJCPSConstraintRangeVal;

/**
 * AJCPSConstraintRange defines a range of
 * Values and constrain a property to those values.
 * The Constraint is applied on the controller side.
 * No validations are done in the Controlee.
 */
@interface AJCPSConstraintRange : NSObject

- (id)initWithHandle:(ajn ::services ::ConstraintRange *)handle;

/**
 * get the IncrementValue of the Constraint Range
 * @return IncrementValue
 */
- (uint16_t)getIncrementValue;

/**
 * Get the MaxValue of the Constraint Range
 * @return MaxValue
 */
- (uint16_t)getMaxValue;

/**
 * Get the MinValue of the Constraint Range
 * @return MinValue
 */
- (uint16_t)getMinValue;

@end