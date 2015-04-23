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

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.DateTimeAJ;
import org.allseen.timeservice.ajinterfaces.TimeAuthority;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * Time Service Clock.
 * Use this class to communication with {@link TimeServiceServer} Clock object.
 */
public class Clock extends TimeClientBase {
    private static final String TAG = "ajts" + Clock.class.getSimpleName();

    /**
     * Implement this interface to handle {@link TimeAuthority#timeSync()} event.
     * This event is sent only from {@link Clock}s for which the method {@link Clock#isAuthority()}
     * returns TRUE.
     */
    public static interface TimeAuthorityHandler {

        /**
         * Handle Time Sync event, which was sent from the given {@link Clock}
         * @param clock The {@link Clock} that sent the TimeSync event.
         * @see org.allseen.timeservice.ajinterfaces.TimeAuthority#timeSync()
         */
        void handleTimeSync(Clock clock);
    }

    //=========================================//

    /**
     * Whether the {@link TimeServiceServer} Clock is an authority clock.
     */
    private boolean isAuthority;

    /**
     * Handler for the Time Sync events
     */
    private TimeAuthorityHandler timeAuthHandler;

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link Clock} object
     * @param objectPath {@link TimeServiceServer} Clock object
     */
    public Clock(TimeServiceClient tsClient, String objectPath) {

        this(tsClient, objectPath, false);
    }

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link Clock} object
     * @param objectPath {@link TimeServiceServer} Clock object
     * @param isAuthority Whether the {@link TimeServiceServer} Clock is a reliable source of time.
     */
    public Clock(TimeServiceClient tsClient, String objectPath, boolean isAuthority) {

        super(tsClient, objectPath);
        this.isAuthority = isAuthority;
    }

    /**
     * Returns whether the {@link Clock} is an authority
     * @return Is clock an authority
     */
    public boolean isAuthority() {

        return isAuthority;
    }
    
    /**
     * Retrieve {@link Version} from the {@link TimeServiceServer} Clock object.
     * @return {@link Version}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Version}
     */
    public short retrieveVersion() throws TimeServiceException {

        Log.d(TAG, "Retrieving Version, objPath: '" + objectPath + "'");

        try {

            short verAJ = getRemoteClock().getVersion();
            
            Log.d(TAG, "Retrieved Version: '" + verAJ + "', objPath: '" + objectPath + "'");

            return verAJ;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Clock.retrieveVersion()", e);
        }
    }


    /**
     * Retrieve {@link DateTime} from the {@link TimeServiceServer} Clock object.
     * @return {@link DateTime}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link DateTime}
     */
    public DateTime retrieveDateTime() throws TimeServiceException {

        Log.d(TAG, "Retrieving DateTime, objPath: '" + objectPath + "'");

        try {

            DateTimeAJ dtAJ = getRemoteClock().getDateTime();
            DateTime dt     = dtAJ.toDateTime();

            Log.d(TAG, "Retrieved DateTime: '" + dt + "', objPath: '" + objectPath + "'");

            return dt;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Clock.getDateTime()", e);
        }
    }

    /**
     * Set {@link DateTime} to the {@link TimeServiceServer} Clock object
     * @param dateTime {@link DateTime} to set
     * @throws TimeServiceException Is thrown if failed to set the {@link DateTime}
     */
    public void setDateTime(DateTime dateTime) throws TimeServiceException {

        if ( dateTime == null ) {

            throw new TimeServiceException("Undefined dateTime");
        }

        Log.d(TAG, "Setting DateTime: '" + dateTime + "', objPath: '" + objectPath + "'");

        try {

            getRemoteClock().setDateTime(new DateTimeAJ(dateTime));
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Clock.setDateTime()", be);
        }
    }

    /**
     * Retrieve whether or not the clock has ever been set
     * @return Returns TRUE if the Clock has been set since the last reboot
     * @throws TimeServiceException Is thrown if failed to retrieve the IsSet status
     */
    public boolean retrieveIsSet() throws TimeServiceException {

        Log.d(TAG, "Retrieving IsSet status, objPath: '" + objectPath + "'");

        try {

            boolean isSet = getRemoteClock().getIsSet();

            Log.d(TAG, "Retrieved isSet status: '" + isSet + "', objPath: '" + objectPath + "'");
            return isSet;
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Clock.getIsSet()", be);
        }
    }

    /**
     * Retrieves {@link AuthorityType} of the clock. <br>
     * This method is only valid for objects that {@link Clock#isAuthority()} returns TRUE.
     * @return {@link AuthorityType} of the clock
     * @throws TimeServiceException Is thrown if failed to retrieve {@link AuthorityType}, or
     * this {@link Clock} is not a Time Authority
     */
    public AuthorityType retrieveAuthorityType() throws TimeServiceException {

        if ( !isAuthority ) {

            throw new TimeServiceException("The clock is not a Time Authority");
        }

        Log.d(TAG, "Retrieving Authority Type, objPath: '" + objectPath + "'");

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.Clock.class,
                                                        TimeAuthority.class});

        byte authTypeByte;
        try{

            authTypeByte = proxy.getInterface(TimeAuthority.class).getAuthorityType();
        }
        catch(BusException be) {

            throw new TimeServiceException("Failed to call TimeAuthorityClock.getAuthorityType()", be);
        }

        AuthorityType authorityType = AuthorityType.getType(authTypeByte);
        if ( authorityType == null ) {

            Log.w(TAG, "Retrieved unrecognized AuthorityType, value: '" + authTypeByte + "', returning: '" +
                            AuthorityType.OTHER + "', objPath: '" + objectPath + "'");

            authorityType = AuthorityType.OTHER;
        }

        Log.d(TAG, "Retrieved AuthorityType: '" + authorityType + "', objPath: '" + objectPath + "'");
        return authorityType;
    }

    /**
     * Register {@link TimeAuthorityHandler} to receive {@link TimeAuthorityHandler#handleTimeSync(Clock)}
     * events. This method is only valid for objects that {@link Clock#isAuthority()} is set to TRUE.
     * @param timeAuthorityHandler {@link TimeAuthorityHandler}
     * @throws IllegalStateException Is thrown if this method was called for the {@link Clock}
     * that is not a Time Authority
     * @throws IllegalArgumentException Is thrown if received undefined timeAuthorityHandler
     */
    public void registerTimeAuthorityHandler(TimeAuthorityHandler timeAuthorityHandler)
                                                throws IllegalStateException {

        if ( !isAuthority ) {

            throw new IllegalStateException("Clock is not a TimeAuthority");
        }

        if ( timeAuthorityHandler == null ) {

            throw new IllegalArgumentException("timeAuthorityHandler is undefined");
        }

        this.timeAuthHandler = timeAuthorityHandler;
        TimeSyncHandler.getInstance().registerClock(this.tsClient.getBus(), this);
    }

    /**
     * Unregister  {@link TimeAuthorityHandler} to stop receiving {@link TimeAuthorityHandler#handleTimeSync(Clock)}
     * events. This method is only valid for objects that {@link Clock#isAuthority()} is set TRUE.
     * @throws IllegalStateException Is thrown if this method was called for the {@link Clock}
     * that is not a Time Authority
     */
    public void unregisterTimeAuthorityHandler() throws IllegalStateException {

        if ( this.timeAuthHandler == null ) {

            Log.w(TAG, "TimeAuthority handler was never registered before, returning, objPath: '" + objectPath + "'");
            return;
        }

        this.timeAuthHandler = null;
        TimeSyncHandler.getInstance().unregisterClock(this);
    }

    /**
     * @return {@link TimeAuthorityHandler} or NULL if it hasn't been registered
     */
    public TimeAuthorityHandler getTimeAuthorityHandler() {

        return timeAuthHandler;
    }

    /**
     * @see org.allseen.timeservice.client.TimeClientBase#release()
     */
    @Override
    public void release() {

        Log.i(TAG, "Releasing Client Clock object: '" + getObjectPath() + "'");

        if ( timeAuthHandler != null ) {

            Log.d(TAG, "Unregistering the 'TimeAuthHandler'");
            unregisterTimeAuthorityHandler();
        }

        super.release();
    }

    /**
     * Set clock authority
     * @param isAuthority
     */
    void setAuthority(boolean isAuthority) {

        this.isAuthority = isAuthority;
    }

    /**
     * Create and return {@link ProxyBusObject} casted to the
     * {@link org.allseen.timeservice.ajinterfaces.Clock}
     * @return {@link org.allseen.timeservice.ajinterfaces.Clock}
     * @throws TimeServiceException Is thrown if failed to create the {@link ProxyBusObject}
     * @see TimeClientBase#getProxyObject(Class[])
     */
    private org.allseen.timeservice.ajinterfaces.Clock getRemoteClock() throws TimeServiceException {

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.Clock.class});
        return proxy.getInterface(org.allseen.timeservice.ajinterfaces.Clock.class);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "[Clock - objPath: '" + objectPath + "', isAuthority: '" + isAuthority + "']";
    }
}
