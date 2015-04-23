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
#import "alljoyn/controlpanel/ActionWithDialog.h"
#import "AJCPSWidget.h"
#import "AJCPSDialog.h"

/**
 * AJCPSActionWithDialog is used to display a Button.
 * Upon pressing the button a Dialog is displayed on the Controller side.
 */
@interface AJCPSActionWithDialog : AJCPSWidget

- (id)initWithHandle:(ajn ::services ::ActionWithDialog *)handle;


/**
 * Get the ChildDialog of the Action
 * @return dialog
 */
- (AJCPSDialog *)getChildDialog;

/**
 * Unregister the BusObjects for this and children Widgets
 * @param bus - the bus used to unregister the busObjects
 * @return status - success/failure
 */
- (QStatus)unregisterObjects:(AJNBusAttachment *)bus;

@end
