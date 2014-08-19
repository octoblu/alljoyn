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

#include <alljoyn/notification/Notification.h>
#include <alljoyn/notification/NotificationService.h>
#include "Transport.h"
#include "NotificationConstants.h"
#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/notification/NotificationAsyncTaskEvents.h>
#include <qcc/Log.h>
#include <qcc/String.h>
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace qcc;

NotificationAsyncTaskEvents Notification::m_NotificationAsyncTaskEvents;
AsyncTaskQueue Notification::m_AsyncTaskQueue(&Notification::m_NotificationAsyncTaskEvents);

Notification::Notification(int32_t messageId,
                           NotificationMessageType messageType, const char* deviceId, const char* deviceName,
                           const char* appId, const char* appName, const char* sender,
                           std::map<qcc::String, qcc::String> const& customAttributes,
                           std::vector<NotificationText> const& notificationText,
                           const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                           const char* richIconObjectPath, const char* richAudioObjectPath,
                           const char* controlPanelServiceObjectPath, const char* originalSender) :
    m_MessageId(messageId), m_Sender(0), m_MessageType(messageType), m_DeviceId(0),
    m_DeviceName(0), m_AppId(0), m_AppName(0), m_CustomAttributes(customAttributes),
    m_Text(notificationText), m_RichIconUrl(0), m_RichAudioUrl(richAudioUrl), m_RichIconObjectPath(0),
    m_RichAudioObjectPath(0), m_ControlPanelServiceObjectPath(0), m_OriginalSender(0)
{
    setDeviceId(deviceId);
    setDeviceName(deviceName);
    setAppId(appId);
    setAppName(appName);
    setSender(sender);
    setRichIconUrl(richIconUrl);
    setRichIconObjectPath(richIconObjectPath);
    setRichAudioObjectPath(richAudioObjectPath);
    setControlPanelServiceObjectPath(controlPanelServiceObjectPath);
    setOriginalSender(originalSender);
}

Notification::Notification(NotificationMessageType messageType, std::vector<NotificationText> const& notificationText) :
    m_MessageId(-1), m_Sender(0),  m_MessageType(messageType), m_DeviceId(0), m_DeviceName(0), m_AppId(0), m_AppName(0), m_Text(notificationText), m_RichIconUrl(0),  m_RichIconObjectPath(0),
    m_RichAudioObjectPath(0), m_ControlPanelServiceObjectPath(0), m_OriginalSender(0)

{
}

Notification::Notification(const Notification& notification)
{
    m_Sender = NULL;
    m_DeviceId = NULL;
    m_DeviceName = NULL;
    m_AppId = NULL;
    m_AppName = NULL;
    m_RichIconUrl = NULL;
    m_RichIconObjectPath = NULL;
    m_RichAudioObjectPath = NULL;
    m_ControlPanelServiceObjectPath = NULL;
    m_OriginalSender = NULL;

    setDeviceId(notification.getDeviceId());
    setDeviceName(notification.getDeviceName());
    setAppId(notification.getAppId());
    setAppName(notification.getAppName());
    setSender(notification.getSenderBusName());
    setRichIconUrl(notification.getRichIconUrl());
    setRichIconObjectPath(notification.getRichIconObjectPath());
    setRichAudioObjectPath(notification.getRichAudioObjectPath());
    setControlPanelServiceObjectPath(notification.getControlPanelServiceObjectPath());
    setOriginalSender(notification.getOriginalSender());

    m_CustomAttributes = notification.m_CustomAttributes;
    m_MessageId = notification.m_MessageId;
    m_MessageType = notification.m_MessageType;
    m_Text = notification.m_Text;
    m_RichAudioUrl = notification.m_RichAudioUrl;
}

Notification::~Notification()
{
    delete m_Sender;
    m_Sender = NULL;

    delete m_DeviceId;
    m_DeviceId = NULL;

    delete m_DeviceName;
    m_DeviceName = NULL;

    delete m_AppId;
    m_AppId = NULL;

    delete m_AppName;
    m_AppName = NULL;

    delete m_RichIconUrl;
    m_RichIconUrl = NULL;

    delete m_RichIconObjectPath;
    m_RichIconObjectPath = NULL;

    delete m_RichAudioObjectPath;
    m_RichAudioObjectPath = NULL;

    delete m_ControlPanelServiceObjectPath;
    m_ControlPanelServiceObjectPath = NULL;

    delete m_OriginalSender;
    m_OriginalSender = NULL;
}

const uint16_t Notification::getVersion() const
{
    return NotificationService::getVersion();
}

const char* Notification::getDeviceId() const
{
    if (m_DeviceId == NULL) {
        return NULL;
    }
    return m_DeviceId->c_str();
}

const char* Notification::getDeviceName() const
{
    if (m_DeviceName == NULL) {
        return NULL;
    }
    return m_DeviceName->c_str();
}

const char* Notification::getAppId() const
{
    if (m_AppId == NULL) {
        return NULL;
    }
    return m_AppId->c_str();
}

const char* Notification::getAppName() const
{
    if (m_AppName == NULL) {
        return NULL;
    }
    return m_AppName->c_str();
}

const std::map<qcc::String, qcc::String>& Notification::getCustomAttributes() const
{
    return m_CustomAttributes;
}

const int32_t Notification::getMessageId() const
{
    return m_MessageId;
}

const std::vector<NotificationText>& Notification::getText() const
{
    return m_Text;
}

const char* Notification::getSenderBusName() const
{
    if (m_Sender == NULL) {
        return NULL;
    }
    return m_Sender->c_str();
}

const NotificationMessageType Notification::getMessageType() const
{
    return m_MessageType;
}

const char* Notification::getRichIconUrl() const
{
    if (m_RichIconUrl == NULL) {
        return NULL;
    }
    return m_RichIconUrl->c_str();
}

const char* Notification::getRichIconObjectPath() const
{
    if (m_RichIconObjectPath == NULL) {
        return NULL;
    }
    return m_RichIconObjectPath->c_str();
}

const char* Notification::getRichAudioObjectPath() const
{
    if (m_RichAudioObjectPath == NULL) {
        return NULL;
    }
    return m_RichAudioObjectPath->c_str();
}

const std::vector<RichAudioUrl>& Notification::getRichAudioUrl() const
{
    return m_RichAudioUrl;
}

const char* Notification::getControlPanelServiceObjectPath() const
{
    if (m_ControlPanelServiceObjectPath == NULL) {
        return NULL;
    }
    return m_ControlPanelServiceObjectPath->c_str();
}

const char* Notification::getOriginalSender() const
{
    if (m_OriginalSender == NULL) {
        return NULL;
    }
    return m_OriginalSender->c_str();
}

void Notification::setAppId(const char* appId) {
    if (appId == NULL) {
        delete m_AppId;
        m_AppId = NULL;
        return;
    }

    if (!m_AppId) {
        m_AppId = new qcc::String(appId);
    } else {
        m_AppId->assign(appId);
    }
}

void Notification::setAppName(const char* appName) {
    if (appName == NULL) {
        delete m_AppName;
        m_AppName = NULL;
        return;
    }

    if (!m_AppName) {
        m_AppName = new qcc::String(appName);
    } else {
        m_AppName->assign(appName);
    }
}

void Notification::setControlPanelServiceObjectPath(
    const char* controlPanelServiceObjectPath) {
    if (controlPanelServiceObjectPath == NULL) {
        delete m_ControlPanelServiceObjectPath;
        m_ControlPanelServiceObjectPath = NULL;
        return;
    }

    if (!m_ControlPanelServiceObjectPath) {
        m_ControlPanelServiceObjectPath = new qcc::String(controlPanelServiceObjectPath);
    } else {
        m_ControlPanelServiceObjectPath->assign(controlPanelServiceObjectPath);
    }
}

void Notification::setCustomAttributes(
    const std::map<qcc::String, qcc::String>& customAttributes) {
    m_CustomAttributes = customAttributes;
}

void Notification::setDeviceId(const char* deviceId) {
    if (deviceId == NULL) {
        delete m_DeviceId;
        m_DeviceId = NULL;
        return;
    }

    if (!m_DeviceId) {
        m_DeviceId = new qcc::String(deviceId);
    } else {
        m_DeviceId->assign(deviceId);
    }
}

void Notification::setDeviceName(const char* deviceName) {
    if (deviceName == NULL) {
        delete m_DeviceName;
        m_DeviceName = NULL;
        return;
    }

    if (!m_DeviceName) {
        m_DeviceName = new qcc::String(deviceName);
    } else {
        m_DeviceName->assign(deviceName);
    }
}

void Notification::setOriginalSender(const char* originalSender) {
    if (originalSender == NULL) {
        delete m_OriginalSender;
        m_OriginalSender = NULL;
        return;
    }

    if (!m_OriginalSender) {
        m_OriginalSender = new qcc::String(originalSender);
    } else {
        m_OriginalSender->assign(originalSender);
    }
}

void Notification::setMessageId(int32_t messageId) {
    m_MessageId = messageId;
}

void Notification::setRichAudioUrl(
    const std::vector<RichAudioUrl>& richAudioUrl) {
    m_RichAudioUrl = richAudioUrl;
}

void Notification::setRichIconUrl(const char* richIconUrl) {
    if (richIconUrl == NULL) {
        delete m_RichIconUrl;
        m_RichIconUrl = NULL;
        return;
    }

    if (!m_RichIconUrl) {
        m_RichIconUrl = new qcc::String(richIconUrl);
    } else {
        m_RichIconUrl->assign(richIconUrl);
    }
}

void Notification::setRichIconObjectPath(const char* richIconObjectPath) {
    if (richIconObjectPath == NULL) {
        delete m_RichIconObjectPath;
        m_RichIconObjectPath = NULL;
        return;
    }

    if (!m_RichIconObjectPath) {
        m_RichIconObjectPath = new qcc::String(richIconObjectPath);
    } else {
        m_RichIconObjectPath->assign(richIconObjectPath);
    }
}

void Notification::setRichAudioObjectPath(const char* richAudioObjectPath) {
    if (richAudioObjectPath == NULL) {
        delete m_RichAudioObjectPath;
        m_RichAudioObjectPath = NULL;
        return;
    }

    if (!m_RichAudioObjectPath) {
        m_RichAudioObjectPath = new qcc::String(richAudioObjectPath);
    } else {
        m_RichAudioObjectPath->assign(richAudioObjectPath);
    }
}

void Notification::setSender(const char* sender) {
    if (sender == NULL) {
        delete m_Sender;
        m_Sender = NULL;
        return;
    }

    if (!m_Sender) {
        m_Sender = new qcc::String(sender);
    } else {
        m_Sender->assign(sender);
    }
}

QStatus Notification::dismiss()
{
    QCC_DbgPrintf(("Notification::dismiss() called OriginalSender:%s MessageId:%d AppId:%s", getOriginalSender(), getMessageId(), getAppId()));

    NotificationMsg* notificationMsg = new NotificationMsg(getOriginalSender(), getMessageId(), getAppId());
    m_AsyncTaskQueue.Enqueue(notificationMsg);

    return ER_OK;
}

