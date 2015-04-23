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

#import "AJCPSConstraintList.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJCPSConstraintList ()
@property (nonatomic) ajn::services::ConstraintList *handle;
@end

@implementation AJCPSConstraintList
- (id)initWithHandle:(ajn::services::ConstraintList *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

/**
 * Get the Constraint Value
 * @return the Constraint Value
 */
- (AJCPSConstraintValue)getConstraintValue
{
	return [self convertToAJCPSConstraintValue:self.handle->getConstraintValue()];
}


/**
 * Get the Property Type of the Constraint
 * @return propertyType of the Constraint
 */
- (AJCPSPropertyType)getPropertyType
{
	return self.handle->getPropertyType();
}

/**
 * Get the Display
 * @return Display Value
 */
- (NSString *)getDisplay
{
   
    const qcc::String str = self.handle->getDisplay();

    return [AJNConvertUtil convertQCCStringtoNSString:str];
}


- (AJCPSConstraintValue)convertToAJCPSConstraintValue:(ajn::services::ConstraintValue) constraintValue
{
    AJCPSConstraintValue tConstraintValue;
    
    switch ([self getPropertyType]) {
        case 	AJCPS_UINT16_PROPERTY :
            tConstraintValue.uint16Value = constraintValue.uint16Value;
            
            break;
        case 	AJCPS_INT16_PROPERTY :
            tConstraintValue.int16Value = constraintValue.int16Value;
            
            break;
        case 	AJCPS_UINT32_PROPERTY :
            tConstraintValue.uint32Value = constraintValue.uint32Value;
            
            break;
        case 	AJCPS_INT32_PROPERTY :
            tConstraintValue.int32Value = constraintValue.int32Value;
            
            break;
        case 	AJCPS_UINT64_PROPERTY :
            tConstraintValue.uint64Value = constraintValue.uint64Value;
            
            break;
        case 	AJCPS_INT64_PROPERTY :
            tConstraintValue.int64Value  = constraintValue.int64Value;
            
            break;
        case 	AJCPS_DOUBLE_PROPERTY :
            tConstraintValue.doubleValue = constraintValue.doubleValue;
            
            break;
        case 	AJCPS_STRING_PROPERTY :
            tConstraintValue.charValue = constraintValue.charValue;
            
            break;
        case 	AJCPS_DATE_PROPERTY : //TODO
            
            break;
        case 	AJCPS_TIME_PROPERTY : //TODO
            
            break;
        default:
            break;
    }
    
    return tConstraintValue;
}
@end