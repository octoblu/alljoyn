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

#ifndef ACTIONWITHDIALOG_H_
#define ACTIONWITHDIALOG_H_

#include <alljoyn/controlpanel/Widget.h>
#include <alljoyn/controlpanel/Dialog.h>

namespace ajn {
namespace services {

/**
 * ActionWithDialog Class. Used to display a Button.
 * Upon pressing the button a Dialog is displayed on the Controller side
 */
class ActionWithDialog : public Widget {
  public:

    /**
     * Constructor for Action with Dialog class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     */
    ActionWithDialog(qcc::String const& name, Widget* rootWidget);

    /**
     * Constructor for Action with Dialog class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - device that contains this Action
     */
    ActionWithDialog(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device);

    /**
     * Destructor for Action with Dialog Class
     */
    virtual ~ActionWithDialog();

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
     * add the ChildDialog for the Action With Dialog
     * @param childDialog - dialog to add as child
     * @return Status - success/failure
     */
    QStatus addChildDialog(Dialog* childDialog);

    /**
     * Get the ChildDialog of the Action
     * @return dialog
     */
    Dialog* getChildDialog() const;

    /**
     * Register the BusObjects for this and children Widgets
     * @param bus - bus used to register the busObjects
     * @param languageSet - languageSet objects need to be registered for
     * @param objectPathPrefix - Prefix for the ObjectPath
     * @param objectPathSuffix - Suffix for the ObjectPath
     * @param isRoot - is this a Root Widget
     * @return Status - success/failure
     */
    virtual QStatus registerObjects(BusAttachment* bus, LanguageSet const& languageSet,
                                    qcc::String const& objectPathPrefix,
                                    qcc::String const& objectPathSuffix, bool isRoot = false);

    /**
     * Unregister the BusObjects for this and children Widgets
     * @param bus - the bus used to unregister the busObjects
     * @return status - success/failure
     */
    virtual QStatus unregisterObjects(BusAttachment* bus);

  private:

    /**
     * The Child Dialog of the Action
     */
    Dialog* m_Dialog;

    /**
     * Add ChildDialog for Action
     * @param bus - busAttachment to use
     * @return status - success/failure
     */
    QStatus addChildren(BusAttachment* bus);

    /**
     * Override default refreshChildren Implementation to refresh Action's child dialog
     * @param bus - bus used to create and register children
     * @return status - success/failure
     */
    QStatus refreshChildren(BusAttachment* bus);

};
} //namespace services
} //namespace ajn

#endif /* ACTIONWITHDIALOG_H_ */
