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

import java.lang.reflect.Method;
import java.util.Collections;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;
import org.allseen.timeservice.ajinterfaces.TimeAuthority;

import android.util.Log;

/**
 * Singleton class that is responsible to receive {@link TimeAuthority#timeSync()} signals
 */
class TimeSyncHandler {
    private static final String TAG = "ajts" + TimeSyncHandler.class.getSimpleName();

    /**
     * Self reference for the {@link TimeSyncHandler} singleton
     */
    private static final TimeSyncHandler SELF = new TimeSyncHandler();

    /**
     * The rule to be used in the {@link BusAttachment#addMatch(String)}
     */
    private static final String MATCH_RULE    = "interface='" + TimeAuthority.IFNAME + "',type='signal',sessionless='t'";

    /**
     * {@link BusAttachment}
     */
    private BusAttachment bus;

    /**
     * Clocks to be notified when Time Sync event arrives.
     * This Set internally uses {@link ConcurrentHashMap} that makes it thread safe.
     * @see ConcurrentHashMap
     * @see Collections#newSetFromMap(java.util.Map)
     */
    private final Set<Clock> clocks;

    /**
     * Signal handler method
     */
    private Method sigHandlerMethod;

    /**
     * Constructor
     */
    private TimeSyncHandler() {

        // Creates thread safe set
        clocks = Collections.newSetFromMap(new ConcurrentHashMap<Clock, Boolean>());

        try {

            sigHandlerMethod = this.getClass().getDeclaredMethod("timeSync");
        } catch (NoSuchMethodException nse) {

             Log.e(TAG, "Not found TimeSync signal handler method", nse);
        }
    }

    /**
     * Returns {@link TimeSyncHandler}
     * @return {@link TimeSyncHandler}
     */
    static TimeSyncHandler getInstance() {

        return SELF;
    }

    /**
     * Adds {@link Clock} event listener
     * @param bus
     * @param clock
     */
    void registerClock(BusAttachment bus, Clock clock) {

        if ( bus == null ) {

            Log.e(TAG, "Failed to register Clock, BusAttachment is undefined");
            return;
        }

        Log.i(TAG, "Registering to receive 'TimeSync' events from the Clock: '" + clock.getObjectPath() + "'");

        if ( clocks.size() == 0 ) {

            registerSignalHandler(bus);
        }

        //Add the clock listener to the clocks list
        clocks.add(clock);
    }

    /**
     * Remove {@link Clock} event listener
     * @param clock
     */
    void unregisterClock(Clock clock) {

        Log.i(TAG, "Stop receiving 'TimeSync' events from the Clock: '" + clock.getObjectPath() + "'");

        //Remove the clock listener from the clocks list
        clocks.remove(clock);

        if ( clocks.size() == 0 ) {

            unregisterSignalHandler();
        }
    }

    /**
     * Signal Handler
     * @see TimeAuthority#timeSync()
     */
    public void timeSync() {

        if ( bus == null ) {

            Log.wtf(TAG, "Received TimeSync signal, but BusAttachment is undefined, returning");
            return;
        }

        bus.enableConcurrentCallbacks();

        String sender   = bus.getMessageContext().sender;
        String objPath  = bus.getMessageContext().objectPath;

        Log.d(TAG, "Received TimeSync signal from object: '" + objPath + "', sender: '" + sender + "', searching for a Clock");
        notifyClock(objPath, sender);
    }

    /**
     * Find the Time Authority Clock by the given senderObjPath and senderName
     * @param senderObjPath
     * @param senderName
     */
    private void notifyClock(String senderObjPath, String senderName) {

        Iterator<Clock> iterator = clocks.iterator();
        while ( iterator.hasNext() ) {

            Clock clock = iterator.next();
            if ( clock.getObjectPath().equals(senderObjPath) &&
                    clock.tsClient.getServerBusName().equals(senderName) ) {

                Log.i(TAG, "Received 'TimeSync' event from Clock: '" + senderObjPath + "', sender: '" + senderName +
                                "', delegating to a listener");

                clock.getTimeAuthorityHandler().handleTimeSync(clock);
                return;
            }
        }

        Log.d(TAG, "Not found any signal listener for the Clock: '" + senderObjPath + "', sender: '" + senderName + "'");
    }

    /**
     * Register signal handler
     */
    private synchronized void registerSignalHandler(BusAttachment bus) {

        Log.d(TAG, "Starting TimeSync signal handler");

        this.bus = bus;

        if ( sigHandlerMethod == null ) {

            return;
        }

        String timeAuthorityIfaceLocalName = TimeAuthority.class.getName();
        Status status = bus.registerSignalHandler(timeAuthorityIfaceLocalName, "timeSync",
                                                    this, sigHandlerMethod);

        if ( status != Status.OK ) {

            Log.e(TAG, "Failed to call 'bus.registerSignalHandler()', Status: '" + status + "'");
            return;
        }

        status  = bus.addMatch(MATCH_RULE);
        if ( status != Status.OK ) {

            Log.e(TAG, "Failed to call bus.addMatch(" + MATCH_RULE + "), Status: '" + status + "'");
            return;
        }
    }

    /**
     * Unregister signal handler
     * @param clock
     */
    private synchronized void unregisterSignalHandler() {

        Log.d(TAG, "Stopping TimeSync signal handler");

        bus.unregisterSignalHandler(this, sigHandlerMethod);

        Status status = bus.removeMatch(MATCH_RULE);
        if ( status == Status.OK ) {

            Log.d(TAG, "Succeeded to call bus.removeMatch(" + MATCH_RULE + "), Status: '" + status + "'");
        }
        else {

            Log.e(TAG, "Failed to call bus.removeMatch(" + MATCH_RULE + "), Status: '" + status + "'");
        }

        //Removing BusAttachment reference
        bus = null;
    }
}
