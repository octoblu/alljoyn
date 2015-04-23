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

#include "TimeServiceTimerUtility.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceTime.h>
#include "TimeServiceUtility.h"

using namespace ajn;
using namespace services;

using namespace tsTimerUtility;

//Create Timer interface
QStatus tsTimerUtility::createInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::TIMER_IFACE, false, IFACE_PROP_VERSION, bus, ptrIfaceDesc);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->SetDescriptionLanguage(IFACE_DESCRIPTION_LAGN.c_str());
    (*ptrIfaceDesc)->SetDescription(IFACE_DESCRIPTION.c_str());

    //Add Interval property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_INTERVAL.c_str(), "(uyyq)", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add Title property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_TITLE.c_str(), "s", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add TimeLeft property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_TIMELEFT.c_str(), "(uyyq)", PROP_ACCESS_READ);
    if (status != ER_OK) {

        return status;
    }

    //Add IsRunning property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_ISRUNNING.c_str(), "b", PROP_ACCESS_READ);
    if (status != ER_OK) {

        return status;
    }

    //Add Repeat property
    status = (*ptrIfaceDesc)->AddProperty(IFACE_PROP_REPEAT.c_str(), "q", PROP_ACCESS_RW);
    if (status != ER_OK) {

        return status;
    }

    //Add Start method
    status = (*ptrIfaceDesc)->AddMethod(IFACE_METHOD_START.c_str(), NULL, NULL, NULL);

    if (status != ER_OK) {

        return status;
    }
    status = (*ptrIfaceDesc)->AddMemberAnnotation(IFACE_METHOD_START.c_str(), org::freedesktop::DBus::AnnotateNoReply, "true");

    if (status != ER_OK) {

        return status;
    }

    //Add Pause method
    status = (*ptrIfaceDesc)->AddMethod(IFACE_METHOD_PAUSE.c_str(), NULL, NULL, NULL);

    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddMemberAnnotation(IFACE_METHOD_PAUSE.c_str(), org::freedesktop::DBus::AnnotateNoReply, "true");

    if (status != ER_OK) {

        return status;
    }

    //Add Reset method
    status = (*ptrIfaceDesc)->AddMethod(IFACE_METHOD_RESET.c_str(), NULL, NULL, NULL);

    if (status != ER_OK) {

        return status;
    }

    //Add Timer Run state changed signal
    status = (*ptrIfaceDesc)->AddSignal(IFACE_SIG_TIMER_RUNSTATECHANGED.c_str(), "b", NULL);
    if (status != ER_OK) {

        return status;
    }

    //Add Timer event changed signal
    status = (*ptrIfaceDesc)->AddSignal(IFACE_SIG_TIMER_EVENT.c_str(), NULL, NULL);
    if (status != ER_OK) {

        return status;
    }

    //Set description of the TimerEvent signal
    status = (*ptrIfaceDesc)->SetMemberDescription(IFACE_SIG_TIMER_EVENT.c_str(), IFACE_SIG_TIMER_EVENT_DESC.c_str(), true);
    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();

    return status;
}

//Unmarshal Period
QStatus tsTimerUtility::unmarshalPeriod(const MsgArg&  msgArg, TimeServicePeriod* period)
{

    uint32_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;

    QStatus status = msgArg.Get("(uyyq)", &hour, &minute, &second, &millisecond);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal interval property")));
        return status;
    }

    status = period->init(hour, minute, second, millisecond);
    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to initialize Period object")));
        return status;
    }

    return status;
}

//Marshal Period
QStatus tsTimerUtility::marshalPeriod(MsgArg& msgArg, const TimeServicePeriod& period)
{

    QStatus status = msgArg.Set("(uyyq)",  period.getHour(), period.getMinute(), period.getSecond(), period.getMillisecond());

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Period object")));
        return status;
    }

    return status;
}

//Unmarshal title
QStatus tsTimerUtility::unmarshalTitle(const MsgArg&  msgArg, qcc::String* title)
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
QStatus tsTimerUtility::marshalTitle(MsgArg& msgArg, const qcc::String& title)
{

    QStatus status = msgArg.Set("s", title.c_str());

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Title")));
        return status;
    }

    return status;
}


// Unmarshal IsRunning property

QStatus tsTimerUtility::unmarshalBoolean(const MsgArg&  msgArg, bool* ret)
{
    QStatus status = msgArg.Get("b", ret);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal IsRunning")));
        return status;
    }

    return status;
}


// Marshal IsRunning property

QStatus tsTimerUtility::marshalBoolean(MsgArg& msgArg, const bool input)
{
    QStatus status = msgArg.Set("b", input);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal IsRunning")));
        return status;
    }

    return status;
}


// Unmarshal Repeat property

QStatus tsTimerUtility::unmarshalRepeat(const MsgArg&  msgArg, uint16_t* repeat)
{
    QStatus status = msgArg.Get("q", repeat);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to unmarshal Repeat")));
        return status;
    }

    return status;
}


// Marshal Repeat property

QStatus tsTimerUtility::marshalRepeat(MsgArg& msgArg, const uint16_t repeat)
{
    QStatus status = msgArg.Set("q", repeat);

    if (status != ER_OK) {

        QCC_LogError(status, (("Failed to marshal Repeat")));
        return status;
    }

    return status;
}

//Create Timerfactory interface
QStatus tsTimerUtility::createFactoryInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc)
{

    QStatus status;

    status = tsUtility::createAJInterface(tsConsts::TIMER_FACTORY_IFACE, true, IFACE_FAC_PROP_VERSION, bus, ptrIfaceDesc);

    if (status != ER_OK) {

        return status;
    }

    status = (*ptrIfaceDesc)->AddMethod(IFACE_FAC_METHOD_NEW_TIMER.c_str(), NULL, "o", "Object Path");

    if (status != ER_OK) {

        return status;
    }


    status = (*ptrIfaceDesc)->AddMethod(IFACE_FAC_METHOD_DELETE_TIMER.c_str(), "o", NULL, "Object Path");

    if (status != ER_OK) {

        return status;
    }

    (*ptrIfaceDesc)->Activate();


    return status;
}

//Marshal object path
QStatus tsTimerUtility::marshalObjectPath(MsgArg& msgArg, const qcc::String& objPath)
{

    QStatus status = msgArg.Set("o", objPath.c_str());
    return status;
}

//Unmarshal object path
QStatus tsTimerUtility::unmarshalObjectPath(const MsgArg& msgArg, qcc::String* objPath)
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
