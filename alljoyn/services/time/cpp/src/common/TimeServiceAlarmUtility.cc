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

#include "TimeServiceAlarmUtility.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceTime.h>
#include "TimeServiceUtility.h"

using namespace ajn;
using namespace services;

using namespace tsAlarmUtility;

//Create Alarm interface
QStatus tsAlarmUtility::createInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::ALARM_IFACE, false, IFACE_PROP_VERSION, bus, ptrIfaceDesc);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->SetDescriptionLanguage(IFACE_DESCRIPTION_LAGN.c_str());
    (*ptrIfaceDesc)->SetDescription(IFACE_DESCRIPTION.c_str());

    //Add Schedule property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_SCHEDULE.c_str(), "((yyyq)y)", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add Title property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_TITLE.c_str(), "s", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add Enabled property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_ENABLED.c_str(), "b", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add AlarmReached signal
    status = (*ptrIfaceDesc)->AddSignal(IFACE_SIG_ALARM_REACHED.c_str(), NULL, NULL);
    if (status != ER_OK) {

        return status;
    }

    //Set description of the AlarmReached signal
    status = (*ptrIfaceDesc)->SetMemberDescription(IFACE_SIG_ALARM_REACHED.c_str(), IFACE_SIG_ALARM_REACHED_DESC.c_str(), true);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();

    return status;
}

//Unmarshal Schedule
QStatus tsAlarmUtility::unmarshalSchedule(const MsgArg&  msgArg, TimeServiceSchedule* schedule)
{

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;

    uint8_t weekDays;

    QStatus status = msgArg.Get("((yyyq)y)", &hour, &minute, &second, &millisecond, &weekDays);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal Schedule property")));
        return status;
    }

    TimeServiceTime time;
    status = time.init(hour, minute, second, millisecond);
    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize Time object")));
        return status;
    }

    status = schedule->init(time, weekDays);
    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize Schedule object")));
        return status;
    }

    return status;
}

//Marshal Schedule
QStatus tsAlarmUtility::marshalSchedule(MsgArg& msgArg, const TimeServiceSchedule& schedule)
{

    const TimeServiceTime& time = schedule.getTime();

    QStatus status = msgArg.Set("((yyyq)y)",  time.getHour(), time.getMinute(), time.getSecond(), time.getMillisecond(),
                                schedule.getWeekDays());

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Schedule object")));
        return status;
    }

    return status;
}

//Unmarshal title
QStatus tsAlarmUtility::unmarshalTitle(const MsgArg&  msgArg, qcc::String* title)
{

    char* titleStr;
    QStatus status = msgArg.Get("s", &titleStr);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal Title")));
        return status;
    }

    title->assign(titleStr);

    return status;
}

//Marshal Title
QStatus tsAlarmUtility::marshalTitle(MsgArg& msgArg, const qcc::String& title)
{

    QStatus status = msgArg.Set("s", title.c_str());

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Title")));
        return status;
    }

    return status;
}

//Unmarshal Enabled property
QStatus tsAlarmUtility::unmarshalEnabled(const MsgArg&  msgArg, bool* isEnabled)
{

    QStatus status = msgArg.Get("b", isEnabled);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal Enabled")));
        return status;
    }

    return status;
}

//Marshal Enabled property
QStatus tsAlarmUtility::marshalEnabled(MsgArg& msgArg, const bool isEnabled)
{

    QStatus status = msgArg.Set("b", isEnabled);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Enabled")));
        return status;
    }

    return status;
}

//Create Alarmfactory interface
QStatus tsAlarmUtility::createFactoryInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::ALARM_FACTORY_IFACE, true, IFACE_FAC_PROP_VERSION, bus, ptrIfaceDesc);

    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddMethod(IFACE_FAC_METHOD_NEW_ALARM.c_str(), NULL, "o", "Object Path");

    if (status != ER_OK) {

        return status;
    }


    status = (*ptrIfaceDesc)->AddMethod(IFACE_FAC_METHOD_DELETE_ALARM.c_str(), "o", NULL, "Object Path");

    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();


    return status;
}

//Marshal object path
QStatus tsAlarmUtility::marshalObjectPath(MsgArg& msgArg, const qcc::String& objPath)
{

    QStatus status = msgArg.Set("o", objPath.c_str());
    return status;
}

//Unmarshal object path
QStatus tsAlarmUtility::unmarshalObjectPath(const MsgArg& msgArg, qcc::String* objPath)
{

    char* objPathChrs;
    QStatus status = msgArg.Get("o", &objPathChrs);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal Object Path")));
        return status;
    }

    objPath->assign(objPathChrs);

    return status;
}
