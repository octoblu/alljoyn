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

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <alljoyn/controlpanel/RootWidget.h>

namespace ajn {
namespace services {

/**
 * Container class. Used to represent a container widget.
 * Container widgets container children widgets and group them together
 */
class Container : public RootWidget {
  public:

    /**
     * Constructor for container Class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     */
    Container(qcc::String const& name, Widget* rootWidget);

    /**
     * Constructor for container Class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param objectPath - the objectPath of the Container
     * @param device - the Device that contains this Container
     */
    Container(qcc::String const& name, Widget* rootWidget, qcc::String const& objectPath, ControlPanelDevice* device);

    /**
     * Constructor for container Class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - the Device that contains this Container
     */
    Container(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device);

    /**
     * Destructor for Container Class
     */
    virtual ~Container();

    /**
     * creates and returns the appropriate BusObject for this Widget
     * @param bus - the bus used to create the widget
     * @param objectPath - the objectPath of the widget
     * @param langIndx - the language Indx
     * @param status - the status indicating success or failure
     * @return a newly created WidgetBusObject
     */
    WidgetBusObject* createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                           uint16_t langIndx, QStatus& status);

    /**
     * Register the BusObjects for this Widget
     * @param bus - the bus used to register the busObjects
     * @param m_LanguageSet -  the languageSet to register the busObjects for
     * @param objectPathPrefix - the objectPathPrefix of the busObject
     * @param objectPathSuffix - the objectPathSuffix of the busObject
     * @param isRoot - is this a rootWidget
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus, LanguageSet const& m_LanguageSet,
                            qcc::String const& objectPathPrefix, qcc::String const& objectPathSuffix, bool isRoot = false);

    /**
     * Register the BusObjects for this Widget
     * @param bus - the bus to be used
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus);


    /**
     * Unregister the BusObjects for this widget
     * @param bus
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

    /**
     * add a Child Widget to the Container
     * @param childWidget - Widget to add
     * @return status - success/failure
     */
    QStatus addChildWidget(Widget* childWidget);

    /**
     * Get the ChildWidget Vector
     * @return children widgets
     */
    const std::vector<Widget*>& getChildWidgets() const;

    /**
     * Get IsDismissable
     * @return isDismissable
     */
    bool getIsDismissable() const;

    /**
     * Set IsDismissable
     * @param isDismissable
     */
    void setIsDismissable(bool isDismissable);

    /**
     * Send a Dismiss Signal
     * @return status - success/failure
     */
    QStatus SendDismissSignal();

  private:

    /**
     * Children Widgets of the Container
     */
    std::vector<Widget*> m_ChildWidgets;

    /**
     * Is this Container dismissable (in a Notification with Action)
     */
    bool m_IsDismissable;

    /**
     * Override default addChildren Implementation to add container's child widgets
     * @param bus - bus used to create and register children
     * @return status - success/failure
     */
    QStatus addChildren(BusAttachment* bus);

    /**
     * Override default refreshChildren Implementation to refresh container's child widgets
     * @param bus - bus used to create and register children
     * @return status - success/failure
     */
    QStatus refreshChildren(BusAttachment* bus);

    /**
     * Create Widget based on WidgetType passed in
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - device that contains Widget
     * @param widgetType - type of widget to create
     * @return newly created widget or NULL if error
     */
    Widget* createWidget(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device, WidgetType widgetType);

};
} //namespace services
} //namespace ajn

#endif /* CONTAINER_H_ */
