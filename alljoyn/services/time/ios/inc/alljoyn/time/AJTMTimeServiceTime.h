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
#import "alljoyn/time/TimeServiceTime.h"

@interface AJTMTimeServiceTime : NSObject

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceTime*)handle;

/**
 * Initialize the object with its data.
 *
 * @param hour Expected range: 0-23
 * @param minute Expected range: 0-59
 * @param second Expected range: 0-59
 * @param millisecond Expected range: 0-999
 * @return ER_OK if the object was initialized successfully and valid arguments have been passed to this method,
 * otherwise ER_BAD_ARGUMENT status of the appropriate argument is returned
 */
-(QStatus)populateWithHour:(uint8_t) hour minute:(uint8_t) minute second:(uint8_t) second millisecond:(uint16_t) millisecond;

/**
 * Checks whether data of the object is valid, the object variables have a correct values.
 *
 *  - hour Expected range: 0-23
 *  - minute Expected range: 0-59
 *  - second Expected range: 0-59
 *  - millisecond Expected range: 0-999
 */
-(bool)isValid;

/**
 * Returns hour
 *
 * @return hour
 */
-(uint8_t)hour;

/**
 * Returns minute
 *
 * @return minute
 */
-(uint8_t)minute;

/**
 * Returns second
 *
 * @return second
 */
-(uint8_t)second;

/**
 * Returns milliseconds
 *
 * @return milliseconds
 */
-(uint8_t)millisecond;

-(const ajn::services::TimeServiceTime&)getHandle;

@end
