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

#ifndef TIMESERVICECONST_H_
#define TIMESERVICECONST_H_

namespace ajn {
namespace services {
namespace tsConsts {

/**
 * Structure to store the error name and the error description.
 * This structure is used by the TimeServiceServerTimerFactory and the TimeServiceServerAlarmFactory
 */
struct ErrorInfo {

    /**
     * Error name
     */
    qcc::String errorName;

    /**
     * The description of the error stored in the errorName
     */
    qcc::String errorDescription;
};

/**
 * This is the prefix for all the interfaces of the Time Service
 */
const qcc::String IFNAME_PREFIX              = "org.allseen.Time";

/**
 * Alarm interface
 */
const qcc::String ALARM_IFACE                = "org.allseen.Time.Alarm";

/**
 * Alarm interface version
 */
const uint16_t ALARM_IFACE_VERSION           = 1;

/**
 * Alarm Factory interface
 */
const qcc::String ALARM_FACTORY_IFACE        = "org.allseen.Time.AlarmFactory";

/**
 * Alarm Factory interface version
 */
const uint16_t ALARM_FACTORY_IFACE_VERSION   = 1;

/**
 * Clock interface
 */
const qcc::String CLOCK_IFACE                = "org.allseen.Time.Clock";

/**
 * Clock interface version
 */
const uint16_t CLOCK_IFACE_VERSION           = 1;

/**
 * Time Authority
 */
const qcc::String TIME_AUTHORITY_IFACE       = "org.allseen.Time.TimeAuthority";

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
} ClockAuthorityType;

/**
 * Timer interface
 */
const qcc::String TIMER_IFACE                = "org.allseen.Time.Timer";

/**
 * Timer interface version
 */
const uint16_t TIMER_IFACE_VERSION           = 1;

/**
 * If timer Repeat is set to this value, the timer will repeat indefinitely
 */
const uint16_t TIMER_REPEAT_FOREVER          = 0xffff;

/**
 * Timer Factory interface
 */
const qcc::String TIMER_FACTORY_IFACE        = "org.allseen.Time.TimerFactory";

/**
 * Timer Factory interface version
 */
const uint16_t TIMER_FACTORY_IFACE_VERSION   = 1;

/**
 * Port number that the TimeServiceServer listens to
 */
const uint16_t PORT                          = 1030;

/**
 * General Error
 */
const qcc::String GENERIC_ERROR         = "org.alljoyn.Error";

/**
 * Invalid Value Error
 */
const qcc::String INVALID_VALUE_ERROR   = "org.alljoyn.Error.InvalidValue";

/**
 * Out Of Range Error
 */
const qcc::String OUT_OF_RANGE          = "org.alljoyn.Error.OutOfRange";

} /* tsCosts */
} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECONST_H_ */
