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
package org.allseen.timeservice.sample.server.logic;

import java.util.Locale;

import org.alljoyn.bus.AboutObj;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.android.storage.PropertyStoreImpl;
import org.alljoyn.services.android.utils.AndroidLogger;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.TimeServiceServer;

import android.content.Context;
import android.util.Log;

/**
 * Singleton that does the following
 * <ul>
 * <li>Manages the AllJoyn protocol initialization.
 * <li>Initializes TimeService elements with accordance to the
 * {@link PreferencesManager} and Announce the changes.
 * <li>Enables to emit timeSync signal
 * </ul>
 */
public class ProtocolManager {

    /**
     * Log Tag
     */
    private static final String TAG = "ProtocolManager";

    /**
     * ServerClock storage
     */
    private ServerClock serverClock = null;

    /**
     * ServerAuthorityClock storage
     */
    private ServerAuthorityClock serverAuthorityClock = null;

    /**
     * ServerAlarmFactory storage
     */
    private ServerAlarmFactory serverAlarmFactory = null;

    /**
     * ServerTimerFactory storage
     */
    private ServerTimerFactory serverTimerFactory = null;

    /**
     * ServerTimer storage
     */
    private ServerTimer timer1 = null;

    /**
     * ServerAlarm storage
     */
    private ServerAlarm alarm1 = null;

    /**
     * Daemon name prefix of the service.
     */
    private static final String DAEMON_NAME_PREFIX = "org.alljoyn.BusNode.TimeService";

    /**
     * Unique prefix indicated that this daemon will be advertised quietly.
     */
    private static final String DAEMON_QUIET_PREFIX = "quiet@";

    /**
     * Android application context.
     */
    private Context context = null;

    /**
     * Alljoyn bus attachment.
     */
    private static BusAttachment busAttachment = null;

    /**
     * Alljoyn about object, needed to receive announcements from available
     * Alljoyn devices.
     */
    private AboutObj aboutObj = null;

    /**
     * The application announced port
     */
    private static final short ANNOUNCE_PORT = 1080;

    /**
     * String for Alljoyn daemon to be advertised with.
     */
    private static String daemonName = null;

    /**
     * Initialize the device list and starts the Alljoyn daemon.
     * 
     * @param context
     *            Android application context
     * 
     */
    public void init(Context context) {
        Log.i(TAG, "init");
        this.context = context;

        boolean prepareDaemonResult = DaemonInit.PrepareDaemon(context.getApplicationContext());
        Log.i(TAG, "PrepareDaemon returned " + prepareDaemonResult);
        if (connectToBus()) {
            initiateTimeServer();
        }
    }

    /**
     * Connect to the bus. Start AboutServer Start TimeServiceServer
     * 
     */
    private boolean connectToBus() {
        Log.i(TAG, "connectToBus");
        if (context == null) {
            Log.e(TAG, "Failed to connect AJ, m_context == null !!");
            return false;
        }
        // prepare bus attachment
        busAttachment = new BusAttachment(context.getPackageName(), BusAttachment.RemoteMessage.Receive);

        // Debug Alljoyn daemon
        // busAttachment.setDaemonDebug("ALL", 7);
        // busAttachment.setLogLevels("ALLJOYN=7");
        // busAttachment.setLogLevels("NETWORK=7");
        // busAttachment.setLogLevels("TCP=7");
        // busAttachment.useOSLogging(true);

        Status reqStatus = busAttachment.connect();
        if (reqStatus != Status.OK) {
            Log.e(TAG, "failed to connect to bus");
            return false;
        } else {
            Log.d(TAG, "Succefully connected to bus");
        }

        // request the name for the daemon and advertise it.
        daemonName = DAEMON_NAME_PREFIX + ".G" + busAttachment.getGlobalGUIDString();
        int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;
        reqStatus = busAttachment.requestName(daemonName, flag);
        if (reqStatus == Status.OK) {
            // advertise the name with a quite prefix for devices to find it
            Status adStatus = busAttachment.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);
            if (adStatus != Status.OK) {
                busAttachment.releaseName(daemonName);
                Log.w(TAG, "failed to advertise daemon name " + daemonName);
                return false;
            } else {
                Log.d(TAG, "Succefully advertised daemon name " + daemonName);
            }
        }

        bindSessionPort(ANNOUNCE_PORT);

        aboutObj = new AboutObj(busAttachment);

        try {

            String keyStoreFileName = context.getFileStreamPath("alljoyn_keystore").getAbsolutePath();
            SrpAnonymousKeyListener m_authListener = new SrpAnonymousKeyListener(new AuthPasswordHandler() {
                private final String TAG = "AlljoynOnAuthPasswordHandler";

                @Override
                public char[] getPassword(String peerName) {
                    return SrpAnonymousKeyListener.DEFAULT_PINCODE;
                }

                @Override
                public void completed(String mechanism, String authPeer, boolean authenticated) {
                    Log.d(TAG, "Auth completed: mechanism = " + mechanism + " authPeer= " + authPeer + " --> " + authenticated);
                    if (!authenticated) {
                        Log.e(TAG, "Failed to Authenticate");
                    }
                }
            }, new AndroidLogger(), new String[] { "ALLJOYN_SRP_KEYX", "ALLJOYN_ECDHE_PSK", "ALLJOYN_PIN_KEYX" });
            Log.i(TAG, "m_authListener.getAuthMechanismsAsString: " + m_authListener.getAuthMechanismsAsString());
            Status authStatus = busAttachment.registerAuthListener(m_authListener.getAuthMechanismsAsString(), m_authListener, keyStoreFileName);
            if (authStatus != Status.OK) {
                Log.e(TAG, "Failed to registerAuthListener");
                return false;
            }
            TimeServiceServer.getInstance().init(busAttachment);
        } catch (TimeServiceException t) {
            Log.e(TAG, "fail to init TimeServiceServer", t);
            return false;
        }
        return true;
    }

    /**
     * Bind to About service port required by EventActions
     * 
     * @param port
     *            to bind
     * @return bus uniqueName in case successful
     */
    private String bindSessionPort(final short port) {
        /*
         * Create a new session listening on the contact port of the about
         * service.
         */
        Mutable.ShortValue contactPort = new Mutable.ShortValue(port);

        SessionOpts sessionOpts = new SessionOpts();
        sessionOpts.traffic = SessionOpts.TRAFFIC_MESSAGES;
        sessionOpts.isMultipoint = true;
        sessionOpts.proximity = SessionOpts.PROXIMITY_ANY;
        sessionOpts.transports = SessionOpts.TRANSPORT_ANY;

        Status status = busAttachment.bindSessionPort(contactPort, sessionOpts, new SessionPortListener() {
            @Override
            public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts sessionOpts) {
                if (sessionPort == port) {
                    return true;
                } else {
                    return false;
                }
            }

            @Override
            public void sessionJoined(short sessionPort, int id, String joiner) {
                Log.i(TAG, String.format("SessionPortListener.sessionJoined(%d, %d, %s)", sessionPort, id, joiner));
            }
        });
        Log.d(TAG, String.format(Locale.US, "BusAttachment.bindSessionPort(%d, %s): %s", contactPort.value, sessionOpts.toString(), status));

        if (status != Status.OK) {
            return "";
        }
        return busAttachment.getUniqueName();
    }

    public boolean isConnectedToBus() {
        if (busAttachment == null) {
            return false;
        }
        boolean isConnected = busAttachment.isConnected();
        Log.i(TAG, "isConnectToBus = " + isConnected);
        return isConnected;
    }

    public BusAttachment getBusAttachment() {
        return busAttachment;
    }

    /**
     * Reads the application preferences using the PreferencesManager. Creates
     * TimeService elements with accordance to the the preferences. Emits
     * Announce signal.
     */
    public void initiateTimeServer() {
        try {

            PreferencesManager.AppPreferences appPreferences = PreferencesManager.getInstance().getPrefrences();

            if (serverClock == null) {
                serverClock = new ServerClock(context);
            }
            if (appPreferences.clockSetable) {
                serverClock.setIsSetable(true);
            } else {
                serverClock.setIsSetable(false);
            }

            if (appPreferences.timeAuthority) {
                if (serverAuthorityClock == null) {
                    serverAuthorityClock = new ServerAuthorityClock(serverClock);
                    TimeServiceServer.getInstance().createTimeAuthorityClock(serverAuthorityClock, AuthorityType.NTP);
                }
            } else {
                if (serverAuthorityClock != null) {
                    serverAuthorityClock.release();
                    serverAuthorityClock = null;
                }
                TimeServiceServer.getInstance().createClock(serverClock);
            }

            if (appPreferences.alarmFactory) {
                if (serverAlarmFactory == null) {
                    serverAlarmFactory = new ServerAlarmFactory(context);
                    TimeServiceServer.getInstance().createAlarmFactory(serverAlarmFactory);
                }
            } else {
                if (serverAlarmFactory != null) {
                    serverAlarmFactory.release();
                    serverAlarmFactory = null;
                }
            }

            if (appPreferences.timerFactory) {
                if (serverTimerFactory == null) {
                    serverTimerFactory = new ServerTimerFactory(context);
                    TimeServiceServer.getInstance().createTimerFactory(serverTimerFactory);
                }
            } else {
                if (serverTimerFactory != null) {
                    serverTimerFactory.release();
                    serverTimerFactory = null;
                }
            }

            if (appPreferences.alarm1) {
                if (alarm1 == null) {
                    alarm1 = new ServerAlarm(context);
                    TimeServiceServer.getInstance().createAlarm(alarm1);
                }
            } else {
                if (alarm1 != null) {
                    alarm1.release();
                    alarm1 = null;
                }
            }

            if (appPreferences.timer1) {
                if (timer1 == null) {
                    timer1 = new ServerTimer(context);
                    TimeServiceServer.getInstance().createTimer(timer1);
                }
            } else {
                if (timer1 != null) {
                    timer1.release();
                    timer1 = null;
                }
            }

            aboutObj.announce(ANNOUNCE_PORT, new PropertyStoreImpl(context));

        } catch (TimeServiceException e) {
            Log.e(TAG, "Failed init TimeServer elements", e);
        }
    }

    /**
     * Sends timeSync signal.
     * 
     * @return true if serverAuthorityClock exists otherwise false
     * @throws TimeServiceException
     */
    public boolean sendTimeSync() throws TimeServiceException {

        if (serverAuthorityClock != null) {
            serverAuthorityClock.timeSync();
            return true;
        }
        return false;
    }

}