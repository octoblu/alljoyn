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

#include "HandleOnboardingSignals.h"
#include "alljoyn/onboarding/LogModule.h"

using namespace ajn::services;


HandleOnboardingSignals::HandleOnboardingSignals(ajn::BusAttachment* bus, OnboardingClientListener* listener) :
    m_bus(bus), m_userListener(listener), m_asyncTaskQueue(NULL)
{
    QCC_DbgTrace(("ConnectionResultHandler::%s", __FUNCTION__));
}

HandleOnboardingSignals::~HandleOnboardingSignals()
{
    m_asyncTaskQueue->Stop();
    delete m_asyncTaskQueue;
}

HandleOnboardingSignals::HandleOnboardingSignals(const HandleOnboardingSignals& handler) :
    m_bus(NULL), m_userListener(NULL), m_asyncTaskQueue(NULL)
{
}

HandleOnboardingSignals& HandleOnboardingSignals::operator=(const HandleOnboardingSignals& handler)
{
    return *this;
}

QStatus HandleOnboardingSignals::RegisterConnectionResultSignalHandler(const ajn::InterfaceDescription::Member* connectionResultSignalMethod)
{
    // the incoming signals are placed in a queue so that the alljoyn thread
    // is released right away
    m_asyncTaskQueue = new AsyncTaskQueue(this);
    m_asyncTaskQueue->Start();

    QStatus status = m_bus->RegisterSignalHandler(this,
                                                  static_cast<MessageReceiver::SignalHandler>(&HandleOnboardingSignals::ConnectionResultSignal),
                                                  connectionResultSignalMethod,
                                                  0);

    return status;
}


QStatus HandleOnboardingSignals::UnregisterConnectionResultSignalHandler(const ajn::InterfaceDescription::Member* connectionResultSignalMethod)
{
    QStatus status = m_bus->UnregisterSignalHandler(this,
                                                    static_cast<MessageReceiver::SignalHandler>(&HandleOnboardingSignals::ConnectionResultSignal),
                                                    connectionResultSignalMethod,
                                                    NULL);

    m_asyncTaskQueue->Stop();
    delete m_asyncTaskQueue;

    return status;
}


void HandleOnboardingSignals::ConnectionResultSignal(const ajn::InterfaceDescription::Member* member,
                                                     const char* srcPath, ajn::Message& message)
{

    QCC_DbgTrace(("ConnectionResultHandler::%s", __FUNCTION__));

    QCC_DbgPrintf(("received signal interface=%s method=%s", message->GetInterface(), message->GetMemberName()));

    if (strcmp(message->GetInterface(), "org.alljoyn.Onboarding") != 0 || strcmp(message->GetMemberName(), "ConnectionResult") != 0) {
        QCC_DbgPrintf(("This is not the signal we are looking for"));
        return;
    }

    SignalMesssage* originalMessage = new SignalMesssage(message);
    m_asyncTaskQueue->Enqueue(originalMessage);
}

///////////////// AsyncTask Methods ////////////////////////
void HandleOnboardingSignals::OnEmptyQueue()
{
}

void HandleOnboardingSignals::OnTask(TaskData const* taskdata)
{
    SignalMesssage const* wrappedMessage = static_cast<SignalMesssage const*>(taskdata);
    ajn::Message* message = (ajn::Message*) &wrappedMessage->m_signalMessage;

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    QStatus status;

    short connectionResultCode;
    char*  connectionResultMessage;

    message->unwrap()->GetArgs(numArgs, args);

    if (numArgs == 1) {
        status = args[0].Get("(ns)", &connectionResultCode, &connectionResultMessage);
    } else {
        status = ER_BAD_ARG_COUNT;
    }

    if (status != ER_OK) {
        QCC_DbgPrintf(("unmarshalling ConnectionResult Signal, failed"));
        return;
    }

    qcc::String resultMessage(connectionResultMessage);
    m_userListener->ConnectionResultSignalReceived(connectionResultCode, resultMessage);
}

