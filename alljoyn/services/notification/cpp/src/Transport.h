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

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include <map>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/notification/NotificationEnums.h>
#include "NotificationConstants.h"

namespace ajn {
namespace services {

class Notification;
class NotificationReceiver;
class NotificationProducerSender;
class NotificationProducerReceiver;
class NotificationProducerListener;
class NotificationDismisserSender;
class NotificationDismisserReceiver;
class SuperAgentBusListener;
class NotificationAnnounceListener;
class NotificationConstants;
class NotificationTransportConsumer;
class NotificationTransportProducer;
class NotificationTransportSuperAgent;
/**
 * Class Used for all Transport related tasks including initializing and shutting
 * down the BusObjects of the services and sending and receiving messages
 */
class Transport {

  public:

    /**
     * Destructor of Transport
     */
    ~Transport();

    /**
     * static function to get an Instance of the Transport class
     * @return pointer to Transport instance
     */
    static Transport* getInstance();

    /**
     * Disabling superagent mode. Needs to be called before
     * starting receiver
     * @return status
     */
    QStatus disableSuperAgent();

    /**
     * Sets the internal NotificationReceiver to the one
     * provided in the parameter
     * @param notificationReceiver
     */
    void setNotificationReceiver(NotificationReceiver* notificationReceiver);

    /**
     * Start the Sender Transport:
     * Create a BusAttachment if one doesn't exist.
     * Create the NotificationTransportProducers (one per MessageType)
     * @param bus - The busAttachment to be used
     * @param startSuperAgent - should the superAgent be started. Defaults to false
     * @return status
     */
    QStatus startSenderTransport(ajn::BusAttachment* bus, bool startSuperAgent = false);

    /**
     * Start the Receiver Transport:
     * Create a BusAttachment if one doesn't exist.
     * create the NotificationTransportConsumer to handle the signals
     * @param bus - the busAttachment to be used
     * @return status
     */
    QStatus startReceiverTransport(ajn::BusAttachment* bus);

    /**
     * Begin listening to found SuperAgent
     * @param senderId
     * @return status - success/failure
     */
    QStatus listenToSuperAgent(const char* senderId);

    /**
     * Begin listening to found SuperAgent
     * @param senderId
     * @return status - success/failure
     */
    QStatus cancelListenToSuperAgent(const char* senderId);

    /**
     * Send Notification
     * @param messageType
     * @param notificationArgs
     * @param ttl
     * @return status
     */
    QStatus sendNotification(NotificationMessageType messageType,
                             ajn::MsgArg const notificationArgs[nsConsts::AJ_NOTIFY_NUM_PARAMS],
                             uint16_t ttl);

    /**
     * Delete the last Signal sent off for this messageType
     * @param messageType
     * @return status
     */
    QStatus deleteLastMsg(NotificationMessageType messageType);

    /**
     * Delete Signal sent off for this messageType
     * @param messageId
     * @return status
     */
    QStatus deleteMsg(int32_t msgId);
    /**
     * Pass on the notification received to the NotificationReceiver
     * @param notification
     */
    void onReceivedNotification(Notification const& notification);

    /**
     * Called on shutdown. Cleans up Alljoyn objects and frees memory where necessary
     */
    void cleanup();

    /**
     * Cleanup the TransportConsumer objects. Also Unregisteres the BusObject depending on input param
     * @param unregister - should BusObject be Unregistered from Bus
     */
    void cleanupTransportConsumer(bool unregister = false);

    /**
     * Cleanup the TransportSuperAgent object. Also Unregisteres the BusObject depending on input param
     * @param unregister - should BusObject be Unregistered from Bus
     */
    void cleanupTransportSuperAgent(bool unregister = false);

    /**
     * Cleanup the SuperAgent bus listener. Also Unregisteres the BusListener depending on input param
     * @param unregister - should BusListener be Unregistered from Bus
     */
    void cleanupSuperAgentBusListener(bool unregister = false);

    /**
     * Cleanup all Sender Transport objects. and Unregister the BusObject
     */
    void cleanupSenderTransport();

    /**
     * Cleanup all Receiver Transport objects. and Unregister the BusObject
     */
    void cleanupReceiverTransport();

    /**
     * Cleanup AnnouncementListener object. Also Unregisteres the listener from the aboutclient
     * @param unregister - should listener be Unregistered from aboutclient
     */
    void cleanupAnnouncementListener(bool unregister = false);
    /**
     * Cleanup cleanupNotificationProducerSender. and Unregister the BusObject.
     */
    void cleanupNotificationProducerSender();
    /**
     * getBusAttachment - returns BusObject
     */
    ajn::BusAttachment* getBusAttachment();

    /**
     * get function for NotificationProducerSender
     */
    NotificationProducerSender* getNotificationProducerSender();

    /**
     * get function for NotificationProducerReceiver
     */
    NotificationProducerReceiver* getNotificationProducerReceiver();

    /**
     * get function for NotificationDismisserSender
     */
    NotificationDismisserSender* getNotificationDismisserSender();

    /**
     * get function for NotificationReceiver
     */
    NotificationReceiver* getNotificationReceiver();

    /**
     * FindSuperAgent
     */
    QStatus FindSuperAgent(const char* busName);


  private:

    /**
     * Private Constructor - singleton Class
     */
    Transport();

    /**
     * Sets the internal busAttachment to the one
     * provided in the parameter
     * @param bus
     * @return success/failure
     */
    QStatus setBusAttachment(ajn::BusAttachment* bus);

    /**
     * Cleanup the TransportProducer objects. Start from messageTypeIndx param and work backwards
     * Also Unregisters the BusObject depending on input param
     * @param messageTypeIndx - messageTypeIndx to start from
     * @param unregister - should BusObject be Unregistered from Bus
     */
    void cleanupTransportProducer(int32_t messageTypeIndx, bool unregister = false);
    /**
     * cleanupSenderProducerReceiver
     */
    void cleanupNotificationProducerReceiver();
    /**
     * cleanupNotificationDismisserSender
     */
    void cleanupNotificationDismisserSender();
    /**
     * cleanupNotificationDismisserReceiver
     */
    void cleanupNotificationDismisserReceiver();
    /**
     * ListenForSuperAgent uses AllJoyn's announcement interface to try and find a SuperAgent
     * Messages are sessionless
     * @return status
     */
    QStatus listenForAnnouncements();

    /**
     * Static instance of Transport. Makes Transport a singleton
     */
    static Transport* s_Instance;

    /**
     * Alljoyn BusAttachment that will be used to connect to Bus
     */
    ajn::BusAttachment* m_Bus;

    /**
     * NotificationReceiver that will receive the incoming notifications
     * Must be created external from library and passed in
     */
    NotificationReceiver* m_Receiver;

    /**
     * The BusObjects for the Producer. One per MessageType
     */
    NotificationTransportProducer* m_Producers[MESSAGE_TYPE_CNT];

    /**
     * The BusObjects for the Consumer.
     */
    NotificationTransportConsumer* m_Consumer;

    /**
     * The BusObjects for the SuperAgent.
     */
    NotificationTransportSuperAgent* m_SuperAgent;

    /**
     * NotificationAnnounceListener to listen for superAgent announce
     */
    NotificationAnnounceListener* m_AnnounceListener;

    /**
     * SuperAgentBusListener - receive found and lost advertise name
     */
    SuperAgentBusListener* m_SuperAgentBusListener;
    /**
     * Boolean to dictate whether we send notifications or swallow them
     */
    bool m_IsSendingDisabled;

    /**
     * Boolean to dictate whether we receive notifications or swallow them
     */
    bool m_IsReceivingDisabled;

    /**
     * Boolean to dictate whether Super Agent functionality is enabled or not
     */
    bool m_IsSuperAgentDisabled;

    /**
     * Boolean to indicate whether a Super Agent was found and being listened to
     */
    bool m_IsListeningToSuperAgent;

    /**
     * NotificationProducerSender
     */
    NotificationProducerSender* m_NotificationProducerSender;
    /**
     * NotificationProducerReceiver
     */
    NotificationProducerReceiver* m_NotificationProducerReceiver;
    /**
     * NotificationProducerListener
     */
    NotificationProducerListener* m_NotificationProducerListener;
    /**
     * NotificationDismisserSender
     */
    NotificationDismisserSender* m_NotificationDismisserSender;
    /**
     * NotificationDismisserReceiver
     */
    NotificationDismisserReceiver* m_NotificationDismisserReceiver;

};
} //namespace services
} //namespace ajn

#endif /* TRANSPORT_H_ */
