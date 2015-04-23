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

#ifndef _ONBOARDING_H
#define _ONBOARDING_H

#include <qcc/platform.h>
#include <qcc/String.h>

#define MAX_SSID_SIZE 32
#define MIN_PSK_SIZE 8
#define MAX_PSK_SIZE 64

namespace ajn {
namespace services {

/**
 * Enumerator for validation error: VALIDATED = 0, UNREACHABLE, UNSUPPORTEDPROTOCOL, UNAUTHORIZED, ERRORMESSAGE
 */
typedef enum {
    VALIDATED = 0,    //!< VALIDATED WIFI configuration was successful
    UNREACHABLE = 1, //!< UNREACHABLE   WIFI not found
    UNSUPPORTEDPROTOCOL = 2, //!< UNSUPPORTEDPROTOCOL using incorrect authentication type
    UNAUTHORIZED = 3, //!< UNAUTHORIZED pass phrase not correct
    ERRORMESSAGE = 4, //!< ERRORMESSAGE  general error message
} OBValidationState;

/**
 * Enumerator for authentication type namely: Any = -1, OPEN = 0, WEP, WPA, WPA2, WPS
 */
typedef enum {
    WPA2_AUTO = -3,                            //!< WPA2_AUTO authentication
    WPA_AUTO = -2,                           //!< WPA_AUTO authentication
    ANY = -1,                           //!< ANY authentication
    OPEN = 0,                          //!< OPEN authentication
    WEP = 1,                           //!< WEP authentication
    WPA_TKIP = 2,                           //!< WPA_TKIP authentication
    WPA_CCMP = 3,                           //!< WPA_CCMP authentication
    WPA2_TKIP = 4,                            //!<WPA2_TKIP authentication
    WPA2_CCMP = 5,                        //!<WPA2_CCMP authentication
    WPS = 6,                          //!<WPS authentication


} OBAuthType;

/**
 * Enum for concurrency availability
 */
typedef enum {
    CONCURRENCY_MIN = 1,
    CONCURRENCY_NONE = 1,
    CONCURRENCY_AVAIL = 2,
    CONCURRENCY_MAX = 2
} OBConcurrency;

/**
 * Structure to hold Scan info
 */
typedef struct {
    /**
     * AP SSID
     */
    qcc::String SSID;
    /**
     *  AP authentication type
     */
    OBAuthType authType;
} OBScanInfo;

/**
 * Enumerator for state of onboarding namely: NOT_CONFIGURED, CONFIGURED_NOT_VALIDATED, CONFIGURED_VALIDATING, CONFIGURED_VALIDATED, CONFIGURED_ERROR, CONFIGURED_RETRY
 */
typedef enum {
    NOT_CONFIGURED = 0, //!< NOT_CONFIGURED blank configuration
    CONFIGURED_NOT_VALIDATED, //!< CONFIGURED_NOT_VALIDATED configuration has been set but didn't validated it.
    CONFIGURED_VALIDATING, //!< CONFIGURED_VALIDATING configuration has been set in checking state.
    CONFIGURED_VALIDATED, //!< CONFIGURED_VALIDATED configuration has been set and validated.
    CONFIGURED_ERROR, //!< CONFIGURED_ERROR configuration has been set and there was an error.
    CONFIGURED_RETRY, //!< CONFIGURED_RETRY configuration has been set and there was an error in retry mode.
} OBState;

/**
 *	Structure to hold the WIFI details
 */
typedef struct {
    /**
     *	SSID is the WIFI's SSID
     */
    qcc::String SSID;
    /**
     *  state is current state
     */
    OBState state;
    /**
     *  WIFI authentication type
     */
    OBAuthType authType;
    /**
     * WIFI passcode in hex
     */
    qcc::String passcode;
} OBInfo;


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
    qcc::String message;
}OBLastError;

inline bool isValidWEPKey(qcc::String key)
{
    size_t size = key.size();
    return (size == 5  ||  // 40-bit ascii
            size == 10 ||  // 40 bit hex
            size == 13 ||  // 104 bit ascii
            size == 26 ||  // 104 bit hex
            size == 16 ||  // 152 bit ascii
            size == 32 ||  // 152-bit hex
            size == 29 ||  // 256-bit ascii
            size == 64);   // 256-bit hex
}

inline bool isValidWPAKey(qcc::String key)
{
    return key.size() >= MIN_PSK_SIZE && key.size() <= MAX_PSK_SIZE;
}

}
}

#endif
