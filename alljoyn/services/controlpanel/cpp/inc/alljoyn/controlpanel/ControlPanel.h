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

#ifndef CONTROLPANEL_H_
#define CONTROLPANEL_H_

#include <alljoyn/BusObject.h>
#include <alljoyn/controlpanel/Container.h>
#include <alljoyn/controlpanel/ControlPanelDevice.h>

namespace ajn {
namespace services {

class ControlPanelBusObject;

/**
 * ControlPanel Class. Class used to create a ControlPanel
 * ControlPanels are made up of a LanguageSet and a RootContainer
 */
class ControlPanel {
  public:

    /**
     * Function used to create a ControlPanel
     * @param languageSet - languageSet of the ControlPanel
     * @return newly created ControlPanel instance
     */
    static ControlPanel* createControlPanel(LanguageSet* languageSet);

    /**
     * ControlPanel constructor
     * @param languageSet - languageSet of controlPanel
     * @param objectPath - objectPath of controlPanel
     * @param device - device that contains this controlPanel
     */
    ControlPanel(LanguageSet const& languageSet, qcc::String objectPath, ControlPanelDevice* device);

    /**
     * Destructor of ControlPanel
     */
    virtual ~ControlPanel();

    /**
     * Set the RootWidget of the ControlPanel
     * @param rootWidget - rootWidget of the ControlPanel
     * @return status - success/failure
     */
    QStatus setRootWidget(Container* rootWidget);

    /**
     * Get the name of the Panel - the name of the rootWidget
     * @return name of the Panel
     */
    qcc::String getPanelName() const;

    /**
     * Get the RootWidget of the ControlPanel
     * @return rootWidget
     */
    Container* getRootWidget() const;

    /**
     * Register the BusObjects for this Widget
     * @param bus - bus used to register the busObjects
     * @param unitName - the unitName of this ControlPanel
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
     * Unregister the BusObjects for this Widget
     * @param bus - bus used to unregister the busObjects
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

    /**
     * Get the LanguageSet of the ControlPanel
     * @return
     */
    const LanguageSet& getLanguageSet() const;

    /**
     * Get the Device of the ControlPanel
     * @return controlPanelDevice
     */
    ControlPanelDevice* getDevice() const;

    /**
     * Get the objectPath
     * @return
     */
    const qcc::String& getObjectPath() const;

    /**
     * Get the RootWidget of the ControlPanel
     * @param Language
     * @return pointer to rootWidget
     */
    Container* getRootWidget(qcc::String const& Language) const;

  private:

    /**
     * Private Constructor for ControlPanel.
     * To construct instance of ControlPanel used createControlPanel
     * @param languageSet
     */
    ControlPanel(LanguageSet const& languageSet);

    /**
     * LanguageSet of this ControlPanel
     */
    LanguageSet m_LanguageSet;

    /**
     * RootWidget of this ControlPanel
     */
    Container* m_RootWidget;

    /**
     * RootWidgetMap of this ControlPanel
     */
    std::map<qcc::String, Container*> m_RootWidgetMap;

    /**
     * The BusObject for this ControlPanel
     */
    ControlPanelBusObject* m_ControlPanelBusObject;

    /**
     * ObjectPath of the ControlPanel
     */
    qcc::String m_ObjectPath;

    /**
     * Device containing the ControlPanel
     */
    ControlPanelDevice* m_Device;

    /**
     * Copy constructor of ControlPanel - private. ControlPanel is not copy-able
     * @param controlPanel - ControlPanel to copy
     */
    ControlPanel(const ControlPanel& controlPanel);

    /**
     * Assignment operator of ControlPanel - private. ControlPanel is not assignable
     * @param controlPanel
     * @return
     */
    ControlPanel& operator=(const ControlPanel& controlPanel);

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
#endif /* CONTROLPANEL_H_ */

