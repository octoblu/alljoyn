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

#ifndef WIDGETPROXYBUSOBJECTLISTENER_H_
#define WIDGETPROXYBUSOBJECTLISTENER_H_

#include <alljoyn/BusAttachment.h>
#include "WidgetBusObject.h"
#include <alljoyn/controlpanel/Widget.h>

namespace ajn {
namespace services {

/**
 * WidgetProxyBusObjectListener class - used as listener for GetAllProperties Async Call
 */
class WidgetProxyBusObjectListener : public ProxyBusObject::Listener {

  public:

    /**
     * Constructor of WidgetProxyBusObjectListener class
     * @param widget - widget getallproperties is being executed for
     * @param busObject - busObject being used for call
     */
    WidgetProxyBusObjectListener(Widget* widget, WidgetBusObject* busObject);

    /**
     * Destructor of WidgetProxyBusObjectListener class
     */
    virtual ~WidgetProxyBusObjectListener();

    /**
     * GetAllProperties callback
     * @param status - status of call: success/failure
     * @param obj - proxybusobject used for call
     * @param values - response MsgArg
     * @param context - context passed in to call
     */
    void GetAllPropertiesCallBack(QStatus status, ProxyBusObject* obj, const MsgArg& values, void* context);

  private:

    /**
     * The widget getAllProperties was being executed for
     */
    Widget* m_Widget;

    /**
     * The BusObject getAllProperties was being executed from
     */
    WidgetBusObject* m_BusObject;
};

} /* namespace services */
} /* namespace ajn */

#endif /* WIDGETPROXYBUSOBJECTLISTENER_H_ */
