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


#ifndef CONTROLPANELLISTENER_H_
#define CONTROLPANELLISTENER_H_

#include <alljoyn/controlpanel/ControlPanelDevice.h>
#include <alljoyn/controlpanel/Property.h>

namespace ajn {
namespace services {

/**
 * An Abstract class with function receive. The implementation of this class
 * can be passed in to the initReceive function and will be the callback for
 * when notifications are received
 */
class ControlPanelListener {
  public:

    /**
     * Constructor for ControlPanelListener
     */
    ControlPanelListener() { };

    /**
     * Destructor for ControlPanelListener
     */
    virtual ~ControlPanelListener() { };

    /**
     * sessionEstablished - callback when a session is established with a device
     * @param device - the device that the session was established with
     */
    virtual void sessionEstablished(ControlPanelDevice* device) = 0;

    /**
     * sessionLost - callback when a session is lost with a device
     * @param device - device that the session was lost with
     */
    virtual void sessionLost(ControlPanelDevice* device) = 0;

    /**
     * signalPropertiesChanged - callback when a property Changed signal is received
     * @param device - device signal was received from
     * @param widget - widget signal was received for
     */
    virtual void signalPropertiesChanged(ControlPanelDevice* device, Widget* widget) = 0;

    /**
     * signalPropertyValueChanged - callback when a property Value Changed signal is received
     * @param device - device signal was received from
     * @param property - Property signal was received for
     */
    virtual void signalPropertyValueChanged(ControlPanelDevice* device, Property* property) = 0;

    /**
     * signalDismiss - callback when a Dismiss signal is received
     * @param device - device signal was received from
     * @param notificationAction - notificationAction signal was received for
     */
    virtual void signalDismiss(ControlPanelDevice* device, NotificationAction* notificationAction) = 0;

    /**
     * ErrorOccured - callback to tell application when something goes wrong
     * @param device - device  that had the error
     * @param status - status associated with error if applicable
     * @param transaction - the type of transaction that resulted in the error
     * @param errorMessage - a log-able error Message
     */
    virtual void errorOccured(ControlPanelDevice* device, QStatus status, ControlPanelTransaction transaction, qcc::String const& errorMessage) = 0;
};
} //namespace services
} //namespace ajn

#endif /* CONTROLPANELLISTENER_H_ */


