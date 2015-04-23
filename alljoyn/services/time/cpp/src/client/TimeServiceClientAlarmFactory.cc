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

#include <alljoyn/time/TimeServiceClientAlarmFactory.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceAlarmUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientAlarmFactory::TimeServiceClientAlarmFactory(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    TimeServiceObjectIntrospector(tsClient, objectPath),
    m_AlarmFactoryIface(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    initInterfaces(tsConsts::ALARM_FACTORY_IFACE, &m_AlarmFactoryIface, &tsAlarmUtility::createFactoryInterface);
}

//Destructor
TimeServiceClientAlarmFactory::~TimeServiceClientAlarmFactory()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Release object resources
void TimeServiceClientAlarmFactory::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgHLPrintf(("Releasing TimeServiceAlarmFactory, objectPath: '%s'", m_ObjectPath.c_str()));

    m_AlarmFactoryIface = NULL;
}

//Creates new Alarm
QStatus TimeServiceClientAlarmFactory::newAlarm(qcc::String* objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!objectPath) {

        QCC_LogError(ER_BAD_ARG_1, ("Failed to call NewAlarm, undefined ObjectPath is given"));
        return ER_BAD_ARG_1;
    }

    ConnectionData connectionData;
    QStatus status = getConnectionData(&connectionData);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call NewAlarm function, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    Message replyMsg(*connectionData.bus);
    status = serverMethodCall(m_AlarmFactoryIface, tsAlarmUtility::IFACE_FAC_METHOD_NEW_ALARM, NULL, 0, &replyMsg);

    if (status != ER_OK) {

        return status;
    }

    size_t numArgs = 0;
    const MsgArg* asArray;
    replyMsg->GetArgs(numArgs, asArray);

    if (numArgs < 1) {

        QCC_LogError(ER_BAD_ARG_COUNT, ("Failed to read NewAlarm response arguments"));
        return ER_BAD_ARG_COUNT;
    }

    return tsAlarmUtility::unmarshalObjectPath(asArray[0], objectPath);
}

//Delete Alarm identified by the given object path.
QStatus TimeServiceClientAlarmFactory::deleteAlarm(const qcc::String& objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    MsgArg msgArg;
    QStatus status = tsAlarmUtility::marshalObjectPath(msgArg, objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal object path of the deleted Alarm"));
        return status;
    }

    ConnectionData connectionData;
    status = getConnectionData(&connectionData);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call DeleteAlarm function, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    QCC_DbgPrintf(("Deleting Alarm on the AlarmFactory: '%s', Deleted Alarm: '%s'", m_ObjectPath.c_str(), objectPath.c_str()));

    Message replyMsg(*connectionData.bus);
    return serverMethodCall(m_AlarmFactoryIface, tsAlarmUtility::IFACE_FAC_METHOD_DELETE_ALARM, &msgArg, 1, &replyMsg);
}

//Retrieves a list of alarm object paths created by the AlarmFactory.
QStatus TimeServiceClientAlarmFactory::retrieveAlarmList(std::vector<qcc::String>* objectPathList)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    return retrieveChildObjects(objectPathList);
}
