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
#import "alljoyn/controlpanel/Dialog.h"
#import "AJCPSRootWidget.h"

/**
 * AJCPSDialog is used to display a Dialog.
 */
@interface AJCPSDialog : AJCPSRootWidget

- (id)initWithHandle:(ajn ::services ::Dialog *)handle;


/**
 * Get the Number of Actions in the Dialog
 * @return Number of Actions
 */
- (uint16_t)getNumActions;

/**
 * Get the Message of the Dialog
 * @return the message
 */
- (NSString *)getMessage;

/**
 * Get the LabelAction1 of the Dialog
 * @return the message
 */
- (NSString *)getLabelAction1;

/**
 * Get the LabelAction2 of the Dialog
 * @return the message
 */
- (NSString *)getLabelAction2;

/**
 * Get the LabelAction3 of the Dialog
 * @return the message
 */
- (NSString *)getLabelAction3;

/**
 * Call to execute this Dialog's Action 1 remotely
 * @return status - success/failure
 */
- (QStatus)executeAction1;

/**
 * Call to execute this Dialog's Action 2 remotely
 * @return status - success/failure
 */
- (QStatus)executeAction2;

/**
 * Call to execute this Dialog's Action 3 remotely
 * @return status - success/failure
 */
- (QStatus)executeAction3;



@end
