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

#import "AJCPSControllerModel.h"
#import "AJCPSAction.h"
#import "AJCPSDialog.h"
#import "AJCPSProperty.h"
#import "AJCPSCPSDate.h"
#import "AJCPSCPSTime.h"
#import "AJCPSControlPanelDevice.h"
#import "AJCPSActionWithDialog.h"
#import "AJCPSConstraintList.h"
#import "alljoyn/about/PropertyStore.h" //for ER codes

@interface ControllerModel ()
@property (strong, nonatomic) NSMutableArray *containerStack; // array of AJCPSContainer *
@property (strong, nonatomic) NSArray *supportedLanguages; //array of NSStrings of languages

@end

@implementation ControllerModel

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSLog(@"alert button has been clicked");
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
}
- (id)init
{
	if (self = [super init]) {
		self.containerStack = [[NSMutableArray alloc]init];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(hasPasscodeInput:) name:@"hasPasscodeForBus" object:nil];
	}
	return self;
}

- (void)hasPasscodeInput:(NSNotification *)notification
{
	if ([notification.name isEqualToString:@"hasPasscodeForBus"]) {
        
        [[[UIAlertView alloc] initWithTitle:@"Authentication failed" message:@"If you've entered a new password - please press Back to reload data." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
	}
}


- (NSString *)widgetTypeToString:(AJCPSWidgetType)widgetType
{
	switch (widgetType) {
		case AJCPS_CONTAINER:
			return @"AJCPS_CONTAINER";
            
		case AJCPS_ACTION:
			return @"AJCPS_ACTION";
            
		case AJCPS_ACTION_WITH_DIALOG:
			return @"AJCPS_ACTION_WITH_DIALOG";
            
		case AJCPS_LABEL:
			return @"AJCPS_LABEL";
            
		case AJCPS_PROPERTY:
			return @"AJCPS_PROPERTY";
            
		case AJCPS_DIALOG:
			return @"AJCPS_DIALOG";
            
        case AJCPS_ERROR:
            return @"AJCPS_ERROR";
            
		default:
			return nil;
	}
    
    //  AJCPS_CONTAINER = 0,         //!< CONTAINER
    //	AJCPS_ACTION = 1,            //!< ACTION
    //	AJCPS_ACTION_WITH_DIALOG = 2, //!< ACTION_WITH_DIALOG
    //	AJCPS_LABEL = 3,             //!< LABEL
    //	AJCPS_PROPERTY = 4,          //!< PROPERTY
    //	AJCPS_DIALOG = 5             //!< DIALOG
}

- (void)loadContainer
{
	AJCPSContainer *container = self.containerStack[[self.containerStack count] - 1];
    
	[self printBasicWidget:container];
    
	// Array of AJCPSWidget objects
    
    @synchronized(self){
	self.widgetsContainer = [container getChildWidgets];
    }
	NSLog(@"Print ChildWidgets: ");
	for (NSInteger i = 0; i < [self.widgetsContainer count]; i++) {
		AJCPSWidgetType widgetType = [[self.widgetsContainer objectAtIndex:i] getWidgetType];
        NSLog(@"Print %@", [self widgetTypeToString:widgetType]);
        [self printBasicWidget:self.widgetsContainer[i]];
        
		switch (widgetType) {
			case AJCPS_CONTAINER:
			{
			}
                break;
                
			case AJCPS_ACTION:
			{
			}
                
                break;
                
			case AJCPS_ACTION_WITH_DIALOG:
			{
                
                [self printDialog:[((AJCPSActionWithDialog*)self.widgetsContainer[i]) getChildDialog]];
			}
                break;
                
			case AJCPS_LABEL:
			{
			}
                break;
                
			case AJCPS_PROPERTY:
			{
                [self printProperty:((AJCPSProperty *)self.widgetsContainer[i])];
			}
                break;
                
			case AJCPS_DIALOG:
			{
                [self printDialog:((AJCPSDialog*)self.widgetsContainer[i])];
			}
                break;
                
                
			default:
			{
				NSLog(@"Unsupported AJCPSWidgetType");
			}
                break;
		}
	}
    
	[self.delegate refreshEntries];
}

- (QStatus)loadRootWidget:(AJCPSRootWidget *)rootWidget
//- (QStatus)printRootWidget:(AJCPSContainer*) rootWidget
{
	if (!rootWidget) {
		NSLog(@"faild to load RootWidget - rootWidget is empty.");
		return ER_FAIL;
	}
	AJCPSWidgetType wType = [rootWidget getWidgetType];
	NSLog(@"---> printRootWidget: %@", [self widgetTypeToString:wType]);
    
	switch (wType) {
		case AJCPS_CONTAINER:
		{
            self.containerStack[0] = [[AJCPSContainer alloc] initWithHandle:(ajn::services::Container*)rootWidget.handle];
            [self loadContainer];
		}
            break;
            
		case AJCPS_ACTION:
		{}
            break;
            
		case AJCPS_ACTION_WITH_DIALOG:
            
		{}
            break;
            
		case AJCPS_LABEL:
		{}
            break;
            
		case AJCPS_PROPERTY:
		{}
            break;
            
		case AJCPS_DIALOG:
		{
            [self printBasicWidget:rootWidget];
            NSString* dialogLabel = [(AJCPSWidget *)rootWidget getLabel];
            AJCPSDialog* dialog = [[AJCPSDialog alloc] initWithHandle:(ajn::services::Dialog *)rootWidget.handle];
            
            uint16_t numActions = [dialog getNumActions];
            NSLog(@"AJCPS_DIALOG numActions: %hu",numActions);
            
            NSMutableString* actionsString = [[NSMutableString alloc] init];
            
            switch (numActions) {
                case 3:
                    [actionsString appendString:[NSString stringWithFormat:@"%@ ",[dialog getLabelAction3]]];
                case 2:
                    [actionsString appendString:[NSString stringWithFormat:@"%@ ",[dialog getLabelAction2]]];
                case 1:
                    [actionsString appendString:[NSString stringWithFormat:@"%@ ",[dialog getLabelAction1]]];
                    break;
                    
                default:
                    break;
            }
            
            [[[UIAlertView alloc] initWithTitle:@"Received Dialog:" message:[NSString stringWithFormat:@"%@\n%@\n%@",dialogLabel,[dialog getMessage], actionsString] delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        }
            break;
            
		default:
		{
			NSLog(@"unsupported AJCPSWidgetType");
		}
            break;
	}
    
	return ER_OK;
}

- (void)printBasicWidget:(AJCPSWidget *)widget
{
	NSLog(@"    widget name: %@", [widget getWidgetName]);
	NSLog(@"    widget version: %hu", [widget getInterfaceVersion]);
	NSLog(@"    widget is %@", [widget getIsSecured] ? @"secured" : @"not secured");
	NSLog(@"    widget is %@", [widget getIsEnabled] ? @"enabled" : @"not enabled");
	NSLog(@"    widget is %@", [widget getIsWritable] ? @"writable" : @"not writable");
    
	if ([[widget getLabel] length])
		NSLog(@"    widget label: %@", [widget getLabel]);
    
    //    if ([widget getBgColor] != UINT32_MAX)
	NSLog(@"    widget bgColor: %u", [widget getBgColor]);
}

-(void)printProperty:(AJCPSProperty *)property
{
    NSMutableString *str = [[NSMutableString alloc]init];
    AJCPSPropertyValue propertyValue;
    [property getPropertyValue:propertyValue];
    
    switch ([property getPropertyType]) {
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
    
    if(![[property getUnitOfMeasure] isEqualToString:@""]) {
        [str appendFormat:@",unitOfMeasure:%@", [property getUnitOfMeasure]];
    }
    
    [str appendString:@"\n"];
    
    AJCPSConstraintRange *constraintRange = [property getConstraintRange];
    
    if (constraintRange) {
        [str appendFormat:@"min:%d,max:%d,inc:%d",[constraintRange getMinValue],[constraintRange getMaxValue],[constraintRange getIncrementValue]];
    }
    
    NSArray *list = [property getConstraintList];
    
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
    
    NSLog(@"%@",str);
    
}

-(void) printDialog:(AJCPSDialog*) dialog
{
    NSLog(@"    Dialog message: %@", [dialog getMessage]);
    NSLog(@"    Dialog numActions: %d", [dialog getNumActions]);
    if (![[dialog getLabelAction1] isEqualToString:@""]) {
        NSLog(@"        Dialog Label for Action1: %@" , [dialog getLabelAction1]);
    }
    if (![[dialog getLabelAction2] isEqualToString:@""]) {
        NSLog(@"        Dialog Label for Action1: %@" , [dialog getLabelAction2]);
    }
    if (![[dialog getLabelAction3] isEqualToString:@""]) {
        NSLog(@"        Dialog Label for Action1: %@" , [dialog getLabelAction3]);
    }
}


#pragma mark - AJCPSControlPanelListener protocol methods
/**
 * sessionEstablished - callback when a session is established with a device
 * @param device - the device that the session was established with
 */
- (void)sessionEstablished:(AJCPSControlPanelDevice *)device
{
    //      An example of how to get all the control panels on the device:
    //    NSArray *controlpanels = [device getAllControlPanels];
    //
    //    for (AJCPSControlPanel *panel in controlpanels) {
    //        NSLog(@"%@",   [panel getPanelName]);
    //    }
    
	NSLog(@"Session has been established with device: %@", [device getDeviceBusName]);
	// Dictionary that contains AJCPSControlPanelControllerUnit's
	NSDictionary *units = [device getDeviceUnits];
    
    if (![units count])
    {
        NSLog(@"Device %@ has no units.", [device getDeviceBusName]);
        return;
    }
    
	NSString *unitsKey = units.allKeys[0];
    
	NSLog(@"Start loading unit: %@", unitsKey);
	self.unit = unitsKey; //Setting the table view titleForHeader
    
	AJCPSControlPanelControllerUnit *unitsValue = [units objectForKey:unitsKey];
	AJCPSHttpControl *httpControl = [unitsValue getHttpControl];
	if (httpControl) {
		NSLog(@"Unit has a HttpControl: ");
		NSLog(@"  HttpControl version: %hu", [httpControl getInterfaceVersion]);
		NSLog(@"  HttpControl url: %@", [httpControl getUrl]);
	}
	// Dictionary that contains AJCPSControlPanel's
	NSDictionary *controlPanels = [unitsValue getControlPanels];
	NSString *controlPanelsKey = controlPanels.allKeys[0];
    
	NSLog(@"-----> Start parsing panelName: %@", controlPanelsKey);
	self.controlPanel = [controlPanels objectForKey:controlPanelsKey];
    
	// Array of languages in an NSString format
	self.supportedLanguages = [[self.controlPanel getLanguageSet] getLanguages];
    
    if (![self.supportedLanguages count]) {
        [[[UIAlertView alloc] initWithTitle:@"Error" message:@"There is no supported language for this Container" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    } else {
        NSString *lang = self.supportedLanguages[0];
        [self populateRootContainer:lang];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
}

/**
 * sessionLost - callback when a session is lost with a device
 * @param device - device that the session was lost with
 */
- (void)sessionLost:(AJCPSControlPanelDevice *)device
{
	NSLog(@"%@] Calling: %@", [[self class] description], NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
    
	[NSThread sleepForTimeInterval:5];
    
	[device endSession];
}

/**
 * signalPropertiesChanged - callback when a property Changed signal is received
 * @param device - device signal was received from
 * @param widget - widget signal was received for
 */
- (void)signalPropertiesChanged:(AJCPSControlPanelDevice *)device widget:(AJCPSWidget *)widget
{
	NSLog(@"[%@] Calling: %@", [[self class] description], NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
    
    [self printBasicWidget:widget];
    
    [self loadContainer];
}

/**
 * signalPropertyValueChanged - callback when a property Value Changed signal is received
 * @param device - device signal was received from
 * @param property - Property signal was received for
 */
- (void)signalPropertyValueChanged:(AJCPSControlPanelDevice *)device property:(AJCPSProperty *)property
{
	NSLog(@"[%@] Calling: %@", [[self class] description], NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
    
    [self printBasicWidget:property];
    [self printProperty:property];
    
    [self loadContainer];
}

/**
 * signalDismiss - callback when a Dismiss signal is received
 * @param device - device signal was received from
 * @param notificationAction - notificationAction signal was received for
 */
- (void)signalDismiss:(AJCPSControlPanelDevice *)device notificationAction:(AJCPSNotificationAction *)notificationAction
{
	NSLog(@"[%@] Calling: %@", [[self class] description], NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });

}

/**
 * ErrorOccured - callback to tell application when something goes wrong
 * @param device - device  that had the error
 * @param status - status associated with error if applicable
 * @param transaction - the type of transaction that resulted in the error
 * @param errorMessage - a log-able error Message
 */
- (void)errorOccured:(AJCPSControlPanelDevice *)device status:(QStatus)status transaction:(AJCPSControlPanelTransaction)transaction errorMessage:(NSString *)errorMessage
{
	NSLog(@"[%@] Calling: %@", [[self class] description], NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate loadEnded];
    });
    
	NSLog(@"error message:'%@'", errorMessage);
    dispatch_async(dispatch_get_main_queue(), ^{
        [[[UIAlertView alloc] initWithTitle:@"Error" message:[NSString stringWithFormat:@"%@" ,errorMessage] delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    });
    
}

#pragma mark - util methods
- (void)pushChildContainer:(AJCPSContainer *)containerToPush
{
	self.containerStack[[self.containerStack count]] = containerToPush;
	[self loadContainer];
}

- (NSInteger)popChildContainer
{
    if([self.containerStack count] > 0) //until the first container is loaded, this array is empty
    {
        [self.containerStack removeObjectAtIndex:[self.containerStack count] - 1];
        if ([self.containerStack count] > 0) {
            [self loadContainer];
        }
    }
	return [self.containerStack count];
}



- (void)populateRootContainer:(NSString*)lang
{
	QStatus status;
    NSLog(@"---------> Now loading language: %@", lang);
	AJCPSContainer *rootContainer = (AJCPSContainer *)[self.controlPanel getRootWidget:lang];
    NSLog(@"-----> Finished loading widget: %@", [rootContainer getWidgetName]);
    status = [self loadRootWidget:rootContainer];
    
    if(ER_OK != status)
    {
        NSLog(@"Failed to load root widget");
    }
}

-(QStatus)switchLanguage:(NSString *)language
{
    QStatus status = ER_OK;
    if ([self.supportedLanguages indexOfObject:language] != NSNotFound) {
        AJCPSContainer *rootContainer = (AJCPSContainer *)[self.controlPanel  getRootWidget:language];
		NSLog(@"-----> Finished loading widget: %@", [rootContainer getWidgetName]);
		status = [self loadRootWidget:rootContainer];
        
        return status;
        
    } else {
        return ER_LANGUAGE_NOT_SUPPORTED;
    }
    
    return status;
    
}

-(QStatus)switchLanguageForNotificationAction:(AJCPSRootWidget *)rootWidget
{
    QStatus status = ER_OK;
    if (rootWidget) {
		NSLog(@"-----> Finished loading widget: %@", [rootWidget getWidgetName]);
		status = [self loadRootWidget:rootWidget];
        return status;
        
    } else {
        return ER_LANGUAGE_NOT_SUPPORTED;
    }
    
    return status;
}

-(void)setSupportedLanguagesForNotificationAction:(AJCPSNotificationAction *) notificationAction
{
    self.supportedLanguages = [[notificationAction getLanguageSet] getLanguages];
    if (![self.supportedLanguages count]) {
        NSLog(@"notification action languages is empty");
    }
}

-(NSInteger)childContainerPosition
{
    return [self.containerStack count];
}
@end
