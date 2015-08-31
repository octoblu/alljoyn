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

#ifndef CONTROLPANELCONTROLLEEUNIT_H_
#define CONTROLPANELCONTROLLEEUNIT_H_

#include <alljoyn/controlpanel/ControlPanel.h>
#include <alljoyn/controlpanel/NotificationAction.h>
#include <alljoyn/controlpanel/HttpControl.h>

namespace ajn {
namespace services {

/**
 * Class that represents a ControlPanel Unit.
 */
class ControlPanelControlleeUnit {
  public:

    /**
     * Constructor of ControlPanelControlleeUnit
     * @param unitName - name of ControlPanelControllee Unit
     */
    ControlPanelControlleeUnit(qcc::String const& unitName);

    /**
     * Destructor of ControlPanelControlleeUnit
     */
    virtual ~ControlPanelControlleeUnit();

    /**
     * Get the UnitName of the ControlPanelControlleeUnit
     * @return unitName
     */
    const qcc::String& getUnitName() const;

    /**
     * Add a ControlPanel to the ControlPanelControlleeUnit
     * @param controlPanel
     * @return status - success/failure
     */
    QStatus addControlPanel(ControlPanel* controlPanel);

    /**
     * Get the ControlPanels of the ControlPanelControlleeUnit
     * @return ControlPanels
     */
    const std::vector<ControlPanel*>& getControlPanels() const;

    /**
     * Add a NotificationAction to the ControlPanelControlleeUnit
     * @param notificationAction
     * @return status - success/failure
     */
    QStatus addNotificationAction(NotificationAction* notificationAction);

    /**
     * Get the NotificationActions of the ControlPanelControlleeUnit
     * @return the NotificationActions
     */
    const std::vector<NotificationAction*>& getNotificationActions() const;

    /**
     * Set the HTTPControl of the ControlPanelControlleeUnit
     * @param httpControl
     * @return status - success/failure
     */
    QStatus setHttpControl(HttpControl* httpControl);

    /**
     * Get the HttpControl of the ControlPanelControlleeUnit
     * @return HttpControl
     */
    const HttpControl* getHttpControl() const;
    /**
     * Register the busObjects of the ControlPanelControlleeUnit
     * @param bus - the bus to register the Objects with
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus);

    /**
     * UnRegister the busObjects of the ControlPanelControlleeUnit
     * @param bus - the bus to unregister the Objects with
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

  private:

    /**
     * Name of Unit
     */
    qcc::String m_UnitName;

    /**
     * Vector of ControlPanels implemented by the Controllee
     */
    std::vector<ControlPanel*> m_ControlPanels;

    /**
     * Vector of NotificationActions implemented by the Controllee
     */
    std::vector<NotificationAction*> m_NotificationActions;

    /**
     * The HTTPControl of the Unit
     */
    HttpControl* m_HttpControl;
};

} /* namespace services */
} /* namespace ajn */
#endif /* CONTROLPANELCONTROLLEEUNIT_H_ */
