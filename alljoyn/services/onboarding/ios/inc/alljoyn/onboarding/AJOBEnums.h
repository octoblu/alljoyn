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

/**
 * Enumerator for validation error: VALIDATED = 0, UNREACHABLE, UNSUPPORTEDPROTOCOL, UNAUTHORIZED, ERRORMESSAGE
 */
typedef NS_ENUM (NSInteger, AJOBValidationState){
    /***
     * WIFI configuration was successful
     */
    VALIDATED = 0,
    /***
     * WIFI not found
     */
    UNREACHABLE = 1,
    /***
     * using incorrect authentication type
     */
    UNSUPPORTEDPROTOCOL = 2,
    /***
     * pass phrase not correct
     */
    UNAUTHORIZED = 3,
    /***
     * general error message
     */
    ERRORMESSAGE = 4,
};

/**
 * Enumerator for authentication type
 */
typedef NS_ENUM (NSInteger, AJOBAuthType) {
    /***
     * WPA2_AUTO authentication
     */
    WPA2_AUTO = -3,
    /***
     * WPA2_AUTO authentication
     */
    WPA_AUTO = -2,
    /***
     * WPA2_AUTO authentication
     */
    ANY = -1,
    /***
     * WPA2_AUTO authentication
     */
    OPEN = 0,
    /***
     * WPA2_AUTO authentication
     */
    WEP = 1,
    /***
     * WPA2_AUTO authentication
     */
    WPA_TKIP = 2,
    /***
     * WPA2_AUTO authentication
     */
    WPA_CCMP = 3,
    /***
     * WPA2_AUTO authentication
     */
    WPA2_TKIP = 4,
    /***
     * WPA2_AUTO authentication
     */
    WPA2_CCMP = 5,
    /***
     * WPA2_AUTO authentication
     */
    WPS = 6,
};

/**
 * NS_ENUM for concurrency availability
 */
typedef NS_ENUM (NSInteger, AJOBConcurrency) {
    /***
     * CONCURRENCY_MIN
     */
    CONCURRENCY_MIN = 1,
    /***
     * CONCURRENCY_NONE
     */
    CONCURRENCY_NONE = 1,
    /***
     * CONCURRENCY_AVAIL
     */
    CONCURRENCY_AVAIL = 2,
    /***
     * CONCURRENCY_MAX
     */
    CONCURRENCY_MAX = 2
};

/**
 * Enumerator for state of onboarding namely: NOT_CONFIGURED, CONFIGURED_NOT_VALIDATED, CONFIGURED_VALIDATING, CONFIGURED_VALIDATED, CONFIGURED_ERROR, CONFIGURED_RETRY
 */
typedef NS_ENUM (NSInteger, AJOBState) {
    /***
     * NOT_CONFIGURED blank configuration
     */
    NOT_CONFIGURED = 0,
    /***
     * CONFIGURED_NOT_VALIDATED configuration has been set but didn't validated it.
     */
    CONFIGURED_NOT_VALIDATED,
    /***
     * CONFIGURED_VALIDATING configuration has been set in checking state.
     */
    CONFIGURED_VALIDATING,
    /***
     * CONFIGURED_VALIDATED configuration has been set and validated.
     */
    CONFIGURED_VALIDATED,
    /***
     * CONFIGURED_ERROR configuration has been set and there was an error.
     */
    CONFIGURED_ERROR,
    /***
     * CONFIGURED_RETRY configuration has been set and there was an error in retry mode.
     */
    CONFIGURED_RETRY,
} ;