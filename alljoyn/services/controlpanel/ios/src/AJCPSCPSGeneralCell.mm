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

#import "AJCPSCPSGeneralCell.h"
#import "AJCPSProperty.h"
#import "AJCPSConstraintRange.h"
#import "AJCPSConstraintList.h"
#import "AJCPSErrorWidget.h"

@implementation CPSGeneralCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Widget Name
        self.widgetNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 0, 310, 20)];
        
        // Hint
        self.hintLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 20, 310, 20)];
        self.hintLabel.textColor = [UIColor colorWithRed:0.4 green:0 blue:0.4 alpha:1];
        
        // Label Details
        self.widgetDetailsLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 20, 270, 110)];
        [self.widgetDetailsLabel setFont:[UIFont systemFontOfSize:11]];
        self.widgetDetailsLabel.lineBreakMode = NSLineBreakByWordWrapping;
        self.widgetDetailsLabel.numberOfLines = 0;

        
    
       // Add UI objects to subview
        [self.contentView addSubview:self.widgetNameLabel];
        [self.contentView addSubview:self.widgetDetailsLabel];
        [self.contentView addSubview:self.hintLabel];
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
}

- (NSString *)widgetHints
{
    NSMutableString *str = [[NSMutableString alloc]init];
    NSArray *propertyHintsStrings = [[NSArray alloc] initWithObjects: @"", @"SWITCH", @"CHECKBOX", @"SPINNER", @"RADIOBUTTON", @"SLIDER",
                                     @"TIMEPICKER", @"DATEPICKER", @"NUMBERPICKER", @"KEYPAD", @"ROTARYKNOB",
                                     @"TEXTVIEW", @"NUMERICVIEW", @"EDITTEXT", nil];
    
    [self setAccessoryType:UITableViewCellAccessoryNone];
    
    NSArray *hints = [self.widget getHints];
    if ([hints count]) {
        
        for (NSInteger i = 0 ; i < [hints count]; i++) {
            NSInteger hint = [hints[i] integerValue];
            
            switch ([self.widget getWidgetType]) {
                case AJCPS_ACTION:
                case AJCPS_ACTION_WITH_DIALOG:
                    if (hint == AJCPS_ACTIONBUTTON)
                        self.hintLabel.text=@"ACTIONBUTTON";
                    else
                        self.hintLabel.text=@"UNKNOWN";
                    break;
                    
                case AJCPS_LABEL:
                    if (hint == AJCPS_TEXTLABEL)
                        self.hintLabel.text=@"TEXTLABEL";
                    else
                        self.hintLabel.text=@"UNKNOWN";
                    break;

                case AJCPS_PROPERTY:
                {
                    AJCPSPropertyValue propertyValue;
                    [(AJCPSProperty *)self.widget getPropertyValue:propertyValue];
                    
                    switch ([(AJCPSProperty *)self.widget getPropertyType]) {
                        case 	AJCPS_BOOL_PROPERTY :
                            [str appendFormat:@"bool property, value:%d",propertyValue.boolValue];
                            
                            break;
                        case 	AJCPS_UINT16_PROPERTY :
                            [str appendFormat:@"uint16 property, value:%d",propertyValue.uint16Value];

                            break;
                        case 	AJCPS_INT16_PROPERTY :
                            [str appendFormat:@"int16 property, value:%d",propertyValue.int16Value];

                            break;
                        case 	AJCPS_UINT32_PROPERTY :
                            [str appendFormat:@"uint32 property, value:%d",propertyValue.uint32Value];

                            break;
                        case 	AJCPS_INT32_PROPERTY :
                            [str appendFormat:@"int32 property, value:%d",propertyValue.int32Value];

                            break;
                        case 	AJCPS_UINT64_PROPERTY :
                            [str appendFormat:@"uint64 property, value:%llu",propertyValue.uint64Value];

                            break;
                        case 	AJCPS_INT64_PROPERTY :
                            [str appendFormat:@"int64 property, value:%lld",propertyValue.int64Value];

                            break;
                        case 	AJCPS_DOUBLE_PROPERTY :
                            [str appendFormat:@"double property, value:%f",propertyValue.doubleValue];

                            break;
                        case 	AJCPS_STRING_PROPERTY :
                            [str appendFormat:@"string property, value:'%s'",propertyValue.charValue];

                            break;
                        case 	AJCPS_DATE_PROPERTY :
                            [str appendFormat:@"date property"]; //TODO support date

                            break;
                        case 	AJCPS_TIME_PROPERTY :
                            [str appendFormat:@"time property"]; //TODO support time

                            break;
                        default:
                            [str appendFormat:@"unknown property"];
                            break;
                    }
                    
                    
                    
                    
                    if (hint > 0 && hint < [propertyHintsStrings count]){
                        self.hintLabel.text=[NSString stringWithFormat:@"%@", propertyHintsStrings[hint]];
                    }
                    else
                        self.hintLabel.text=@"UNKNOWN";
                    
                    if(![[((AJCPSProperty *)self.widget) getUnitOfMeasure] isEqualToString:@""]) {
                        [str appendFormat:@",unitOfMeasure:%@", [((AJCPSProperty *)self.widget) getUnitOfMeasure]];
                    }

                    [str appendString:@"\n"];
                    
                    AJCPSConstraintRange *constraintRange = [((AJCPSProperty *)self.widget) getConstraintRange];
                    
                    if (constraintRange) {
                        [str appendFormat:@"min:%d,max:%d,inc:%d",[constraintRange getMinValue],[constraintRange getMaxValue],[constraintRange getIncrementValue]];
                    }
                    
                    NSArray *list = [((AJCPSProperty *)self.widget) getConstraintList];
                    
                    if ([list count]) {
                        [str appendString:@"list:"];
                    }
                    
                    for (AJCPSConstraintList *constraintList in list) {
                        [str appendFormat:@"%@",[constraintList getDisplay]];
                        
                        AJCPSConstraintValue propertyValue = [constraintList getConstraintValue];
                        
                        switch ([constraintList getPropertyType]) {
                            
                            case 	AJCPS_UINT16_PROPERTY :
                                [str appendFormat:@"(%d)",propertyValue.uint16Value];
                                
                                break;
                            case 	AJCPS_INT16_PROPERTY :
                                [str appendFormat:@"(%d)",propertyValue.int16Value];
                                
                                break;
                            case 	AJCPS_UINT32_PROPERTY :
                                [str appendFormat:@"(%d)",propertyValue.uint32Value];
                                
                                break;
                            case 	AJCPS_INT32_PROPERTY :
                                [str appendFormat:@"(%d)",propertyValue.int32Value];
                                
                                break;
                            case 	AJCPS_UINT64_PROPERTY :
                                 [str appendFormat:@"(%llu)",propertyValue.uint64Value];
                                
                                break;
                            case 	AJCPS_INT64_PROPERTY :
                                 [str appendFormat:@"(%lld)",propertyValue.int64Value];
                                
                                break;
                            case 	AJCPS_DOUBLE_PROPERTY :
                                [str appendFormat:@"(%f)",propertyValue.doubleValue];
                                
                                break;
                            case 	AJCPS_STRING_PROPERTY :
                                 [str appendFormat:@"('%s')",propertyValue.charValue];
                                
                                break;
                            default:
                                NSLog(@"unknown property");
                                break;
                        }
                        
                    }
                    
                }
                    break;
                    
                case AJCPS_CONTAINER:
                    if (hint == AJCPS_VERTICAL_LINEAR)
                        self.hintLabel.text=@"VERTICAL_LINEAR";
                    else if (hint == AJCPS_HORIZONTAL_LINEAR)
                        self.hintLabel.text=@"HORIZONTAL_LINEAR";
                    else
                        self.hintLabel.text=@"UNKNOWN";
                    
                    [self setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];//TODO

                    break;
                    
                case AJCPS_DIALOG:
                    if (hint == AJCPS_ALERTDIALOG)
                        self.hintLabel.text=@"ALERTDIALOG";
                    else
                        self.hintLabel.text=@"UNKNOWN";
                    break;
                    


                    
                    
            }
            [str appendFormat:@"%@",(i == ([hints count] - 1)) ? @"\n" : @", "];
        }
    } else {
        [str appendString:@"No hints for this widget"];
    }
    
    return str;
}

- (NSString *)widgetType
{
    
    switch ([self.widget getWidgetType]) {
        case AJCPS_CONTAINER:
        {
            return @"Container";
            
        }
            break;
            
        case AJCPS_ACTION:
        {
            return @"Action";
            
        }
            
            break;
            
        case AJCPS_ACTION_WITH_DIALOG:
        {
            return @"Action & Dialog";
        }
            break;
            
        case AJCPS_LABEL:
        {
            return @"Label";
            
        }
            break;
            
        case AJCPS_PROPERTY:
        {
            return @"Property";
        }
            break;
            
        case AJCPS_DIALOG:
        {
            return @"Dialog";
        }
            break;
            
        case AJCPS_ERROR:
        {
            return @"Error Widget";
        }
            break;
            
        default:
        {
            return @"Unsupported AJCPSWidgetType";
            
        }
            break;
    }
    
    return nil;
}

-(void)setWidget:(AJCPSWidget *)widget
{
    _widget = widget;
    self.widgetNameLabel.text  = [NSString stringWithFormat:@"%@,%@",[self.widget getLabel],[self widgetType]];

    NSString *widgetLabel;
    
    if ([[widget getLabel] length])
        widgetLabel = [widget getLabel];
    
    
    NSString *detailedText = [NSString stringWithFormat:@"label:'%@'\n%@,%@,%@\nbg:0x%x\n%@",
                              widgetLabel,
                              [widget getIsSecured] ? @"secured" : @"not secured",
                              [widget getIsEnabled] ? @"enabled" : @"not enabled",
                              [widget getIsWritable] ? @"writable" : @"not writable",
                              [widget getBgColor],
                              [self widgetHints]];
    
    if ([widget getWidgetType] == AJCPS_ERROR) {
        AJCPSWidget *originalWidget = [(AJCPSErrorWidget *)self.widget getOriginalWidget];
        
        self.hintLabel.text = [NSString stringWithFormat:@"Original Widget name:%@",[originalWidget getWidgetName]];
    }
    
    
    
    //[self printHints:widget widgetType:widgetType indent:indent];
    
    NSLog(@" widgetName:%@ widgetLabel:%@", [self.widget getLabel], widgetLabel);
    
	self.widgetDetailsLabel.text = detailedText;

}


@end
