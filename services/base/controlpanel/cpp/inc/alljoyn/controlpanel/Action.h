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

#ifndef ACTION_H_
#define ACTION_H_

#include <alljoyn/controlpanel/Widget.h>

namespace ajn {
namespace services {

/**
 * Action Class. Used to display a Button.
 * This is an abstract class. The executeCallback function needs to be overridden.
 */
class Action : public Widget {
  public:

    /**
     * Constructor for Action class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     */
    Action(qcc::String const& name, Widget* rootWidget);

    /**
     * Constructor for Action class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - the Device that contains this Action
     */
    Action(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device);


    /**
     * Destructor for Action Class
     */
    virtual ~Action();

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
     * Call to execute this Action remotely
     * @return status - success/failure
     */
    QStatus executeAction();

    /**
     * Callback for when Action is executed.
     * Virtual function that needs to be implemented.
     * @return success/failure
     */
    virtual bool executeCallBack();
};
} //namespace services
} //namespace ajn

#endif /* ACTION_H_ */
