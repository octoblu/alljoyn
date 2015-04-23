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
#import "AJCPSWidget.h"
#import "alljoyn/controlpanel/Property.h"
#import "AJCPSConstraintRange.h"
#import "alljoyn/controlpanel/CPSDate.h"
#import "alljoyn/controlpanel/CPSTime.h"
#import "AJCPSCPSDate.h"
#import "AJCPSCPSTime.h"

/**
 * Union that allows the definition of a function pointer
 * to get the Properties value.
 */
typedef union {
    /**
     * Get Value for type uint16t
     * @return value of Property
     */
    uint16_t uint16Value;
    /**
     * Get Value for type int16_t
     * @return value of Property
     */
    int16_t int16Value;
    /**
     * Get Value for type uint32_t
     * @return value of Property
     */
    uint32_t uint32Value;
    /**
     * Get Value for type int32_t
     * @return value of Property
     */
    int32_t int32Value;
    /**
     * Get Value for type uint64_t
     * @return value of Property
     */
    uint64_t uint64Value;
    /**
     * Get Value for type int64_t
     * @return value of Property
     */
    int64_t int64Value;
    /**
     * Get Value for type double
     * @return value of Property
     */
    double doubleValue;
    /**
     * Get Value for type const char*
     * @return value of Property
     */
    const char* charValue;
    
    /**
     * Get Value for type bool
     * @return value of Property
     */
    bool boolValue;
    /**
     * Get Value for type CPSDate
     * @return value of Property
     */
    // CPSDate* dateValue; TODO: arc forbids objc objects here
    /**
     * Get Value for type CPSTime
     * @return value of Property
     */
    // CPSTime* timeValue; TODO: arc forbids objc objects here
    
    /**
     * Get Value for type CPSDate
     * @return value of Property
     */
    ajn::services::CPSDate* dateValue;
    /**
     * Get Value for type CPSTime
     * @return value of Property
     */
    ajn::services::CPSTime* timeValue;
    
} AJCPSPropertyValue;

/**
 * AJCPSProperty is used to display a property Widget.
 */
@interface AJCPSProperty : AJCPSWidget

- (id)initWithHandle:(ajn ::services ::Property *)handle;

/**
 * Get the PropertyType
 * @return PropertyType
 */
- (AJCPSPropertyType)getPropertyType;

/**
 * Get the PropertyValue of the property
 * @param propertyValue - the property value according to the property type
 * @return the value of the property
 */
- (void)getPropertyValue:(AJCPSPropertyValue &)propertyValue;

/**
 * Get the Unit of Measure
 * @return Unit of Measure Values
 */
- (NSString *)getUnitOfMeasure;

/**
 * Get the Constraint List vector
 * @return the Constraint List vector
 */
//const std::vector<ConstraintList>&
- (NSArray *)getConstraintList;



/**
 * Get the Constraint Range
 * @return the Constraint Range
 */
- (AJCPSConstraintRange *)getConstraintRange;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromBool:(bool)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromUnsignedShort:(uint16_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromShort:(int16_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromUnsignedLong:(uint32_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromLong:(int32_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromUnsignedLongLong:(uint64_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromLongLong:(int64_t)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromDouble:(double)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromCString:(const char *)value;



/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromDate:(AJCPSCPSDate *)value;

/**
 * Set the new Value
 * @param value - new Value to be set to
 * @return status - success/failure
 */
- (QStatus)setValueFromTime:(AJCPSCPSTime *)value;

@end