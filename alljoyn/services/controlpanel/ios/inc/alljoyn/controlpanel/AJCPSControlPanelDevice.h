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
#import "alljoyn/Status.h"
#import "AJNSessionOptions.h"
#import "alljoyn/controlpanel/ControlPanelDevice.h"
#import "AJCPSControlPanelControllerUnit.h"
#import "AJCPSNotificationAction.h"
#import "AJCPSControlPanelListener.h"

/**
 * AJCPSControlPanelDevice
 */
@interface AJCPSControlPanelDevice : NSObject
/**
 * Constructor for ControlPanelDevice
 * @param handle handle to the instance
 */
- (id)initWithHandle:(ajn ::services ::ControlPanelDevice *)handle;

/**
 * startSessionAsync - start a session with Device Asynchronously
 * @return status - success/failure
 */
- (QStatus)startSessionAsync;

/**
 * startSession - start session with device synchronously
 * @return status - success/failure
 */
- (QStatus)startSession;

/**
 * endSession - endSession with device
 * @return status - success/failure
 */
- (QStatus)endSession;

/**
 * ShutDown device - end Session and release units
 * @return status - success/failure
 */
- (QStatus)shutdownDevice;

/**
 * getDeviceBusName - get the busName of the device
 * @return deviceBusName - busName of device
 */
- (NSString *)getDeviceBusName;

/**
 * getSessionId - get the SessionId of the remote Session with device
 * @return const ajn::SessionId
 */
- (AJNSessionId)getSessionId;

/**
 * getDeviceUnits
 * @return the ControlPanelUnits of this Device
 */
// const std::map<qcc::String, ControlPanelControllerUnit*>& getDeviceUnits const;

- (NSDictionary *)getDeviceUnits;

/**
 * getAllControlPanels - returns an array with all controlPanels contained by this device
 * @return an array with all the controlPanel defined as children of this device
 */
- (NSArray *)getAllControlPanels;

/**
 * Get an existing unit using the objectPath
 * @param objectPath - objectPath of unit
 * @return ControlPanelUnit pointer
 */
- (AJCPSControlPanelControllerUnit *)getControlPanelUnit:(NSString *)objectPath;

/**
 * addControlPanelUnit - add a ControlPanel unit using the objectPath and interfaces passed in
 * @param objectPath - objectPath received in the announce
 * @param interfaces - interfaces received in the announce
 * @return ControlPanelUnit pointer
 */

- (AJCPSControlPanelControllerUnit *)addControlPanelUnit:(NSString *)objectPath interfaces:(NSArray *)interfaces;

/**
 * addNotificationAction - add a Notification using an objectPath received in a notification
 * @param objectPath - objectPath used to create the NotificationAction
 * @return NotificationAction pointer
 */
- (AJCPSNotificationAction *)addNotificationAction:(NSString *)objectPath;

/**
 * Delete and shutdown the NotificationAction
 * @param notificationAction - notificationAction to remove
 * @return status - success/failure
 */
- (QStatus)removeNotificationAction:(AJCPSNotificationAction *)notificationAction;

/**
 * Get the Listener defined for this SessionHandler
 * @return
 */
- (id <AJCPSControlPanelListener> )getListener;

/**
 * Set the Listener defined for this SessionHandler
 * @param listener AJCPSControlPanelListener that will receive session/signal events notifications.
 * @return status - success/failure
 */
- (QStatus)setListener:(id <AJCPSControlPanelListener> )listener;

@property (nonatomic, readonly)ajn::services::ControlPanelDevice * handle;

@end