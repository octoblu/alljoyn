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
#import "alljoyn/config/AJCFGConfigServiceListener.h"
#import "alljoyn/config/AJCFGPropertyStoreImpl.h"

/**
 ConfigServiceListener is the sample implementation. it creates and initialize a Config service Listener to handle Config service callbacks.
 */
@interface ConfigServiceListener : NSObject <AJCFGConfigServiceListener>

/**
 Designated initializer.
 Create a ConfigServiceListener Object using the passed propertyStore and AJNBusAttachment.
 @param propertyStore A reference to a property store.
 @param bus A reference to the AJNBusAttachment.
 @return ConfigServiceListener if successful.
 */
- (id)initWithPropertyStore:(AJCFGPropertyStoreImpl *)propertyStore andBus:(AJNBusAttachment *)bus;

/**
 Restart of the device - method not implemented.
 @return ER_OK if successful.
 */
- (QStatus)restart;

/**
 Factory reset of the device - return to default values including password!
 @return ER_OK if successful.
 */
- (QStatus)factoryReset;

/**
 Receive Passphrase info and persist it.
 @param daemonRealm Daemon realm to persist.
 @param passcode passcode content.
 @return ER_OK if successful.
 */
- (QStatus)setPassphrase:(NSString *)daemonRealm withPasscode:(NSString *)passcode;


@end
