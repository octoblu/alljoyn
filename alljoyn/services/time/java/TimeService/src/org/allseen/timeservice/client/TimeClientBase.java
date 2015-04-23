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

package org.allseen.timeservice.client;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.ProxyBusObject;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.TimeServiceServer;

/**
 * The base class for Time Service client classes
 */
class TimeClientBase {

    /**
     * {@link TimeServiceClient} managing this object
     */
    protected TimeServiceClient tsClient;

    /**
     * The object path of the {@link TimeServiceServer} object
     */
    protected final String objectPath;

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this object
     * @param objectPath Object path of the {@link TimeServiceServer} object
     */
    protected TimeClientBase(TimeServiceClient tsClient, String objectPath) {

        this.tsClient   = tsClient;
        this.objectPath = objectPath;
    }

    /**
     * Object path of the {@link TimeServiceServer} object
     * @return Object Path
     */
    public String getObjectPath() {

        return objectPath;
    }

    /**
     * Release the object resources. <br>
     * It's a programming error to call another method on this object after the release method has been called.
     */
    public void release() {

        tsClient = null;
    }

    /**
     * Verifies that tsClient is initialized
     * @throws TimeServiceException Is thrown if tsClient is not initialized
     */
    void checkTsClientValidity() throws TimeServiceException {

        if ( tsClient == null ) {

            throw new TimeServiceException("Not initialized object");
        }
    }

    /**
     * Return {@link BusAttachment} if it is defined
     * @return {@link BusAttachment}
     * @throws TimeServiceException Is thrown if {@link BusAttachment} if it is defined
     */
    BusAttachment getBus() throws TimeServiceException {

        checkTsClientValidity();

        BusAttachment bus = tsClient.getBus();

        if ( bus == null ) {

            throw new TimeServiceException("BusAttachment is not initialized");
        }

        return bus;
    }

    /**
     * Returns Session Id if the session is established
     * @return Session Id
     * @throws TimeServiceException Is thrown if the session is not established
     */
    Integer getSid() throws TimeServiceException {

        checkTsClientValidity();

        Integer sid = tsClient.getSessionId();
        if ( sid == null ) {

            throw new TimeServiceException("Session is not established");
        }
        return sid;
    }

    /**
     * Creates {@link ProxyBusObject} from the given proxyClasses
     * @param proxyClasses
     * @return {@link ProxyBusObject}
     * @throws TimeServiceException If failed to create the {@link ProxyBusObject}, i.e.
     * session not established
     * @see BusAttachment#getProxyBusObject(String, String, int, Class[], boolean)
     */
     ProxyBusObject getProxyObject(Class<?>[] proxyClasses) throws TimeServiceException {

        BusAttachment bus = getBus();
        Integer sid       = getSid();

        return bus.getProxyBusObject(tsClient.getServerBusName(), objectPath, sid, proxyClasses);
     }

}
