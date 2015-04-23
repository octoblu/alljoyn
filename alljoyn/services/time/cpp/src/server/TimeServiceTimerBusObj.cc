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

#include <alljoyn/AllJoynStd.h>
#include <alljoyn/time/TimeServiceTimerBusObj.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceUtility.h"
#include "../common/TimeServiceTimerUtility.h"
#include <alljoyn/time/TimeServiceConstants.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceTimerBusObj::TimeServiceTimerBusObj(qcc::String const& objectPath) : BusObject(objectPath.c_str()), m_Timer(NULL),
    m_ObjectPath(objectPath), m_TimerEventSignalMethod(NULL), m_RunStateChangedSignalMethod(NULL), m_IsInitialized(false),
    m_IsAnnounced(false)

{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceTimerBusObj::~TimeServiceTimerBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}


//Initialize the Bus Object. Register it on the BusAttachment
QStatus TimeServiceTimerBusObj::init(TimeServiceServerTimer* Timer, const std::vector<qcc::String>& notAnnounced)
{

    return init(Timer, notAnnounced, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceTimerBusObj::init(TimeServiceServerTimer* Timer, const std::vector<qcc::String>& notAnnounced,
                                     const qcc::String& description, const qcc::String& language, Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    m_IsAnnounced  = true;
    QStatus status = init(Timer, description, language, translator);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed to initialize timer. Object:'%s'", m_ObjectPath.c_str()));
        return status;
    }

    //org.allseen.Timer interface has AllSeenIntrospectable descriptions, so we add the AllSeenIntrospectable interface
    //to the announcement to support Events & Actions feature
    status = tsUtility::setInterfaceAnnounce(this, bus, org::allseen::Introspectable::InterfaceName, true);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed to add org::allseen::Introspectable interface, Object:'%s'", m_ObjectPath.c_str()));
        return status;
    }

    //Subtract from the interfaces to be announced those that shouldn't be announced
    tsUtility::subtract(this, bus, notAnnounced);

    return status;
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceTimerBusObj::init(TimeServiceServerTimer* Timer, const qcc::String& description, const qcc::String& language,
                                     Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    QCC_DbgHLPrintf(("Initializing Timer '%s'", m_ObjectPath.c_str()));

    if (!Timer) {

        QCC_LogError(ER_FAIL, ("Timer is undefined"));
        return ER_FAIL;
    }

    m_Timer = Timer;

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    status = createTimerInterface(bus);
    if (status != ER_OK) {

        return status;
    }

    status = createCustomInterfaceHook(bus);
    if (status != ER_OK) {

        return status;
    }

    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to register Timer BusObject"));
        return status;
    }

    tsUtility::setIntrospectionDescription(this, description, language, translator);

    m_IsInitialized = true;

    return status;
}

//Releases object resources
void TimeServiceTimerBusObj::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing Timer '%s'", m_ObjectPath.c_str()));

    m_Timer = NULL;
    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        return;
    }

    bus->UnregisterBusObject(*this);

    m_IsInitialized = false;
}

//TRUE if the object has been initialized by the call to the init() method
bool TimeServiceTimerBusObj::isInitialized() const
{

    return m_IsInitialized;
}

//Returns object path of this Timer
qcc::String const& TimeServiceTimerBusObj::getObjectPath() const
{

    return m_ObjectPath;
}

//Creates Timer interface if it hasn't been created and adds it to the BusObject
QStatus TimeServiceTimerBusObj::createTimerInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;
    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*>(bus->GetInterface(tsConsts::TIMER_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsTimerUtility::createInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create Timer interface"));
            return status;
        }
    }

    status = addTimerInterface(*ifaceDesc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add Timer interface"));
        return status;
    }

    m_TimerEventSignalMethod      = ifaceDesc->GetMember(tsTimerUtility::IFACE_SIG_TIMER_EVENT.c_str());
    m_RunStateChangedSignalMethod = ifaceDesc->GetMember(tsTimerUtility::IFACE_SIG_TIMER_RUNSTATECHANGED.c_str());

    const ajn::InterfaceDescription::Member* startMethod;
    const ajn::InterfaceDescription::Member* pauseMethod;
    const ajn::InterfaceDescription::Member* resetMethod;

    startMethod     = ifaceDesc->GetMember(tsTimerUtility::IFACE_METHOD_START.c_str());
    pauseMethod     = ifaceDesc->GetMember(tsTimerUtility::IFACE_METHOD_PAUSE.c_str());
    resetMethod     = ifaceDesc->GetMember(tsTimerUtility::IFACE_METHOD_RESET.c_str());

    status = AddMethodHandler(startMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceTimerBusObj::handleStart));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add startMethod method handler"));
        return status;
    }

    status = AddMethodHandler(pauseMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceTimerBusObj::handlePause));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add pauseMethod method handler"));
        return status;
    }

    status = AddMethodHandler(resetMethod,
                              static_cast<MessageReceiver::MethodHandler>(&TimeServiceTimerBusObj::handleReset));
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add resetMethod method handler"));
        return status;
    }
    return status;
}

//Hook method to allow creating custom interface
QStatus TimeServiceTimerBusObj::createCustomInterfaceHook(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    return ER_OK;
}

//Add the interface to this object
QStatus TimeServiceTimerBusObj::addTimerInterface(const InterfaceDescription& iface)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
#if !defined(NDEBUG)
    const char* ifaceName = iface.GetName();
#endif
    QStatus status = AddInterface(iface, m_IsAnnounced ? ANNOUNCED : UNANNOUNCED);
    if (status != ER_OK) {
#if !defined(NDEBUG)
        QCC_LogError(status, ("Failed to add the Interface: '%s', objectPath: '%s'", ifaceName, m_ObjectPath.c_str()));
#endif
        return status;
    }

    return status;
}

//Callback for Alljoyn when GetProperty is called on this BusObject
QStatus TimeServiceTimerBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'GET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 != strcmp(tsConsts::TIMER_IFACE.c_str(), interfaceName)) {

        return ER_BUS_UNKNOWN_INTERFACE;
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_VERSION.c_str(), propName)) {

        return handleGetVersion(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_INTERVAL.c_str(), propName)) {

        return handleGetInterval(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_TIMELEFT.c_str(), propName)) {

        return handleGetTimeLeft(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_TITLE.c_str(), propName)) {

        return handleGetTitle(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_ISRUNNING.c_str(), propName)) {

        return handleGetIsRunning(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_REPEAT.c_str(), propName)) {

        return handleGetRepeat(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}


//Callback for Alljoyn when SetProperty is called on this BusObject
QStatus TimeServiceTimerBusObj::Set(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'SET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 != strcmp(tsConsts::TIMER_IFACE.c_str(), interfaceName)) {

        return ER_BUS_UNKNOWN_INTERFACE;
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_INTERVAL.c_str(), propName)) {

        return handleSetInterval(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_TITLE.c_str(), propName)) {

        return handleSetTitle(msgArg);
    }

    if (0 == strcmp(tsTimerUtility::IFACE_PROP_REPEAT.c_str(), propName)) {

        return handleSetRepeat(msgArg);
    }

    return ER_BUS_PROPERTY_ACCESS_DENIED;
}

//Handle Get Version request
QStatus TimeServiceTimerBusObj::handleGetVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    msgArg.typeId       = ALLJOYN_UINT16;
    msgArg.v_uint16     = tsConsts::TIMER_IFACE_VERSION;

    return ER_OK;
}

//Handle Get interval request
QStatus TimeServiceTimerBusObj::handleGetInterval(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.interval() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const TimeServicePeriod interval = m_Timer->getInterval();
    if (!interval.isValid()) {

        QCC_LogError(ER_INVALID_DATA, ("Failed to execute Get.interval(), Schedule is invalid, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_INVALID_DATA;
    }

    QStatus status = tsTimerUtility::marshalPeriod(msgArg, interval);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal interval property, objectPath: '%s'", m_ObjectPath.c_str()));
    }


    return status;
}

//Handle Get time left request
QStatus TimeServiceTimerBusObj::handleGetTimeLeft(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.TimeLeft() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const TimeServicePeriod interval = m_Timer->getTimeLeft();
    if (!interval.isValid()) {

        QCC_LogError(ER_INVALID_DATA, ("Failed to execute Get.TimeLeft(), Schedule is invalid, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_INVALID_DATA;
    }

    QStatus status = tsTimerUtility::marshalPeriod(msgArg, interval);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal TimeLeft property, objectPath: '%s'", m_ObjectPath.c_str()));
    }


    return status;
}

//Handle Get Title request
QStatus TimeServiceTimerBusObj::handleGetTitle(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.Title() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const qcc::String title  = m_Timer->getTitle();
    QStatus status           = tsTimerUtility::marshalTitle(msgArg, title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Title property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Get is running request
QStatus TimeServiceTimerBusObj::handleGetIsRunning(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling isRunning() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const bool isRunning = m_Timer->isRunning();
    QStatus status       = tsTimerUtility::marshalBoolean(msgArg, isRunning);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal isRunning property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Get repeat request
QStatus TimeServiceTimerBusObj::handleGetRepeat(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.Repeat() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const uint16_t repeat = m_Timer->getRepeat();
    QStatus status        = tsTimerUtility::marshalRepeat(msgArg, repeat);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Repeat property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Set interval request
QStatus TimeServiceTimerBusObj::handleSetInterval(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Interval() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    TimeServicePeriod interval;
    QStatus status = tsTimerUtility::unmarshalPeriod(msgArg, &interval);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Interval() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Interval property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    status = m_Timer->setInterval(interval);

    if (status != ER_OK) {

        QCC_LogError(status, ("Application has failed to set the Interval property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Get Title request
QStatus TimeServiceTimerBusObj::handleSetTitle(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Title() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    qcc::String title;
    QStatus status = tsTimerUtility::unmarshalTitle(msgArg, &title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Title() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Title property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Timer->setTitle(title);

    return status;
}

//Handle Set Repeat request
QStatus TimeServiceTimerBusObj::handleSetRepeat(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Repeat() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    uint16_t repeat;
    QStatus status = tsTimerUtility::unmarshalRepeat(msgArg, &repeat);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Repeat() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Repeat property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Timer->setRepeat(repeat);

    return status;
}

/**
 * Emit TimerReached signal
 */
QStatus TimeServiceTimerBusObj::sendTimerEvent()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    if (!m_TimerEventSignalMethod) {

        QCC_LogError(ER_FAIL, ("TimerEvent signal method was not set, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_FAIL;
    }

    QCC_DbgPrintf(("Emitting TimerEvent signal, TTL: '%d', objPath: '%s'", tsTimerUtility::TIMER_REACHED_TTL_SECONDS, m_ObjectPath.c_str()));

    uint8_t flags  =  ALLJOYN_FLAG_SESSIONLESS;
    QStatus status = Signal(NULL, 0, *m_TimerEventSignalMethod, NULL, 0, tsTimerUtility::TIMER_REACHED_TTL_SECONDS, flags);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to send TimerEvent signal, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return status;
}

/**
 * Emit RunStateChanged signal
 */
QStatus TimeServiceTimerBusObj::sendRunStateChanged(bool isRunning)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    if (!m_RunStateChangedSignalMethod) {

        QCC_LogError(ER_FAIL, ("RunStateChanged signal method was not set, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_FAIL;
    }

    QCC_DbgPrintf(("Emitting RunStateChanged signal, objPath: '%s'", m_ObjectPath.c_str()));

    MsgArg arg;
    tsTimerUtility::marshalBoolean(arg, isRunning);

    SessionId sId = TimeServiceServer::getInstance()->getSessionId();

    if (0 == sId) {

        //From the application point of view it's not a problem, it just means that currently there is no active session
        QCC_LogError(ER_BUS_NO_SESSION, ("Unable to send RunStateChanged signal, no session is established, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_BUS_NO_SESSION;
    }

    QStatus status = Signal(NULL,  sId, *m_RunStateChangedSignalMethod, &arg, 1);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to send RunStateChanged signal, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return status;
}

/**
 * Handle start method
 */
void TimeServiceTimerBusObj::handleStart(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Handling handleStart() method call, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Timer->start();
}

/**
 * Handle pause method
 */
void TimeServiceTimerBusObj::handlePause(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Handling handlePause() method call, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Timer->pause();
}

/**
 * Handle Reset method
 */
void TimeServiceTimerBusObj::handleReset(const InterfaceDescription::Member* member, Message& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Handling handleReset() method call, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Timer->reset();

    QStatus status = MethodReply(msg);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call MethodReply, objectPath: '%s'", m_ObjectPath.c_str()));
        return;
    }
}
