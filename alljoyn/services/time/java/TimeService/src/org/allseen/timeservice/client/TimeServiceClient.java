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

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;
import org.alljoyn.services.common.AnnouncementHandler;
import org.alljoyn.services.common.BusObjectDescription;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.ajinterfaces.TimeAuthority;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * The main class to initialize the Time Service in the client mode. The class
 * is initialized with the {@link BusObjectDescription} array received from the
 * {@link AnnouncementHandler#onAnnouncement(String, short, BusObjectDescription[], java.util.Map)}
 * . To retrieve the Time Service items that were found in the provided
 * {@link BusObjectDescription} array, use getter methods such as
 * {@link TimeServiceClient#getAnnouncedClockList()}.
 */
public class TimeServiceClient {
    private static final String TAG = "ajts" + TimeServiceClient.class.getSimpleName();

    /**
     * Helper class for creation or releasing Time Service objects following
     * analyzing of the received Announcement signal.
     * 
     * @param <T>
     *            The type of the created object
     */
    private class TSItemsManager<T extends TimeClientBase> {

        /**
         * The {@link Class} object of the created Time Service object
         */
        private final Class<T> createdClass;

        /**
         * List in the {@link TimeServiceClient} where to store the created
         * object
         */
        private List<T> storage;

        /**
         * Constructor
         * 
         * @param createdClass
         *            The {@link Class} object of the created Time Service
         *            object
         * @param storage
         *            List in the {@link TimeServiceClient} where to store the
         *            created object
         */
        TSItemsManager(Class<T> createdClass, List<T> storage) {

            this.createdClass = createdClass;
            this.storage = storage;
        }

        /**
         * Create the object with the given objPath
         * 
         * @param objectPath
         *            objectPath to create the Time Service item object
         * @return Created object
         * @throws Exception
         *             Is thrown if failed to create the object
         */
        public Object create(String objectPath) throws Exception {

            T createdObj = createdClass.getConstructor(TimeServiceClient.class, String.class).newInstance(TimeServiceClient.this, objectPath);

            storage.add(createdObj);
            return createdObj;
        }

        /**
         * Releases all the objects in the storage and cleans the storage.
         */
        public void release() {

            for (T tObj : storage) {

                tObj.release();
            }

            storage.clear();
            storage = null;
        }
    }

    // ==========================================================//

    /**
     * The {@link BusAttachment} that is used by the {@link TimeServiceClient}
     * to reach the {@link TimeServiceServer}
     */
    private BusAttachment bus;

    /**
     * The bus name to reach the {@link TimeServiceServer}
     */
    private final String serverBusName;

    /**
     * The id if the device hosting the {@link TimeServiceServer}
     */
    private final String deviceId;

    /**
     * The name of the application that started the {@link TimeServiceServer}
     */
    private final UUID appId;

    /**
     * Manages session with the {@link TimeServiceServer}
     */
    private TimeServiceSessionListener sessionListener;

    /**
     * Map from the announced AJ interface to the {@link TSItemsManager} that
     * allows to create and release Time Service objects
     */
    private Map<String, TSItemsManager<?>> managerMap;

    /**
     * The list of {@link Clock} objects that were found in the Announcement
     */
    private final List<Clock> clocks;

    /**
     * The list of {@link Alarm} objects that were found in the Announcement
     */
    private final List<Alarm> alarms;

    /**
     * The list of {@link Timer} objects that were found in the Announcement
     */
    private final List<Timer> timers;

    /**
     * The list of {@link AlarmFactory} objects that were found in the
     * Announcement
     */
    private final List<AlarmFactory> alarmFactories;

    /**
     * The list of {@link TimerFactory} objects that were found in the
     * Announcement
     */
    private final List<TimerFactory> timerFactories;

    /**
     * Constructor
     * 
     * @param bus
     *            The {@link BusAttachment} that is used by the
     *            {@link TimeServiceClient} to reach the
     *            {@link TimeServiceServer}
     * @param serverBusName
     *            The bus name to reach the {@link TimeServiceServer}
     * @param deviceId
     *            The id if the device hosting the {@link TimeServiceServer}
     * @param appId
     *            The name of the application that started the
     *            {@link TimeServiceServer}
     * @param objDescs
     *            Array of {@link BusObjectDescription} received with
     *            Announcement
     * @throws IllegalArgumentException
     *             Is thrown if the given {@link BusAttachment} is not
     *             initialized or not connected to the Router.
     */
    public TimeServiceClient(BusAttachment bus, String serverBusName, String deviceId, UUID appId, AboutObjectDescription[] objDescs) throws IllegalArgumentException {

        checkObjectValidity(bus, serverBusName);

        this.bus = bus;
        this.serverBusName = serverBusName;
        this.deviceId = deviceId;
        this.appId = appId;

        clocks = new ArrayList<Clock>();
        alarms = new ArrayList<Alarm>();
        timers = new ArrayList<Timer>();
        alarmFactories = new ArrayList<AlarmFactory>();
        timerFactories = new ArrayList<TimerFactory>();

        managerMap = new HashMap<String, TSItemsManager<?>>();
        initFactoryMap();
        analyzeObjectDescriptions(objDescs);
    }

    /**
     * The {@link BusAttachment} that is used by the {@link TimeServiceClient}
     * to reach the {@link TimeServiceServer}
     * 
     * @return {@link BusAttachment}
     */
    public BusAttachment getBus() {
        return bus;
    }

    /**
     * The bus name to reach the {@link TimeServiceServer}
     * 
     * @return The bus name of the Time Service Server
     */
    public String getServerBusName() {
        return serverBusName;
    }

    /**
     * The id if the device hosting the {@link TimeServiceServer}
     * 
     * @return device Id
     */
    public String getDeviceId() {
        return deviceId;
    }

    /**
     * The name of the application that started the {@link TimeServiceServer}
     * 
     * @return The application id
     */
    public UUID getAppId() {
        return appId;
    }

    /**
     * List of the {@link Clock} objects found in the received object
     * descriptions
     * 
     * @return List of {@link Clock} objects
     */
    public List<Clock> getAnnouncedClockList() {

        return Collections.unmodifiableList(clocks);
    }

    /**
     * List of the {@link Alarm} objects found in the received object
     * descriptions
     * 
     * @return List of {@link Alarm} objects
     */
    public List<Alarm> getAnnouncedAlarmList() {

        return Collections.unmodifiableList(alarms);
    }

    /**
     * List of the {@link AlarmFactory} objects found in the received object
     * descriptions
     * 
     * @return List of {@link AlarmFactory} objects
     */
    public List<AlarmFactory> getAnnouncedAlarmFactoryList() {

        return Collections.unmodifiableList(alarmFactories);
    }

    /**
     * List of the {@link Timer} objects found in the received object
     * descriptions
     * 
     * @return List of {@link Timer} objects
     */
    public List<Timer> getAnnouncedTimerList() {

        return Collections.unmodifiableList(timers);
    }

    /**
     * List of the {@link TimerFactory} objects found in the received object
     * descriptions
     * 
     * @return List of {@link TimerFactory} objects
     */
    public List<TimerFactory> getAnnouncedTimerFactoryList() {

        return Collections.unmodifiableList(timerFactories);
    }

    /**
     * Returns TRUE if {@link org.allseen.timeservice.ajinterfaces.Clock}
     * interface was found in the received {@link BusObjectDescription} array.
     * If this method returns TRUE,
     * {@link TimeServiceClient#getAnnouncedClockList()} will return a
     * {@link List} of found {@link Clock}s.
     * 
     * @return TRUE if {@link Clock} objects were found in the received
     *         {@link BusObjectDescription} array.
     */
    public boolean isClockAnnounced() {

        return clocks.size() != 0;
    }

    /**
     * Returns TRUE if {@link org.allseen.timeservice.ajinterfaces.Alarm}
     * interface was found in the received {@link BusObjectDescription} array.
     * If this method returns TRUE,
     * {@link TimeServiceClient#getAnnouncedAlarmList()} will return a
     * {@link List} of found {@link Alarm}s.
     * 
     * @return TRUE if {@link Alarm} objects were found in the received
     *         {@link BusObjectDescription} array.
     */
    public boolean isAlarmAnnounced() {

        return alarms.size() != 0;
    }

    /**
     * Returns TRUE if {@link org.allseen.timeservice.ajinterfaces.AlarmFactory}
     * interface was found in the received {@link BusObjectDescription} array.
     * If this method returns TRUE,
     * {@link TimeServiceClient#getAnnouncedAlarmFactoryList()} will return a
     * {@link List} of found {@link AlarmFactory}s.
     * 
     * @return TRUE if {@link AlarmFactory} objects were found in the received
     *         {@link BusObjectDescription} array.
     */
    public boolean isAlarmFactoryAnnounced() {

        return alarmFactories.size() != 0;
    }

    /**
     * Returns TRUE if {@link org.allseen.timeservice.ajinterfaces.Timer}
     * interface was found in the received {@link BusObjectDescription} array.
     * If this method returns TRUE,
     * {@link TimeServiceClient#getAnnouncedTimerList()} will return a
     * {@link List} of found {@link Timer}s.
     * 
     * @return TRUE if {@link Timer} objects were found in the received
     *         {@link BusObjectDescription} array.
     */
    public boolean isTimerAnnounced() {

        return timers.size() != 0;
    }

    /**
     * Returns TRUE if {@link org.allseen.timeservice.ajinterfaces.TimerFactory}
     * interface was found in the received {@link BusObjectDescription} array.
     * If this method returns TRUE,
     * {@link TimeServiceClient#getAnnouncedTimerFactoryList()} will return a
     * {@link List} of found {@link TimerFactory}s.
     * 
     * @return TRUE if {@link TimerFactory} objects were found in the received
     *         {@link BusObjectDescription} array.
     */
    public boolean isTimerFactoryAnnounced() {

        return timerFactories.size() != 0;
    }

    /**
     * Return whether a session with the {@link TimeServiceServer} bus has been
     * created
     * 
     * @return TRUE if the session has been created
     */
    public boolean isConnected() {

        return getSessionId() != null;
    }

    /**
     * Returns current session id or NULL if the session is not established or
     * lost.
     * 
     * @return session id
     */
    public Integer getSessionId() {

        if (sessionListener == null) {

            return null;
        }

        return sessionListener.getSessionId();
    }

    /**
     * Cleans the object and releases its resources <br>
     * It's a programming error to call another method on this object after the
     * release method has been called.
     */
    public void release() {

        Log.i(TAG, "Release was called, cleaning TimeService objects");

        // Prevent usage of the TimeServiceClient stale object
        if (managerMap != null) {

            for (TSItemsManager<?> manager : managerMap.values()) {

                manager.release();
            }

            managerMap.clear();
            managerMap = null;
        }

        if (sessionListener != null) {

            sessionListener.release();
        }

        // Last command for all the users of this BusAttachment for the release
        // purposes
        bus = null;
    }

    /**
     * Creates the session asynchronously with the {@link TimeServiceServer}
     * bus.
     * 
     * @param sessionListenerHandler
     *            Session related events will be passed via the
     *            {@link SessionListenerHandler}
     */
    public void joinSessionAsync(SessionListenerHandler sessionListenerHandler) {

        if (sessionListener == null) {

            sessionListener = new TimeServiceSessionListener(this);
        }

        sessionListener.joinSessionAsync(sessionListenerHandler);
    }

    /**
     * Leave previously established session
     * 
     * @return {@link Status} the method call status
     * @throws IllegalStateException
     *             if
     *             {@link TimeServiceClient#joinSessionAsync(SessionListenerHandler)}
     *             method has never called before
     * @see BusAttachment#leaveSession(int)
     * @see Status
     */
    public Status leaveSession() {

        if (sessionListener == null) {

            throw new IllegalStateException("Join session method has never called before");
        }

        return sessionListener.leaveSession();
    }

    /**
     * Checks validity of the received arguments. Objects should be initialized.
     * {@link BusAttachment} should be connected to the Router.
     * 
     * @param bus
     *            {@link BusAttachment} to test, should be initialized and
     *            connected.
     * @param serverBusName
     *            The bus name of the {@link TimeServiceServer}.
     * @throws IllegalArgumentException
     *             if non valid arguments were received
     */
    private void checkObjectValidity(BusAttachment bus, String serverBusName) {

        if (bus == null) {

            throw new IllegalArgumentException("BusAttachment is not initialized");
        }

        if (!bus.isConnected()) {

            throw new IllegalArgumentException("BusAttachment is not connected");
        }

        if (serverBusName == null || serverBusName.length() == 0) {

            throw new IllegalArgumentException("serverBusName is not initialized");
        }
    }

    /**
     * Initialize the managerMao with the required for object creation data
     */
    private void initFactoryMap() {

        managerMap.put(org.allseen.timeservice.ajinterfaces.Alarm.IFNAME, new TSItemsManager<Alarm>(Alarm.class, alarms));
        managerMap.put(org.allseen.timeservice.ajinterfaces.Clock.IFNAME, new TSItemsManager<Clock>(Clock.class, clocks));
        managerMap.put(org.allseen.timeservice.ajinterfaces.Timer.IFNAME, new TSItemsManager<Timer>(Timer.class, timers));
        managerMap.put(org.allseen.timeservice.ajinterfaces.AlarmFactory.IFNAME, new TSItemsManager<AlarmFactory>(AlarmFactory.class, alarmFactories));

        managerMap.put(org.allseen.timeservice.ajinterfaces.TimerFactory.IFNAME, new TSItemsManager<TimerFactory>(TimerFactory.class, timerFactories));
    }

    /**
     * Iterates over the array of {@link BusObjectDescription} and creates the
     * Time Service objects in depend on the found interfaces
     * 
     * @param objectDescs
     */
    private void analyzeObjectDescriptions(AboutObjectDescription[] objectDescs) {

        for (AboutObjectDescription bod : objectDescs) {

            String objPath = bod.path;
            String[] ifaces = bod.interfaces;

            boolean timeAuth = false; // whether current objPath implements
                                      // TimeAuthority
            Clock currClock = null;

            for (String iface : ifaces) {

                // Not a Time Service interface
                if (!iface.startsWith(TimeServiceConst.IFNAME_PREFIX)) {
                    continue;
                }

                // If the interface is TimeAuthority, store it
                if (iface.equals(TimeAuthority.IFNAME)) {

                    timeAuth = true;
                    continue;
                }

                TSItemsManager<?> manager = managerMap.get(iface);
                if (manager == null) {

                    Log.w(TAG, "Received unsupported interface: '" + iface + "', objPath: '" + objPath + "', from busName: '" + serverBusName + "'");
                    continue;
                }

                Object object;
                try {

                    object = manager.create(objPath);
                } catch (Exception e) {

                    Log.e(TAG, "Failed to create the TimeClient object, objPath: '" + objPath + "', interface: '" + iface + "' " + "received from bus: '" + serverBusName + "'", e);
                    continue;
                }

                Log.d(TAG, "Created TimeClient object, received from busName: '" + serverBusName + "' - " + object);

                if (object instanceof Clock) {

                    currClock = (Clock) object;
                }
            }// for::ifaces

            if (currClock != null && timeAuth) {

                currClock.setAuthority(timeAuth);
                Log.d(TAG, "Setting TimeAuthority for the Clock - " + currClock);
            }
        }
    }

}
