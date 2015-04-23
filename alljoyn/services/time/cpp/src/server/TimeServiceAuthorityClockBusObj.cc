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

#include "TimeServiceAuthorityClockBusObj.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceServer.h>
#include "../common/TimeServiceClockUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceAuthorityClockBusObj::TimeServiceAuthorityClockBusObj(qcc::String const& objectPath,
                                                                 tsConsts::ClockAuthorityType type) :
    TimeServiceClockBusObj(objectPath), m_TimeSyncSignalMethod(NULL), m_Type(type)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objectPath.c_str()));
}

//Destructor
TimeServiceAuthorityClockBusObj::~TimeServiceAuthorityClockBusObj()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));
}

//Add TimeAuthority interface
QStatus TimeServiceAuthorityClockBusObj::addClockInterface(BusAttachment* bus)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));

    QStatus status = ER_OK;

    status = TimeServiceClockBusObj::addClockInterface(bus);

    if (status != ER_OK) {

        return status;
    }

    InterfaceDescription* ifaceDesc = const_cast<InterfaceDescription*>(bus->GetInterface(tsConsts::TIME_AUTHORITY_IFACE.c_str()));

    if (!ifaceDesc) {

        status = tsClockUtility::createAuthorityInterface(bus, &ifaceDesc);
        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to create TimeAuthority interface"));
            return status;
        }
    }

    status = AddInterface(*ifaceDesc, ANNOUNCED);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to add TimeAuthority interface"));
        return status;
    }

    m_TimeSyncSignalMethod = ifaceDesc->GetMember(tsClockUtility::IFACE_AUTH_SIG_TIME_SYNC.c_str());

    return status;
}

//Hanlde Get request
QStatus TimeServiceAuthorityClockBusObj::Get(const char* interfaceName, const char* propName, MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));

    QStatus status = TimeServiceClockBusObj::Get(interfaceName, propName, msgArg);

    //If status is ER_OK or any other type of failure, it means that SuperClass::Get method has handled the request and either
    //succeeded or failed so we return the status.
    //If SuperClass::Get returned ER_BUS_NO_SUCH_PROPERTY, it means that it didn't handle the request, so this class may handle it
    if (status != ER_BUS_NO_SUCH_PROPERTY) {

        return status;
    }

    //Check the GetVersion was called for this interface and not for the interface of the Super Class
    if (0 == strcmp(tsConsts::TIME_AUTHORITY_IFACE.c_str(), interfaceName) &&
        0 == strcmp(tsClockUtility::IFACE_AUTH_PROP_VERSION.c_str(), propName)) {

        return handleGetAuthVersion(msgArg);
    }
    if (0 == strcmp(tsClockUtility::IFACE_AUTH_PROP_TYPE.c_str(), propName)) {

        return handleGetAuthorityType(msgArg);
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

//Handle Get Version
QStatus TimeServiceAuthorityClockBusObj::handleGetAuthVersion(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));

    msgArg.typeId   = ALLJOYN_UINT16;
    msgArg.v_uint16 = tsConsts::TIME_AUTHORITY_IFACE_VERSION;

    return ER_OK;
}

//Handle Get.AuthorityType property request
QStatus TimeServiceAuthorityClockBusObj::handleGetAuthorityType(MsgArg& msgArg)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));
    QCC_DbgPrintf(("Handling Get.AuthorityType() property call, objectPath: '%s'", getObjectPath().c_str()));

    QStatus status = tsClockUtility::marshalAuthorityType(msgArg, m_Type);
    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal AuthorityType, objectPath: '%s'", getObjectPath().c_str()));
        return status;
    }

    return status;
}

//Send TimeSync signal
QStatus TimeServiceAuthorityClockBusObj::sendTimeSync()
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, getObjectPath().c_str()));

    if (!m_TimeSyncSignalMethod) {

        QCC_LogError(ER_FAIL, ("TimeSync signal method was not set, objectPath: '%s'", getObjectPath().c_str()));
        return ER_FAIL;
    }

    QCC_DbgPrintf(("Emitting TimeSync signal, TTL: '%d', objPath: '%s'", tsClockUtility::TIME_SYNC_TTL_SECONDS, getObjectPath().c_str()));

    uint8_t flags  =  ALLJOYN_FLAG_SESSIONLESS;
    QStatus status = Signal(NULL, 0, *m_TimeSyncSignalMethod, NULL, 0, tsClockUtility::TIME_SYNC_TTL_SECONDS, flags);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to send TimeSync signal, objectPath: '%s'", getObjectPath().c_str()));
        return status;
    }

    return status;
}
