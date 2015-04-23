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
#import "AJNBusAttachment.h"
#import "alljoyn/controlpanel/ControlPanel.h"
#import "AJCPSLanguageSet.h"
#import "AJCPSControlPanelDevice.h"
#import "AJCPSContainer.h"

/**
 * AJCPSControlPanel Class used to create a ControlPanel.
 * ControlPanels are made up of a LanguageSet and a RootContainer
 */
@interface AJCPSControlPanel : NSObject

- (id)initWithHandle:(ajn ::services ::ControlPanel *)handle;

/**
 * Get the name of the Panel - the name of the rootWidget
 * @return name of the Panel
 */
- (NSString *)getPanelName;

/**
 * Register the BusObjects for this Widget
 * @param bus - bus used to register the busObjects
 * @return status - success/failure
 */
- (QStatus)registerObjects:(AJNBusAttachment *)bus;

/**
 * Unregister the BusObjects for this Widget
 * @param bus - bus used to unregister the busObjects
 * @return status - success/failure
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus;

/**
 * Get the LanguageSet of the ControlPanel
 * @return
 */
- (AJCPSLanguageSet *)getLanguageSet;

/**
 * Get the Device of the ControlPanel
 * @return controlPanelDevice
 */
- (AJCPSControlPanelDevice *)getDevice;

/**
 * Get the objectPath
 * @return
 */
- (NSString *)getObjectPath;

/**
 * Get the RootWidget of the ControlPanel
 * @param Language the language to use for the action can be NULL meaning default.
 * @return pointer to rootWidget
 */
- (AJCPSContainer *)getRootWidget:(NSString *)Language;

@end