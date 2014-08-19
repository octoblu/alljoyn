/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <vector>
#include <map>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include <alljoyn/Status.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>
#include "NotificationAsyncTaskEvents.h"
#include <qcc/String.h>

namespace ajn {
namespace services {

/**
 * NotificationMsg
 * This message will be sent when user uses interface NotificationProducer
 * Implicit or explicit copy constructor must work.
 */
class NotificationMsg : public TaskData {
  public:
    /**
     * NotificationMsg constructor
     */
    NotificationMsg(const char* originalSender, int32_t messageId, const char* appId) :
        m_OriginalSender(originalSender), m_MessageId(messageId), m_AppId(appId)
    {
    }
    /**
     * NotificationMsg destructor
     */
    ~NotificationMsg()
    {
    }

  public:
    /**
     * The original sender of the notification
     */
    const qcc::String m_OriginalSender;
    /**
     * The Notification's Message Id
     */
    const int32_t m_MessageId;
    /**
     * The Notification's App Id
     */
    const qcc::String m_AppId;
};


/**
 * The Notification Class is used to send and receive Notifications
 */
class Notification {
  public:

    /**
     * Constructor for Notification
     * @param messageId
     * @param messageType
     * @param deviceId
     * @param deviceName
     * @param appId
     * @param appName
     * @param sender
     * @param customAttributes
     * @param notificationText
     * @param richIconUrl
     * @param richAudioUrl
     * @param richIconObjectPath
     * @param richAudioObjectPath
     * @param controlPanelServiceObjectPath
     * @param originalSender
     */
    Notification(int32_t messageId, NotificationMessageType messageType,
                 const char* deviceId, const char* deviceName,
                 const char* appId, const char* appName,
                 const char* sender, std::map<qcc::String, qcc::String> const& customAttributes,
                 std::vector<NotificationText> const& notificationText,
                 const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                 const char* richIconObjectPath, const char* richAudioObjectPath,
                 const char* controlPanelServiceObjectPath, const char* originalSender);


    /**
     * Construct for Notification
     * @param messageType
     * @param notificationText
     */
    Notification(NotificationMessageType messageType,
                 std::vector<NotificationText> const& notificationText);

    /**
     * Destructor of Notification
     */
    ~Notification();

    /**
     * copy constructor
     */
    Notification(const Notification& notification);
    /**
     * Get the Version
     * @return version
     */
    const uint16_t getVersion() const;

    /**
     * Get the device Id
     * @return deviceId
     */
    const char* getDeviceId() const;

    /**
     * Get the Device Name
     * @return deviceName
     */
    const char* getDeviceName() const;

    /**
     * Get the app Id
     * @return appId
     */
    const char* getAppId() const;

    /**
     * Get the app Name
     * @return appName
     */
    const char* getAppName() const;

    /**
     * Get the map of customAttributes
     * @return customAttributes
     */
    const std::map<qcc::String, qcc::String>& getCustomAttributes() const;

    /**
     * Get the Message Id
     * @return notificationId
     */
    const int32_t getMessageId() const;

    /**
     * Get the Sender
     * @return Sender
     */
    const char* getSenderBusName() const;

    /**
     * Get the MessageType
     * @return MessageType
     */
    const NotificationMessageType getMessageType() const;

    /**
     * Get the Notification Text
     * @return notificationText
     */
    const std::vector<NotificationText>& getText() const;

    /**
     * Get the Rich Icon Url
     * @return richIconUrl
     */
    const char* getRichIconUrl() const;

    /**
     * Get the Rich Icon Object Path
     * @return richIconObjectPath
     */
    const char* getRichIconObjectPath() const;

    /**
     * Get the Rich Audio Object Path
     * @return richAudioObjectPath
     */
    const char* getRichAudioObjectPath() const;

    /**
     * Get the Rich Audio Urls
     * @return RichAudioUrl
     */
    const std::vector<RichAudioUrl>& getRichAudioUrl() const;

    /**
     * Get the ControlPanelService object path
     * @return ControlPanelServiceObjectPath
     */
    const char* getControlPanelServiceObjectPath() const;

    /**
     * Get the OriginalSender object path
     * @return OriginalSender
     */
    const char* getOriginalSender() const;

    /**
     * Set the App Id of the Notification
     * @param appId
     */
    void setAppId(const char* appId);

    /**
     * Set the App Name of the Notification
     * @param appName
     */
    void setAppName(const char* appName);

    /**
     * Set the Control Panel Service Object Path of the Notification
     * @param controlPanelServiceObjectPath
     */
    void setControlPanelServiceObjectPath(
        const char* controlPanelServiceObjectPath);

    /**
     * Set the Custom Attributed of the Notification
     * @param customAttributes
     */
    void setCustomAttributes(
        const std::map<qcc::String, qcc::String>& customAttributes);

    /**
     * Set the deviceId of the Notification
     * @param deviceId
     */
    void setDeviceId(const char* deviceId);

    /**
     * Set the deviceName of the Notification
     * @param deviceName
     */
    void setDeviceName(const char* deviceName);

    /**
     * Set the messageId of the Notification
     * @param messageId
     */
    void setMessageId(int32_t messageId);

    /**
     * Set the richAudioUrl of the Notification
     * @param richAudioUrl
     */
    void setRichAudioUrl(const std::vector<RichAudioUrl>& richAudioUrl);

    /**
     * Set the richIconUrl of the Notification
     * @param richIconUrl
     */
    void setRichIconUrl(const char* richIconUrl);

    /**
     * Set the richIconObjectPath of the Notification
     * @param richIconObjectPath
     */
    void setRichIconObjectPath(const char* richIconObjectPath);

    /**
     * Set the richAudioObjectPath of the Notification
     * @param richAudioObjectPath
     */
    void setRichAudioObjectPath(const char* richAudioObjectPath);

    /**
     * Set the sender of the Notification
     * @param sender
     */
    void setSender(const char* sender);
    /**
     * dismiss
     */
    QStatus dismiss();

    /**
     * Responsible to get and handle events comming from the queue.
     */
    static NotificationAsyncTaskEvents m_NotificationAsyncTaskEvents;
    /**
     * Purpose is to handle tasks asynchronously.
     * Handling user dismiss of the notification.
     */
    static AsyncTaskQueue m_AsyncTaskQueue;

  private:
    /**
     * assignment operator is forbidden
     */
    Notification& operator=(const Notification&);

    /**
     * set the original sender
     */
    void setOriginalSender(const char* originalSender);

    /**
     * The Notification's Message Id
     */
    int32_t m_MessageId;

    /**
     * The Notifications Sender
     */
    qcc::String* m_Sender;

    /**
     * The Notifications MessageType
     */
    NotificationMessageType m_MessageType;

    /**
     * The Notification's Device Id
     */
    qcc::String* m_DeviceId;

    /**
     * The Notification's Device Name
     */
    qcc::String* m_DeviceName;

    /**
     * The Notification's App Id
     */
    qcc::String* m_AppId;

    /**
     * The Notification's App Name
     */
    qcc::String* m_AppName;

    /**
     * The Notification's map of Custom Attributs
     */
    std::map<qcc::String, qcc::String> m_CustomAttributes;

    /**
     * The Notification's vector of Notification Texts
     */
    std::vector<NotificationText> m_Text;

    /**
     * The Notification's rich Icon Url
     */
    qcc::String* m_RichIconUrl;

    /**
     * The Notification's vector of Rich Audio Urls
     */
    std::vector<RichAudioUrl> m_RichAudioUrl;

    /**
     * The Notification's rich Icon Object Path
     */
    qcc::String* m_RichIconObjectPath;

    /**
     * The Notification's rich Audio Object
     */
    qcc::String* m_RichAudioObjectPath;

    /**
     * The Notification's ControlPanelService object path
     */
    qcc::String* m_ControlPanelServiceObjectPath;

    /**
     * The Notification's original sender
     */
    qcc::String* m_OriginalSender;
};

} //namespace services
} //namespace ajn

#endif /* NOTIFICATION_H_ */

