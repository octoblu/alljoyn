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


#include "TimerSignalHandler.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceTimerUtility.h"

namespace ajn {
namespace services {

const qcc::String TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS = "interface='" + tsConsts::TIMER_IFACE + "',type='signal',sessionless='t'";
const qcc::String TIMER_IFACE_SIG_INTERFACE_MATCH = "interface='" + tsConsts::TIMER_IFACE + "',type='signal'";


TimerSignalHandler* TimerSignalHandler::s_Instance = NULL;

class TimerSignalData : public TaskData {

  public:

    /**
     * Constructor
     * @param returnArgs MsgArg with the signal information
     * @param Timer timer object
     * @param name name of signal
     */
    TimerSignalData(const ajn::MsgArg* returnArgs,  TimeServiceClientTimer* timer, const qcc::String& name) : m_Args(*returnArgs),
        m_Timer(timer), m_Name(name)
    {

        QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, timer->getObjectPath().c_str()));
    };


    const ajn::MsgArg m_Args;
    TimeServiceClientTimer* m_Timer;
    qcc::String m_Name;

};

class TimerSignalTask : public AsyncTask {

  public:

  private:

    virtual void OnEmptyQueue() { }

    virtual void OnTask(TaskData const* taskdata);
};

TimerSignalTask TimerSignalHandler::s_SignalTask;

AsyncTaskQueue TimerSignalHandler::s_SignalQueue(&TimerSignalHandler::s_SignalTask);

void TimerSignalTask::OnTask(const ajn::services::TaskData*taskdata)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const TimerSignalData* signalData = static_cast<const TimerSignalData*>(taskdata);

    QCC_DbgPrintf(("On Timer '%s' task, Object: '%s', delegating to the listener", signalData->m_Name.c_str(),
                   signalData->m_Timer->getObjectPath().c_str()));

    if (signalData->m_Name.compare(tsTimerUtility::IFACE_SIG_TIMER_EVENT) == 0) {

        signalData->m_Timer->getTimerHandler()->handleTimerEvent(signalData->m_Timer);
    } else if (signalData->m_Name.compare(tsTimerUtility::IFACE_SIG_TIMER_RUNSTATECHANGED) == 0) {

        bool runState;
        QStatus status = tsTimerUtility::unmarshalBoolean(signalData->m_Args, &runState);
        if (status != ER_OK) {
            QCC_LogError(ER_FAIL, ("Could not get run state although the signal arrived"));
            return;
        }

        signalData->m_Timer->getTimerHandler()->handleRunStateChanged(signalData->m_Timer, runState);
    }
}

TimerSignalHandler::TimerSignalHandler() : m_Protector(),
    m_TimerSignalMethod(NULL),
    m_TimerRunStateChangedSignalMethod(NULL)

{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

TimerSignalHandler::~TimerSignalHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

TimerSignalHandler* TimerSignalHandler::getInstance() {

    if (!s_Instance) {

        s_Instance = new TimerSignalHandler();
    }
    return s_Instance;
}

QStatus TimerSignalHandler::registerTimer(TimeServiceClientTimer* timer)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, timer->getObjectPath().c_str()));

    QStatus status = ER_OK;

    BusAttachment* busAttachment = timer->getTsClient().getBus();

    if (!busAttachment) {

        QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
        return ER_FAIL;
    }

    if (!m_TimerSignalMethod) {

        const InterfaceDescription* iFace = busAttachment->GetInterface(tsConsts::TIMER_IFACE.c_str());

        if (!iFace) {
            QCC_LogError(ER_FAIL, ("Could not get Timer interface"));
            return ER_FAIL;
        }

        m_TimerSignalMethod = iFace->GetMember(tsTimerUtility::IFACE_SIG_TIMER_EVENT.c_str());

        if (!m_TimerSignalMethod) {
            QCC_LogError(ER_FAIL, ("Could not get signal member in Timer interface"));
            return ER_FAIL;
        }

        m_TimerRunStateChangedSignalMethod = iFace->GetMember(tsTimerUtility::IFACE_SIG_TIMER_RUNSTATECHANGED.c_str());
        if (!m_TimerRunStateChangedSignalMethod) {

            QCC_LogError(ER_FAIL, ("Could not get the run state changed signal member in Timer interface"));
            return ER_FAIL;
        }
    }

    if (m_Timers.empty()) {

        status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimerSignalHandler::handleTimerSignal),
                                                      m_TimerSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not register the SignalHandler for the 'TimerEvent' signal"));
            return status;
        } else {

            QCC_DbgPrintf(("Registered the SignalHandler successfully for the 'TimerEvent' signal"));
        }

        status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimerSignalHandler::handleTimerSignal),
                                                      m_TimerRunStateChangedSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not register the SignalHandler for the 'RunStateChanged' signal"));
            return status;
        } else {

            QCC_DbgPrintf(("Registered the SignalHandler successfully for the 'RunStateChanged' signal"));
        }


        QCC_DbgPrintf(("StatusSignalHandler Match String is: %s", TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS.c_str()));

        status = busAttachment->AddMatch(TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS.c_str());
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not add filter match."));
        }

        QCC_DbgPrintf(("StatusSignalHandler Match String is: %s", TIMER_IFACE_SIG_INTERFACE_MATCH.c_str()));

        status = busAttachment->AddMatch(TIMER_IFACE_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not add filter match."));
        }

        s_SignalQueue.Start();
    }

    QCC_DbgPrintf(("Registering the Timer: '%s' to listen for timer signals", timer->getObjectPath().c_str()));

    status = m_Protector.Lock();
    if (status != ER_OK) {

        return status;
    }

    m_Timers.push_back(timer);

    status = m_Protector.Unlock();
    if (status != ER_OK) {

        return status;
    }

    return status;
}

void TimerSignalHandler::unRegisterTimer(TimeServiceClientTimer* timer)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, timer->getObjectPath().c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {

        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));
        return;
    }

    std::vector<TimeServiceClientTimer*>::iterator itr = m_Timers.begin();
    while (itr != m_Timers.end()) {

        if ((*itr)->getObjectPath().compare(timer->getObjectPath()) == 0) {

            const TimeServiceClient& client = (*itr)->getTsClient();

            if (client.getServerBusName().compare(timer->getTsClient().getServerBusName()) == 0) {

                QCC_DbgPrintf(("Unregistering the Timer: '%s' from listening for Timer signals", timer->getObjectPath().c_str()));

                //Don't break if found, look to remove all the similar objects in the vector
                itr = m_Timers.erase(itr);
                continue;
            }
        }

        ++itr;
    }

    status = m_Protector.Unlock();
    if (status != ER_OK) {

        QCC_LogError(status, ("Mutex UNlock failed. signal handler vector compromised"));
    }

    if (m_Timers.empty()) {
        BusAttachment* busAttachment = timer->getTsClient().getBus();

        if (!busAttachment) {

            QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
            return;
        }

        QStatus status = busAttachment->RemoveMatch(TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not remove Match Rule: '%s'.", TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS.c_str()));
        } else {

            QCC_DbgPrintf(("The Add Match rule: '%s' removed successfully", TIMER_IFACE_SIG_INTERFACE_MATCH_SESSIONLESS.c_str()));
        }

        status = busAttachment->RemoveMatch(TIMER_IFACE_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not remove Match Rule: '%s'.", TIMER_IFACE_SIG_INTERFACE_MATCH.c_str()));
        } else {

            QCC_DbgPrintf(("The Add Match rule: '%s' removed successfully", TIMER_IFACE_SIG_INTERFACE_MATCH.c_str()));
        }


        status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimerSignalHandler::handleTimerSignal),
                                                        m_TimerSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not unregister signal handler 'TimerEvent' signal handler."));
        } else {

            QCC_DbgPrintf(("TimerEvent signal handler has been removed successfully"));
        }


        status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&TimerSignalHandler::handleTimerSignal),
                                                        m_TimerRunStateChangedSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not unregister 'RunStateChanged' signal handler."));
        } else {

            QCC_DbgPrintf(("RunStateChanged signal handler has been removed successfully"));
        }

        s_SignalQueue.Stop();
    }
}

TimeServiceClientTimer* TimerSignalHandler::findTimerByObjectPathForDevice(qcc::String objPath, qcc::String sender)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objPath.c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {

        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));
        return NULL;
    }

    for (std::vector<TimeServiceClientTimer*>::const_iterator itr = m_Timers.begin(); itr != m_Timers.end(); itr++) {

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

void TimerSignalHandler::handleTimerSignal(const ajn::InterfaceDescription::Member* member,
                                           const char* srcPath, ajn::Message& msg)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const char* sender = msg->GetSender();
    QCC_DbgHLPrintf(("Received TimerSignal signal from the object: '%s', sender: '%s'", srcPath, sender));

    const ajn::MsgArg* returnArgs;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, returnArgs);
    if (numArgs && (numArgs != 1)) {
        QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
    } else {
        TimeServiceClientTimer* timer = findTimerByObjectPathForDevice(srcPath, sender);
        if (timer) {
            TimerSignalData* taskdata = new TimerSignalData(returnArgs, timer, member->name);

            s_SignalQueue.Enqueue(taskdata);
        } else {
            QCC_DbgPrintf(("couldn't find '%s' for bus '%s'", srcPath, msg->GetSender()));
        }

    }
}

} /* namespace services */
} /* namespace ajn */
