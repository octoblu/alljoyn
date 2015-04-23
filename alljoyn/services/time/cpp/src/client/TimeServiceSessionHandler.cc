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

#include "TimeServiceSessionHandler.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceConstants.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceSessionHandler::TimeServiceSessionHandler(TimeServiceClient* tsClient) : JoinSessionAsyncCB(),
    SessionListener(),
    m_TsClient(tsClient),
    m_SessionListener(NULL),
    m_SessionId(0),
    m_IsConnected(false)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Destructor
TimeServiceSessionHandler::~TimeServiceSessionHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Release this object resources
void TimeServiceSessionHandler::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Releasing SessionHandler"));
    leaveSession();

    m_SessionListener   = NULL;
    m_TsClient          = NULL;
    m_IsConnected       = false;
}

//Whether the session has been established with the server side
bool TimeServiceSessionHandler::isConnected() const
{

    return m_IsConnected;
}

//Returns session id
ajn::SessionId TimeServiceSessionHandler::getSessionId() const
{

    return m_SessionId;
}

//Join session asynchronously with the server side
QStatus TimeServiceSessionHandler::joinSessionAsync(TimeServiceSessionListener* sessionListener)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!sessionListener) {

        QCC_LogError(ER_BAD_ARG_1, ("SessionListener is undefined"));
        return ER_BAD_ARG_1;
    }

    BusAttachment* bus = NULL;
    if (!objectsValid(&bus)) {

        return ER_FAIL;
    }

    m_SessionListener                 = sessionListener;
    const qcc::String& serverBusName  = m_TsClient->getServerBusName();

    if (m_IsConnected) {

        QCC_DbgPrintf(("The session is already joined with the server: '%s', SessionId: '%u'", serverBusName.c_str(), m_SessionId));
        return ER_ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED;
    }

    QCC_DbgPrintf(("Joining session with the server: '%s'", serverBusName.c_str()));

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    QStatus status = bus->JoinSessionAsync(serverBusName.c_str(), tsConsts::PORT, this, opts, this);

    if (status != ER_OK) {

        QCC_LogError(status, ("Join session call with the server: '%s' has failed", serverBusName.c_str()));
        return status;
    }

    return status;
}

//Leave the session
QStatus TimeServiceSessionHandler::leaveSession()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_IsConnected) {

        QCC_LogError(ER_FAIL, ("Can't execute leave session, the session is not connected"));
        m_SessionListener = NULL;

        return ER_FAIL;
    }

    if (!m_SessionId) {

        QCC_DbgPrintf(("Leave Session was called but SessionId is 0, returning OK"));
        m_SessionListener = NULL;

        return ER_OK;
    }

    BusAttachment* bus = NULL;
    if (!objectsValid(&bus)) {

        return ER_FAIL;
    }

    QStatus status = bus->LeaveSession(m_SessionId);

    QCC_DbgPrintf(("Leaving the session with the server: '%s', SessionId: '%u', Status: '%u'",
                   m_TsClient->getServerBusName().c_str(), m_SessionId, status));

    if (status == ER_OK) {

        m_IsConnected       = false;
        m_SessionListener   = NULL;
        m_SessionId         = 0;
    }

    return status;
}

//Called when JoinSessionAsync() completes.
void TimeServiceSessionHandler::JoinSessionCB(QStatus status, ajn::SessionId id, const ajn::SessionOpts& opts, void* context)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("JoinSessionCb was called, Status: '%u', SessionId: '%u'", status, id));

    if (status == ER_OK || status == ER_ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED) {

        m_SessionId    = id;
        m_IsConnected  = true;
    }

    BusAttachment* bus = NULL;
    if (!objectsValid(&bus)) {

        return;
    }

    bus->EnableConcurrentCallbacks();

    if (m_SessionListener) {

        QCC_DbgPrintf(("Invoking SessionListener.sessionJoined()"));
        m_SessionListener->sessionJoined(m_TsClient, status);
    }
}

//SessionLost called when a session is lost
void TimeServiceSessionHandler::SessionLost(ajn::SessionId sessionId, SessionLostReason reason)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("SessionLost has been called SessionId: '%u', reason: '%u'", sessionId, reason));

    m_IsConnected = false;
    m_SessionId   = 0;

    BusAttachment* bus = NULL;
    if (!objectsValid(&bus)) {

        return;
    }

    bus->EnableConcurrentCallbacks();

    if (m_SessionListener) {

        QCC_DbgPrintf(("Invoking SessionListener.sessionLost()"));
        m_SessionListener->sessionLost(m_TsClient, reason);
    }
}

//Check whether BusAttachmetn and TimeServiceClient are valid objects
bool TimeServiceSessionHandler::objectsValid(BusAttachment** bus)
{

    if (!m_TsClient) {

        QCC_LogError(ER_FAIL, ("Looks like TimeServiceSessionHandler has been previously released."));
        return false;
    }

    BusAttachment* localBus = m_TsClient->getBus();
    if (!localBus) {

        QCC_LogError(ER_FAIL, ("Looks like TimeServiceClient has been previously released, no BusAttachment."));
        return false;
    }

    *bus = localBus;
    return true;
}
