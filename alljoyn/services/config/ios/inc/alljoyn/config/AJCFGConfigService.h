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
#import "AJNBusAttachment.h"
#import "AJNBusObject.h"
#import "AJCFGConfigServiceListenerImpl.h"
#import "AJCFGPropertyStoreImpl.h"
#import "alljoyn/services_common/AJSVCGenericLogger.h"

/**
 AJCFGConfigService is an AllJoyn BusObject that implements the org.alljoyn.Config standard interface.
 Applications that provide AllJoyn IoE services with config capability.
 */
@interface AJCFGConfigService : AJNBusObject

/**
 Designated initializer.
 Create a AJCFGConfigService Object.
 @param bus A reference to the AJNBusAttachment.
 @param propertyStore A reference to a property store.
 @param listener A reference to a Config service listener.
 @return AJCFGConfigService if successful.
 */
- (id)initWithBus:(AJNBusAttachment *)bus propertyStore:(AJCFGPropertyStoreImpl *)propertyStore listener:(AJCFGConfigServiceListenerImpl *)listener;

/**
 Register the ConfigService on the alljoyn bus.
 @return ER_OK if successful.
 */
- (QStatus)registerService;

/**
 * Unregister the AJCFGConfigService on the alljoyn bus.
 */
- (void)unregisterService;

#pragma mark - Logger methods
/**
 Receive AJSVCGenericLogger to use for logging.
 @param logger Implementation of AJSVCGenericLogger.
 @return previous logger.
 */
- (void)setLogger:(id <AJSVCGenericLogger> )logger;

/**
 Get the currently-configured logger implementation.
 @return logger Implementation of AJSVCGenericLogger.
 */
- (id <AJSVCGenericLogger> )logger;

/**
 Set log level filter for subsequent logging messages.
 @param newLogLevel New log level enum value.
 @return logLevel New log level enum value that was in effect prior to this change.
 */
- (void)setLogLevel:(QLogLevel)newLogLevel;

/**
 Get log level filter value currently in effect.
 @return logLevel Log level enum value currently in effect.
 */
- (QLogLevel)logLevel;

@end
