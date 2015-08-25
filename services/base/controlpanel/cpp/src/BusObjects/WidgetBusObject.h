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

#ifndef WIDGETBUSOBJECT_H_
#define WIDGETBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/controlpanel/Widget.h>
#include "IntrospectionNode.h"

namespace ajn {
namespace services {

/**
 * WidgetBusObject class. Base class to create a BusObject for a Widget
 */
class WidgetBusObject : public ajn::BusObject {

    friend class WidgetProxyBusObjectListener;
  public:

    /**
     * Constructor for WidgetBusObject class
     * @param objectPath - objectPath of BusObject
     * @param langIndx - the languageIndex of the BusObject
     * @param status - success/failure
     * @param widget - the widget associated with the BusObject
     */
    WidgetBusObject(qcc::String const& objectPath, uint16_t langIndx, QStatus& status, Widget* widget);

    /**
     * Destructor of the WidgetBusObject class
     */
    virtual ~WidgetBusObject() = 0;

    /**
     * Get the Version of the Interface represented by this BusObject
     * @return - interfaceVersion
     */
    virtual uint16_t getInterfaceVersion();

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg to fill
     * @return status - success/failure
     */
    virtual QStatus Get(const char* interfaceName, const char* propName, MsgArg& val);

    /**
     * Callback for Alljoyn when SetProperty is called on this BusObject
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg that contains the new Value
     * @return status - success/failure
     */
    virtual QStatus Set(const char* interfaceName, const char* propName, MsgArg& val);

    /**
     * Send a signal when a property has changed
     * @return status - success/failure
     */
    virtual QStatus SendPropertyChangedSignal();

    /**
     * Callback for PropertyChanged signal
     * @param member - signal received
     * @param srcPath - objectPath of signal
     * @param msg - Message received
     */
    void PropertyChanged(const InterfaceDescription::Member* member, const char* srcPath, Message& msg);

    /**
     * Set the RemoteController to allow remote calls
     * @param bus - bus used to create the proxyBusObject
     * @param deviceBusName - busName of the device
     * @param sessionId - sessionId of Remote device
     * @return status - success/failure
     */
    virtual QStatus setRemoteController(BusAttachment* bus, qcc::String const& deviceBusName, SessionId sessionId);

    /**
     * Check compatibility of the versions
     * @return status - success/failure
     */
    virtual QStatus checkVersions();

    /**
     * Fill the properties of the widget
     * @return status - success/failure
     */
    virtual QStatus fillProperties();

    /**
     * Refresh the properties of the widget with async call
     * @return status - success/failure
     */
    virtual QStatus refreshProperties();

    /**
     * Introspect to receive childNodes
     * @param childNodes - childNodes found during introspection
     * @return status - success/failure
     */
    virtual QStatus Introspect(std::vector<IntrospectionNode>& childNodes);

    /**
     * @internal Explicitly provide implementation for virtual method.
     */
    void Introspect(const InterfaceDescription::Member* member, Message& msg)
    {
        BusObject::Introspect(member, msg);
    }

    /**
     * remove the SignalHandler of the BusObject
     * @param bus - busAttachment used to remove the signalHandlers
     * @return status - success/failure
     */
    virtual QStatus UnregisterSignalHandler(BusAttachment* bus);

  protected:

    /**
     * Unmarshal and parse the result for a GetAllProperties request
     * @param allPropValues - msrgArg with all property values
     * @return status - success/failure
     */
    QStatus fillAllProperties(MsgArg const& allPropValues);

    /**
     * add the Default Interface Variables
     * @param intf
     * @return status - success/failure
     */
    QStatus addDefaultInterfaceVariables(InterfaceDescription* intf);

    /**
     * Add the SignalHandler for propertyChanged signal
     * @param bus - bus used to add signal
     * @return status - success/failure
     */
    virtual QStatus addSignalHandler(BusAttachment* bus);

    /**
     * Fill Property in Widget
     * @param key - keyName of Property
     * @param variant - the value of the Property
     * @return status - success/failure
     */
    virtual QStatus fillProperty(char* key, MsgArg* variant);

    /**
     * Pointer to Widget represented by this BusObject
     */
    Widget* m_Widget;

    /**
     * Pointer to ProxybusObject for this widget
     */
    ProxyBusObject* m_Proxy;

    /**
     * The language Index of this BusObject
     */
    uint16_t m_LanguageIndx;

    /**
     * ObjectPath of this BusObject
     */
    qcc::String m_ObjectPath;

    /**
     * InterfaceDecription of this BusObject
     */
    InterfaceDescription* m_InterfaceDescription;

    /**
     * The pointer used to send signal/register Signal Handler
     */
    const ajn::InterfaceDescription::Member* m_SignalPropertyChanged;
};
} //namespace services
} //namespace ajn
#endif /* WIDGETBUSOBJECT_H_ */
