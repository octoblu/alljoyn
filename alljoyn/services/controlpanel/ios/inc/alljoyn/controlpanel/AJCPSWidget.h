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
#import <AJNBusAttachment.h>

#import "AJCPSLanguageSet.h"
#import "AJCPSControlPanelEnums.h"
#import "AJCPSControlPanelEnums.h"
#import "alljoyn/controlpanel/Widget.h"

@class AJCPSControlPanelDevice;

/**
 * AJCPSWidget is a base class for all Widgets.
 */
@interface AJCPSWidget : NSObject

- (id)initWithHandle:(ajn ::services ::Widget *)handle;

/**
 * Get the widgetType of the Widget
 * @return widgetType
 */
- (AJCPSWidgetType)getWidgetType;

/**
 * Get the name of the Widget
 * @return widgetName
 */
- (NSString *)getWidgetName;

/**
 * Get the mode of the Widget
 * @return controlPanelMode
 */
- (AJCPSControlPanelMode)getControlPanelMode;

/**
 * Get this widget's RootWidget
 * @return rootWidget
 */
- (const AJCPSWidget *)getRootWidget;

/**
 * Get the Device of the widget
 * @return device
 */
- (const AJCPSControlPanelDevice *)getDevice;

/**
 * Get the Interface Version of the Widget
 * @return interface Version
 */
- (const uint16_t)getInterfaceVersion;

/**
 * Get the isSecured boolean
 * @return true/false
 */
- (bool)getIsSecured;

/**
 * Get IsEnabled boolean
 * @return true/false
 */
- (bool)getIsEnabled;

/**
 * Get IsWritable boolean
 * @return true/false
 */
- (bool)getIsWritable;

/**
 * Get the States of the Widget
 * @return states
 */
- (uint32_t)getStates;

/**
 * Get the BgColor of the Widget
 * @return bgColor
 */
- (uint32_t)getBgColor;

/**
 * Get the Label of the Widget
 * @return label
 */
- (NSString *)getLabel;

/**
 * Get the array of Hints for the Widget
 * @return Array of hints
 */
//    const std::vector<uint16_t>& getHints;
- (NSArray *)getHints;

/**
 * Register the BusObjects for this Widget
 * @param bus The bus used to register the busObject
 * @param objectPath The objectPath of the busObject
 * @return ER_OK if successful.
 */
- (QStatus)registerObjects:(AJNBusAttachment *)bus atObjectPath:(NSString *)objectPath;

/**
 * Refresh the Widget
 * @param bus bus used for refreshing the object
 * @return ER_OK if successful.
 */
- (QStatus)refreshObjects:(AJNBusAttachment *)bus;

/**
 * Unregister the BusObjects for this widget
 * @param bus A reference to the AJNBusAttachment.
 * @return ER_OK if successful.
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus;

@property (nonatomic, readonly)ajn::services::Widget * handle;

@end