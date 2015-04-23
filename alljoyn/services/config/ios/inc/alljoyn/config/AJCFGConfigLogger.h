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
#import "alljoyn/services_common/AJSVCGenericLogger.h"

/**
 ConfigLogger enable application logging by a given logger.
 */
@interface AJCFGConfigLogger : NSObject

/**
 * Create a ConfigLogger Shared instance.
 * @return ConfigLogger instance(created only once).
 */
+ (id)sharedInstance;

/**
 Set the logger to be in use.
 @param logger The logger to be in used.
 </br> If the given logger is nil -  will use AJSVCGenericLoggerDefaultImpl.
 */
- (void)setLogger:(id <AJSVCGenericLogger> )logger;

/**
 Return the logger that being used.
 */
- (id <AJSVCGenericLogger> )logger;

@end
