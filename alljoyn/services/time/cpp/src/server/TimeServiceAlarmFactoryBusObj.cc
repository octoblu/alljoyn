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

#include "TimeServiceAlarmFactoryBusObj.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceAlarmUtility.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceServerAlarm.h>
#include <alljoyn/time/TimeServiceAlarmBusObj.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceAlarmFactoryBusObj::TimeServiceAlarmFactoryBusObj(qcc::String const& objectPath) : BusObject(objectPath.c_str()), m_AlarmFactory(NULL),
    m_ObjectPath(objectPath), m_Translator(NULL)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceAlarmFactoryBusObj::~TimeServiceAlarmFactoryBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceAlarmFactoryBusObj::init(TimeServiceServerAlarmFactory* alarmFactory)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    return init(alarmFactory, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceAlarmFactoryBusObj::init(TimeServiceServerAlarmFactory* alarmFactory, const qcc::String& description,
                                            const qcc::String& language, Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    QCC_DbgHLPrintf(("Initializing AlarmFactory '%s'", m_ObjectPath.c_str()));

    if (!alarmFactory) {

        QCC_LogError(ER_FAIL, ("AlarmFactory is undefined"));
        return ER_FAIL;
    }

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    m_AlarmFactory       = alarmFactory;
    m_Description        = description;
    m_Language           = language;
    m_Translator         = translator;


    status = addAlarmFactoryInterface(bus);
    if (status != ER_OK) {

        return status;
    }

    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to register AlarmFactory BusObject"));
        return status;
    }

    return ER_OK;
}

//Releases object resources
void TimeServiceAlarmFactoryBusObj::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing AlarmFactory '%s'", m_ObjectPath.c_str()));

    m_AlarmFactory     = NULL;
    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        return;
    }

    bus->UnregisterBusObject(*this);
}

//Returns object path of this Alarm
qcc::String const& TimeServiceAlarmFactoryBusObj::getObjectPath() const
{

    return m_ObjectPath;
}

//Adds the interface to this AlarmFactory BusObject.
QStatus TimeServiceAlarmFactoryBusObj::addAlarmFactoryInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*> (bus->GetInterface(tsConsts::ALARM_FACTORY_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsAlarmUtility::createFactoryInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create AlarmFactory interface"));
            return status;
        }
    }

    status = AddInterface(*ifaceDesc, ANNOUNCED);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add AlarmFactory interface"));
        return status;
    }

    const ajn::InterfaceDescription::Member* newAlarmMethod;
    const ajn::InterfaceDescription::Member* deleteAlarmMethod;

    newAlarmMethod    = ifaceDesc->GetMember(tsAlarmUtility::IFACE_FAC_METHOD_NEW_ALARM.c_str());
    deleteAlarmMethod = ifaceDesc->GetMember(tsAlarmUtility::IFACE_FAC_METHOD_DELETE_ALARM.c_str());

    status = AddMethodHandler(newAlarmMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceAlarmFactoryBusObj::handleNewAlarm));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add NewAlarm method handler"));
        return status;
    }

    status = AddMethodHandler(deleteAlarmMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceAlarmFactoryBusObj::handleDeleteAlarm));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add DeleteAlarm method handler"));
        return status;
    }

    return status;
}

//Callback for Alljoyn when GetProperty is called on this BusObject.
QStatus TimeServiceAlarmFactoryBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    if (0 != strcmp(tsConsts::ALARM_FACTORY_IFACE.c_str(), interfaceName)) {

        return ER_BUS_NO_SUCH_INTERFACE;
    }

    QCC_DbgPrintf(("Received 'GET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));


    if (0 == strcmp(tsAlarmUtility::IFACE_FAC_PROP_VERSION.c_str(), propName)) {

        return handleGetVersion(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

//Handles Get Version property request
QStatus TimeServiceAlarmFactoryBusObj::handleGetVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    msgArg.typeId   = ALLJOYN_UINT16;
    msgArg.v_uint16 = tsConsts::ALARM_FACTORY_IFACE_VERSION;

    return ER_OK;
}

//Handle New Alarm
void TimeServiceAlarmFactoryBusObj::handleNewAlarm(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling NewAlarm() method call, objectPath: '%s'", m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        replyError(ER_FAIL, NULL, msg);
        return;
    }

    bus->EnableConcurrentCallbacks();

    TimeServiceServerAlarm* alarm = NULL;
    tsConsts::ErrorInfo error;

    QStatus status = m_AlarmFactory->newAlarm(&alarm, &error);

    if (status != ER_OK || !alarm) {

        QCC_LogError(status, ("Failed to create NewAlarm, objPath: '%s'", m_ObjectPath.c_str()));
        replyError(status, &error, msg);
        return;
    }

    //Create child object path for this factory
    qcc::String alarmObjPath = m_ObjectPath + tsAlarmUtility::OBJ_PATH_PREFIX;
    status = TimeServiceServer::getInstance()->generateObjectPath(&alarmObjPath);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to generate object path for the new Alarm, objectPath: '%s'", m_ObjectPath.c_str()));

        m_AlarmFactory->handleError(alarm);
        replyError(status, NULL, msg);
        return;
    }

    TimeServiceAlarmBusObj* alarmBusObj = new TimeServiceAlarmBusObj(alarmObjPath);
    status = alarmBusObj->init(alarm, m_Description, m_Language, m_Translator);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to initialize AlarmBusObj, objectPath: '%s'", m_ObjectPath.c_str()));

        m_AlarmFactory->handleError(alarm);

        delete alarmBusObj;
        replyError(status, NULL, msg);
        return;
    }

    status = alarm->setAlarmBusObj(alarmBusObj);
    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to initialize the new Alarm, objectPath: '%s'", m_ObjectPath.c_str()));

        m_AlarmFactory->handleError(alarm);

        delete alarmBusObj;
        replyError(status, NULL, msg);
        return;
    }

    ajn::MsgArg replyArg[1];
    status = tsAlarmUtility::marshalObjectPath(replyArg[0], alarmObjPath);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to marshal NewAlarm response, objectPath: '%s'", m_ObjectPath.c_str()));

        m_AlarmFactory->handleError(alarm);

        replyError(status, NULL, msg);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call MethodReply, objectPath: '%s'", m_ObjectPath.c_str()));

        m_AlarmFactory->handleError(alarm);
        return;
    }

    QCC_DbgHLPrintf(("New Alarm has been created '%s'", alarmObjPath.c_str()));
}

//Handle Delete Alarm
void TimeServiceAlarmFactoryBusObj::handleDeleteAlarm(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling DeleteAlarm() method call, objectPath: '%s'", m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        replyError(ER_FAIL, NULL, msg);
        return;
    }

    bus->EnableConcurrentCallbacks();

    const ajn::MsgArg* msgArg = msg->GetArg(0);
    if (!msgArg) {

        QCC_LogError(ER_BAD_ARG_COUNT, ("Failed to read objectPath to be deleted, objPath: '%s'", m_ObjectPath.c_str()));
        replyError(ER_BAD_ARG_COUNT, NULL, msg);
        return;
    }

    qcc::String alarmObjPath;
    QStatus status = tsAlarmUtility::unmarshalObjectPath(*msgArg, &alarmObjPath);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to unmarhal objectPath to be deleted, objPath: '%s'", m_ObjectPath.c_str()));
        replyError(status, NULL, msg);
        return;
    }

    tsConsts::ErrorInfo errorInfo;
    status = m_AlarmFactory->deleteAlarm(alarmObjPath, &errorInfo);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to delete Alarm, objPath: '%s'", alarmObjPath.c_str()));
        replyError(status, &errorInfo, msg);
        return;
    }

    status = MethodReply(msg);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call MethodReply, objectPath: '%s'", m_ObjectPath.c_str()));
        return;
    }

    QCC_DbgHLPrintf(("The Alarm '%s' has been deleted successfully", alarmObjPath.c_str()));
}

//Reply Error with description
void TimeServiceAlarmFactoryBusObj::replyError(QStatus status, const tsConsts::ErrorInfo* errorInfo, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus replyStatus = ER_OK;

    if (!errorInfo || errorInfo->errorName.length() == 0) {

        replyStatus = MethodReply(msg, status);
    } else {

        replyStatus = MethodReply(msg, errorInfo->errorName.c_str(), errorInfo->errorDescription.c_str());
    }

    if (replyStatus != ER_OK) {

        QCC_LogError(replyStatus, ("Failed to call MethodReply with error, objectPath: '%s'", m_ObjectPath.c_str()));
    }
}
