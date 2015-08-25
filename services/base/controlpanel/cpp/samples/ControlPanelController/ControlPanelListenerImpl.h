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

#ifndef CONTROLPANELLISTENERIMPL_H_
#define CONTROLPANELLISTENERIMPL_H_

#include <alljoyn/controlpanel/ControlPanelListener.h>
#include <alljoyn/controlpanel/ControlPanelController.h>

/*
 *
 */
class ControlPanelListenerImpl : public ajn::services::ControlPanelListener {
  public:

    ControlPanelListenerImpl(ajn::services::ControlPanelController* controller);

    ~ControlPanelListenerImpl();

    void sessionEstablished(ajn::services::ControlPanelDevice* device);

    void sessionLost(ajn::services::ControlPanelDevice* device);

    void errorOccured(ajn::services::ControlPanelDevice* device, QStatus status,
                      ajn::services::ControlPanelTransaction transaction, qcc::String const& error);

    void signalPropertiesChanged(ajn::services::ControlPanelDevice* device, ajn::services::Widget* widget);

    void signalPropertyValueChanged(ajn::services::ControlPanelDevice* device, ajn::services::Property* property);

    void signalDismiss(ajn::services::ControlPanelDevice* device, ajn::services::NotificationAction* notificationAction);

  private:

    ajn::services::ControlPanelController* m_Controller;

    std::vector<qcc::String> m_ConnectedDevices;

};

#endif /* CONTROLPANELLISTENERIMPL_H_ */
