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
#import "AJCPSRootWidget.h"
#import "alljoyn/controlpanel/Container.h"

/**
 * AJCPSContainer class is used to represent a container widget,
 * container widgets container children widgets and group them together.
 */
@interface AJCPSContainer : AJCPSRootWidget

- (id)initWithHandle:(ajn::services::Container *)handle;

/**
 * Register the BusObjects for this Widget
 * @param bus A reference to the AJNBusAttachment.
 * @return status - success/failure
 */
- (QStatus)registerObjects:(AJNBusAttachment *)bus;

/**
 * Unregister the BusObjects for this widget
 * @param bus A reference to the AJNBusAttachment.
 * @return status - success/failure
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus;


/**
 * Get the ChildWidget Vector
 * @return children widgets
 */
//const std : : vector <Widget *>& getChildWidgets() const;
- (NSArray *)getChildWidgets;

/**
 * Get IsDismissable
 * @return isDismissable
 */
- (bool)getIsDismissable;

@end