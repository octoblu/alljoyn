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

#include "WidgetProxyBusObjectListener.h"
#include "../ControlPanelConstants.h"
#include "WidgetBusObject.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

WidgetProxyBusObjectListener::WidgetProxyBusObjectListener(Widget* widget, WidgetBusObject* busObject) :
    m_Widget(widget), m_BusObject(busObject)
{


}

WidgetProxyBusObjectListener::~WidgetProxyBusObjectListener()
{

}

void WidgetProxyBusObjectListener::GetAllPropertiesCallBack(QStatus status, ProxyBusObject* obj, const MsgArg& values, void* context)
{
    if (!m_Widget || !m_BusObject) {
        QCC_DbgHLPrintf(("WidgetProxyBusObjectListener does not have widget or BusObject set"));
        delete this;     //Finished using listener - needs to be deleted
        return;
    }

    ControlPanelDevice* device = m_Widget->getDevice();
    ControlPanelListener* listener = device ? device->getListener() : NULL;
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Something went wrong reloading properties"));
        if (listener) {
            listener->errorOccured(device, status, REFRESH_PROPERTIES, "Something went wrong reloading properties");
        }
        delete this;     //Finished using listener - needs to be deleted
        return;
    }

    status = m_BusObject->fillAllProperties(values);
    if (status != ER_OK) {
        QCC_LogError(status, ("Something went wrong reloading properties"));
        if (listener) {
            listener->errorOccured(device, status, REFRESH_PROPERTIES, "Something went wrong reloading properties");
        }
        delete this;     //Finished using listener - needs to be deleted
        return;
    }

    if (listener) {
        listener->signalPropertiesChanged(device, m_Widget);
    }

    delete this;     //Finished using listener - needs to be deleted
}

} /* namespace services */
} /* namespace ajn */
