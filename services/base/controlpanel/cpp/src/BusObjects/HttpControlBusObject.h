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

#ifndef HTTPCONTROLBUSOBJECT_H_
#define HTTPCONTROLBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/controlpanel/HttpControl.h>

namespace ajn {
namespace services {

/**
 * HttpControlBusObject - BusObject for HttpControls
 */
class HttpControlBusObject : public BusObject {
  public:

    /**
     * Constructor for HttpControlBusObject
     * @param bus - bus used to create the interface
     * @param objectPath - objectPath of BusObject
     * @param status - success/failure
     * @param httpControl - httpControl Widget
     */
    HttpControlBusObject(BusAttachment* bus, qcc::String const& objectPath,
                         QStatus& status, HttpControl* httpControl);

    /**
     * Destructor for HttpControlBusObject
     */
    virtual ~HttpControlBusObject();

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
     * Callback when GetUrl  is called
     * @param member - the member (method) of the interface that was executed
     * @param msg - the Message of the method
     */
    void HttpControlGetUrl(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);

    /**
     * Set the RemoteController to allow remote calls
     * @param bus - bus used to create the proxyBusObject
     * @param deviceBusName - busName of the device
     * @param sessionId - sessionId of Remote device
     * @return status - success/failure
     */
    QStatus setRemoteController(BusAttachment* bus, qcc::String const& deviceBusName, SessionId sessionId);

    /**
     * Check compatibility of the versions
     * @return status - success/failure
     */
    QStatus checkVersions();

    /**
     * Get the url of the remote HttpControl
     * @param bus - bus used for method call
     * @return status - success/failure
     */
    QStatus GetUrl(BusAttachment* bus);

  private:

    /**
     * HttpControl widget of this BusObject
     */
    HttpControl* m_HttpControl;

    /**
     * Pointer to ProxybusObject for this HttpControl
     */
    ProxyBusObject* m_Proxy;

    /**
     * ObjectPath of this BusObject
     */
    qcc::String m_ObjectPath;

    /**
     * InterfaceDecription of this BusObject
     */
    InterfaceDescription* m_InterfaceDescription;
};

} /* namespace services */
} /* namespace ajn */
#endif /* HTTPCONTROLBUSOBJECT_H_ */

