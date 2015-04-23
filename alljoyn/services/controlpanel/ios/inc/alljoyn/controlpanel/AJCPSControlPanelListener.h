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
#import "AJCPSProperty.h"

@class AJCPSControlPanelDevice;
@class AJCPSNotificationAction;

/**
 * AJCPSControlPanelListener protocol
 */
@protocol AJCPSControlPanelListener <NSObject>

/**
 * sessionEstablished - callback when a session is established with a device
 * @param device - the device that the session was established with
 */
- (void)sessionEstablished:(AJCPSControlPanelDevice *)device;

/**
 * sessionLost - callback when a session is lost with a device
 * @param device - device that the session was lost with
 */
- (void)sessionLost:(AJCPSControlPanelDevice *)device;

/**
 * signalPropertiesChanged - callback when a property Changed signal is received
 * @param device - device signal was received from
 * @param widget - widget signal was received for
 */
- (void)signalPropertiesChanged:(AJCPSControlPanelDevice *)device widget:(AJCPSWidget *)widget;

/**
 * signalPropertyValueChanged - callback when a property Value Changed signal is received
 * @param device - device signal was received from
 * @param property - Property signal was received for
 */
- (void)signalPropertyValueChanged:(AJCPSControlPanelDevice *)device property:(AJCPSProperty *)property;

/**
 * signalDismiss - callback when a Dismiss signal is received
 * @param device - device signal was received from
 * @param notificationAction - notificationAction signal was received for
 */
- (void)signalDismiss:(AJCPSControlPanelDevice *)device notificationAction:(AJCPSNotificationAction *)notificationAction;

/**
 * ErrorOccured - callback to tell application when something goes wrong
 * @param device - device  that had the error
 * @param status - status associated with error if applicable
 * @param transaction - the type of transaction that resulted in the error
 * @param errorMessage - a log-able error Message
 */
- (void)errorOccured:(AJCPSControlPanelDevice *)device status:(QStatus)status transaction:(AJCPSControlPanelTransaction)transaction errorMessage:(NSString *)errorMessage;

@end