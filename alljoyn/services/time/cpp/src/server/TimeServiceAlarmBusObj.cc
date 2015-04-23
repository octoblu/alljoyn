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
#include <alljoyn/time/TimeServiceAlarmBusObj.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceUtility.h"
#include "../common/TimeServiceAlarmUtility.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceSchedule.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceAlarmBusObj::TimeServiceAlarmBusObj(qcc::String const& objectPath) : BusObject(objectPath.c_str()), m_Alarm(NULL),
    m_ObjectPath(objectPath), m_AlarmReachedSignalMethod(NULL), m_IsInitialized(false), m_IsAnnounced(false)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceAlarmBusObj::~TimeServiceAlarmBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}


//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceAlarmBusObj::init(TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced)
{

    return init(alarm, notAnnounced, qcc::String::Empty, qcc::String::Empty, NULL);
}

//Initialize the Bus Object. Register it on the BusAttachment.
QStatus TimeServiceAlarmBusObj::init(TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced,
                                     const qcc::String& description, const qcc::String& language, Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    m_IsAnnounced  = true;
    QStatus status = init(alarm, description, language, translator);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed to initialize alarm. Object:'%s'", m_ObjectPath.c_str()));
        return status;
    }

    //org.allseen.Alarm interface has AllSeenIntrospectable descriptions, so we add the AllSeenIntrospectable interface
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

//Initialize the Bus Object. Register it on the BusAttachment.
QStatus TimeServiceAlarmBusObj::init(TimeServiceServerAlarm* alarm, const qcc::String& description, const qcc::String& language,
                                     Translator* translator)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    QCC_DbgHLPrintf(("Initializing Alarm '%s'", m_ObjectPath.c_str()));

    if (!alarm) {

        QCC_LogError(ER_FAIL, ("Alarm is undefined"));
        return ER_FAIL;
    }

    m_Alarm = alarm;

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    status = createAlarmInterface(bus);
    if (status != ER_OK) {

        return status;
    }

    status = createCustomInterfaceHook(bus);
    if (status != ER_OK) {

        return status;
    }

    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to register Alarm BusObject"));
        return status;
    }

    tsUtility::setIntrospectionDescription(this, description, language, translator);

    m_IsInitialized = true;

    return status;
}

//Releases object resources
void TimeServiceAlarmBusObj::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing Alarm '%s'", m_ObjectPath.c_str()));

    m_Alarm = NULL;
    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        return;
    }

    bus->UnregisterBusObject(*this);

    m_IsInitialized = false;
}

//TRUE if the object has been initialized by the call to the init() method
bool TimeServiceAlarmBusObj::isInitialized() const
{

    return m_IsInitialized;
}

//Returns object path of this Alarm
qcc::String const& TimeServiceAlarmBusObj::getObjectPath() const
{

    return m_ObjectPath;
}

//Creates Alarm interface if it hasn't been created and adds it to the BusObject
QStatus TimeServiceAlarmBusObj::createAlarmInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;
    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*>(bus->GetInterface(tsConsts::ALARM_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsAlarmUtility::createInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create Alarm interface"));
            return status;
        }
    }

    status = addAlarmInterface(*ifaceDesc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add Alarm interface"));
        return status;
    }

    m_AlarmReachedSignalMethod = ifaceDesc->GetMember(tsAlarmUtility::IFACE_SIG_ALARM_REACHED.c_str());

    return status;
}

//Hook method to allow creating custom interface
QStatus TimeServiceAlarmBusObj::createCustomInterfaceHook(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    return ER_OK;
}

//Add the interface to this object
QStatus TimeServiceAlarmBusObj::addAlarmInterface(const InterfaceDescription& iface)
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
QStatus TimeServiceAlarmBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'GET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 != strcmp(tsConsts::ALARM_IFACE.c_str(), interfaceName)) {

        return ER_BUS_UNKNOWN_INTERFACE;
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_VERSION.c_str(), propName)) {

        return handleGetVersion(msgArg);
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_SCHEDULE.c_str(), propName)) {

        return handleGetSchedule(msgArg);
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_TITLE.c_str(), propName)) {

        return handleGetTitle(msgArg);
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_ENABLED.c_str(), propName)) {

        return handleGetEnabled(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}


//Callback for Alljoyn when SetProperty is called on this BusObject
QStatus TimeServiceAlarmBusObj::Set(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'SET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 != strcmp(tsConsts::ALARM_IFACE.c_str(), interfaceName)) {

        return ER_BUS_UNKNOWN_INTERFACE;
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_SCHEDULE.c_str(), propName)) {

        return handleSetSchedule(msgArg);
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_TITLE.c_str(), propName)) {

        return handleSetTitle(msgArg);
    }

    if (0 == strcmp(tsAlarmUtility::IFACE_PROP_ENABLED.c_str(), propName)) {

        return handleSetEnabled(msgArg);
    }

    return ER_BUS_PROPERTY_ACCESS_DENIED;
}

//Handle Get Version request
QStatus TimeServiceAlarmBusObj::handleGetVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    msgArg.typeId       = ALLJOYN_UINT16;
    msgArg.v_uint16     = tsConsts::ALARM_IFACE_VERSION;

    return ER_OK;
}

//Handle Get Schedule request
QStatus TimeServiceAlarmBusObj::handleGetSchedule(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QCC_DbgPrintf(("Handling Get.Schedule() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const TimeServiceSchedule schedule = m_Alarm->getSchedule();
    if (!schedule.isValid()) {

        QCC_LogError(ER_INVALID_DATA, ("Failed to execute Get.Schedule(), Schedule is invalid, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_INVALID_DATA;
    }

    QStatus status = tsAlarmUtility::marshalSchedule(msgArg, schedule);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Schedule property, objectPath: '%s'", m_ObjectPath.c_str()));
    }


    return status;
}

//Handle Get Title request
QStatus TimeServiceAlarmBusObj::handleGetTitle(MsgArg& msgArg)
{
    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.Title() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const qcc::String title  = m_Alarm->getTitle();
    QStatus status           = tsAlarmUtility::marshalTitle(msgArg, title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Title property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Get Enabled request
QStatus TimeServiceAlarmBusObj::handleGetEnabled(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.Enabled() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    const bool isEnabled = m_Alarm->isEnabled();
    QStatus status       = tsAlarmUtility::marshalEnabled(msgArg, isEnabled);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal Enabled property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Set Schedule request
QStatus TimeServiceAlarmBusObj::handleSetSchedule(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Schedule() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    TimeServiceSchedule schedule;
    QStatus status = tsAlarmUtility::unmarshalSchedule(msgArg, &schedule);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Schedule() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Schedule property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    status = m_Alarm->setSchedule(schedule);

    if (status != ER_OK) {

        QCC_LogError(status, ("Application has failed to set the Schedule property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Get Title request
QStatus TimeServiceAlarmBusObj::handleSetTitle(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Title() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    qcc::String title;
    QStatus status = tsAlarmUtility::unmarshalTitle(msgArg, &title);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Title() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Title property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Alarm->setTitle(title);

    return status;
}

//Handle Set Enabled request
QStatus TimeServiceAlarmBusObj::handleSetEnabled(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.Enabled() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    bool isEnabled;
    QStatus status = tsAlarmUtility::unmarshalEnabled(msgArg, &isEnabled);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.Enabled() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering Enabled property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Alarm->setEnabled(isEnabled);

    return status;
}

//Emit AlarmReached signal
QStatus TimeServiceAlarmBusObj::sendAlarmReached()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    if (!m_AlarmReachedSignalMethod) {

        QCC_LogError(ER_FAIL, ("AlarmReached signal method was not set, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_FAIL;
    }

    QCC_DbgPrintf(("Emitting AlarmReached signal, TTL: '%d', objPath: '%s'", tsAlarmUtility::ALARM_REACHED_TTL_SECONDS, m_ObjectPath.c_str()));

    uint8_t flags  =  ALLJOYN_FLAG_SESSIONLESS;
    QStatus status = Signal(NULL, 0, *m_AlarmReachedSignalMethod, NULL, 0, tsAlarmUtility::ALARM_REACHED_TTL_SECONDS, flags);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to send AlarmReached signal, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return status;
}
