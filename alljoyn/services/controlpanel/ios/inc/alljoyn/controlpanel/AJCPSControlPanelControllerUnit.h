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
#import "alljoyn/controlpanel/ControlPanelControllerUnit.h"
#import "AJCPSHttpControl.h"


@class AJCPSControlPanelDevice;
@class AJCPSControlPanel;
@class AJCPSNotificationAction;

/**
 * AJCPSControlPanelControllerUnit class represents a ControlPanel Unit.
 */
@interface AJCPSControlPanelControllerUnit : NSObject

- (id)initWithHandle:(ajn ::services ::ControlPanelControllerUnit *)handle;

/**
 * ControlPanelUnit
 * @param unitName the unit name.
 * @param device the control panel device object.
 */
- (id)initControlPanelControllerUnit:(NSString *)unitName device:(AJCPSControlPanelDevice *)device;

/**
 * add a HttpControl
 * @param objectPath of HTTPControl.
 * @return ER_OK if successful.
 */
- (QStatus)addHttpControl:(NSString *)objectPath;

/**
 * addControlPanel
 * @param objectPath the control panel object path.
 * @param panelName the control panel name.
 * @return ER_OK if successful.
 */
- (QStatus)addControlPanel:(NSString *)objectPath panelName:(NSString *)panelName;

/**
 * addNotificationAction to controlpanel unit
 * @param objectPath the objectpath of the notification action
 * @param actionName the actionName parsed from the objectpath
 * @return ER_OK if successful.
 */
- (QStatus)addNotificationAction:(NSString *)objectPath actionName:(NSString *)actionName;

/**
 * remove a Notification Action
 * @param actionName the action name.
 * @return ER_OK if successful.
 */
- (QStatus)removeNotificationAction:(NSString *)actionName;

/**
 * Fill control panels and the HTTPControl
 * @return ER_OK if successful.
 */
- (QStatus)registerObjects;

/**
 * Called when shutting down device
 * @return ER_OK if successful.
 */
- (QStatus)shutdownUnit;

/**
 * getDevice
 * @return ControlPanelDevice*
 */
- (AJCPSControlPanelDevice *)getDevice;

/**
 * getUnitName
 * @return string
 */
- (NSString *)getUnitName;

/**
 * Get the ControlPanels of the Unit
 * @return controlPanels map
 */
//    const std::map<qcc::String, ControlPanel*>& getControlPanels;
- (NSDictionary *)getControlPanels;

/**
 * Get the NotificationActions of the Unit
 * @return NotificationActions map
 */
//    const std::map<qcc::String, NotificationAction*>& getNotificationActions;
- (NSDictionary *)getNotificationActions;

/**
 * Get a ControlPanel of the Unit
 * @param panelName - name of the Panel to get
 * @return ControlPanel or NULL if it doesn't' exist
 */
- (AJCPSControlPanel *)getControlPanel:(NSString *)panelName;

/**
 * Get a NotificationAction of the Unit
 * @param actionName - name of the NotificaitonAction to get
 * @return NotificationAction or NULL if it doesn't exist
 */
- (AJCPSNotificationAction *)getNotificationAction:(NSString *)actionName;

/**
 * Get the HttpControl of the Unit
 * @return httpControl
 */
- (AJCPSHttpControl *)getHttpControl;

@property (nonatomic, readonly)ajn::services::ControlPanelControllerUnit * handle;

@end