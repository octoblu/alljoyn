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

#import "AJCPSControlPanelControllerUnit.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJCPSControlPanelDevice.h"
#import "AJCPSNotificationAction.h"
#import "AJCPSHttpControl.h"
#import "AJCPSControlPanel.h"

@interface AJCPSControlPanelControllerUnit ()

@property (nonatomic) ajn::services::ControlPanelControllerUnit *handle;

@end


@implementation AJCPSControlPanelControllerUnit

- (id)initWithHandle:(ajn::services::ControlPanelControllerUnit *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

- (id)initControlPanelControllerUnit:(NSString *)unitName device:(AJCPSControlPanelDevice *)device
{
	self = [super init];
	if (self) {
		self.handle = new ajn::services::ControlPanelControllerUnit([AJNConvertUtil convertNSStringToQCCString:unitName], [device handle]);
	}
	return self;
}

- (QStatus)addHttpControl:(NSString *)objectPath
{
	return self.handle->addHttpControl([AJNConvertUtil convertNSStringToQCCString:objectPath]);
}

- (QStatus)addControlPanel:(NSString *)objectPath panelName:(NSString *)panelName
{
	return self.handle->addControlPanel([AJNConvertUtil convertNSStringToQCCString:objectPath], [AJNConvertUtil convertNSStringToQCCString:panelName]);
}

- (QStatus)addNotificationAction:(NSString *)objectPath actionName:(NSString *)actionName
{
	return self.handle->addNotificationAction([AJNConvertUtil convertNSStringToQCCString:objectPath], [AJNConvertUtil convertNSStringToQCCString:actionName]);
}

- (QStatus)removeNotificationAction:(NSString *)actionName
{
	return self.handle->removeNotificationAction([AJNConvertUtil convertNSStringToQCCString:actionName]);
}

- (QStatus)registerObjects
{
	return self.handle->registerObjects();
}

- (QStatus)shutdownUnit
{
	return self.handle->shutdownUnit();
}

- (AJCPSControlPanelDevice *)getDevice
{
	return [[AJCPSControlPanelDevice alloc] initWithHandle:self.handle->getDevice()];
}

- (NSString *)getUnitName
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getUnitName()];
}

- (NSDictionary *)getControlPanels
{
	const std::map <qcc::String, ajn::services::ControlPanel *> cpp_ControlPanels = self.handle->getControlPanels();
    
	NSMutableDictionary *controlPanels = [[NSMutableDictionary alloc]init];
    
	for (std::map <qcc::String, ajn::services::ControlPanel *>::const_iterator itr = cpp_ControlPanels.begin(); itr != cpp_ControlPanels.end(); itr++) {
		NSString *key = [AJNConvertUtil convertQCCStringtoNSString:itr->first];
		AJCPSControlPanel *value = [[AJCPSControlPanel alloc] initWithHandle:itr->second];
        
		[controlPanels setObject:value forKey:key];
	}
    
	return controlPanels;
}

- (NSDictionary *)getNotificationActions
{
	const std::map <qcc::String, ajn::services::NotificationAction *> cpp_NotificationActions = self.handle->getNotificationActions();
    
	NSMutableDictionary *notificationActions = [[NSMutableDictionary alloc]init];
    
	for (std::map <qcc::String, ajn::services::NotificationAction *>::const_iterator itr = cpp_NotificationActions.begin(); itr != cpp_NotificationActions.end(); itr++) {
		NSString *key = [AJNConvertUtil convertQCCStringtoNSString:itr->first];
		AJCPSNotificationAction *value = [[AJCPSNotificationAction alloc] initWithHandle:itr->second];
        
		[notificationActions setObject:value forKey:key];
	}
    
	return notificationActions;
}

- (AJCPSControlPanel *)getControlPanel:(NSString *)panelName
{
	return [[AJCPSControlPanel alloc]initWithHandle:self.handle->getControlPanel([AJNConvertUtil convertNSStringToQCCString:panelName])];
}

- (AJCPSNotificationAction *)getNotificationAction:(NSString *)actionName
{
	return [[AJCPSNotificationAction alloc]initWithHandle:self.handle->getNotificationAction([AJNConvertUtil convertNSStringToQCCString:actionName])];
}

- (AJCPSHttpControl *)getHttpControl
{
    if (!self.handle->getHttpControl()) {
        return nil;
    }
	return [[AJCPSHttpControl alloc] initWithHandle:self.handle->getHttpControl()];
}

@end