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

#include "ConfigServiceListenerImpl.h"
#include <iostream>

using namespace ajn;
using namespace services;

ConfigServiceListenerImpl::ConfigServiceListenerImpl(AboutDataStore& store, BusAttachment& bus,
                                                     CommonBusListener& busListener, OnboardingControllerImpl& obController) :
    ConfigService::Listener(), m_AboutDataStore(&store), m_Bus(&bus), m_BusListener(&busListener), m_OnboardingController(&obController)
{
}

QStatus ConfigServiceListenerImpl::Restart()
{
    std::cout << "Restart has been called !!!" << std::endl;
    return ER_OK;
}

QStatus ConfigServiceListenerImpl::FactoryReset()
{
    std::cout << "FactoryReset has been called!!!" << std::endl;
    m_AboutDataStore->FactoryReset();
    std::cout << "Clearing Key Store" << std::endl;
    m_Bus->ClearKeyStore();
    std::cout << "Calling Offboard" << std::endl;
    m_OnboardingController->Offboard();
    return ER_OK;
}

QStatus ConfigServiceListenerImpl::SetPassphrase(const char* daemonRealm, size_t passcodeSize, const char* passcode, ajn::SessionId sessionId)
{
    qcc::String passCodeString(passcode, passcodeSize);
    std::cout << "SetPassphrase has been called daemonRealm=" << daemonRealm << " passcode="
              << passCodeString.c_str() << " passcodeLength=" << passcodeSize << std::endl;

    PersistPassword(daemonRealm, passCodeString.c_str());

    std::cout << "Clearing Key Store" << std::endl;
    m_Bus->ClearKeyStore();
    m_Bus->EnableConcurrentCallbacks();

    std::vector<SessionId> sessionIds = m_BusListener->getSessionIds();
    for (size_t i = 0; i < sessionIds.size(); i++) {
        if (sessionIds[i] == sessionId) {
            continue;
        }
        m_Bus->LeaveSession(sessionIds[i]);
        std::cout << "Leaving session with id: " << sessionIds[i];
    }
    m_AboutDataStore->write();
    return ER_OK;
}

ConfigServiceListenerImpl::~ConfigServiceListenerImpl()
{
}

void ConfigServiceListenerImpl::PersistPassword(const char* daemonRealm, const char* passcode)
{
    ajn::MsgArg argPasscode;
    ajn::MsgArg argDaemonrealm;
    argPasscode.Set("s", passcode);
    argDaemonrealm.Set("s", daemonRealm);
    m_AboutDataStore->SetField("Passcode", argPasscode);
    m_AboutDataStore->SetField("Daemonrealm", argDaemonrealm);
}
