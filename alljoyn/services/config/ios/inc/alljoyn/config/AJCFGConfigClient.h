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
#import "alljoyn/services_common/AJSVCGenericLogger.h"

/**
 AJCFGConfigClient is a helper class used by an AllJoyn IoE client application to communicate with ConfigService that implements the org.alljoyn.Config .
 */
@interface AJCFGConfigClient : NSObject

/**
 Designated initializer.
 Create a AJCFGConfigClient Object using the passed AJNBusAttachment.
 @param bus A reference to the AJNBusAttachment.
 @return AJCFGConfigClient if successful.
 */
- (id)initWithBus:(AJNBusAttachment *)bus;

/**
 Factory reset remote method call using the passed bus name.
 @param busName Unique or well-known name of AllJoyn bus.
 @return ER_OK if successful.
 */
- (QStatus)factoryResetWithBus:(NSString *)busName;

/**
 Factory reset remote method call using the passed bus name and session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)factoryResetWithBus:(NSString *)busName sessionId:(AJNSessionId)sessionId;

/**
 Restart remote method call using the passed bus name.
 @param busName Unique or well-known name of AllJoyn bus.
 @return ER_OK if successful.
 */
- (QStatus)restartWithBus:(NSString *)busName;

/**
 Restart remote method call using the passed bus name and session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)restartWithBus:(NSString *)busName sessionId:(AJNSessionId)sessionId;

/**
 Set pass code remote method call.
 @param busName Unique or well-known name of AllJoyn bus.
 @param daemonRealm The new DaemonRealm.
 @param newPasscodeSize The new pass code size.
 @param newPasscode The new pass code.
 @return ER_OK if successful.
 */
- (QStatus)setPasscodeWithBus:(NSString *)busName daemonRealm:(NSString *)daemonRealm newPasscodeSize:(size_t)newPasscodeSize newPasscode:(const uint8_t *)newPasscode;

/**
 Set pass code remote method call using a session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param daemonRealm The new DaemonRealm .
 @param newPasscodeSize The new pass code size.
 @param newPasscode The new pass code.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)setPasscodeWithBus:(NSString *)busName daemonRealm:(NSString *)daemonRealm newPasscodeSize:(size_t)newPasscodeSize newPasscode:(const uint8_t *)newPasscode sessionId:(AJNSessionId)sessionId;

/**
 Get configurations remote method call.
 @param busName Unique or well-known name of AllJoyn bus.
 @param languageTag The language used to pull the data by.
 @param configs A reference to configurations filled by the method.
 @return ER_OK if successful.
 */
- (QStatus)configurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configs:(NSMutableDictionary **)configs;

/**
 Get configurations remote method call using a session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param languageTag The language used to pull the data by. use @"" to get the current default language.
 @param configs A reference to configurations filled by the method.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)configurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configs:(NSMutableDictionary **)configs sessionId:(AJNSessionId)sessionId;

/**
 Update configurations remote method call.
 @param busName Unique or well-known name of. AllJoyn bus.
 @param languageTag The language used to update the data by.
 @param configs A reference to configurations to be used by the method.
 @return ER_OK if successful.
 */
- (QStatus)updateConfigurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configs:(NSMutableDictionary **)configs;

/**
 Update configurations remote method call using a session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param languageTag The language used to update the data by.
 @param configs A reference to configurations to be used by the method.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)updateConfigurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configs:(NSMutableDictionary **)configs sessionId:(AJNSessionId)sessionId;

/**
 Delete configurations remote method call.
 @param busName Unique or well-known name of AllJoyn bus.
 @param languageTag languageTag is the language used to update the data by
 @param configNames A reference to configurations to be used by the method.
 @return ER_OK if successful.
 */
- (QStatus)resetConfigurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configNames:(NSMutableArray *)configNames;

/**
 Delete configurations remote method call using a session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param languageTag languageTag is the language used to update the data by
 @param configNames A reference to configurations to be used by the method.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)resetConfigurationsWithBus:(NSString *)busName languageTag:(NSString *)languageTag configNames:(NSMutableArray *)configNames sessionId:(AJNSessionId)sessionId;

/**
 Get version remote method call.
 @param busName Unique or well-known name of AllJoyn bus.
 @param version A reference to be filled by the method.
 @return ER_OK if successful.
 */
- (QStatus)versionWithBus:(NSString *)busName version:(int&)version;

/**
 Get version remote method call using a session id.
 @param busName Unique or well-known name of AllJoyn bus.
 @param version A reference to be filled by the method.
 @param sessionId The session received after joining alljoyn session.
 @return ER_OK if successful.
 */
- (QStatus)versionWithBus:(NSString *)busName version:(int&)version sessionId:(AJNSessionId)sessionId;

#pragma mark - Logger methods
/**
 Receive AJSVCGenericLogger to use for logging.
 @param logger Implementation of AJSVCGenericLogger.
 @return previous logger
 */
- (void)setLogger:(id <AJSVCGenericLogger> )logger;

/**
 Get the currently-configured logger implementation
 @return logger Implementation of AJSVCGenericLogger.
 */
- (id <AJSVCGenericLogger> )logger;

/**
 Set log level filter for subsequent logging messages.
 @param newLogLevel New log level enum value.
 @return logLevel Log level enum value that was in effect prior to this change.
 */
- (void)setLogLevel:(QLogLevel)newLogLevel;

/**
 Get log level filter value currently in effect.
 @return logLevel Log level enum value currently in effect.
 */
- (QLogLevel)logLevel;

@end
