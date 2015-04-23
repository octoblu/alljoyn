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
#import "alljoyn/onboarding/Onboarding.h"
#import "AJOBEnums.h"

#define MAX_SSID_SIZE 32
#define MIN_PSK_SIZE 8
#define MAX_PSK_SIZE 64

#define BIT_MASK 0x00FF

/**
 * AJOBSOnboarding class
 */
@interface AJOBSOnboarding: NSObject



/**
 *	Structure to hold the WIFI details
 */
typedef struct {
    /**
     *	SSID is the WIFI's SSID
     */
    NSString *SSID;
    /**
     *  state is current state
     */
    AJOBState state;
    /**
     *  WIFI authentication type
     */
    AJOBAuthType authType;
    /**
     * WIFI passcode in hex
     */
    NSString *passcode;
} AJOBInfo;

/**
 * Structure to hold the Last error received from the WIFI connection
 */
typedef struct {
    /**
     *	last validated state.
     */
    short validationState;
    /**
     *  error message
     */
    NSString *message;
}AJOBLastError;

/**
 * Convert passcode to Hex format.
 * @param passcode passcode to be covert.
 * @return Passcode in Hex format
 */
+(NSString*)passcodeToHex:(NSString*) passcode;

/**
 * Passcode validation for WEP authentication
 * @param key The passcode key
 * @return bool
 */
+(bool)isValidWEPKey:(NSString *) key;

/**
 * Passcode validation for WPA authentication
 * @param key The passcoce key
 * @return bool
 */
+(bool)isValidWPAKey:(NSString *) key;

/**
 * Convert AJOBInfo object to OBInfo object.
 * @param ajOBInfo The AJOBInfo object.
 * @return OBInfo object.
 */
+(ajn::services::OBInfo)toOBInfo:(AJOBInfo) ajOBInfo;

/**
 * Convert OBInfo object to AJOBInfo object.
 * @param obInfo The OBInfo object.
 * @return AJOBInfo object.
 */
+(AJOBInfo)toAJOBInfo:(ajn::services::OBInfo) obInfo;

/**
 * Convert OBLastError object to AJOBLastError object.
 * @param obLastError The OBLastError object.
 * @return AJOBLastError object.
 */
+(AJOBLastError)toAJOBLastError:(ajn::services::OBLastError) obLastError;

@end
