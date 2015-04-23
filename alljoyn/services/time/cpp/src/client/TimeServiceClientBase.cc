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

#include <alljoyn/time/TimeServiceClientBase.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/LogModule.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClientBase::TimeServiceClientBase(const TimeServiceClient& tsClient, const qcc::String& objectPath) :
    m_TsClient(tsClient),
    m_ObjectPath(objectPath),
    m_ProxyObj(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Destructor
TimeServiceClientBase::~TimeServiceClientBase()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (m_ProxyObj) {

        delete m_ProxyObj;
        m_ProxyObj = NULL;
    }
}

//Object path of the  Time Service server side object
const qcc::String& TimeServiceClientBase::getObjectPath() const
{

    return m_ObjectPath;
}

//Get TimeServiceClient
const TimeServiceClient& TimeServiceClientBase::getTsClient() const
{

    return m_TsClient;
}

//Populates ConnectionData, if all its fields are available
QStatus TimeServiceClientBase::getConnectionData(ConnectionData* connData) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    BusAttachment* bus = m_TsClient.getBus();

    if (!bus) {

        QCC_LogError(ER_FAIL, ("No BusAttachment, Apparently TimeServiceClient has been released"));
        return ER_FAIL;
    }

    SessionId sid;
    QStatus status = m_TsClient.getSessionId(sid);

    if (status != ER_OK) {

        return status;
    }

    connData->bus   = bus;
    connData->sid   = sid;

    return ER_OK;
}

//Create interface on the BusAttachment identified by the given ifaceName
void TimeServiceClientBase::initInterfaces(const qcc::String& ifaceName, InterfaceDescription** ptrIface,
                                           IntefaceFactoryMethod factoryMethod)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    BusAttachment* bus = m_TsClient.getBus();
    (*ptrIface)        = const_cast<InterfaceDescription*>(bus->GetInterface(ifaceName.c_str()));

    //Interface has been previously created
    if (*ptrIface) {

        return;
    }

    QCC_DbgPrintf(("Initializing '%s' interface for the object: '%s'", ifaceName.c_str(), m_ObjectPath.c_str()));

    QStatus status = factoryMethod(bus, ptrIface);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to create the interface: '%s'", ifaceName.c_str()));
    }
}

//Prepare proxy bus object
QStatus TimeServiceClientBase::prepareProxyObject(InterfaceDescription* ifaceDesc)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    ConnectionData connData;
    QStatus status = getConnectionData(&connData);

    if (status != ER_OK) {

        return status;
    }

    if (!ifaceDesc) {

        QCC_LogError(ER_BUS_NO_SUCH_INTERFACE, ("Unable to create ProxyBusObject, the interface description hasn't been defined"));
        return ER_BUS_NO_SUCH_INTERFACE;
    }

    //If no proxy, or the proxy's session doesn't exist anymore
    if (!m_ProxyObj || m_ProxyObj->GetSessionId() != connData.sid) {

        //Delete the previous proxy object
        if (m_ProxyObj) {

            QCC_DbgPrintf(("Deleting old ProxyBusObject, objectPath: '%s'", m_ObjectPath.c_str()));
            delete m_ProxyObj;
        }

        QCC_DbgPrintf(("Creating ProxyBusObject, objectPath: '%s'", m_ObjectPath.c_str()));
        m_ProxyObj = new ProxyBusObject(*connData.bus, m_TsClient.getServerBusName().c_str(), m_ObjectPath.c_str(),
                                        connData.sid);
    }

    //If ifaceDesc hasn't been set for this proxy => add it
    if (!m_ProxyObj->GetInterface(ifaceDesc->GetName())) {

        status = m_ProxyObj->AddInterface(*ifaceDesc);
    }

    return status;
}

//Retrieve server property
QStatus TimeServiceClientBase::retrieveServerProperty(InterfaceDescription* ifaceDesc, const qcc::String& propName,
                                                      MsgArg& msgArg)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Retrieving '%s' property, objectPath: '%s'", propName.c_str(), m_ObjectPath.c_str()));

    QStatus status = prepareProxyObject(ifaceDesc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to create ProxyBusObject, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    status = m_ProxyObj->GetProperty(ifaceDesc->GetName(), propName.c_str(), msgArg);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to retrieve '%s' property, objectPath: '%s'", propName.c_str(), m_ObjectPath.c_str()));
    }

    return status;
}

//Set server property
QStatus TimeServiceClientBase::setServerProperty(InterfaceDescription* ifaceDesc, const qcc::String& propName,
                                                 MsgArg& msgArg)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Setting '%s' property, objectPath: '%s'", propName.c_str(), m_ObjectPath.c_str()));

    QStatus status = prepareProxyObject(ifaceDesc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to create ProxyBusObject, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    status = m_ProxyObj->SetProperty(ifaceDesc->GetName(), propName.c_str(), msgArg);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to set '%s' property, objectPath: '%s'", propName.c_str(), m_ObjectPath.c_str()));
    }

    return status;
}

//Call server method
QStatus TimeServiceClientBase::serverMethodCall(InterfaceDescription* ifaceDesc, const qcc::String& methodName,
                                                const ajn::MsgArg* argsIn, size_t numArgs, Message* msgOut)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    QCC_DbgPrintf(("Calling method '%s' , objectPath: '%s'", methodName.c_str(), m_ObjectPath.c_str()));

    QStatus status = prepareProxyObject(ifaceDesc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to create ProxyBusObject, objectPath: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    if (msgOut) {

        status = m_ProxyObj->MethodCall(ifaceDesc->GetName(),
                                        methodName.c_str(),
                                        argsIn,
                                        numArgs,
                                        (*msgOut));

        if ((*msgOut)->GetType() == MESSAGE_ERROR) {

#if !defined(NDEBUG) //Debug mode

            qcc::String errorName;
            const char* msg = (*msgOut)->GetErrorName(&errorName);
#endif

            QCC_LogError(status, ("Error '%s' while calling method '%s' on '%s', interface: '%s':'%s ", errorName.c_str(), methodName.c_str(),
                                  m_ObjectPath.c_str(), ifaceDesc->GetName(), msg ? msg : ""));
        }
    } else {

        status = m_ProxyObj->MethodCall(ifaceDesc->GetName(),
                                        methodName.c_str(),
                                        argsIn,
                                        numArgs);
    }

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to call '%s' method, objectPath: '%s'", methodName.c_str(), m_ObjectPath.c_str()));
    }

    return status;
}
