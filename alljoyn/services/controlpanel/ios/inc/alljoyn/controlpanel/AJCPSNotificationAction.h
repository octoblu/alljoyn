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
#import "alljoyn/controlpanel/NotificationAction.h"
#import "AJCPSLanguageSet.h"
#import "AJCPSRootWidget.h"

/**
 * AJCPSNotificationAction
 */
@interface AJCPSNotificationAction : NSObject

- (id)initWithHandle:(ajn ::services ::NotificationAction *)handle;


/**
 * Get the name of the NotificationAction - the name of the rootWidget
 * @return name of the NotificationAction
 */
- (NSString *)getNotificationActionName;

/**
 * Register the BusObjects for this Widget
 * @param bus - bus used to register the busObjects
 * @return status - success/failure
 */
- (QStatus)registerObjects:(AJNBusAttachment *)bus;

/**
 * Unregister the BusObjects of the NotificationAction class
 * @param bus - bus used to unregister the objects
 * @return status - success/failure
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus;

/**
 * Get the LanguageSet of the NotificationAction
 * @return
 */
- (AJCPSLanguageSet *)getLanguageSet;

/**
 * Get the Device of the NotificationAction
 * @return controlPanelDevice
 */
- (AJCPSControlPanelDevice *)getDevice;

/**
 * Get the objectPath
 * @return
 */
- (NSString *)getObjectPath;

/**
 * Get the RootWidget of the NotificationAction
 * @param Language - languageSet of RootWidget to retrieve
 * @return rootWidget
 */
- (AJCPSRootWidget *)getRootWidget:(NSString *)Language;


@property (nonatomic, readonly)ajn::services::NotificationAction * handle;

@end
