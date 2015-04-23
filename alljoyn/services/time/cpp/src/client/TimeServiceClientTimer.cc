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

#include <alljoyn/time/TimeServiceClientTimer.h>
#include "TimerSignalHandler.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceTimerUtility.h"
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/Message.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientTimer::TimeServiceClientTimer(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    TimeServiceObjectIntrospector(tsClient, objectPath),
    m_TimerHandler(NULL),
    m_TimerIface(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    initInterfaces(tsConsts::TIMER_IFACE, &m_TimerIface, &tsTimerUtility::createInterface);
}

//Destructor
TimeServiceClientTimer::~TimeServiceClientTimer()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Release object resources
void TimeServiceClientTimer::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgHLPrintf(("Releasing ClientTimer, objectPath: '%s'", getObjectPath().c_str()));
    unRegisterTimerHandler();

    m_TimerIface = NULL;
}

//Register TimerHandler to receive Timer related events.
QStatus TimeServiceClientTimer::registerTimerHandler(TimerHandler*handler)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    m_TimerHandler = handler;
    return TimerSignalHandler::getInstance()->registerTimer(this);
}

//Stop receiving Timer related events.
void TimeServiceClientTimer::unRegisterTimerHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_TimerHandler) {

        QCC_DbgPrintf(("Can not unregister TimerHandler, it hasn't been registered before"));
        return;
    }

    m_TimerHandler = NULL;
    TimerSignalHandler::getInstance()->unRegisterTimer(this);
}

//Returns Timer handler that has been registered or NULL
TimerHandler* TimeServiceClientTimer::getTimerHandler()
{

    return m_TimerHandler;
}

//Retrieve timer interval from the TimeServiceServer Timer object.
QStatus TimeServiceClientTimer::retrieveInterval(TimeServicePeriod* interval)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!interval) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined interval"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_INTERVAL, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsTimerUtility::unmarshalPeriod(msgArg, interval);

}

//Set Timer interval to the TimeServiceServer Timer object.
QStatus TimeServiceClientTimer::setInterval(const TimeServicePeriod& interval)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting interval, objectPath: '%s'", m_ObjectPath.c_str()));

    if (!interval.isValid()) {

        QCC_LogError(ER_BAD_ARG_1, ("Interval is not a valid object, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = tsTimerUtility::marshalPeriod(msgArg, interval);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal interval, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_INTERVAL, msgArg);
}

//Retrieve amount of Time Left until the TimeServiceServer fires.
QStatus TimeServiceClientTimer::retrieveTimeLeft(TimeServicePeriod* period)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Retrieving timeleft, objectPath: '%s'", m_ObjectPath.c_str()));

    if (!period) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined period pointer"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_TIMELEFT, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsTimerUtility::unmarshalPeriod(msgArg, period);
}

//Retrieve whether the TimeServiceClientTimer is currently running
QStatus TimeServiceClientTimer::retrieveIsRunning(bool& isRunning)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Retrieving isRunning, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;

    QStatus status = retrieveServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_ISRUNNING, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsTimerUtility::unmarshalBoolean(msgArg, &isRunning);
}

//Retrieve how many times this TimeServiceClientTimer should repeat itself.
QStatus TimeServiceClientTimer::retrieveRepeat(uint16_t& repeat)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Retrieving repeat, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;

    QStatus status = retrieveServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_REPEAT, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsTimerUtility::unmarshalRepeat(msgArg, &repeat);
}

//Set how many times this TimeServiceClientTimer should repeat itself.
QStatus TimeServiceClientTimer::setRepeat(short repeat)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting repeat, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;
    QStatus status = tsTimerUtility::marshalRepeat(msgArg, repeat);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal repeat, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_REPEAT, msgArg);
}

//Retrieve Title from the server Alarm object.
QStatus TimeServiceClientTimer::retrieveTitle(qcc::String* title)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!title) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined Title"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_TITLE, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsTimerUtility::unmarshalTitle(msgArg, title);
}

//Set Title to the server Alarm object.
QStatus TimeServiceClientTimer::setTitle(const qcc::String& title)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting Title, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;
    QStatus status = tsTimerUtility::marshalTitle(msgArg, title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Title, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_TimerIface, tsTimerUtility::IFACE_PROP_TITLE, msgArg);
}

// Start timer
QStatus TimeServiceClientTimer::start()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    return serverMethodCall(m_TimerIface, tsTimerUtility::IFACE_METHOD_START, NULL, 0);
}

// Start timer
QStatus TimeServiceClientTimer::pause()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    return serverMethodCall(m_TimerIface, tsTimerUtility::IFACE_METHOD_PAUSE, NULL, 0);
}

// Start timer
QStatus TimeServiceClientTimer::reset()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    ConnectionData connectionData;
    QStatus status = getConnectionData(&connectionData);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call reset function, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    Message msg(*connectionData.bus);
    return serverMethodCall(m_TimerIface, tsTimerUtility::IFACE_METHOD_RESET, NULL, 0, &msg);
}
