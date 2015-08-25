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

#include <alljoyn/controlpanel/ControlPanelBusListener.h>
#include <iostream>
#include <algorithm>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {

ControlPanelBusListener::ControlPanelBusListener() :
    BusListener(), SessionPortListener(), SessionListener(), m_SessionPort(0)
{
}

ControlPanelBusListener::~ControlPanelBusListener()
{
}

void ControlPanelBusListener::setSessionPort(ajn::SessionPort sessionPort)
{
    m_SessionPort = sessionPort;
}

SessionPort ControlPanelBusListener::getSessionPort()
{
    return m_SessionPort;
}

bool ControlPanelBusListener::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts)
{
    if (sessionPort != m_SessionPort) {
        return false;
    }

    QCC_DbgPrintf(("Accepting JoinSessionRequest"));
    return true;
}

void ControlPanelBusListener::SessionJoined(SessionPort sessionPort, SessionId sessionId, const char* joiner)
{
    if (std::find(m_SessionIds.begin(), m_SessionIds.end(), sessionId) != m_SessionIds.end()) {
        return;
    }
    m_SessionIds.push_back(sessionId);
}

void ControlPanelBusListener::SessionMemberAdded(SessionId sessionId, const char* uniqueName)
{
    if (std::find(m_SessionIds.begin(), m_SessionIds.end(), sessionId) != m_SessionIds.end()) {
        return;
    }
    m_SessionIds.push_back(sessionId);
}

void ControlPanelBusListener::SessionMemberRemoved(SessionId sessionId, const char* uniqueName)
{
    std::vector<SessionId>::iterator it = std::find(m_SessionIds.begin(), m_SessionIds.end(), sessionId);
    if (it != m_SessionIds.end()) {
        m_SessionIds.erase(it);
    }
}

void ControlPanelBusListener::SessionLost(SessionId sessionId, SessionLostReason reason)
{
    std::vector<SessionId>::iterator it = std::find(m_SessionIds.begin(), m_SessionIds.end(), sessionId);
    if (it != m_SessionIds.end()) {
        m_SessionIds.erase(it);
    }
}

const std::vector<SessionId>& ControlPanelBusListener::getSessionIds() const
{
    return m_SessionIds;
}

} /* namespace services */
} /* namespace ajn */
