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

/**
 AJOBOnboardingClientListener
 A helper class used by an AllJoyn IoE client application to receive OnboardingService signal notification.
The user of the class need to implement it method(s)
 */

@protocol AJOBOnboardingClientListener <NSObject>

/**
 ConnectionResultSignalReceived
 Needs to be implemented by the user for the desired behaviour when receiving the ConnectResult Signal
 @param connectionResultCode connection result code
 @param connectionResultMessage connection result message
 */
-(void)connectionResultSignalReceived:(int) connectionResultCode connectionResultMessage:(NSString*) connectionResultMessage;

@end
