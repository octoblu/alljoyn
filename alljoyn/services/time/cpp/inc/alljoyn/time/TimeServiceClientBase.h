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

#ifndef TIMESERVICECLIENTBASE_H_
#define TIMESERVICECLIENTBASE_H_

#include <qcc/String.h>
#include <alljoyn/BusAttachment.h>

namespace ajn {
namespace services {

class TimeServiceClient;

/**
 * Base class for the TimeService client classes
 */
class TimeServiceClientBase {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this object
     * @param objectPath Object path of the Time Service server side object
     */
    TimeServiceClientBase(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientBase();

    /**
     * Object path of the  Time Service server side object
     *
     * @return object path
     */
    const qcc::String& getObjectPath() const;

    /**
     * Returns TimeServiceClient that is managing this object
     */
    const TimeServiceClient& getTsClient() const;

  protected:

    /**
     * TimeServiceClient object
     */
    const TimeServiceClient& m_TsClient;

    /**
     * Object path of the server side object
     */
    const qcc::String m_ObjectPath;

    /**
     * Proxy Bus Object that is used to call server methods
     */
    ProxyBusObject* m_ProxyObj;

    /**
     * This structure includes the relevant information about connection that is established
     * between the client and server
     */
    struct ConnectionData {

        /**
         * BusAttachment that was used for session establishment
         */
        BusAttachment* bus;

        /**
         * The id of the session that was established
         */
        SessionId sid;
    };

    /**
     * Pointer to the createInterface() method of the appropriate utility namespace.
     * This method creates an AllJoyn interface and adds it to the BusAttachment, if it
     * hasn't been created before.
     */
    typedef QStatus (*IntefaceFactoryMethod)(BusAttachment*, InterfaceDescription**);

    /**
     * Populates ConnectionData, if all its fields are available
     */
    QStatus getConnectionData(ConnectionData* connData) const;

    /**
     * Create interface on the BusAttachment identified by the given ifaceName
     *
     * @param ifaceName Interface name to initialize
     * @param ptrIface Out variable. Pointer to the InterfaceDescription pointer
     * @param factoryMethod Pointer to the utility function creating the interface
     */
    void initInterfaces(const qcc::String& ifaceName, InterfaceDescription** ptrIface, IntefaceFactoryMethod factoryMethod);

    /**
     * Creates ProxyBusObject if it hasn't been created before, or if the session id of the
     * existing ProxyBusObject object is different from the existing session.
     * The given ifaceDesc is added to the ProxyBusObject if it hasn't been added before
     *
     * @param ifaceDesc InterfaceDescription to add to the ProxyBusObject
     *
     * @return status
     */
    QStatus prepareProxyObject(InterfaceDescription* ifaceDesc);

    /**
     * Retrieve server property by calling GetProperty() method of the created ProxyBusObject.
     * ProxyBusObject is created by the prepareProxyObject() method.
     *
     * @param ifaceDesc interface description for the required interface
     * @param propName The name of the property to retrieve
     * @param msgArg Out variable.
     *
     * @return status
     */
    QStatus retrieveServerProperty(InterfaceDescription* ifaceDesc, const qcc::String& propName, MsgArg& msgArg);

    /**
     * Set server property by calling SetProperty() method of the created ProxyBusObject.
     * ProxyBusObject is created by the prepareProxyObject() method.
     *
     * @param ifaceDesc interface description for the required interface
     * @param propName The name of the property to retrieve
     * @param msgArg Out variable.
     *
     * @return status
     */
    QStatus setServerProperty(InterfaceDescription* ifaceDesc, const qcc::String& propName, MsgArg& msgArg);

    /**
     * Call server method
     *
     * @param ifaceDesc interface description for the required interface
     * @param methodName The name of the method to call
     * @param argsIn input arguments to method
     * @param numArgs number of input arguments sent to method
     * @param msgOut Out variable. If NULL the method call will be fire and forget.
     *
     * @return status
     */
    QStatus serverMethodCall(InterfaceDescription* ifaceDesc, const qcc::String& methodName,
                             const ajn::MsgArg*argsIn, size_t numArgs, Message* msgOut = NULL);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTBASE_H_ */
