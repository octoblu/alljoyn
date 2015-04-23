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
#import "AJOBSOnboarding.h"
#import "AJOBOnboardingClientListener.h"

/**
 * AJOBSOnboardingClient class
 */
@interface AJOBSOnboardingClient : NSObject

/**
 * Constructor of OnboardingClient
 * @param bus is a reference to BusAttachment
 * @param listener Onboarding connevtion signal listener
 */
-(id)initWithBus:(AJNBusAttachment*) bus listener:(id <AJOBOnboardingClientListener>) listener;

/**
 * Configure WiFi a specified bus name.
 * @param busName busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param ajOBInfo reference to  OBInfo.
 * @param resultStatus the status of the request [out].
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)configureWiFi:(NSString*) busName obInfo:(AJOBInfo&) ajOBInfo resultStatus:(short&) resultStatus sessionId:(AJNSessionId) sessionId;

/**
 * Configure WiFi a specified bus name.
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param ajOBInfo reference to  OBInfo
 * @param resultStatus the status of the request [out]
 * @return ER_OK if successful.
 */
-(QStatus)configureWiFi:(NSString*) busName obInfo:(AJOBInfo&) ajOBInfo resultStatus:(short&) resultStatus;

/**
 * Connect WiFi a specified bus name.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)connectTo:(NSString*) busName  sessionId:(AJNSessionId) sessionId;

/**
 * Connect WiFi a specified bus name.
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @return ER_OK if successful.
 */
-(QStatus)connectTo:(NSString*) busName ;

/**
 * Offboard WiFi a specified bus name.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)offboardFrom:(NSString*) busName  sessionId:(AJNSessionId) sessionId;


/**
 * Offboard WiFi a specified bus name.
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @return ER_OK if successful.
 */
-(QStatus)offboardFrom:(NSString*) busName ;

/**
 * GetVersion retrieves the Onboarding version
 * @param busName  Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param version [out].
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)version:(NSString*) busName  version:(int&) version sessionId:(AJNSessionId) sessionId;

/**
 * GetVersion retrieves the Onboarding version
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName  Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param version [out].
 * @return ER_OK if successful.
 */
-(QStatus)version:(NSString*) busName  version:(int&) version;

/**
 * GetState retrieves  the last state of the WIFI connection attempt
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param state [out].
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)state:(NSString*) busName  state:(short&) state sessionId:(AJNSessionId) sessionId;

/**
 * GetState retrieves  the last state of the WIFI connection attempt
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param state [out]
 * @return ER_OK if successful.
 */
-(QStatus)state:(NSString*) busName  state:(short&) state;

/**
 * GetLastError retrieves the last error of WIFI connection attempt
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param lastError [out].
 * @param sessionId the session received  after joining alljoyn session
 * @return ER_OK if successful.
 */
-(QStatus)lastError:(NSString*) busName lastError:(AJOBLastError&) lastError sessionId:(AJNSessionId) sessionId;

/**
 * GetLastError retrieves the last error of WIFI connection attempt
 * Since no sessionId has been specified - the service will find an existing route to end point rather than access directly.
 * @param busName Unique or well-known name of AllJoyn node to retrieve Onboarding data from.
 * @param lastError [out].
 * @return ER_OK if successful.
 */
-(QStatus)lastError:(NSString*) busName lastError:(AJOBLastError&) lastError;

@end
