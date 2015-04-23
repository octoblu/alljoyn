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

#import "alljoyn/controlpanel/ControlPanelListener.h"
#import "AJCPSControlPanelListener.h"
#import "alljoyn/about/AJNConvertUtil.h"

#ifndef ALLJOYN_CONTROLPANEL_OBJC_AJCPSCONTROLPANELLISTENERADAPTER_H
#define ALLJOYN_CONTROLPANEL_OBJC_AJCPSCONTROLPANELLISTENERADAPTER_H

using namespace ajn::services;

class AJCPSControlPanelListenerAdapter : public ajn::services::ControlPanelListener
{
public:
    id<AJCPSControlPanelListener> listener;

    
    AJCPSControlPanelListenerAdapter(id<AJCPSControlPanelListener> listener) { this->listener = listener;}
    
    id<AJCPSControlPanelListener> getListener() {return this->listener;}
    
    /**
     * sessionEstablished - callback when a session is established with a device
     * @param device - the device that the session was established with
     */
    virtual void sessionEstablished(ControlPanelDevice* device) { [listener sessionEstablished:[[AJCPSControlPanelDevice alloc]initWithHandle:device]];}
    
    /**
     * sessionLost - callback when a session is lost with a device
     * @param device - device that the session was lost with
     */
    virtual void sessionLost(ControlPanelDevice* device) { [listener sessionLost:[[AJCPSControlPanelDevice alloc]initWithHandle:device]];}
    
    /**
     * signalPropertiesChanged - callback when a property Changed signal is received
     * @param device - device signal was received from
     * @param widget - widget signal was received for
     */
    virtual void signalPropertiesChanged(ControlPanelDevice* device, Widget* widget) {

        AJCPSWidget *new_widget = [[AJCPSWidget alloc]initWithHandle:widget];
        
        [listener signalPropertiesChanged:[[AJCPSControlPanelDevice alloc]initWithHandle:device]
                        widget:new_widget];
    }
    
    /**
     * signalPropertyValueChanged - callback when a property Value Changed signal is received
     * @param device - device signal was received from
     * @param property - Property signal was received for
     */
    virtual void signalPropertyValueChanged(ControlPanelDevice* device, Property* property)
    {
        AJCPSControlPanelDevice *qcp_device = [[AJCPSControlPanelDevice alloc]initWithHandle:device];
        
        AJCPSProperty *qcp_property = [[AJCPSProperty alloc]initWithHandle:property];
        
        [listener signalPropertyValueChanged:qcp_device property:qcp_property];
    }
    
    /**
     * signalDismiss - callback when a Dismiss signal is received
     * @param device - device signal was received from
     * @param notificationAction - notificationAction signal was received for
     */
    virtual void signalDismiss(ControlPanelDevice* device, NotificationAction* notificationAction)
    {
        AJCPSControlPanelDevice *qcp_device = [[AJCPSControlPanelDevice alloc]initWithHandle:device];
        
        AJCPSNotificationAction *qcp_notification_action = [[AJCPSNotificationAction alloc]initWithHandle:notificationAction];
        
        [listener signalDismiss:qcp_device notificationAction:qcp_notification_action];
    }
    
    /**
     * ErrorOccured - callback to tell application when something goes wrong
     * @param device - device  that had the error
     * @param status - status associated with error if applicable
     * @param transaction - the type of transaction that resulted in the error
     * @param errorMessage - a log-able error Message
     */
    virtual void errorOccured(ControlPanelDevice* device, QStatus status, ControlPanelTransaction transaction, qcc::String const& errorMessage)
    {
        AJCPSControlPanelDevice *qcp_device = [[AJCPSControlPanelDevice alloc]initWithHandle:device];
        
        AJCPSControlPanelTransaction qcp_controlpanel_transaction = transaction;
        
        NSString *qcp_error_message = [AJNConvertUtil convertQCCStringtoNSString:errorMessage];
        
        [listener errorOccured:qcp_device status:status transaction:qcp_controlpanel_transaction errorMessage:qcp_error_message];
    }

    virtual ~AJCPSControlPanelListenerAdapter() { };
    
    private:
        
};


#endif //ALLJOYN_CONTROLPANEL_OBJC_AJCPSCONTROLPANELLISTENERADAPTER_H
