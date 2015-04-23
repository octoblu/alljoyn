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

#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceSessionListener.h>
#include "TimeServiceSessionHandler.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceClient::TimeServiceClient() : m_Bus(NULL), m_SessionHandler(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Destructor
TimeServiceClient::~TimeServiceClient()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}


//Initialize the object
QStatus TimeServiceClient::init(BusAttachment* bus, const qcc::String& serverBusName, const qcc::String& deviceId,
                                const qcc::String& appId, const ajn::AboutObjectDescription& aboutObjectDescription)
{

    QCC_DbgTrace(("%s, ServerBusName: '%s', DeviceId: '%s', AppId: '%s'", __FUNCTION__, serverBusName.c_str(),
                  deviceId.c_str(), appId.c_str()));

    QStatus status = checkObjectsValidity(bus, serverBusName);
    if (status != ER_OK) {

        return status;
    }

    m_Bus               = bus;
    m_ServerBusName     = serverBusName;
    m_DeviceId          = deviceId;
    m_AppId             = appId;

    QCC_DbgHLPrintf(("Initializing TimeServiceClient for the server: '%s'", m_ServerBusName.c_str()));

    initManagerMap();
    analyzeObjectDescriptions(aboutObjectDescription);

    return ER_OK;
}

//Release object resources
void TimeServiceClient::release()
{

    QCC_DbgTrace(("%s, ServerBusName: '%s', DeviceId: '%s', AppId: '%s'", __FUNCTION__, m_ServerBusName.c_str(),
                  m_DeviceId.c_str(), m_AppId.c_str()));

    QCC_DbgHLPrintf(("Releasing TimeServiceClient for the server: '%s'", m_ServerBusName.c_str()));

    //When TSItemsManager object is destructed, it iteratively deletes all its TimeService objects
    for (std::map<qcc::String, TsItemsManagerBase*>::iterator iter = m_ManagerMap.begin(); iter != m_ManagerMap.end(); ++iter) {

        delete iter->second;
    }

    m_ManagerMap.clear();

    if (m_SessionHandler) {

        delete m_SessionHandler;
        m_SessionHandler = NULL;
    }

    m_Bus = NULL;
}

//Return BusAttachment
BusAttachment* TimeServiceClient::getBus() const
{

    return m_Bus;
}

//The bus name that is used to reach TimeServiceServer
const qcc::String& TimeServiceClient::getServerBusName() const
{

    return m_ServerBusName;
}

//The id of the device hosting the TimeServiceServer
const qcc::String& TimeServiceClient::getDeviceId() const
{

    return m_DeviceId;
}

//The name of the application that started the TimeServiceServer
const qcc::String& TimeServiceClient::getAppId() const
{

    return m_AppId;
}

//Returns list of the TimeServiceClientClock objects found in the received object descriptions
const std::vector<TimeServiceClientClock*>& TimeServiceClient::getAnnouncedClockList() const
{

    return m_Clocks;
}

//Returns list of the TimeServiceClientAlarm objects found in the received object descriptions
const std::vector<TimeServiceClientAlarm*>& TimeServiceClient::getAnnouncedAlarmList() const
{

    return m_Alarms;
}

//Returns list of the TimeServiceClientAlarmFactory objects found in the received object descriptions
const std::vector<TimeServiceClientAlarmFactory*>& TimeServiceClient::getAnnouncedAlarmFactoryList() const
{

    return m_AlarmFactories;
}

//Returns list of the TimeServiceClientTimer objects found in the received object descriptions
const std::vector<TimeServiceClientTimer*>& TimeServiceClient::getAnnouncedTimerList() const
{

    return m_Timers;
}

//Returns list of the TimeServiceClientTimerFactory objects found in the received object descriptions
const std::vector<TimeServiceClientTimerFactory*>& TimeServiceClient::getAnnouncedTimerFactoryList() const
{

    return m_TimerFactories;
}

//Creates the session asynchronously with the server.
QStatus TimeServiceClient::joinSessionAsync(TimeServiceSessionListener* sessionListener)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_SessionHandler) {

        m_SessionHandler = new TimeServiceSessionHandler(this);
    }

    return m_SessionHandler->joinSessionAsync(sessionListener);
}

//Leave previously established session
QStatus TimeServiceClient::leaveSession()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_SessionHandler) {

        QCC_LogError(ER_FAIL, ("The session hasn't been joined before"));
        return ER_FAIL;
    }

    return m_SessionHandler->leaveSession();
}

//Returns TRUE if there is a session established with the server
bool TimeServiceClient::isConnected() const
{

    if (!m_SessionHandler) {

        QCC_LogError(ER_FAIL, ("The session hasn't been joined before"));
        return false;
    }

    return m_SessionHandler->isConnected();
}

//Returns current session id, use the method isConnected()
QStatus TimeServiceClient::getSessionId(ajn::SessionId& sessionId) const
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_SessionHandler) {

        QCC_LogError(ER_FAIL, ("The session hasn't been joined before"));
        return ER_FAIL;
    }

    if (!m_SessionHandler->isConnected()) {

        QCC_LogError(ER_FAIL, ("The session hasn't been established"));
        return ER_FAIL;
    }

    sessionId = m_SessionHandler->getSessionId();

    return ER_OK;
}

//Checks validity of the received BusAttachment and serverBusName
QStatus TimeServiceClient::checkObjectsValidity(BusAttachment* bus, const qcc::String& serverBusName)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (m_Bus != NULL) {

        QCC_LogError(ER_FAIL, ("Could not accept this BusAttachment, a different bus attachment already exists"));
        return ER_BAD_ARG_1;
    }

    if (!bus || !bus->IsStarted() || !bus->IsConnected()) {

        QCC_LogError(ER_BAD_ARG_1, ("Could not accept this BusAttachment, bus attachment not started or not connected"));
        return ER_BAD_ARG_1;
    }

    if (serverBusName.length() == 0) {

        QCC_LogError(ER_BAD_ARG_2, ("serverBusName is empty"));
        return ER_BAD_ARG_2;
    }

    return ER_OK;
}

//Populate ManagerMap with the relevant TimeService interface names
void TimeServiceClient::initManagerMap()
{

    m_ManagerMap.insert(std::pair<qcc::String, TsItemsManagerBase*>(tsConsts::ALARM_IFACE,
                                                                    new TSItemsManager<TimeServiceClientAlarm>(*this, &m_Alarms)));

    m_ManagerMap.insert(std::pair<qcc::String, TsItemsManagerBase*>(tsConsts::ALARM_FACTORY_IFACE,
                                                                    new TSItemsManager<TimeServiceClientAlarmFactory>(*this, &m_AlarmFactories)));

    m_ManagerMap.insert(std::pair<qcc::String, TsItemsManagerBase*>(tsConsts::CLOCK_IFACE,
                                                                    new TSItemsManager<TimeServiceClientClock>(*this, &m_Clocks)));

    m_ManagerMap.insert(std::pair<qcc::String, TsItemsManagerBase*>(tsConsts::TIMER_IFACE,
                                                                    new TSItemsManager<TimeServiceClientTimer>(*this, &m_Timers)));

    m_ManagerMap.insert(std::pair<qcc::String, TsItemsManagerBase*>(tsConsts::TIMER_FACTORY_IFACE,
                                                                    new TSItemsManager<TimeServiceClientTimerFactory>(*this, &m_TimerFactories)));
}

//Analyze received object descriptions and create relevant TimeService objects
void TimeServiceClient::analyzeObjectDescriptions(const AboutObjectDescription& aboutObjectDescription)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    size_t numPaths = aboutObjectDescription.GetPaths(NULL, 0);
    const char** paths = new const char*[numPaths];
    aboutObjectDescription.GetPaths(paths, numPaths);
    for (size_t p = 0; p < numPaths; p++) {
        qcc::String objectPath(paths[p]);
        size_t numInterfaces = aboutObjectDescription.GetInterfaces(objectPath.c_str(), NULL, 0);
        const char** interfaces = new const char*[numInterfaces];
        aboutObjectDescription.GetInterfaces(objectPath.c_str(), interfaces, numInterfaces);
        bool isAuthority = false;
        TimeServiceClientClock* currentClock = NULL;

        for (size_t i = 0; i < numInterfaces; i++) {
            qcc::String iface(interfaces[i]);

            //If IFNAME_PREFIX is not a prefix of the iface, then It's not a TimeService interface
            if (iface.find(tsConsts::IFNAME_PREFIX) == qcc::String::npos) {

                continue;
            }

            //Check if Interface is a TimeAuthority
            if (iface.compare(tsConsts::TIME_AUTHORITY_IFACE) == 0) {

                isAuthority = true;
                continue;
            }

            //Check if current interface is a known TimeService interface, if TRUE create the object using the TSItemsManager
            TSItemsManagerMap::iterator mgrIter = m_ManagerMap.find(iface.c_str());

            if (mgrIter == m_ManagerMap.end()) {

                QCC_DbgPrintf(("Received unsupported TimeService interface : '%s', objectPath: '%s', from the server: '%s'", iface.c_str(),
                               objectPath.c_str(), m_ServerBusName.c_str()));
                continue;
            }

            QCC_DbgPrintf(("Creating TimeClient object: '%s', interface: '%s', from the server: '%s'", objectPath.c_str(),
                           iface.c_str(), m_ServerBusName.c_str()));

            //Check if current interface is Clock, create Clock object and save it
            if (iface.compare(tsConsts::CLOCK_IFACE) == 0) {

                currentClock = static_cast<TimeServiceClientClock*>(mgrIter->second->create(objectPath));
            } else {

                mgrIter->second->create(objectPath);
            }

        }        //for::ifaces
        delete [] interfaces;

        if (isAuthority && currentClock) {

            QCC_DbgPrintf(("TimeClient Clock object: '%s', IsAuthority: '%d'", objectPath.c_str(), isAuthority));
            currentClock->setAuthority(isAuthority);
        }
    }    //for p
    delete [] paths;
}

//Base class constructor for the TsItemsManager providing its API
TimeServiceClient::TsItemsManagerBase::TsItemsManagerBase()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Base class destructor for the TsItemsManager providing its API
TimeServiceClient::TsItemsManagerBase::~TsItemsManagerBase()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}


//TSItemsManager Constructor
template <class TSItem>
TimeServiceClient::TSItemsManager<TSItem>::TSItemsManager(const TimeServiceClient& tsClient, std::vector<TSItem*>* storage) :
    TsItemsManagerBase(),
    m_Storage(storage),
    m_TsClient(tsClient)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}


//TSItemsManager destructor Constructor
template <class TSItem>
TimeServiceClient::TSItemsManager<TSItem>::~TSItemsManager()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Create TimeService item
template <class TSItem>
TimeServiceClientBase* TimeServiceClient::TSItemsManager<TSItem>::create(const qcc::String& objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    TSItem* tsItem = new TSItem(m_TsClient, objectPath);
    m_Storage->push_back(tsItem);

    return tsItem;
}

//Release TimeService objects stored in the storage
template <class TSItem>
void TimeServiceClient::TSItemsManager<TSItem>::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    //After deleting the TSItem object its destructor calls release() method on the object
    for (typename std::vector<TSItem*>::iterator iter = m_Storage->begin(); iter != m_Storage->end(); ++iter) {

        delete *iter;
    }

    m_Storage->clear();
}
