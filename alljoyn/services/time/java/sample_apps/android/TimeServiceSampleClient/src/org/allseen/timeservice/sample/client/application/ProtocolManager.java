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

package org.allseen.timeservice.sample.client.application;

import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.android.utils.AndroidLogger;
import org.allseen.timeservice.TimeServiceConst;

import android.content.Context;
import android.util.Log;

/**
 * Initializes an Alljoyn daemon and manages a list of AllJoyn devices the
 * daemon is announced on. This class will also enable the user to connect
 * Alljoyn bus attachment and disconnect from it.
 */
public class ProtocolManager {
    private static final String TAG = "ProtocolManager";

    public static final String ALARM_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".Alarm";
    public static final String ALARM_FACTORY_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".AlarmFactory";
    public static final String CLOCK_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".Clock";
    public static final String TIME_AUTHORITY_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".TimeAuthority";
    public static final String TIMER_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".Timer";
    public static final String TIMER_FACTORY_INTERFACE = TimeServiceConst.IFNAME_PREFIX + ".TimerFactory";

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
     * Announce handler.
     */
    private AboutListener announcementHandler;

    /**
     * String for Alljoyn daemon to be advertised with.
     */
    private static String daemonName = null;

    /**
     * Initialize the device list and starts the Alljoyn daemon.
     * 
     * @param context
     *            Android application context
     */
    protected void init(Context context, AboutListener announceHandler) {
        Log.i(TAG, "init");
        this.context = context;
        this.announcementHandler = announceHandler;
        boolean prepareDaemonResult = DaemonInit.PrepareDaemon(context.getApplicationContext());
        Log.i(TAG, "PrepareDaemon returned " + prepareDaemonResult);
        connectToBus();
    }

    /**
     * Creates new busAttachment, connects it. Register authListener on the bus.
     * Starts about service client. Registers the announce handler.
     */
    public void connectToBus() {
        Log.i(TAG, "connectToBus");
        if (context == null) {
            Log.e(TAG, "Failed to connect AJ, m_context == null !!");
            return;
        }
        // prepare bus attachment
        busAttachment = new BusAttachment(context.getPackageName(), BusAttachment.RemoteMessage.Receive);
        Status reqStatus = busAttachment.connect();
        if (reqStatus != Status.OK) {
            Log.e(TAG, "failed to connect to bus");
            return;
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
                return;
            } else {
                Log.d(TAG, "Succefully advertised daemon name " + daemonName);
            }
        }

        try {
            busAttachment.registerAboutListener(announcementHandler);
            busAttachment.whoImplements(new String[] { TimeServiceConst.IFNAME_PREFIX + "*" });

            // Add authentication listener - needed for TimeService secure calls
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
                return;
            }
        } catch (Exception e) {
            Log.e(TAG, "fail to startAboutClient", e);
        }
    }

    /**
     * Remove Match from Alljoyn bus attachment, Stop about client and cancel
     * bus advertise name.
     */
    public void disconnectFromBus() {
        Log.i(TAG, "disconnectFromBus");
        /*
         * It is important to unregister the BusObject before disconnecting from
         * the bus. Failing to do so could result in a resource leak.
         */
        try {
            if (busAttachment != null && busAttachment.isConnected()) {
                busAttachment.cancelWhoImplements(new String[] { TimeServiceConst.IFNAME_PREFIX + "*" });
                busAttachment.unregisterAboutListener(announcementHandler);
                busAttachment.cancelAdvertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);
                busAttachment.releaseName(daemonName);
                busAttachment.disconnect();
                busAttachment = null;
            }
        } catch (Exception e) {
            Log.e(TAG, "Error when disconnectFromAJ ");
            e.printStackTrace();
        }
        Log.i(TAG, "bus disconnected");

    }

    /**
     * 
     * @return true if the bus is connected.
     */
    public boolean isConnectedToBus() {
        if (busAttachment == null) {
            return false;
        }
        boolean isConnected = busAttachment.isConnected();
        Log.i(TAG, "isConnectToBus = " + isConnected);
        return isConnected;
    }

    /**
     * 
     * @return the busAttachment.
     */
    public BusAttachment getBusAttachment() {
        return busAttachment;
    }

}
