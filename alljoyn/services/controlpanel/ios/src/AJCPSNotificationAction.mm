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

#import "AJCPSNotificationAction.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJCPSControlPanelDevice.h"

@interface AJCPSNotificationAction ()
@property (nonatomic) ajn::services::NotificationAction *handle;
@end


@implementation AJCPSNotificationAction

- (id)initWithHandle:(ajn::services::NotificationAction *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

- (NSString *)getNotificationActionName
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getNotificationActionName()];
}

/**
 * Register the BusObjects for this Widget
 * @param bus - bus used to register the busObjects
 * @return status - success/failure
 */
- (QStatus)registerObjects:(AJNBusAttachment *)bus
{
	return self.handle->registerObjects((ajn::BusAttachment *)[bus handle]);
}

/**
 * Unregister the BusObjects of the NotificationAction class
 * @param bus - bus used to unregister the objects
 * @return status - success/failure
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus
{
	return self.handle->unregisterObjects((ajn::BusAttachment *)[bus handle]);
}

/**
 * Get the LanguageSet of the NotificationAction
 * @return
 */
- (AJCPSLanguageSet *)getLanguageSet
{
	return [[AJCPSLanguageSet alloc] initWithHandle:(ajn::services::LanguageSet *)&self.handle->getLanguageSet()];
}

/**
 * Get the Device of the NotificationAction
 * @return controlPanelDevice
 */
- (AJCPSControlPanelDevice *)getDevice
{
	return [[AJCPSControlPanelDevice alloc]initWithHandle:self.handle->getDevice()];
}

/**
 * Get the objectPath
 * @return
 */
- (NSString *)getObjectPath
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}

/**
 * Get the RootWidget of the NotificationAction
 * @param Language - languageSet of RootWidget to retrieve
 * @return rootWidget
 */
- (AJCPSRootWidget *)getRootWidget:(NSString *)Language
{
	return [[AJCPSRootWidget alloc]initWithHandle:self.handle->getRootWidget([AJNConvertUtil convertNSStringToQCCString:Language])];
}

@end
