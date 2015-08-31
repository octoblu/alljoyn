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

#ifndef CONTROLLERNOTIFICATIONRECEIVER_H_
#define CONTROLLERNOTIFICATIONRECEIVER_H_

#include <vector>
#include <alljoyn/controlpanel/ControlPanelController.h>
#include <alljoyn/notification/NotificationReceiver.h>
#include <alljoyn/notification/Notification.h>

/**
 * Class that will receive Notifications. Implements NotificationReceiver
 * Receives list of applications to filter by and will only display notifications
 * from those applications
 */
class ControllerNotificationReceiver : public ajn::services::NotificationReceiver {
  public:

    /**
     * Constructor
     */
    ControllerNotificationReceiver(ajn::services::ControlPanelController* controlPanelController);

    /**
     * Destructor
     */
    ~ControllerNotificationReceiver();

    /**
     * Receive - function that receives a notification
     * @param notification
     */
    void Receive(ajn::services::Notification const& notification);

    /**
     * receive Dismiss signal
     * @param message id
     * @param application id
     */
    void Dismiss(const int32_t msgId, const qcc::String appId);
  private:

    ajn::services::ControlPanelController* m_Controller;

};

#endif /* CONTROLLERNOTIFICATIONRECEIVER_H_ */
