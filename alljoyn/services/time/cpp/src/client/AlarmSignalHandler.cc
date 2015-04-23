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


#include "AlarmSignalHandler.h"
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceAlarmUtility.h"

namespace ajn {
namespace services {

const qcc::String ALARM_SIG_INTERFACE_MATCH = "interface='" + tsConsts::ALARM_IFACE + "',type='signal',sessionless='t'";

AlarmSignalHandler* AlarmSignalHandler::s_Instance = NULL;

class AlarmSignalData : public TaskData {

  public:

    /**
     * Constructor
     * @param returnArgs MsgArg with the signal information
     * @param AppId
     */
    AlarmSignalData(const ajn::MsgArg* returnArgs,  TimeServiceClientAlarm*alarm) : m_Args(returnArgs), m_Alarm(alarm)
    {

        QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, alarm->getObjectPath().c_str()));
    };


    const ajn::MsgArg* m_Args;
    TimeServiceClientAlarm* m_Alarm;

};

class AlarmSignalTask : public AsyncTask {
  public:

  private:

    virtual void OnEmptyQueue() { }

    virtual void OnTask(TaskData const* taskdata);
};

AlarmSignalTask AlarmSignalHandler::s_SignalTask;

AsyncTaskQueue AlarmSignalHandler::s_SignalQueue(&AlarmSignalHandler::s_SignalTask);

void AlarmSignalTask::OnTask(const ajn::services::TaskData*taskdata)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const AlarmSignalData* signalData = static_cast<const AlarmSignalData*>(taskdata);

    QCC_DbgPrintf(("On AlarmReached task, Object: '%s', delegating to the listener",
                   signalData->m_Alarm->getObjectPath().c_str()));

    signalData->m_Alarm->getAlarmHandler()->handleAlarmReached(signalData->m_Alarm);
}

AlarmSignalHandler::AlarmSignalHandler() : m_Protector(), m_AlarmSignalMethod(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

AlarmSignalHandler::~AlarmSignalHandler()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

AlarmSignalHandler* AlarmSignalHandler::getInstance() {

    if (!s_Instance) {

        s_Instance = new AlarmSignalHandler();
    }

    return s_Instance;
}

QStatus AlarmSignalHandler::registerAlarm(TimeServiceClientAlarm* alarm)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, alarm->getObjectPath().c_str()));

    QStatus status = ER_OK;

    BusAttachment* busAttachment = alarm->getTsClient().getBus();

    if (!busAttachment) {

        QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
        return ER_FAIL;
    }

    if (!m_AlarmSignalMethod) {

        const InterfaceDescription* iFace = busAttachment->GetInterface(tsConsts::ALARM_IFACE.c_str());
        if (!iFace) {
            QCC_LogError(ER_FAIL, ("Could not get alarm interface"));
            return ER_FAIL;
        }

        m_AlarmSignalMethod = iFace->GetMember(tsAlarmUtility::IFACE_SIG_ALARM_REACHED.c_str());

        if (!m_AlarmSignalMethod) {
            QCC_LogError(ER_FAIL, ("Could not get signal member in alarm interface"));
            return ER_FAIL;
        }
    }

    if (m_Alarms.empty()) {

        status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&AlarmSignalHandler::handleAlarmSignal),
                                                      m_AlarmSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not register the SignalHandler"));
            return status;
        } else {

            QCC_DbgPrintf(("Registered the SignalHandler successfully"));
        }

        QCC_DbgPrintf(("AlarmReachedSignalHandler Match String is: %s", ALARM_SIG_INTERFACE_MATCH.c_str()));

        status = busAttachment->AddMatch(ALARM_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not Add Match rule."));
        }

        s_SignalQueue.Start();
    }

    QCC_DbgPrintf(("Registering the Alarm: '%s' to listen for AlarmReached signals", alarm->getObjectPath().c_str()));

    status = m_Protector.Lock();
    if (status != ER_OK) {

        return status;
    }

    m_Alarms.push_back(alarm);

    status = m_Protector.Unlock();
    if (status != ER_OK) {

        return status;
    }

    return status;
}

void AlarmSignalHandler::unRegisterAlarm(TimeServiceClientAlarm* alarm)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, alarm->getObjectPath().c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));

        return;
    }

    std::vector<TimeServiceClientAlarm*>::iterator itr = m_Alarms.begin();
    while (itr != m_Alarms.end()) {

        if ((*itr)->getObjectPath().compare(alarm->getObjectPath()) == 0) {

            const TimeServiceClient& client = (*itr)->getTsClient();

            if (client.getServerBusName().compare(alarm->getTsClient().getServerBusName()) == 0) {

                QCC_DbgPrintf(("Unregistering the Alarm: '%s' from listening for AlarmReached signals", alarm->getObjectPath().c_str()));

                //Don't break if found, look to remove all the similar objects in the vector
                itr = m_Alarms.erase(itr);
                continue;
            }
        }

        ++itr;
    }
    status = m_Protector.Unlock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex UNlock failed. signal handler vector compromised"));

    }

    if (m_Alarms.empty()) {

        BusAttachment* busAttachment = alarm->getTsClient().getBus();

        if (!busAttachment) {

            QCC_LogError(ER_FAIL, ("No BusAttachment, apparently TimeServiceClient has been previously released"));
            return;
        }

        QStatus status = busAttachment->RemoveMatch(ALARM_SIG_INTERFACE_MATCH.c_str());
        if (status != ER_OK) {

            QCC_LogError(status, ("Could not remove Add Match rule."));
        } else {

            QCC_DbgPrintf(("The Add Match rule: '%s' removed successfully", ALARM_SIG_INTERFACE_MATCH.c_str()));
        }

        status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&AlarmSignalHandler::handleAlarmSignal),
                                                        m_AlarmSignalMethod, NULL);

        if (status != ER_OK) {

            QCC_LogError(status, ("Could not unregister signal handler."));
        } else {

            QCC_DbgPrintf(("Signal handler has been removed successfully"));
        }

        s_SignalQueue.Stop();

    }
}

TimeServiceClientAlarm* AlarmSignalHandler::findAlarmByObjectPathForDevice(qcc::String objPath, qcc::String sender)
{

    QCC_DbgTrace(("%s, ObjectPath: '%s'", __FUNCTION__, objPath.c_str()));

    QStatus status = m_Protector.Lock();
    if (status != ER_OK) {
        QCC_LogError(status, ("Mutex lock failed. signal handler vector compromised"));

        return NULL;
    }

    for (std::vector<TimeServiceClientAlarm*>::const_iterator itr = m_Alarms.begin(); itr != m_Alarms.end(); itr++) {

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

void AlarmSignalHandler::handleAlarmSignal(const ajn::InterfaceDescription::Member* member,
                                           const char* srcPath, ajn::Message& msg)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const char* sender = msg->GetSender();
    QCC_DbgHLPrintf(("Received AlarmReached signal from the object: '%s', sender: '%s'", srcPath, sender));

    const ajn::MsgArg* returnArgs;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, returnArgs);

    TimeServiceClientAlarm* alarm = findAlarmByObjectPathForDevice(srcPath, sender);

    if (alarm) {

        AlarmSignalData* taskdata = new AlarmSignalData(returnArgs, alarm);
        s_SignalQueue.Enqueue(taskdata);
    } else {

        QCC_DbgPrintf(("Couldn't find TimeServiceAlarm listener, ObjectPath: '%s' for bus '%s'", srcPath, msg->GetSender()));
    }
}

} /* namespace services */
} /* namespace ajn */
