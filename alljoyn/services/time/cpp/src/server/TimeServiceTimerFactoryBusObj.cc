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

#include "TimeServiceTimerFactoryBusObj.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceTimerUtility.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceServerTimer.h>
#include <alljoyn/time/TimeServiceTimerBusObj.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceTimerFactoryBusObj::TimeServiceTimerFactoryBusObj(qcc::String const& objectPath) : BusObject(objectPath.c_str()), m_TimerFactory(NULL),
    m_ObjectPath(objectPath), m_Translator(NULL)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceTimerFactoryBusObj::~TimeServiceTimerFactoryBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceTimerFactoryBusObj::init(TimeServiceServerTimerFactory* TimerFactory)
{

    return init(TimerFactory, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceTimerFactoryBusObj::init(TimeServiceServerTimerFactory* TimerFactory, const qcc::String& description,
                                            const qcc::String& language, Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    QCC_DbgHLPrintf(("Initializing TimerFactory '%s'", m_ObjectPath.c_str()));

    if (!TimerFactory) {

        QCC_LogError(ER_FAIL, ("TimerFactory is undefined"));
        return ER_FAIL;
    }

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    m_TimerFactory       = TimerFactory;
    m_Description        = description;
    m_Language           = language;
    m_Translator         = translator;


    status = addTimerFactoryInterface(bus);
    if (status != ER_OK) {

        return status;
    }

    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to register TimerFactory BusObject"));
        return status;
    }

    return ER_OK;
}

//Releases object resources
void TimeServiceTimerFactoryBusObj::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing TimerFactory '%s'", m_ObjectPath.c_str()));

    m_TimerFactory     = NULL;
    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        return;
    }

    bus->UnregisterBusObject(*this);
}

//Returns object path of this Timer
qcc::String const& TimeServiceTimerFactoryBusObj::getObjectPath() const
{

    return m_ObjectPath;
}

//Adds the interface to this TimerFactory BusObject.
QStatus TimeServiceTimerFactoryBusObj::addTimerFactoryInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*> (bus->GetInterface(tsConsts::TIMER_FACTORY_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsTimerUtility::createFactoryInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create TimerFactory interface"));
            return status;
        }
    }

    status = AddInterface(*ifaceDesc, ANNOUNCED);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add TimerFactory interface"));
        return status;
    }

    const ajn::InterfaceDescription::Member* newTimerMethod;
    const ajn::InterfaceDescription::Member* deleteTimerMethod;

    newTimerMethod    = ifaceDesc->GetMember(tsTimerUtility::IFACE_FAC_METHOD_NEW_TIMER.c_str());
    deleteTimerMethod = ifaceDesc->GetMember(tsTimerUtility::IFACE_FAC_METHOD_DELETE_TIMER.c_str());

    status = AddMethodHandler(newTimerMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceTimerFactoryBusObj::handleNewTimer));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add NewTimer method handler"));
        return status;
    }

    status = AddMethodHandler(deleteTimerMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceTimerFactoryBusObj::handleDeleteTimer));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add DeleteTimer method handler"));
        return status;
    }

    return status;
}

//Callback for Alljoyn when GetProperty is called on this BusObject.
QStatus TimeServiceTimerFactoryBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    if (0 != strcmp(tsConsts::TIMER_FACTORY_IFACE.c_str(), interfaceName)) {

        return ER_BUS_NO_SUCH_INTERFACE;
    }

    QCC_DbgPrintf(("Received 'GET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));


    if (0 == strcmp(tsTimerUtility::IFACE_FAC_PROP_VERSION.c_str(), propName)) {

        return handleGetVersion(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

//Handles Get Version property request
QStatus TimeServiceTimerFactoryBusObj::handleGetVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    msgArg.typeId   = ALLJOYN_UINT16;
    msgArg.v_uint16 = tsConsts::TIMER_FACTORY_IFACE_VERSION;

    return ER_OK;
}

//Handle New Timer
void TimeServiceTimerFactoryBusObj::handleNewTimer(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling NewTimer() method call, objectPath: '%s'", m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        replyError(ER_FAIL, NULL, msg);
        return;
    }

    bus->EnableConcurrentCallbacks();

    TimeServiceServerTimer* timer = NULL;
    tsConsts::ErrorInfo error;

    QStatus status = m_TimerFactory->newTimer(&timer, &error);

    if (status != ER_OK || !timer) {

        QCC_LogError(status, ("Failed to create NewTimer, objPath: '%s'", m_ObjectPath.c_str()));
        replyError(status, &error, msg);
        return;
    }

    //Create child object path for this factory
    qcc::String TimerObjPath = m_ObjectPath + tsTimerUtility::OBJ_PATH_PREFIX;
    status = TimeServiceServer::getInstance()->generateObjectPath(&TimerObjPath);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to generate object path for the new Timer, objectPath: '%s'", m_ObjectPath.c_str()));

        m_TimerFactory->handleError(timer);
        replyError(status, NULL, msg);
        return;
    }

    TimeServiceTimerBusObj* timerBusObj = new TimeServiceTimerBusObj(TimerObjPath);
    status = timerBusObj->init(timer, m_Description, m_Language, m_Translator);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to initialize TimerBusObj, objectPath: '%s'", m_ObjectPath.c_str()));

        m_TimerFactory->handleError(timer);

        delete timerBusObj;
        replyError(status, NULL, msg);
        return;
    }

    status = timer->setTimerBusObj(timerBusObj);
    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to initialize the new Timer, objectPath: '%s'", m_ObjectPath.c_str()));

        m_TimerFactory->handleError(timer);

        delete timerBusObj;
        replyError(status, NULL, msg);
        return;
    }

    ajn::MsgArg replyArg[1];
    status = tsTimerUtility::marshalObjectPath(replyArg[0], TimerObjPath);

    if (status != ER_OK) {

        QCC_LogError(ER_FAIL, ("Failed to marshal NewTimer response, objectPath: '%s'", m_ObjectPath.c_str()));

        m_TimerFactory->handleError(timer);

        replyError(status, NULL, msg);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call MethodReply, objectPath: '%s'", m_ObjectPath.c_str()));

        m_TimerFactory->handleError(timer);
        return;
    }

    QCC_DbgHLPrintf(("New Timer has been created '%s'", TimerObjPath.c_str()));
}

//Handle Delete Timer
void TimeServiceTimerFactoryBusObj::handleDeleteTimer(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling DeleteTimer() method call, objectPath: '%s'", m_ObjectPath.c_str()));

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

    qcc::String TimerObjPath;
    QStatus status = tsTimerUtility::unmarshalObjectPath(*msgArg, &TimerObjPath);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to unmarhal objectPath to be deleted, objPath: '%s'", m_ObjectPath.c_str()));
        replyError(status, NULL, msg);
        return;
    }

    tsConsts::ErrorInfo errorInfo;
    status = m_TimerFactory->deleteTimer(TimerObjPath, &errorInfo);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to delete Timer, objPath: '%s'", TimerObjPath.c_str()));
        replyError(status, &errorInfo, msg);
        return;
    }

    status = MethodReply(msg);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call MethodReply, objectPath: '%s'", m_ObjectPath.c_str()));
        return;
    }

    QCC_DbgHLPrintf(("The Timer '%s' has been deleted successfully", TimerObjPath.c_str()));
}

//Reply Error with description
void TimeServiceTimerFactoryBusObj::replyError(QStatus status, const tsConsts::ErrorInfo* errorInfo, Message& msg)
{

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
