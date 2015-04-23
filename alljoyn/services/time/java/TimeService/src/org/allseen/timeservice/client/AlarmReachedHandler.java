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
import org.allseen.timeservice.ajinterfaces.Alarm;

import android.util.Log;

/**
 * Singleton class that is responsible to receive {@link Alarm#alarmReached()} signals
 */
class AlarmReachedHandler {
    private static final String TAG = "ajts" + AlarmReachedHandler.class.getSimpleName();

    /**
     * Self reference for the {@link AlarmReachedHandler} singleton
     */
    private static final AlarmReachedHandler SELF = new AlarmReachedHandler();

    /**
     * The rule to be used in the {@link BusAttachment#addMatch(String)}
     */
    private static final String MATCH_RULE        = "interface='" + Alarm.IFNAME + "',type='signal',sessionless='t'";

    /**
     * {@link BusAttachment}
     */
    private BusAttachment bus;

    /**
     * Alarms to be notified when Alarm reached event arrives.
     * This Set internally uses {@link ConcurrentHashMap} that makes it thread safe.
     * @see ConcurrentHashMap
     * @see Collections#newSetFromMap(java.util.Map)
     */
    private final Set<org.allseen.timeservice.client.Alarm> alarms;

    /**
     * Signal handler method
     */
    private Method sigHandlerMethod;

    /**
     * Constructor
     */
    private AlarmReachedHandler() {

        // Creates thread safe set
        alarms = Collections.newSetFromMap(new ConcurrentHashMap<org.allseen.timeservice.client.Alarm, Boolean>());

        try {

            sigHandlerMethod = this.getClass().getDeclaredMethod("alarmReached");
        } catch (NoSuchMethodException nse) {

             Log.e(TAG, "Not found AlarmReached signal handler method", nse);
        }
    }

    /**
     * Returns {@link AlarmReachedHandler}
     * @return {@link AlarmReachedHandler}
     */
    static AlarmReachedHandler getInstance() {

        return SELF;
    }

    /**
     * Adds {@link org.allseen.timeservice.client.Alarm} event listener
     * @param bus
     * @param alarm
     */
    void registerAlarm(BusAttachment bus, org.allseen.timeservice.client.Alarm alarm) {

        if ( bus == null ) {

            Log.e(TAG, "Failed to register Alarm, BusAttachment is undefined");
            return;
        }

        Log.i(TAG, "Registering to receive 'AlarmReached' events from the Alarm: '" + alarm.getObjectPath() + "'");

        if ( alarms.size() == 0 ) {

            registerSignalHandler(bus);
        }

        //Add the alarm listener to the alarms list
        alarms.add(alarm);
    }

    /**
     * Remove {@link org.allseen.timeservice.client.Alarm} event listener
     * @param alarm
     */
    void unregisterAlarm(org.allseen.timeservice.client.Alarm alarm) {

        Log.i(TAG, "Stop receiving 'AlarmReached' events from the Alarm: '" + alarm.getObjectPath() + "'");

        //Remove the alarm listener from the alarms list
        alarms.remove(alarm);

        if ( alarms.size() == 0 ) {

            unregisterSignalHandler();
        }
    }

    /**
     * Signal Handler
     * @see Alarm#alarmReached()
     */
    public void alarmReached() {

        if ( bus == null ) {

            Log.wtf(TAG, "Received AlarmReached signal, but BusAttachment is undefined, returning");
            return;
        }

        bus.enableConcurrentCallbacks();

        String sender   = bus.getMessageContext().sender;
        String objPath  = bus.getMessageContext().objectPath;

        Log.d(TAG, "Received AlarmReached signal from object: '" + objPath + "', sender: '" + sender + "', searching for the Alarm");
        notifyAlarm(objPath, sender);
    }

    /**
     * Find Alarm by the given senderObjPath and senderName
     * @param senderObjPath
     * @param senderName
     */
    private void notifyAlarm(String senderObjPath, String senderName) {

        Iterator<org.allseen.timeservice.client.Alarm> iterator = alarms.iterator();
        while ( iterator.hasNext() ) {

            org.allseen.timeservice.client.Alarm alarm = iterator.next();
            if ( alarm.getObjectPath().equals(senderObjPath) &&
                    alarm.tsClient.getServerBusName().equals(senderName) ) {

                Log.i(TAG, "Received 'AlarmReached' event from Alarm: '" + senderObjPath + "', sender: '" + senderName +
                                "', delegating to a listener");

                alarm.getAlarmHandler().handleAlarmReached(alarm);
                return;
            }
        }

        Log.d(TAG, "Not found any signal listener for the Alarm: '" + senderObjPath + "', sender: '" + senderName + "'");
    }

    /**
     * Register signal handler
     */
    private synchronized void registerSignalHandler(BusAttachment bus) {

        Log.d(TAG, "Starting AlarmReached signal handler");

        this.bus = bus;

        if ( sigHandlerMethod == null ) {

            return;
        }

        String alarmIfaceLocalName = Alarm.class.getName();
        Status status = bus.registerSignalHandler(alarmIfaceLocalName, "alarmReached",
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
     */
    private synchronized void unregisterSignalHandler() {

        Log.d(TAG, "Stopping AlarmReached signal handler");

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
