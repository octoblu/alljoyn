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

#include <alljoyn/controlpanel/RootWidget.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../BusObjects/NotificationActionBusObject.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {

RootWidget::RootWidget(qcc::String const& name, Widget* rootWidget, WidgetType widgetType) :
    Widget(name, rootWidget, widgetType), m_NotificationActionBusObject(0), m_ObjectPath("")
{
}

RootWidget::RootWidget(qcc::String const& name, Widget* rootWidget, qcc::String const& objectPath, ControlPanelDevice* device,
                       WidgetType widgetType) :
    Widget(name, rootWidget, device, widgetType), m_NotificationActionBusObject(0), m_ObjectPath(objectPath)
{
}

RootWidget::~RootWidget()
{
}

QStatus RootWidget::SendDismissSignal()
{
    if (!m_NotificationActionBusObject) {
        return ER_BUS_OBJECT_NOT_REGISTERED;
    }

    return ((NotificationActionBusObject*)m_NotificationActionBusObject)->SendDismissSignal();
}

QStatus RootWidget::setNotificationActionBusObject(BusObject* notificationActionBusObject)
{
    if (!notificationActionBusObject) {
        QCC_DbgHLPrintf(("Could not add a NULL notificationActionBusObject"));
        return ER_BAD_ARG_1;
    }

    if (m_NotificationActionBusObject) {
        QCC_DbgHLPrintf(("Could not set notificationActionBusObject. NotificationActionBusObject already set"));
        return ER_BUS_PROPERTY_ALREADY_EXISTS;
    }

    m_NotificationActionBusObject = notificationActionBusObject;
    return ER_OK;
}

QStatus RootWidget::unregisterObjects(BusAttachment* bus)
{
    QStatus status = Widget::unregisterObjects(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister BusObjects"));
    }

    if (m_NotificationActionBusObject) {
        bus->UnregisterBusObject(*m_NotificationActionBusObject);
        delete m_NotificationActionBusObject;
        m_NotificationActionBusObject = 0;
    }
    return status;
}

QStatus RootWidget::registerObjects(BusAttachment* bus)
{
    return Widget::registerObjects(bus, m_ObjectPath);
}

QStatus RootWidget::registerObjects(BusAttachment* bus, qcc::String const& objectPath)
{
    return Widget::registerObjects(bus, objectPath);
}

QStatus RootWidget::registerObjects(BusAttachment* bus, LanguageSet const& languageSet, qcc::String const& objectPathPrefix,
                                    qcc::String const& objectPathSuffix, bool isRoot)
{
    return Widget::registerObjects(bus, languageSet, objectPathPrefix, objectPathSuffix, isRoot);
}

} /* namespace services */
} /* namespace ajn */
