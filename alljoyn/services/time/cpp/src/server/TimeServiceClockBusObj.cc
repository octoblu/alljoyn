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

#include "TimeServiceClockBusObj.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceClockUtility.h"
#include <alljoyn/time/TimeServiceConstants.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClockBusObj::TimeServiceClockBusObj(qcc::String const& objectPath) : BusObject(objectPath.c_str()), m_Clock(NULL),
    m_ObjectPath(objectPath)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceClockBusObj::~TimeServiceClockBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    release();
}

//Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
QStatus TimeServiceClockBusObj::init(TimeServiceServerClock* clock)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    QCC_DbgHLPrintf(("Initializing Clock '%s'", m_ObjectPath.c_str()));

    if (!clock) {

        QCC_LogError(ER_FAIL, ("Clock is undefined"));
        return ER_FAIL;
    }

    m_Clock = clock;

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    status = addClockInterface(bus);
    if (status != ER_OK) {

        return status;
    }

    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to register Clock BusObject"));
        return status;
    }

    return ER_OK;
}

//Releases object resources
void TimeServiceClockBusObj::release()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgHLPrintf(("Releasing Clock '%s'", m_ObjectPath.c_str()));

    m_Clock = NULL;
    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        return;
    }

    bus->UnregisterBusObject(*this);
}

//Returns object path of this Clock
qcc::String const& TimeServiceClockBusObj::getObjectPath() const
{

    return m_ObjectPath;
}

//Handle property Get
QStatus TimeServiceClockBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'GET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 == strcmp(tsConsts::CLOCK_IFACE.c_str(), interfaceName) &&
        0 == strcmp(tsClockUtility::IFACE_PROP_VERSION.c_str(), propName)) {

        return handleGetVersion(msgArg);
    }

    if (0 == strcmp(tsClockUtility::IFACE_PROP_DATE_TIME.c_str(), propName)) {

        return handleGetDateTime(msgArg);
    }

    if (0 == strcmp(tsClockUtility::IFACE_PROP_IS_SET.c_str(), propName)) {

        return handleGetIsSet(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

//Handle Set Property
QStatus TimeServiceClockBusObj::Set(const char* interfaceName, const char* propName, MsgArg& val)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    BusAttachment* bus = TimeServiceServer::getInstance()->getBusAttachment();
    if (!bus) {

        QCC_LogError(ER_FAIL, ("TimeService Server hasn't been initialized"));
        return ER_FAIL;
    }

    bus->EnableConcurrentCallbacks();

    QCC_DbgPrintf(("Received 'SET' request for: '%s'::'%s', objectPath: '%s'", interfaceName, propName, m_ObjectPath.c_str()));

    if (0 == strcmp(tsClockUtility::IFACE_PROP_DATE_TIME.c_str(), propName)) {

        return handleSetDateTime(val);
    }

    QCC_LogError(ER_BUS_PROPERTY_ACCESS_DENIED, ("The property '%s' has read only permission", propName));
    return ER_BUS_PROPERTY_ACCESS_DENIED;
}

//Handle Get Version
QStatus TimeServiceClockBusObj::handleGetVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    msgArg.typeId   = ALLJOYN_UINT16;
    msgArg.v_uint16 = tsConsts::CLOCK_IFACE_VERSION;

    return ER_OK;
}

//Handle Get DateTime
QStatus TimeServiceClockBusObj::handleGetDateTime(MsgArg& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.DateTime() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    TimeServiceDateTime dateTime = m_Clock->getDateTime();
    if (!dateTime.isValid()) {

        QCC_LogError(ER_INVALID_DATA, ("Failed to execute Get.DateTime(), DateTime is invalid, objectPath: '%s'", m_ObjectPath.c_str()));
        return ER_INVALID_DATA;
    }

    QStatus status = tsClockUtility::marshalDateTime(msg, dateTime);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal DateTime property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle GetIsSet
QStatus TimeServiceClockBusObj::handleGetIsSet(MsgArg& msg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Get.IsSet() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    QStatus status = tsClockUtility::marshalIsSet(msg, m_Clock->isSet());

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal IsSet property, objectPath: '%s'", m_ObjectPath.c_str()));
    }

    return status;
}

//Handle Set DateTime
QStatus TimeServiceClockBusObj::handleSetDateTime(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));
    QCC_DbgPrintf(("Handling Set.DateTime() property call, objectPath: '%s'", m_ObjectPath.c_str()));

    TimeServiceDateTime dateTime;
    QStatus status = tsClockUtility::unmarshalDateTime(msgArg, &dateTime);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to execute Set.DateTime() property"));
        return status;
    }

    QCC_DbgPrintf(("Delivering DateTime property to the application, objectPath: '%s'", m_ObjectPath.c_str()));
    m_Clock->setDateTime(dateTime);

    return status;
}

//Create and add interface
QStatus TimeServiceClockBusObj::addClockInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, m_ObjectPath.c_str()));

    QStatus status = ER_OK;

    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*> (bus->GetInterface(tsConsts::CLOCK_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsClockUtility::createInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create Clock interface"));
            return status;
        }
    }

    status = AddInterface(*ifaceDesc, ANNOUNCED);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add Clock interface"));
        return status;
    }

    return status;
}
