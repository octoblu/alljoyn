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

#ifndef ERRORWIDGET_H_
#define ERRORWIDGET_H_

#include <alljoyn/controlpanel/Widget.h>

namespace ajn {
namespace services {

/**
 * ErrorWidget class - used when there's an in error retrieving widget information
 */
class ErrorWidget : public Widget {

  public:

    /**
     * Constructor for Error Eisget
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param originalWidget - pointer to original widget that received the error
     * @param device - the device containing this Widget
     */
    ErrorWidget(qcc::String const& name, Widget* rootWidget, Widget* originalWidget, ControlPanelDevice* device);

    /**
     *
     */
    virtual ~ErrorWidget();

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
     * @param bus - the bus used to register the busObject
     * @param objectPath - the objectPath of the busObject
     * @return
     */
    QStatus registerObjects(BusAttachment* bus, qcc::String const& objectPath);

    /**
     * Unregister the BusObjects for this widget
     * @param bus
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

    /**
     * Get the Original Widget
     */
    Widget* getOriginalWidget() const;

  private:

    /**
     * the widget that was originally created unsuccessfully resulting in the creation if the ErrorWidget
     */
    Widget* m_OriginalWidget;
};
} //namespace services
} //namespace ajn

#endif /* ERRORWIDGET_H_ */
