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

#ifndef NOTIFICATIONACTION_H_
#define NOTIFICATIONACTION_H_

#include <alljoyn/controlpanel/RootWidget.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/controlpanel/ControlPanelDevice.h>

namespace ajn {
namespace services {

class NotificationActionBusObject;

/**
 * NotificationAction class.
 */
class NotificationAction {
  public:

    /**
     * Function used to create a NotificationAction
     * @param languageSet - languageSet of NotificationAction
     * @return A newly created NotificationAction object or NULL
     */
    static NotificationAction* createNotificationAction(LanguageSet* languageSet);

    /**
     * NotificationAction constructor
     * @param languageSet - languageSet of NotificationAction
     * @param objectPath - objectPath of NotificationAction
     * @param device - device that contains this NotificationAction
     */
    NotificationAction(LanguageSet const& languageSet, qcc::String objectPath, ControlPanelDevice* device);

    /**
     * Destructor of NotificationAction class
     */
    virtual ~NotificationAction();

    /**
     * Set the rootWidget of the NotificationAction
     * @param rootWidget - widget to set as RootWidget
     * @return status - success/failure
     */
    QStatus setRootWidget(RootWidget* rootWidget);

    /**
     * Get the name of the NotificationAction - the name of the rootWidget
     * @return name of the NotificationAction
     */
    qcc::String getNotificationActionName() const;

    /**
     * Register the BusObjects of the Notification Action
     * @param bus - bus to register the objects on
     * @param unitName - unitName to use in ObjectPath
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus, qcc::String const& unitName);

    /**
     * Register the BusObjects for this Widget
     * @param bus - bus used to register the busObjects
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus);

    /**
     * Unregister the BusObjects of the NotificationAction class
     * @param bus - bus used to unregister the objects
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

    /**
     * Get the LanguageSet of the NotificationAction
     * @return
     */
    const LanguageSet& getLanguageSet() const;

    /**
     * Get the Device of the NotificationAction
     * @return controlPanelDevice
     */
    ControlPanelDevice* getDevice() const;

    /**
     * Get the objectPath
     * @return
     */
    const qcc::String& getObjectPath() const;

    /**
     * Get the RootWidget of the NotificationAction
     * @param Language - languageSet of RootWidget to retrieve
     * @return rootWidget
     */
    RootWidget* getRootWidget(qcc::String const& Language) const;

    /**
     * Callback when DismissSignal is received by the BusObject
     */
    void DismissSignal();

  private:

    /**
     * Private constructor of NotificationAction class
     * @param languageSet - languageSet of NotificationAction
     */
    NotificationAction(LanguageSet const& languageSet);

    /**
     * The LanguageSet of the NotificationAction
     */
    LanguageSet m_LanguageSet;

    /**
     * The RootWidget of the NotificationAction
     */
    RootWidget* m_RootWidget;

    /**
     * RootWidgetMap of this NotificationAction
     */
    std::map<qcc::String, RootWidget*> m_RootWidgetMap;

    /**
     * The BusObject of the NotificationAction
     */
    NotificationActionBusObject* m_NotificationActionBusObject;

    /**
     * ObjectPath of the NotificationAction
     */
    qcc::String m_ObjectPath;

    /**
     * Device containing the NotificationAction
     */
    ControlPanelDevice* m_Device;

    /**
     * Copy constructor of NotificationAction - private. NotificationAction is not copy-able
     * @param notificationAction - notificationAction to copy
     */
    NotificationAction(const NotificationAction& notificationAction);

    /**
     * Assignment operator of NotificationAction - private. NotificationAction is not assignable
     * @param notificationAction
     * @return
     */
    NotificationAction& operator=(const NotificationAction& notificationAction);

    /**
     * Check the compatibility of the versions with remote controlpanel
     * @return status - success/failure
     */
    QStatus checkVersions();

    /**
     * Add the Child Widgets
     * @return status - success/failure
     */
    QStatus addChildren();
};

} /* namespace services */
} /* namespace ajn */
#endif /* NOTIFICATIONACTION_H_ */


