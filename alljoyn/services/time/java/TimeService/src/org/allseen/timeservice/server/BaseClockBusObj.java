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

package org.allseen.timeservice.server;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.bus.Status;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.DateTimeAJ;

import android.util.Log;

/**
 * This is the base class for all the {@link org.allseen.timeservice.ajinterfaces.Clock} implementations.
 * Most of the {@link org.allseen.timeservice.ajinterfaces.Clock} related calls delegated to
 * the {@link Clock} object.
 */
//Commented out because currently JavaBinding doesn't support finding AJ interfaces in the SuperClasses
//so they must be implemented by the leaf classes
abstract class BaseClockBusObj /*implements Clock*/ {
    private static final String TAG = "ajts" + BaseClockBusObj.class.getSimpleName();

    /**
     * The clock to be notified with the {@link Clock} related messages
     */
    private Clock clock;

    /**
     * The object path of this {@link BusObject}
     */
    private final String objectPath;

    /**
     * Constructor
     * @param clock {@link Clock} handler
     * @param objectPath Object path of this object
     * @throws TimeServiceException Is thrown if failed to created {@link ClockBusObj}
     */
    protected BaseClockBusObj(Clock clock, String objectPath) throws TimeServiceException {

        if ( clock == null ) {

            throw new TimeServiceException("Undefined clock");
        }

        if ( !(this instanceof BusObject) ) {

            throw new TimeServiceException("Class not a BusObject");
        }

        this.clock       = clock;
        this.objectPath  = objectPath;

        Status status = getBus().registerBusObject((BusObject)this, this.objectPath);

        if ( status != Status.OK ) {

            throw new TimeServiceException("Failed to register BusObject, objPath: '" + objectPath +
                                               "', Status: '" + status + "'");
        }

        Log.i(TAG, "Clock BusObject, objectPath: '" + objectPath + "' registered successfully");
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Clock#getVersion()
     */
    public short getVersion() {

        return org.allseen.timeservice.ajinterfaces.Clock.VERSION;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Clock#getDateTime()
     */
    public DateTimeAJ getDateTime() throws BusException {

        DateTimeAJ dateTimeAJ;

        try {

            Log.d(TAG, "getDateTime is called, objPath: '" + objectPath + "', handling...");

            getBus().enableConcurrentCallbacks();

            DateTime dateTime = clock.getDateTime();

            if ( dateTime == null ) {

                throw new TimeServiceException("Undefined dateTime");
            }

            dateTimeAJ = new DateTimeAJ(dateTime);
        }
        catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getDateTime', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning DateTime: '" + dateTimeAJ + "', objPath: '" + objectPath + "'");
        return dateTimeAJ;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Clock#setDateTime(DateTimeAJ)
     */
    public void setDateTime(DateTimeAJ dateTimeAJ) throws BusException {

        try {

            Log.d(TAG, "setDateTime is called, objPath: '" + objectPath + "'. Setting to: '" + dateTimeAJ + "'");

            getBus().enableConcurrentCallbacks();
            clock.setDateTime(dateTimeAJ.toDateTime());
        }
        catch (IllegalArgumentException ilae) {

            Log.e(TAG, "Failed to execute 'setDateTime', objPath: '" + objectPath + "'", ilae);
            throw new ErrorReplyBusException(TimeServiceConst.INVALID_VALUE_ERROR, ilae.getMessage());
        }
        catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setDateTime', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Clock#getIsSet()
     */
    public boolean getIsSet() throws BusException {

        try {

            Log.d(TAG, "getIsSet is called, objPath: '" + objectPath + "', handling...");

            getBus().enableConcurrentCallbacks();
            boolean isSet = clock.getIsSet();

            Log.d(TAG, "Returning isSet status: '" + isSet + "', objPath: '" + objectPath + "'");
            return isSet;
        }
        catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getIsSet', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    //================================================//

     /**
      * @return object path of this object
      */
     protected String getObjectPath() {

         return objectPath;
     }

     /**
      * Releases object resources
      */
     protected void release() {

         Log.d(TAG, "Releasing Server Clock object, objPath: '" + objectPath + "'");
         clock = null;

         try {

            getBus().unregisterBusObject((BusObject)this);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to unregister BusObject, objPath: '" + objectPath + "'", tse);
        }
     }

    /**
     * Access {@link TimeServiceServer} to get the {@link BusAttachment}.
     * If {@link BusAttachment} is undefined, {@link TimeServiceException} is thrown.
     * @return {@link BusAttachment}
     * @throws TimeServiceException
     */
    protected BusAttachment getBus() throws TimeServiceException {

        BusAttachment bus = TimeServiceServer.getInstance().getBusAttachment();

        if ( bus == null ) {

            throw new TimeServiceException("TimeServiceServer is not initialized");
        }

        return bus;
    }
}
