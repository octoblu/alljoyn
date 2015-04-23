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

#ifndef TIMESERVICECLIENT_H_
#define TIMESERVICECLIENT_H_

#include <map>
#include <vector>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>
#include <alljoyn/time/TimeServiceClientAlarm.h>
#include <alljoyn/time/TimeServiceClientAlarmFactory.h>
#include <alljoyn/time/TimeServiceClientClock.h>
#include <alljoyn/time/TimeServiceClientTimer.h>
#include <alljoyn/time/TimeServiceClientTimerFactory.h>

namespace ajn {
namespace services {

class TimeServiceSessionListener;
class TimeServiceSessionHandler;

/**
 * The main class to initialize the Time Service in the client mode.
 * The class is initialized with the AnnounceHandler::ObjectDescriptions
 * received from the Announcement.
 * To retrieve the Time Service items that were found in the provided
 * AnnounceHandler::ObjectDescriptions, use appropriate
 * getter methods such as TimeServiceClient.getAnnouncedClockList().
 */
class TimeServiceClient {

  public:

    /**
     * Constructor
     */
    TimeServiceClient();

    /**
     * Destructor
     */
    ~TimeServiceClient();

    /**
     * Initialize TimeServiceClient
     * @param bus The BusAttachment that is used by the TimeServiceClient
     * to reach the TimeServiceServer
     * @param serverBusName The bus name to reach the TimeServiceServer
     * @param deviceId The id of the device hosting the TimeServiceServer
     * @param appId The name of the application that started the TimeServiceServer
     * @param aboutObjectDescription AboutObjectDescription received with Announcement
     *
     * @return ER_OK if succeeded to initialize the TimeServiceClient
     */
    QStatus init(BusAttachment* bus, const qcc::String& serverBusName, const qcc::String& deviceId,
                 const qcc::String& appId, const ajn::AboutObjectDescription& aboutObjectDescription);

    /**
     * Cleans the object and releases its resources.
     * It's a programming error to call another method on this object after the release method has been called.
     */
    void release();

    /**
     * BusAttachment that is used by the TimeServiceClient
     *
     * @return BusAttachment that is used by the TimeServiceClient
     */
    BusAttachment* getBus() const;

    /**
     * The bus name that is used to reach TimeServiceServer
     *
     * @return The bus name that is used to reach TimeServiceServer
     */
    const qcc::String& getServerBusName() const;

    /**
     * The id of the device hosting the TimeServiceServer
     *
     * @return Device id
     */
    const qcc::String& getDeviceId() const;

    /**
     * The name of the application that started the TimeServiceServer
     *
     * @return The application name
     */
    const qcc::String& getAppId() const;

    /**
     * Returns list of the TimeServiceClientClock objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientClock objects
     */
    const std::vector<TimeServiceClientClock*>& getAnnouncedClockList() const;

    /**
     * Returns list of the TimeServiceClientAlarm objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientAlarm objects
     */
    const std::vector<TimeServiceClientAlarm*>& getAnnouncedAlarmList() const;

    /**
     * Returns list of the TimeServiceClientAlarmFactory objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientAlarmFactory objects
     */
    const std::vector<TimeServiceClientAlarmFactory*>& getAnnouncedAlarmFactoryList() const;

    /**
     * Returns list of the TimeServiceClientTimer objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientTimer objects
     */
    const std::vector<TimeServiceClientTimer*>& getAnnouncedTimerList() const;

    /**
     * Returns list of the TimeServiceClientTimerFactory objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientTimerFactory objects
     */
    const std::vector<TimeServiceClientTimerFactory*>& getAnnouncedTimerFactoryList() const;

    /**
     * Creates the session asynchronously with the server.
     *
     * @param sessionListener Session related events will be passed via the TimeServiceSessionListener
     *
     * @return status ER_OK If the join session call succeeded
     */
    QStatus joinSessionAsync(TimeServiceSessionListener* sessionListener);

    /**
     * Leave previously established session
     *
     * @return status ER_OK if the session was disconnected successfully
     */
    QStatus leaveSession();

    /**
     * Return whether a session with the TimeServiceServer has been created
     *
     * @return TRUE if the session has been created
     */
    bool isConnected() const;

    /**
     * Set the given sessionId with id of the session that has been established with the TimeServiceServer.
     *
     * @param sessionId Out variable. Set to be session id that has been established with the server.
     *
     * @return ER_OK if the connection with the server has been established.
     */
    QStatus getSessionId(ajn::SessionId& sessionId) const;

  private:

    /**
     * Base class for the TsItemsManager providing its API
     */
    class TsItemsManagerBase {

      public:

        /**
         * Constructor
         */
        TsItemsManagerBase();

        /**
         * Destructor
         */
        virtual ~TsItemsManagerBase();

        /**
         * Create a TimeServiceClient object
         *
         * @param objectPath
         */
        virtual TimeServiceClientBase* create(const qcc::String& objectPath) = 0;

        /**
         * Call release on the objects stored in the storage
         */
        virtual void release() = 0;
    };

    /**
     * Helper factory class for creation and releasing TimeService objects
     */
    template <class TSItem>
    class TSItemsManager : public TsItemsManagerBase {

      public:

        /**
         * Constructor
         *
         * @param tsClient TimeServiceClient
         * @param storage TimeService item storage
         */
        TSItemsManager(const TimeServiceClient& tsClient, std::vector<TSItem*>* storage);

        /**
         * Destructor
         */
        ~TSItemsManager();

        TimeServiceClientBase* create(const qcc::String& objectPath);
        void release();

      private:

        /**
         * Reference to the TimeServiceClient items storage
         */
        std::vector<TSItem*>* m_Storage;

        /**
         * TimeServiceClient the outer class of this object
         */
        const TimeServiceClient& m_TsClient;
    };

    //=======================================================//

    /**
     * BusAttachment to be used by the TimeServiceClient
     */
    BusAttachment* m_Bus;

    /**
     * Bus unique name of the TimeServiceServer
     */
    qcc::String m_ServerBusName;

    /**
     * DeviceId of the TimeServiceServer
     */
    qcc::String m_DeviceId;

    /**
     * AppId of the TimeServiceServer
     */
    qcc::String m_AppId;

    /**
     * Manages session with the server side
     */
    TimeServiceSessionHandler* m_SessionHandler;

    /**
     *  Map from the TimeService interface to the TsItemsManagerBase
     */
    typedef std::map<qcc::String, TsItemsManagerBase*> TSItemsManagerMap;

    /**
     * Stores TSItemsManager objects
     */
    TSItemsManagerMap m_ManagerMap;

    /**
     * List of the alarm objects found in the received object descriptions
     */
    std::vector<TimeServiceClientAlarm*> m_Alarms;

    /**
     * List of the alarm factory objects found in the received object descriptions
     */
    std::vector<TimeServiceClientAlarmFactory*> m_AlarmFactories;

    /**
     * List of the clocks objects found in the received object descriptions
     */
    std::vector<TimeServiceClientClock*> m_Clocks;

    /**
     * List of the timer objects found in the received object descriptions
     */
    std::vector<TimeServiceClientTimer*> m_Timers;

    /**
     * List of the timer factory objects found in the received object descriptions
     */
    std::vector<TimeServiceClientTimerFactory*> m_TimerFactories;

    /**
     * Private Copy constructor of TimeServiceClient.
     * TimeServiceClient is not copy-able
     *
     * @param tsClient TimeServiceClient to copy
     */
    TimeServiceClient(const TimeServiceClient& tsClient);

    /**
     * Private assignment operator of TimeServiceClient.
     * TimeServiceClient is not assignable
     *
     * @param tsClient
     *
     * @return TimeServiceClient
     */
    TimeServiceClient& operator=(const TimeServiceClient& tsClient);

    /**
     * Checks validity of the received BusAttachment and serverBusName
     *
     * @param bus
     * @param serverBusName
     *
     * @return ER_OK if the received arguments are valid, otherwise ER_BAD_ARGUMENT status
     * of the appropriate argument is returned
     */
    QStatus checkObjectsValidity(BusAttachment* bus, const qcc::String& serverBusName);

    /**
     * Populate ManagerMap with the relevant TimeService interface names
     */
    void initManagerMap();

    /**
     * Analyze received object descriptions and create relevant TimeService objects
     * using the TSItemsManager
     *
     * @param objDescs Object Descriptions received with the Announcement
     */
    void analyzeObjectDescriptions(const AboutObjectDescription& objectDescs);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENT_H_ */
