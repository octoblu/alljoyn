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
#import "alljoyn/controlpanel/ConstraintList.h"
#import "AJCPSControlPanelEnums.h"

/**
 * ConstraintValue - a union combining all possible values
 * that can make up a constraint
 */
typedef union {
    /**
     * Value for uint16_t
     */
    uint16_t uint16Value;
    /**
     * Value for int16_t
     */
    int16_t int16Value;
    /**
     * Value for uint32_t
     */
    uint32_t uint32Value;
    /**
     * Value for int32_t
     */
    int32_t int32Value;
    /**
     * Value for uint64_t
     */
    uint64_t uint64Value;
    /**
     * Value for int64_t
     */
    int64_t int64Value;
    /**
     * Value for double
     */
    double doubleValue;
    /**
     * Value for const char*
     */
    const char* charValue;
} AJCPSConstraintValue;

/**
 * AJCPSConstraintList defines a list of
 * Values and constrain a property to those values.
 * The Constraint is applied on the controller side.
 * No validations are done in the Controllee
 */
@interface AJCPSConstraintList : NSObject

- (id)initWithHandle:(ajn ::services ::ConstraintList *)handle;

/**
 * Get the Constraint Value
 * @return the Constraint Value
 */
- (AJCPSConstraintValue)getConstraintValue;

/**
 * Get the Property Type of the Constraint
 * @return propertyType of the Constraint
 */
- (AJCPSPropertyType)getPropertyType;

/**
 * Get the Display
 * @return Display Value
 */
- (NSString *)getDisplay;

@end