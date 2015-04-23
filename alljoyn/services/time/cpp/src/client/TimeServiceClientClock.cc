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

#include <alljoyn/time/TimeServiceClientClock.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "TimeAuthoritySignalHandler.h"
#include "../common/TimeServiceClockUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientClock::TimeServiceClientClock(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    TimeServiceClientBase(tsClient, objectPath),
    m_IsAuthority(false),
    m_ClockIface(NULL),
    m_TimeAuthIface(NULL),
    m_TimeAuthorityHandler(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    initInterfaces(tsConsts::CLOCK_IFACE, &m_ClockIface, &tsClockUtility::createInterface);
    initInterfaces(tsConsts::TIME_AUTHORITY_IFACE, &m_TimeAuthIface, &tsClockUtility::createAuthorityInterface);
}

//Destructor
TimeServiceClientClock::~TimeServiceClientClock()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Release object resources
void TimeServiceClientClock::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgHLPrintf(("Releasing ClientClock, objectPath: '%s'", getObjectPath().c_str()));

    if (m_IsAuthority) {

        unRegisterTimeAuthorityHandler();
    }

    m_ClockIface    = NULL;
    m_TimeAuthIface = NULL;
}

//Whether the server clock is a Time Authority
bool TimeServiceClientClock::isAuthority() const
{

    return m_IsAuthority;
}

//Retrieve DateTime from the server Clock object.
QStatus TimeServiceClientClock::retrieveDateTime(TimeServiceDateTime* dateTime)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!dateTime) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined DateTime"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_ClockIface, tsClockUtility::IFACE_PROP_DATE_TIME, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsClockUtility::unmarshalDateTime(msgArg, dateTime);
}

//Set DateTime to the server Clock object
QStatus TimeServiceClientClock::setDateTime(const TimeServiceDateTime& dateTime)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting DateTime, objectPath: '%s'", m_ObjectPath.c_str()));

    if (!dateTime.isValid()) {

        QCC_LogError(ER_BAD_ARG_1, ("DateTime is not a valid object, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = tsClockUtility::marshalDateTime(msgArg, dateTime);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal DateTime, ObjectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    return setServerProperty(m_ClockIface, tsClockUtility::IFACE_PROP_DATE_TIME, msgArg);
}

//Retrieve IsSet from the server Clock object.
QStatus TimeServiceClientClock::retrieveIsSet(bool* isSet)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!isSet) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined isSet"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_ClockIface, tsClockUtility::IFACE_PROP_IS_SET, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsClockUtility::unmarshalIsSet(msgArg, isSet);
}

//Retrieves Authority Type of the server clock.
QStatus TimeServiceClientClock::retrieveAuthorityType(tsConsts::ClockAuthorityType* authType)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_IsAuthority) {

        QCC_LogError(ER_FAIL, ("This Clock '%s' is not a Time Authority", m_ObjectPath.c_str()));
        return ER_FAIL;
    }

    if (!authType) {

        QCC_LogError(ER_BAD_ARG_1, ("Undefined authType"));
        return ER_BAD_ARG_1;
    }

    MsgArg msgArg;
    QStatus status = retrieveServerProperty(m_TimeAuthIface, tsClockUtility::IFACE_AUTH_PROP_TYPE, msgArg);

    if (status != ER_OK) {

        return status;
    }

    return tsClockUtility::unmarshalAuthorityType(msgArg, authType);
}

//Set TimeAuthority
void TimeServiceClientClock::setAuthority(bool isAuthority)
{

    m_IsAuthority = isAuthority;
}

//Register TimeAuthorityHandler to receive TimeSync events
QStatus TimeServiceClientClock::registerTimeAuthorityHandler(TimeAuthorityHandler* handler)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_IsAuthority) {

        QCC_LogError(ER_FAIL, ("Clock is not authority."));
        return ER_FAIL;
    }

    if (!handler) {

        QCC_LogError(ER_FAIL, ("No handler given."));
        return ER_FAIL;
    }

    m_TimeAuthorityHandler = handler;
    return TimeAuthoritySignalHandler::getInstance()->registerClock(this);
}

//Unregister TimeAuthorityHandler to stop receiving TimeSync
void TimeServiceClientClock::unRegisterTimeAuthorityHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_TimeAuthorityHandler) {

        QCC_DbgPrintf(("Can not unregister TimeAuthorityHandler, it hasn't been registered before"));
        return;
    }

    TimeAuthoritySignalHandler::getInstance()->unRegisterClock(this);
    m_TimeAuthorityHandler = NULL;
}

//TimeAuthorityHandler that has been registered or NULL
TimeAuthorityHandler* TimeServiceClientClock::getTimeAuthorityHandler()
{

    return m_TimeAuthorityHandler;
}
