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

/**
 AJCFGConfigServiceListener is a helper protocol used by AJCFGConfigServiceListenerImplAdapter to receive ConfigService calls.
 The user of the class need to implement the protocol methods.
 */
@protocol AJCFGConfigServiceListener <NSObject>

@required
/**
 Application should implement restart of the device.
 @return ER_OK if successful.
 */
- (QStatus)restart;

/**
 Application should implement factoryReset of the device ,return to default values including password!
 @return ER_OK if successful.
 */
- (QStatus)factoryReset;

/**
 Application should receive Passphrase info and persist it.
 @param daemonRealm Daemon realm to persist.
 @param passcode passcode content.
 @return ER_OK if successful.
 */
- (QStatus)setPassphrase:(NSString *)daemonRealm withPasscode:(NSString *)passcode;

@end
