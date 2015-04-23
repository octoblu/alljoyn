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


#include "alljoyn/time/TimeServiceServer.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "TimeServiceGlobalStringSequencer.h"
#include "../common/TimeServiceClockUtility.h"
#include "../common/TimeServiceAlarmUtility.h"
#include "../common/TimeServiceTimerUtility.h"
#include "TimeServiceClockBusObj.h"
#include "TimeServiceAuthorityClockBusObj.h"
#include <alljoyn/time/TimeServiceAlarmBusObj.h>
#include <alljoyn/time/TimeServiceTimerBusObj.h>
#include "TimeServiceAlarmFactoryBusObj.h"
#include "TimeServiceTimerFactoryBusObj.h"

using namespace ajn;
using namespace services;

//Initialize self pointer
TimeServiceServer* TimeServiceServer::s_Instance(NULL);

//Constructor
TimeServiceServer::TimeServiceServer() : SessionListener(), SessionPortListener(), m_Sid(0), m_Bus(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Desctructor
TimeServiceServer::~TimeServiceServer()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    shutdown();
}

//Returns TimeServiceServer singleton instance
TimeServiceServer* TimeServiceServer::getInstance()
{

    if (!s_Instance) {

        s_Instance = new TimeServiceServer();
    }

    return s_Instance;
}

//Initialize TimeServiceServer
QStatus TimeServiceServer::init(BusAttachment* busAttachment)
{

    QCC_DbgHLPrintf(("Starting TimeServiceServer"));

    if (m_Bus != NULL) {

        QCC_LogError(ER_FAIL, ("Could not accept this BusAttachment, a different bus attachment already exists"));
        return ER_FAIL;
    }

    if (!busAttachment || !busAttachment->IsStarted() || !busAttachment->IsConnected()) {

        QCC_LogError(ER_FAIL, ("Could not accept this BusAttachment, bus attachment not started or not connected"));
        return ER_FAIL;
    }

    TransportMask transportMask = TRANSPORT_ANY;
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, transportMask);

    SessionPort port = tsConsts::PORT;
    QStatus status   = busAttachment->BindSessionPort(port, opts, *this);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to bind session port"));
        return status;
    }

    // This is used to create object paths for the service objects,
    // so needs to be initialized
    TimeServiceGlobalStringSequencer::getInstance()->init();

    m_Bus = busAttachment;
    return ER_OK;
}

//Stops the TimeServiceServer
void TimeServiceServer::shutdown()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_Bus) {

        QCC_LogError(ER_FAIL, ("Can't shutdown the server, it hasn't been initialized"));
        return;
    }

    QCC_DbgHLPrintf(("Shutting down TimeServiceServer"));

    QCC_DbgPrintf(("Unbinding session port"));
    QStatus status = m_Bus->UnbindSessionPort(tsConsts::PORT);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to unbind session port"));
    }

    //Session has been established
    if (m_Sid != 0) {

        QCC_DbgPrintf(("Removing SessionListener and closing the session: '%d'", m_Sid));
        m_Bus->SetSessionListener(m_Sid, NULL);

        QStatus status = m_Bus->LeaveSession(m_Sid);
        if (status == ER_OK) {

            m_Sid = 0;
        } else {

            QCC_LogError(status, ("Failed to leave the session '%d'", m_Sid));
        }
    }

    m_Bus = NULL;
}

//Return BusAttachment
BusAttachment* TimeServiceServer::getBusAttachment() const
{

    return m_Bus;
}

//True if the TimeServiceServer has been initialized and started
bool TimeServiceServer::isStarted() const
{

    return (m_Bus != NULL);
}

//Handle session lost
void TimeServiceServer::SessionLost(ajn::SessionId sessionId, SessionLostReason reason)
{

    QCC_DbgTrace(("%s, SID: '%u'", __FUNCTION__, sessionId));

    if (!m_Bus) {

        QCC_LogError(ER_FAIL, ("Received session lost, but TimeServiceServer is not initialized"));
        return;
    }

    m_Bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received sessionLost, sessionId: '%u', reason: '%d'", sessionId, reason));
    m_Bus->SetSessionListener(sessionId, NULL);

    m_Sid  = 0;
}

//Handle session join request
bool TimeServiceServer::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts)
{

    QCC_DbgTrace(("%s, Joiner: '%s'", __FUNCTION__, joiner));

    if (sessionPort != tsConsts::PORT) {

        QCC_DbgPrintf(("Received JoinSession request for an unknown port: '%d', from : '%s'", sessionPort, joiner));
        return false;
    }

    QCC_DbgPrintf(("Received JoinSession request, from : '%s'", joiner));
    return true;
}

//Handle joiner
void TimeServiceServer::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner)
{

    QCC_DbgTrace(("%s, SID: '%u'", __FUNCTION__, id));

    if (!m_Bus) {

        QCC_LogError(ER_FAIL, ("Received session joined, but TimeServiceServer is not initialized"));
        return;
    }

    m_Bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("The joiner: '%s' has joined the session: '%d'", joiner, id));
    QStatus status = m_Bus->SetSessionListener(id, this);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to set SessionListener for the session, sid: '%d'", id));
    }

    m_Sid = id;
}

//Create Clock
QStatus TimeServiceServer::activateClock(TimeServiceServerClock* clock) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    qcc::String objectPath(tsClockUtility::OBJ_PATH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for Clock"));
        return status;
    }

    if (!clock) {

        QCC_LogError(ER_FAIL, ("Received undefined Clock"));
        return ER_FAIL;
    }

    TimeServiceClockBusObj* clockBusObj = new TimeServiceClockBusObj(objectPath);
    status = clockBusObj->init(clock);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Clock BusObject"));

        delete clockBusObj;
        return status;
    }

    status = clock->setClockBusObj(clockBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Clock"));
        delete clockBusObj;
        return status;
    }

    return status;
}

//Create TimeAuthority clock
QStatus TimeServiceServer::activateTimeAuthorityClock(TimeServiceServerAuthorityClock* clock,
                                                      tsConsts::ClockAuthorityType type) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    qcc::String objectPath(tsClockUtility::OBJ_PATH_AUTH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for the AuthorityClock"));
        return status;
    }

    if (!clock) {

        QCC_LogError(ER_FAIL, ("Received undefined AuthorityClock"));
        return ER_FAIL;
    }

    TimeServiceClockBusObj* clockBusObj = new TimeServiceAuthorityClockBusObj(objectPath, type);
    status = clockBusObj->init(clock);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize AuthorityClock BusObject"));

        delete clockBusObj;
        return status;
    }

    status = clock->setClockBusObj(clockBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize AuthorityClock"));

        delete clockBusObj;
        return status;
    }

    return status;
}

//Create Alarm
QStatus TimeServiceServer::activateAlarm(TimeServiceServerAlarm* alarm) const
{

    return activateAlarm(alarm, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Create Alarm with description.
QStatus TimeServiceServer::activateAlarm(TimeServiceServerAlarm* alarm, const qcc::String& description,
                                         const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!alarm) {

        QCC_LogError(ER_FAIL, ("Received undefined Alarm"));
        return ER_FAIL;
    }

    qcc::String objectPath(tsAlarmUtility::OBJ_PATH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for Alarm"));
        return status;
    }

    std::vector<qcc::String> emptyVector;

    TimeServiceAlarmBusObj* alarmBusObj = new TimeServiceAlarmBusObj(objectPath);
    status = alarmBusObj->init(alarm, emptyVector, description, language, translator);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Alarm BusObject"));

        delete alarmBusObj;
        return status;
    }

    status = alarm->setAlarmBusObj(alarmBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Alarm"));

        delete alarmBusObj;
        return status;
    }

    return status;
}

// Register user defined custom Alarm.
QStatus TimeServiceServer::registerCustomAlarm(TimeServiceAlarmBusObj* alarmBusObj, TimeServiceServerAlarm* alarm,
                                               const std::vector<qcc::String>& notAnnounced) const
{

    return registerCustomAlarm(alarmBusObj, alarm, notAnnounced, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Register user defined custom Alarm with description.
QStatus TimeServiceServer::registerCustomAlarm(TimeServiceAlarmBusObj* alarmBusObj, TimeServiceServerAlarm* alarm,
                                               const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                                               const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!alarmBusObj) {

        QCC_LogError(ER_FAIL, ("Received undefined alarmBusObj"));
        return ER_FAIL;
    }

    if (!alarm) {

        QCC_LogError(ER_FAIL, ("Received undefined Alarm"));
        return ER_FAIL;
    }

    if (!alarmBusObj->isInitialized()) {

        QStatus status = alarmBusObj->init(alarm, notAnnounced, description, language, translator);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to initialize Alarm BusObject"));
            return status;
        }
    }

    QStatus status = alarm->setAlarmBusObj(alarmBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Alarm BusObject"));
        return status;
    }

    return status;
}

//Create AlarmFactory.
QStatus TimeServiceServer::activateAlarmFactory(TimeServiceServerAlarmFactory* alarmFactory) const
{

    return activateAlarmFactory(alarmFactory, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Create AlarmFactory with description
QStatus TimeServiceServer::activateAlarmFactory(TimeServiceServerAlarmFactory* alarmFactory, const qcc::String& description,
                                                const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!alarmFactory) {

        QCC_LogError(ER_FAIL, ("Received undefined AlarmFactory"));
        return ER_FAIL;
    }

    qcc::String objectPath(tsAlarmUtility::FAC_OBJ_PATH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for the AlarmFactory"));
        return status;
    }

    TimeServiceAlarmFactoryBusObj* alarmFactoryBusObj = new TimeServiceAlarmFactoryBusObj(objectPath);
    status = alarmFactoryBusObj->init(alarmFactory, description, language, translator);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize AlarmFactory BusObject"));

        delete alarmFactoryBusObj;
        return status;
    }

    status = alarmFactory->setAlarmFactoryBusObj(alarmFactoryBusObj);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize AlarmFactory BusObject"));

        delete alarmFactoryBusObj;
        return status;
    }

    return status;
}

//Create Timer
QStatus TimeServiceServer::activateTimer(TimeServiceServerTimer* Timer) const
{

    return activateTimer(Timer, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Create Timer with description.
QStatus TimeServiceServer::activateTimer(TimeServiceServerTimer* Timer, const qcc::String& description,
                                         const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!Timer) {

        QCC_LogError(ER_FAIL, ("Received undefined Timer"));
        return ER_FAIL;
    }

    qcc::String objectPath(tsTimerUtility::OBJ_PATH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for Timer"));
        return status;
    }

    std::vector<qcc::String> emptyVector;

    TimeServiceTimerBusObj* TimerBusObj = new TimeServiceTimerBusObj(objectPath);
    status = TimerBusObj->init(Timer, emptyVector, description, language, translator);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Timer BusObject"));

        delete TimerBusObj;
        return status;
    }

    status = Timer->setTimerBusObj(TimerBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Timer"));

        delete TimerBusObj;
        return status;
    }

    return status;
}

// Register user defined custom Timer.
QStatus TimeServiceServer::registerCustomTimer(TimeServiceTimerBusObj* TimerBusObj, TimeServiceServerTimer* Timer,
                                               const std::vector<qcc::String>& notAnnounced) const
{

    return registerCustomTimer(TimerBusObj, Timer, notAnnounced, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Register user defined custom Timer with description.
QStatus TimeServiceServer::registerCustomTimer(TimeServiceTimerBusObj* TimerBusObj, TimeServiceServerTimer* Timer,
                                               const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                                               const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!TimerBusObj) {

        QCC_LogError(ER_FAIL, ("Received undefined TimerBusObj"));
        return ER_FAIL;
    }

    if (!Timer) {

        QCC_LogError(ER_FAIL, ("Received undefined Timer"));
        return ER_FAIL;
    }

    if (!TimerBusObj->isInitialized()) {

        QStatus status = TimerBusObj->init(Timer, notAnnounced, description, language, translator);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to initialize Timer BusObject"));
            return status;
        }
    }

    QStatus status = Timer->setTimerBusObj(TimerBusObj);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize Timer BusObject"));
        return status;
    }

    return status;
}

//Create TimerFactory.
QStatus TimeServiceServer::activateTimerFactory(TimeServiceServerTimerFactory* timerFactory) const
{

    return activateTimerFactory(timerFactory, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Create TimerFactory with description
QStatus TimeServiceServer::activateTimerFactory(TimeServiceServerTimerFactory* timerFactory, const qcc::String& description,
                                                const qcc::String& language, Translator* translator) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!timerFactory) {

        QCC_LogError(ER_FAIL, ("Received undefined TimerFactory"));
        return ER_FAIL;
    }

    qcc::String objectPath(tsTimerUtility::FAC_OBJ_PATH_PREFIX);
    QStatus status = generateObjectPath(&objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to generate object path for the TimerFactory"));
        return status;
    }

    TimeServiceTimerFactoryBusObj* timerFactoryBusObj = new TimeServiceTimerFactoryBusObj(objectPath);
    status = timerFactoryBusObj->init(timerFactory, description, language, translator);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize TimerFactory BusObject"));

        delete timerFactoryBusObj;
        return status;
    }

    status = timerFactory->setTimerFactoryBusObj(timerFactoryBusObj);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to initialize TimerFactory BusObject"));

        delete timerFactoryBusObj;
        return status;
    }

    return status;
}

//Generate object path
QStatus TimeServiceServer::generateObjectPath(qcc::String*objectPath) const
{

    return TimeServiceGlobalStringSequencer::getInstance()->append(objectPath);
}

// get session id
SessionId TimeServiceServer::getSessionId()
{
    return m_Sid;
}

