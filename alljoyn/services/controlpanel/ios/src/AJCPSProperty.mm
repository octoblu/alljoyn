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

#import "alljoyn/controlpanel/Property.h"
#import "AJCPSProperty.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJCPSControlPanelDevice.h"
#import "AJCPSConstraintList.h"

@interface AJCPSProperty ()
//using AJCPSWidget handle
@end


@implementation AJCPSProperty

- (id)initWithHandle:(ajn::services::Property *)handle
{
	self = [super initWithHandle:handle];
	if (self) {
	}
	return self;
}

/**
 * Get the PropertyType
 * @return PropertyType
 */
- (AJCPSPropertyType)getPropertyType
{
	return ((ajn::services::Property *)self.handle)->getPropertyType();
}

/**
 * Get the PropertyValue of the property
 * @return the value of the property
 */
- (void)getPropertyValue:(AJCPSPropertyValue &)propertyValue
{
	[self convertToAJCPSPropertyValue:((ajn::services::Property *)self.handle)->getPropertyValue() to:&propertyValue];
}

/**
 * Get the Unit of Measure
 * @return Unit of Measure Values
 */
- (NSString *)getUnitOfMeasure
{
	return [AJNConvertUtil convertQCCStringtoNSString:((ajn::services::Property *)self.handle)->getUnitOfMeasure()];
}

/**
 * Get the Constraint List vector
 * @return the Constraint List vector
 */
//const std::vector<ConstraintList>&
- (NSArray *)getConstraintList
{
    // TODO: the list gets erased when we leave this loop
    
	NSMutableArray *constraintList = [[NSMutableArray alloc]init];
    
	for (int i = 0; i != ((ajn::services::Property *)self.handle)->getConstraintList().size(); i++) {
		[constraintList addObject:[[AJCPSConstraintList alloc] initWithHandle:(ajn::services::ConstraintList *)&((ajn::services::Property *)self.handle)->getConstraintList().at(i)]];
	}
    
	return constraintList;
}

- (AJCPSConstraintRange *)getConstraintRange
{
    ajn::services::ConstraintRange *constraintRange = ((ajn::services::Property *)self.handle)->getConstraintRange();
    if (constraintRange) {
        return [[AJCPSConstraintRange alloc] initWithHandle:constraintRange];
    }
    else
        return nil;
}

- (QStatus)setValueFromBool:(bool)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromUnsignedShort:(uint16_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromShort:(int16_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromUnsignedLong:(uint32_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromLong:(int32_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromUnsignedLongLong:(uint64_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromLongLong:(int64_t)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromDouble:(double)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromCString:(const char *)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (QStatus)setValueFromDate:(AJCPSCPSDate *)value
{
	return ((ajn::services::Property *)self.handle)->setValue(*[value handle]);
}

- (QStatus)setValueFromTime:(AJCPSCPSTime *)value
{
	return ((ajn::services::Property *)self.handle)->setValue(value);
}

- (void)convertToAJCPSPropertyValue:(ajn::services::PropertyValue) constraintValue to:(AJCPSPropertyValue *)tConstraintValue
{
    switch ([self getPropertyType]) {
        case 	AJCPS_BOOL_PROPERTY :
            tConstraintValue->boolValue = constraintValue.boolValue;
            
            break;
        case 	AJCPS_UINT16_PROPERTY :
            tConstraintValue->uint16Value = constraintValue.uint16Value;
            
            break;
        case 	AJCPS_INT16_PROPERTY :
            tConstraintValue->int16Value = constraintValue.int16Value;
            
            break;
        case 	AJCPS_UINT32_PROPERTY :
            tConstraintValue->uint32Value = constraintValue.uint32Value;
            
            break;
        case 	AJCPS_INT32_PROPERTY :
            tConstraintValue->int32Value = constraintValue.int32Value;
            
            break;
        case 	AJCPS_UINT64_PROPERTY :
            tConstraintValue->uint64Value = constraintValue.uint64Value;
            
            break;
        case 	AJCPS_INT64_PROPERTY :
            tConstraintValue->int64Value  = constraintValue.int64Value;
            
            break;
        case 	AJCPS_DOUBLE_PROPERTY :
            tConstraintValue->doubleValue = constraintValue.doubleValue;
            
            break;
        case 	AJCPS_STRING_PROPERTY :
            tConstraintValue->charValue = constraintValue.charValue;
            
            break;
        case 	AJCPS_DATE_PROPERTY : //TODO
            
            break;
        case 	AJCPS_TIME_PROPERTY : //TODO
            
            break;
        default:
            break;
    }
}

@end
