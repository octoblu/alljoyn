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

#include <alljoyn/time/TimeServiceClientTimerFactory.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include "../common/TimeServiceTimerUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientTimerFactory::TimeServiceClientTimerFactory(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    TimeServiceObjectIntrospector(tsClient, objectPath),
    m_TimerFactoryIface(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    initInterfaces(tsConsts::TIMER_FACTORY_IFACE, &m_TimerFactoryIface, &tsTimerUtility::createFactoryInterface);
}

//Destructor
TimeServiceClientTimerFactory::~TimeServiceClientTimerFactory()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Release object resources
void TimeServiceClientTimerFactory::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgHLPrintf(("Releasing TimeServiceTimerFactory, objectPath: '%s'", m_ObjectPath.c_str()));

    m_TimerFactoryIface = NULL;
}

//Creates new Timer
QStatus TimeServiceClientTimerFactory::newTimer(qcc::String* objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!objectPath) {

        QCC_LogError(ER_BAD_ARG_1, ("Failed to call NewTimer, undefined ObjectPath is given"));
        return ER_BAD_ARG_1;
    }

    ConnectionData connectionData;
    QStatus status = getConnectionData(&connectionData);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call NewTimer function, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    Message replyMsg(*connectionData.bus);
    status = serverMethodCall(m_TimerFactoryIface, tsTimerUtility::IFACE_FAC_METHOD_NEW_TIMER, NULL, 0, &replyMsg);

    if (status != ER_OK) {

        return status;
    }

    size_t numArgs = 0;
    const MsgArg* asArray;
    replyMsg->GetArgs(numArgs, asArray);

    if (numArgs < 1) {

        QCC_LogError(ER_BAD_ARG_COUNT, ("Failed to read NewTimer response arguments"));
        return ER_BAD_ARG_COUNT;
    }

    return tsTimerUtility::unmarshalObjectPath(asArray[0], objectPath);
}

//Delete Timer identified by the given object path.
QStatus TimeServiceClientTimerFactory::deleteTimer(const qcc::String& objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    MsgArg msgArg;
    QStatus status = tsTimerUtility::marshalObjectPath(msgArg, objectPath);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to marshal object path of the deleted Timer"));
        return status;
    }

    ConnectionData connectionData;
    status = getConnectionData(&connectionData);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call DeleteTimer function, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    QCC_DbgPrintf(("Deleting Timer on the TimerFactory: '%s', Deleted Timer: '%s'", m_ObjectPath.c_str(), objectPath.c_str()));

    Message replyMsg(*connectionData.bus);
    return serverMethodCall(m_TimerFactoryIface, tsTimerUtility::IFACE_FAC_METHOD_DELETE_TIMER, &msgArg, 1, &replyMsg);
}

//Retrieves a list of timer object paths created by the TimerFactory.
QStatus TimeServiceClientTimerFactory::retrieveTimerList(std::vector<qcc::String>* objectPathList)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    return retrieveChildObjects(objectPathList);
}
