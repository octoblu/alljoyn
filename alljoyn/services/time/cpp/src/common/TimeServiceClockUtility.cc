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

#include "TimeServiceClockUtility.h"
#include <alljoyn/time/LogModule.h>
#include "TimeServiceUtility.h"

using namespace ajn;
using namespace services;

using namespace tsClockUtility;

//Create Clock interface
QStatus tsClockUtility::createInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::CLOCK_IFACE, true, IFACE_PROP_VERSION, bus, ptrIfaceDesc);
    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_DATE_TIME.c_str(), "((qyy)(yyyq)n)", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_IS_SET.c_str(), "b", PROP_ACCESS_READ);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();

    return status;
}

//Unmarshal DateTime
QStatus tsClockUtility::unmarshalDateTime(const MsgArg& val, TimeServiceDateTime* dateTime)
{

    uint16_t year;
    uint8_t month;
    uint8_t day;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;

    int16_t offsetMinutes;

    QStatus status = val.Get("((qyy)(yyyq)n)", &year, &month, &day, &hour, &minute, &second, &millisecond, &offsetMinutes);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal DateTime property")));
        return status;
    }

    TimeServiceTime time;
    status = time.init(hour, minute, second, millisecond);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize Time object")));
        return status;
    }

    TimeServiceDate date;
    status = date.init(year, month, day);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize Date object")));
        return status;
    }

    status = dateTime->init(date, time, offsetMinutes);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize DateTime object")));
        return status;
    }

    return status;
}

//Marshal DateTime
QStatus tsClockUtility::marshalDateTime(MsgArg& msg, const TimeServiceDateTime& dateTime)
{
    const TimeServiceDate& date = dateTime.getDate();
    const TimeServiceTime& time = dateTime.getTime();

    QStatus status = msg.Set("((qyy)(yyyq)n)", date.getYear(), date.getMonth(), date.getDay(),
                             time.getHour(), time.getMinute(), time.getSecond(), time.getMillisecond(),
                             dateTime.getOffsetMinutes());

    return status;
}

//Unmarshal IsSet
QStatus tsClockUtility::unmarshalIsSet(const MsgArg& val, bool* isSet)
{
    bool isSetArg;
    QStatus status = val.Get("b", &isSetArg);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to unmarshal IsSet"));
        return status;
    }

    (*isSet) = isSetArg;

    return status;
}

//Marshal IsSet
QStatus tsClockUtility::marshalIsSet(MsgArg& msg, bool isSet)
{

    QStatus status = msg.Set("b", isSet);
    return status;
}

//Create Time Authority interface
QStatus tsClockUtility::createAuthorityInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::TIME_AUTHORITY_IFACE, false, IFACE_AUTH_PROP_VERSION, bus, ptrIfaceDesc);

    if (status != ER_OK) {

        return ER_OK;
    }

    status = (*ptrIfaceDesc)->AddProperty(IFACE_AUTH_PROP_TYPE.c_str(), "y", PROP_ACCESS_READ);
    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddSignal(IFACE_AUTH_SIG_TIME_SYNC.c_str(), NULL, NULL);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();

    return status;
}

//Marshal Authority Type
QStatus tsClockUtility::marshalAuthorityType(MsgArg& msg, tsConsts::ClockAuthorityType type)
{

    QStatus status = msg.Set("y", type);
    return status;
}

//Unmarshal AuthorityType
QStatus tsClockUtility::unmarshalAuthorityType(const MsgArg& msg, tsConsts::ClockAuthorityType* type)
{

    uint8_t rcvdType;
    QStatus status = msg.Get("y", &rcvdType);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarhal AuthorityType")));
        return status;
    }

    //The condition is under assumption that AuthorityType Enum is a continues sequence.
    //Adding new values to the AuthorityType Enum, must be followed by updating this condition
    if ((rcvdType < tsConsts::OTHER) ||
        (rcvdType > tsConsts::USER_DESIGNATED)) {

        QCC_DbgPrintf(("Received unrecognized AuthorityType: '%d', returning 'OTHER'", rcvdType));

        rcvdType = tsConsts::OTHER;
    }

    (*type) = static_cast<tsConsts::ClockAuthorityType>(rcvdType);
    return status;
}
