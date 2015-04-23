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


#include "TimeAuthoritySignalHandler.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceClockUtility.h"

namespace ajn {
namespace services {

const qcc::String TIME_SYNC_SIG_INTERFACE_MATCH = "interface='" + tsConsts::TIME_AUTHORITY_IFACE + "',type='signal',sessionless='t'";

TimeAuthoritySignalHandler* TimeAuthoritySignalHandler::s_Instance = NULL;

class TimeAuthoritySignalData : public TaskData {

  public:

    /**
     * Constructor
     * @param returnArgs MsgArg with the signal information
     * @param AppId
     */
    TimeAuthoritySignalData(const ajn::MsgArg* returnArgs,  TimeServiceClientClock* clock) : m_Args(returnArgs),
        m_Clock(clock)
    {

        QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, clock->getObjectPath().c_str()));
    };

    const ajn::MsgArg* m_Args;
    TimeServiceClientClock* m_Clock;

};

class TimeAuthoritySignalTask : public AsyncTask {

  public:

  private:

    virtual void OnEmptyQueue() { }

    virtual void OnTask(TaskData const* taskdata);
};

TimeAuthoritySignalTask TimeAuthoritySignalHandler::s_SignalTask;

AsyncTaskQueue TimeAuthoritySignalHandler::s_SignalQueue(&TimeAuthoritySignalHandler::s_SignalTask);

void TimeAuthoritySignalTask::OnTask(const ajn::services::TaskData* taskdata)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const TimeAuthoritySignalData* signalData = static_cast<const TimeAuthoritySignalData*>(taskdata);

    QCC_DbgPrintf(("On TimeSync task, Object: '%s', delegating to the listener",
                   signalData->m_Clock->getObjectPath().c_str()));

    signalData->m_Clock->getTimeAuthorityHandler()->handleTimeSync(signalData->m_Clock);
}

TimeAuthoritySignalHandler::TimeAuthoritySignalHandler() : m_Protector(), m_ClockSignalMethod(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

TimeAuthoritySignalHandler::~TimeAuthoritySignalHandler()
{
}

TimeAuthoritySignalHandler* TimeAuthoritySignalHandler::getInstance()
{

    if (!s_Instance) {
        s_Instance = new TimeAuthoritySignalHandler();
    }

    return s_Instance;
}

QStatus TimeAuthoritySignalHandler::registerClock(TimeServiceClientClock* clock)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, clock->getObjectPath().c_str()));

    QStatus status = ER_OK;

    BusAttachment* busAttachment = clock->getTsClient().getBus();

    if (!busAttachment) {

        QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
        return ER_FAIL;
    }

    if (!m_ClockSignalMethod) {

        const InterfaceDescription* iFace = busAttachment->GetInterface(tsConsts::TIME_AUTHORITY_IFACE.c_str());
        if (!iFace) {

            QCC_LogError(ER_FAIL, ("Could not get clock interface"));
            return ER_FAIL;
        }

        m_ClockSignalMethod = iFace->GetMember(tsClockUtility::IFACE_AUTH_SIG_TIME_SYNC.c_str());

        if (!m_ClockSignalMethod) {

            QCC_LogError(ER_FAIL, ("Could not get signal member in clock interface"));
            return ER_FAIL;
        }
    }

    if (m_Clocks.empty()) {

        status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimeAuthoritySignalHandler::handleClockSignal),
                                                      m_ClockSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not register the SignalHandler"));
            return status;
        } else {

            QCC_DbgPrintf(("Registered the SignalHandler successfully"));
        }

        QCC_DbgPrintf(("TimeSyncSignalHandler AddMatch Rule is: %s", TIME_SYNC_SIG_INTERFACE_MATCH.c_str()));

        status = busAttachment->AddMatch(TIME_SYNC_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not Add Match rule."));
        }

        s_SignalQueue.Start();
    }

    QCC_DbgPrintf(("Registering the Clock: '%s' to listen for TimeSync signals", clock->getObjectPath().c_str()));

    status = m_Protector.Lock();
    if (status != ER_OK) {

        return status;
    }

    m_Clocks.push_back(clock);

    status = m_Protector.Unlock();
    if (status != ER_OK) {

        return status;
    }

    return status;
}

void TimeAuthoritySignalHandler::unRegisterClock(TimeServiceClientClock* clock)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, clock->getObjectPath().c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));

        return;
    }

    std::vector<TimeServiceClientClock*>::iterator itr = m_Clocks.begin();
    while (itr != m_Clocks.end()) {

        if ((*itr)->getObjectPath().compare(clock->getObjectPath()) == 0) {

            const TimeServiceClient& client = (*itr)->getTsClient();

            if (client.getServerBusName().compare(clock->getTsClient().getServerBusName()) == 0) {

                QCC_DbgPrintf(("Unregistering the Clock: '%s' from listening for TimeSync signals", clock->getObjectPath().c_str()));

                itr = m_Clocks.erase(itr);
                continue;
            }
        }

        ++itr;
    }

    status = m_Protector.Unlock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex UNlock failed. signal handler vector compromised"));

    }

    if (m_Clocks.empty()) {

        BusAttachment* busAttachment = clock->getTsClient().getBus();

        if (!busAttachment) {

            QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
            return;
        }

        QStatus status = busAttachment->RemoveMatch(TIME_SYNC_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not remove Add Match rule."));
        } else {

            QCC_DbgPrintf(("The Add Match rule: '%s' removed successfully", TIME_SYNC_SIG_INTERFACE_MATCH.c_str()));
        }

        status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimeAuthoritySignalHandler::handleClockSignal),
                                                        m_ClockSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not unregister signal handler."));
        } else {

            QCC_DbgPrintf(("Signal handler has been removed successfully"));
        }

        s_SignalQueue.Stop();
    }
}

TimeServiceClientClock* TimeAuthoritySignalHandler::findClockByObjectPathForDevice(qcc::String objPath, qcc::String sender)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objPath.c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));

        return NULL;
    }

    for (std::vector<TimeServiceClientClock*>::const_iterator itr = m_Clocks.begin(); itr != m_Clocks.end(); itr++) {

        if ((*itr)->getObjectPath().compare(objPath) == 0) {

            const TimeServiceClient& client = (*itr)->getTsClient();

            if (client.getServerBusName().compare(sender) == 0) {
                status = m_Protector.Unlock();
                if (status != ER_OK) {
                    QCC_LogError(status, ("Mutex UNlock failed from within find block. signal handler vector compromised"));

                    return NULL;
                }
                return *itr;
            }
        }
    }

    status = m_Protector.Unlock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex UNlock failed. signal handler vector compromised"));

        return NULL;
    }

    return NULL;
}

void TimeAuthoritySignalHandler::handleClockSignal(const ajn::InterfaceDescription::Member* member,
                                                   const char* srcPath, ajn::Message& msg)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const char* sender = msg->GetSender();
    QCC_DbgHLPrintf(("Received TimeSync signal from the object: '%s', sender: '%s'", srcPath, sender));

    const ajn::MsgArg* returnArgs;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, returnArgs);

    TimeServiceClientClock* clock = findClockByObjectPathForDevice(srcPath, sender);

    if (clock) {

        TimeAuthoritySignalData* taskdata = new TimeAuthoritySignalData(returnArgs, clock);

        s_SignalQueue.Enqueue(taskdata);
    } else {

        QCC_DbgPrintf(("Couldn't find TimeServiceClock listener, ObjectPath: '%s', bus '%s'", srcPath, msg->GetSender()));
    }
}

} /* namespace services */
} /* namespace ajn */
