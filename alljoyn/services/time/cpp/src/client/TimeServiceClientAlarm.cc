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

#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceClientClock.h>
#include <alljoyn/time/TimeServiceClientAlarm.h>
#include <alljoyn/time/TimeServiceConstants.h>

#include "AlarmSignalHandler.h"
#include "../common/TimeServiceAlarmUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientAlarm::TimeServiceClientAlarm(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    TimeServiceObjectIntrospector(tsClient, objectPath),
    m_AlarmIface(NULL),
    m_AlarmHandler(NULL)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
    initInterfaces(tsConsts::ALARM_IFACE, &m_AlarmIface, &tsAlarmUtility::createInterface);
}

//Destructor
TimeServiceClientAlarm::~TimeServiceClientAlarm()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}

//Release object resources
void TimeServiceClientAlarm::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing ClientAlarm, objectPath: '%s'", getObjectPath().c_str()));

    unRegisterAlarmHandler();

    m_AlarmIface = NULL;
}

//Retrieve Schedule from the server Alarm object.
QStatus TimeServiceClientAlarm::retrieveSchedule(TimeServiceSchedule* schedule)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!schedule) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined Schedule"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_SCHEDULE, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsAlarmUtility::unmarshalSchedule(msgArg, schedule);
}

//Set Schedule to the server Alarm object.
QStatus TimeServiceClientAlarm::setSchedule(const TimeServiceSchedule& schedule)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting Schedule, objectPath: '%s'", m_ObjectPath.c_str()));

    if (!schedule.isValid()) {

        QCC_LogError(ER_BAD_ARG_1, ("Schedule is not a valid object, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = tsAlarmUtility::marshalSchedule(msgArg, schedule);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Schedule, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_SCHEDULE, msgArg);
}

//Retrieve Title from the server Alarm object.
QStatus TimeServiceClientAlarm::retrieveTitle(qcc::String* title)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    if (!title) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined Title"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_TITLE, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsAlarmUtility::unmarshalTitle(msgArg, title);
}

//Set Title to the server Alarm object.
QStatus TimeServiceClientAlarm::setTitle(const qcc::String& title)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting Title, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;
    QStatus status = tsAlarmUtility::marshalTitle(msgArg, title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Title, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_TITLE, msgArg);
}

//Retrieve IsEnabled from the server Alarm object.
QStatus TimeServiceClientAlarm::retrieveIsEnabled(bool* isEnabled)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!isEnabled) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined isEnabled"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_ENABLED, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsAlarmUtility::unmarshalEnabled(msgArg, isEnabled);
}

//Set IsEnabled status to the server Alarm object.
QStatus TimeServiceClientAlarm::setEnabled(bool isEnabled)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting IsEnabled, objectPath: '%s'", m_ObjectPath.c_str()));

    MsgArg msgArg;
    QStatus status = tsAlarmUtility::marshalEnabled(msgArg, isEnabled);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal IsEnabled, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_AlarmIface, tsAlarmUtility::IFACE_PROP_ENABLED, msgArg);
}

//Register AlarmHandler to receive AlarmReached events
QStatus TimeServiceClientAlarm::registerAlarmHandler(AlarmHandler* handler)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    if (!handler) {

        QCC_LogError(ER_FAIL, ("No handler given."));
        return ER_FAIL;
    }

    m_AlarmHandler = handler;
    return AlarmSignalHandler::getInstance()->registerAlarm(this);
}

//Unregister AlarmHandler to receive AlarmReached events
void TimeServiceClientAlarm::unRegisterAlarmHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    if (!m_AlarmHandler) {

        QCC_DbgPrintf(("Can not unregister AlarmHandler, it hasn't been registered before."));
        return;
    }

    AlarmSignalHandler::getInstance()->unRegisterAlarm(this);
    m_AlarmHandler = NULL;
}

//AlarmHandler that has been registered or NULL
AlarmHandler* TimeServiceClientAlarm::getAlarmHandler()
{

    return m_AlarmHandler;
}
