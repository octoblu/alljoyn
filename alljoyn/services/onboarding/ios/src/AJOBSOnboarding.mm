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

#import "AJOBSOnboarding.h"
#import "alljoyn/about/AJNConvertUtil.h"

static NSString * const PASSCODE_FORMAT = @"%02X";

@implementation AJOBSOnboarding

+(bool)isValidWEPKey:(NSString *) key
{
    size_t size = [key length];
    return (size == 5  ||  // 40-bit ascii
            size == 10 ||  // 40 bit hex
            size == 13 ||  // 104 bit ascii
            size == 26 ||  // 104 bit hex
            size == 16 ||  // 152 bit ascii
            size == 32 ||  // 152-bit hex
            size == 29 ||  // 256-bit ascii
            size == 64);   // 256-bit hex
}

+(bool)isValidWPAKey:(NSString *) key
{
    return [key length] >= MIN_PSK_SIZE && [key length] <= MAX_PSK_SIZE;
}

+(NSString*)passcodeToHex:(NSString*) passcode
{
    const char *pass = [passcode UTF8String];
    NSMutableString *passcodeHex = [NSMutableString string];
    while (*pass) {
        [passcodeHex appendFormat:PASSCODE_FORMAT, *pass++ & BIT_MASK];
    }
    return passcodeHex;
}

+(ajn::services::OBInfo)toOBInfo:(AJOBInfo) ajOBInfo
{
    ajn::services::OBInfo obInfo;
    obInfo.SSID = [AJNConvertUtil convertNSStringToQCCString:ajOBInfo.SSID];
    obInfo.state = (ajn::services::OBState)ajOBInfo.state;
    obInfo.authType =  (ajn::services::OBAuthType)ajOBInfo.authType;
    obInfo.passcode = [AJNConvertUtil convertNSStringToQCCString:ajOBInfo.passcode];
    return obInfo;
}

+(AJOBInfo)toAJOBInfo:(ajn::services::OBInfo) obInfo
{
    AJOBInfo ajOBInfo;
    ajOBInfo.SSID = [AJNConvertUtil convertQCCStringtoNSString:obInfo.SSID];
    ajOBInfo.state = obInfo.state;
    ajOBInfo.authType = obInfo.authType;
    ajOBInfo.passcode = [AJNConvertUtil convertQCCStringtoNSString:obInfo.passcode];
    return ajOBInfo;
}

+(AJOBLastError)toAJOBLastError:(ajn::services::OBLastError) obLastError
{
    AJOBLastError ajOBLastError;
    ajOBLastError.validationState = obLastError.validationState;
    ajOBLastError.message = [AJNConvertUtil convertQCCStringtoNSString:obLastError.message];
    return ajOBLastError;
}

@end
