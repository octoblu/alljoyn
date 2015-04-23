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
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import <Foundation/Foundation.h>
//#import "alljoyn/time/TimeServiceConstants.h"

/**
 * This is the prefix for all the interfaces of the Time Service
 */
static NSString* const IFNAME_PREFIX              = @"org.allseen.Time";

/**
 * Alarm interface
 */
static NSString* const ALARM_IFACE                = @"org.allseen.Time.Alarm";

/**
 * Alarm interface version
 */
const uint16_t ALARM_IFACE_VERSION          = 1;

/**
 * Alarm Factory interface
 */
static NSString* const ALARM_FACTORY_IFACE        = @"org.allseen.Time.AlarmFactory";

/**
 * Alarm Factory interface version
 */
const uint16_t ALARM_FACTORY_IFACE_VERSION   = 1;

/**
 * Clock interface
 */
static NSString* const CLOCK_IFACE                = @"org.allseen.Time.Clock";

/**
 * Clock interface version
 */
const uint16_t CLOCK_IFACE_VERSION           = 1;

/**
 * Time Authority
 */
static NSString* const TIME_AUTHORITY             = @"org.allseen.Time.TimeAuthority";

/**
 * Time Authority interface version
 */
const uint16_t TIME_AUTHORITY_IFACE_VERSION  = 1;

/**
 * The different sources a TimeAuthority Clock can synchronize its time against.
 */
typedef enum {

    OTHER           = 0,
    NTP             = 1,
    CELLULAR        = 2,
    GPS             = 3,
    USER_DESIGNATED = 4,
} AJTMClockAuthorityType;

/**
 * Timer interface
 */
static NSString* const TIMER_IFACE                = @"org.allseen.Time.Timer";

/**
 * Timer interface version
 */
const uint16_t TIMER_IFACE_VERSION           = 1;

/**
 * Timer Factory interface
 */
static NSString* const TIMER_FACTORY_IFACE        = @"org.allseen.Time.TimerFactory";

/**
 * Timer Factory interface version
 */
const uint16_t TIMER_FACTORY_IFACE_VERSION   = 1;

/**
 * Port number that the TimeServiceServer listens to
 */
const uint16_t PORT                     = 1030;

/**
 * General Error
 */
static NSString* const GENERIC_ERROR         = @"org.alljoyn.Error";

/**
 * Invalid Value Error
 */
static NSString* const INVALID_VALUE_ERROR   = @"org.alljoyn.Error.InvalidValue";

/**
 * Out Of Range Error
 */
static NSString* const OUT_OF_RANGE          = @"org.alljoyn.Error.OutOfRange";

@interface AJTMTimeServiceConstants : NSObject


@end
