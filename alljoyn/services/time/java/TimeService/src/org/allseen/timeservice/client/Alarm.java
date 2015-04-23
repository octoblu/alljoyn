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
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.ScheduleAJ;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * Time Service Alarm.
 * Use this class to communicate with {@link TimeServiceServer} Alarm object.
 */
public class Alarm extends ObjectIntrospector {
    private static final String TAG = "ajts" + Alarm.class.getSimpleName();

    /**
     * Implement this interface to handle the
     * {@link org.allseen.timeservice.ajinterfaces.Alarm#alarmReached()} event.
     */
    public static interface AlarmHandler {

        /**
         * Handle Alarm reached event, which was sent from the given {@link Alarm}
         * @param alarm The {@link Alarm} that sent the AlarmReached event.
         * @see org.allseen.timeservice.ajinterfaces.Alarm#alarmReached()
         */
        void handleAlarmReached(Alarm alarm);
    }

    //=========================================//

    /**
     * Handler for the Alarm Reached events
     */
    private AlarmHandler alarmHandler;

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link Alarm} object
     * @param objectPath {@link TimeServiceServer} Alarm object
     */
    public Alarm(TimeServiceClient tsClient, String objectPath) {

        super(tsClient, objectPath);
    }

    /**
     * Retrieve {@link Version} from the {@link TimeServiceServer} Alarm object.
     * @return {@link Version}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Version}
     */
    public short retrieveVersion() throws TimeServiceException {

        Log.d(TAG, "Retrieving Version, objPath: '" + objectPath + "'");

        try {

            short verAJ = getRemoteAlarm().getVersion();
            
            Log.d(TAG, "Retrieved Version: '" + verAJ + "', objPath: '" + objectPath + "'");

            return verAJ;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Alarm.retrieveVersion()", e);
        }
    }

    /**
     * Retrieve {@link Schedule} from the {@link TimeServiceServer} Alarm object.
     * @return {@link Schedule}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Schedule}
     */
    public Schedule retrieveSchedule() throws TimeServiceException {

        Log.d(TAG, "Retrieving Schedule, objPath: '" + objectPath + "'");

        try {

            ScheduleAJ scheduleAJ = getRemoteAlarm().getSchedule();
            Schedule schedule     = scheduleAJ.toSchedule();

            Log.d(TAG, "Retrieved Schedule: '" + schedule + "', objPath: '" + objectPath + "'");

            return schedule;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Alarm.getSchedule()", e);
        }
    }

    /**
     * Set {@link Schedule} to the {@link TimeServiceServer} Alarm object
     * @param schedule Alarm {@link Schedule}
     * @throws TimeServiceException Is thrown if failed to set the {@link Schedule}
     */
    public void setSchedule(Schedule schedule) throws TimeServiceException {

        if ( schedule == null ) {

            throw new TimeServiceException("Undefined schedule");
        }

        Log.d(TAG, "Setting Alarm schedule: '" + schedule + "', objPath: '" + objectPath + "'");

        try {

            getRemoteAlarm().setSchedule( new ScheduleAJ(schedule) );
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Alarm.setSchedule()", be);
        }
    }

    /**
     * Retrieve title from the {@link TimeServiceServer} Alarm object.
     * Title is an optional textual description of what this Alarm is set for.
     * @return Alarm title
     * @throws TimeServiceException Is thrown if failed to retrieve the alarm title
     */
    public String retrieveTitle() throws TimeServiceException {

        Log.d(TAG, "Retrieving Alarm title, objPath: '" + objectPath + "'");

        try {

            String title = getRemoteAlarm().getTitle();

            Log.d(TAG, "Retrieved title: '" + title + "', objPath: '" + objectPath + "'");

            return title;
        }
        catch(BusException be) {

            throw new TimeServiceException("Failed to call Alarm.getTitle()", be);
        }
    }

    /**
     * Set title to the {@link TimeServiceServer} Alarm object.
     * @param title Alarm title.
     * Title is an optional textual description of what this Alarm is set for.
     * @throws TimeServiceException Is thrown if failed to set the alarm title
     */
    public void setTitle(String title) throws TimeServiceException {

        if ( title == null ) {

            throw new TimeServiceException("Undefined title");
        }

        Log.d(TAG, "Setting title: '" + title + "', objPath: '" + objectPath + "'");

        try {

            getRemoteAlarm().setTitle(title);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Alarm.setTitle()", be);
        }
    }

    /**
     * Retrieve from the {@link TimeServiceServer} Alarm whether it is enabled.
     * @return Whether the Alarm is enabled
     * @throws TimeServiceException Is thrown if failed to retrieve isEnabled Alarm status
     */
    public boolean retrieveIsEnabled() throws TimeServiceException {

        Log.d(TAG, "Retrieving IsEnabled status, objPath: '" + objectPath + "'");

        try {

            boolean isEnabled = getRemoteAlarm().getEnabled();

            Log.d(TAG, "Retrieved IsEnabled status: '" + isEnabled + "', objPath: '" + objectPath + "'");
            return isEnabled;
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Alarm.getEnabled()", be);
        }
    }

    /**
     * Set {@link TimeServiceServer} Alarm if it is enabled.
     * @param enabled Whether the Alarm is enabled
     * @throws TimeServiceException Is thrown if failed to set isEnabled Alarm status
     */
    public void setEnabled(boolean enabled) throws TimeServiceException {

        Log.d(TAG, "Setting Enabled status '" + enabled + "', objPath: '" + objectPath + "'");

        try {

            getRemoteAlarm().setEnabled(enabled);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Alarm.setEnabled()", be);
        }
    }

    /**
     * Register {@link AlarmHandler} to receive {@link AlarmHandler#handleAlarmReached(Alarm)} events.
     * @param alarmHandler
     * @throws IllegalArgumentException Is thrown if undefined alarmHandler was received
     */
    public void registerAlarmHandler(AlarmHandler alarmHandler) {

        if ( alarmHandler == null ) {

            throw new IllegalArgumentException("alarmHandler is undefined");
        }

        this.alarmHandler = alarmHandler;
        AlarmReachedHandler.getInstance().registerAlarm(tsClient.getBus(), this);
    }

    /**
     * Unregister {@link AlarmHandler} to stop receiving {@link AlarmHandler#handleAlarmReached(Alarm)} events.
     */
    public void unregisterAlarmHandler() {

        if ( this.alarmHandler == null ) {

            Log.w(TAG, "AlarmHandler was never registered before, returning, objPath: '" + objectPath + "'");
            return;
        }

        this.alarmHandler = null;
        AlarmReachedHandler.getInstance().unregisterAlarm(this);
    }

    /**
     * @return {@link AlarmHandler} or NULL if it hasn't been registered
     */
    public AlarmHandler getAlarmHandler() {

        return alarmHandler;
    }

    /**
     * @see org.allseen.timeservice.client.TimeClientBase#release()
     */
    @Override
    public void release() {

        Log.i(TAG, "Releasing Client Alarm object: '" + getObjectPath() + "'");

        if ( alarmHandler != null ) {

            Log.d(TAG, "Unregistering the 'AlarmHandler'");
            unregisterAlarmHandler();
        }

        super.release();
    }

    /**
     * Create and return {@link ProxyBusObject} casted to the
     * {@link org.allseen.timeservice.ajinterfaces.Alarm}
     * @return {@link org.allseen.timeservice.ajinterfaces.Alarm}
     * @throws TimeServiceException Is thrown if failed to create the {@link ProxyBusObject}
     * @see TimeClientBase#getProxyObject(Class[])
     */
    private org.allseen.timeservice.ajinterfaces.Alarm getRemoteAlarm() throws TimeServiceException {

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.Alarm.class});
        return proxy.getInterface(org.allseen.timeservice.ajinterfaces.Alarm.class);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "[Alarm - objPath: '" + objectPath + "']";
    }
}
