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

package org.alljoyn.onboarding.test;

import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.Map;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.onboarding.OnboardingService.AuthType;
import org.alljoyn.onboarding.client.OnboardingClientImpl;
import org.alljoyn.onboarding.transport.OBLastError;
import org.alljoyn.onboarding.transport.OnboardingTransport;
import org.alljoyn.onboarding.transport.OnboardingTransport.ConfigureWifiMode;
import org.alljoyn.onboarding.transport.ScanInfo;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.common.utils.GenericLogger;
import org.alljoyn.services.common.utils.TransportUtil;

import android.app.AlertDialog;
import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.HandlerThread;
import android.util.Log;
import android.widget.Toast;

/**
 * The OnboardingApplication class handles all the AllJoyn devices the
 * application is announced on, and enable the user to perform all the AllJoyn
 * onboarding service methods. (get the scan information, do onboarding,
 * offboarding etc.)
 */
public class OnboardingApplication extends Application implements AuthPasswordHandler, AboutListener {

    public static final String TAG = "OnboardingClient";
    public static final String TAG_PASSWORD = "OnboardingApplication_password";

    private BusAttachment m_Bus;
    private IskWifiManager m_wifiManager;
    private HashMap<String, SoftAPDetails> m_devicesMap;
    private SoftAPDetails m_currentPeer;
    private String m_realmName;
    private BroadcastReceiver m_receiver;

    /**
     * The daemon should advertise itself "quietly" (directly to the calling
     * port) This is to reply directly to a TC looking for a daemon
     */
    private static final String DAEMON_QUIET_PREFIX = "quiet@";

    private final GenericLogger m_logger = new GenericLogger() {
        @Override
        public void debug(String TAG, String msg) {
            Log.d(TAG, msg);
        }

        @Override
        public void info(String TAG, String msg) {
            // To change body of implemented methods use File | Settings | File
            // Templates.
            Log.i(TAG, msg);
        }

        @Override
        public void warn(String TAG, String msg) {
            // To change body of implemented methods use File | Settings | File
            // Templates.
            Log.w(TAG, msg);
        }

        @Override
        public void error(String TAG, String msg) {
            // To change body of implemented methods use File | Settings | File
            // Templates.
            Log.e(TAG, msg);
        }

        @Override
        public void fatal(String TAG, String msg) {
            // To change body of implemented methods use File | Settings | File
            // Templates.
            Log.wtf(TAG, msg);
        }
    };
    private OnboardingClientImpl m_onboardingClient;

    static {
        try {
            System.loadLibrary("alljoyn_java");
        } catch (Exception e) {
            System.out.println("can't load library alljoyn_java");
        }
    }

    // ======================================================================
    /*
     * (non-Javadoc)
     * 
     * @see android.app.Application#onCreate()
     */
    @Override
    public void onCreate() {

        super.onCreate();
        HandlerThread busThread = new HandlerThread("BusHandler");
        busThread.start();
        m_wifiManager = new IskWifiManager(getApplicationContext());
        m_devicesMap = new HashMap<String, SoftAPDetails>();

        // Receiver
        m_receiver = new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent) {

                if (WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())) {

                    NetworkInfo networkInfo = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);

                    String str = "";
                    if (networkInfo.getState().equals(State.CONNECTED)) {
                        WifiInfo wifiInfo = intent.getParcelableExtra(WifiManager.EXTRA_WIFI_INFO);
                        str = wifiInfo.getSSID();
                    } else {
                        str = networkInfo.getState().toString().toLowerCase(Locale.getDefault());
                    }
                    Intent networkIntent = new Intent(Keys.Actions.ACTION_CONNECTED_TO_NETWORK);
                    networkIntent.putExtra(Keys.Extras.EXTRA_NETWORK_SSID, str);
                    sendBroadcast(networkIntent);
                }
            }
        };
        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        registerReceiver(m_receiver, filter);

    }

    // ======================================================================
    public GenericLogger getLogger() {
        return m_logger;
    }

    // ======================================================================
    public IskWifiManager getIskWifiManager() {
        return m_wifiManager;
    }

    // ======================================================================
    /**
     * @param msg
     *            Given a msg, create and display a toast on the screen.
     */
    public void makeToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

    // ======================================================================
    /**
     * Sets the daemon realm name.
     * 
     * @param realmName
     *            The daemon realm name.
     */
    public void setRealmName(String realmName) {
        m_realmName = realmName;
    }

    // ======================================================================
    /*
     * (non-Javadoc)
     * 
     * @see android.app.Application#onTerminate()
     */
    @Override
    public void onTerminate() {
        super.onTerminate();
        unregisterReceiver(m_receiver);
    }

    // ======================================================================

    /**
     * Connect to the Alljoyn bus and register bus objects.
     */
    public void doConnect() {

        /* All communication through AllJoyn begins with a BusAttachment. */
        boolean b = DaemonInit.PrepareDaemon(getApplicationContext());
        System.out.println(b);
        String ss = getPackageName();
        m_Bus = new BusAttachment(ss, BusAttachment.RemoteMessage.Receive);
        Status status = m_Bus.connect();
        Log.d(TAG, "bus.connect status: " + status);

        // Pump up the daemon debug level
        m_Bus.setDaemonDebug("ALL", 7);
        m_Bus.setLogLevels("ALL=7");
        m_Bus.useOSLogging(true);

        String keyStoreFileName = null;

        // request the name
        int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;
        String DAEMON_NAME = m_realmName;// "org.alljoyn.BusNode.Dashboard2";
        Status reqStatus = m_Bus.requestName(DAEMON_NAME, flag);
        if (reqStatus == Status.OK) {
            // advertise the name
            // advertise the name with a quite prefix for TC to find it
            Status adStatus = m_Bus.advertiseName(DAEMON_QUIET_PREFIX + DAEMON_NAME, SessionOpts.TRANSPORT_ANY);
            if (adStatus != Status.OK) {
                m_Bus.releaseName(DAEMON_NAME);
                m_logger.warn(TAG, "failed to advertise daemon name " + DAEMON_NAME);
            } else {
                m_logger.debug(TAG, "Succefully advertised daemon name " + DAEMON_NAME);
            }
        }

        // set keyListener
        keyStoreFileName = getApplicationContext().getFileStreamPath("alljoyn_keystore").getAbsolutePath();
        if (keyStoreFileName != null && keyStoreFileName.length() > 0) {
            SrpAnonymousKeyListener authListener = new SrpAnonymousKeyListener(OnboardingApplication.this, m_logger, new String[] { "ALLJOYN_SRP_KEYX", "ALLJOYN_ECDHE_PSK", "ALLJOYN_PIN_KEYX" });
            Status authStatus = m_Bus.registerAuthListener(authListener.getAuthMechanismsAsString(), authListener, keyStoreFileName);
            m_logger.debug(TAG, "BusAttachment.registerAuthListener status = " + authStatus);
            if (authStatus != Status.OK) {
                m_logger.debug(TAG, "Failed to register Auth listener status = " + authStatus.toString());
            }
        }

        m_Bus.registerAboutListener(this);
        m_Bus.whoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });
    }

    @Override
    public void announced(String busName, int version, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> aboutMap) {
        Map<String, Object> newMap = new HashMap<String, Object>();
        try {
            newMap = TransportUtil.fromVariantMap(aboutMap);
            String deviceId = (newMap.get(AboutKeys.ABOUT_APP_ID).toString());
            String deviceFriendlyName = (String) newMap.get(AboutKeys.ABOUT_DEVICE_NAME);
            m_logger.debug(TAG, "onAnnouncement received: with parameters: busName:" + busName + ", port:" + port + ", deviceid" + deviceId + ", deviceName:" + deviceFriendlyName);
            addDevice(deviceId, busName, port, deviceFriendlyName, objectDescriptions, newMap);

        } catch (BusException e) {
            e.printStackTrace();
        }
    }

    // ======================================================================
    /**
     * Disconnect from Alljoyn bus and unregister bus objects.
     */
    public void doDisconnect() {
        /*
         * It is important to unregister the BusObject before disconnecting from
         * the bus. Failing to do so could result in a resource leak.
         */
        try {
            if (m_Bus != null) {
                m_Bus.cancelWhoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });
                m_Bus.unregisterAboutListener(this);
                m_Bus.clearKeyStore();
                m_logger.info(TAG_PASSWORD, "Bus attachment clear key store");
                m_Bus.cancelAdvertiseName(DAEMON_QUIET_PREFIX + m_realmName, SessionOpts.TRANSPORT_ANY);
                m_Bus.releaseName(m_realmName);
                m_Bus.disconnect();
                m_Bus = null;
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // ======================================================================

    // Add an AllJoym device to the application.
    private void addDevice(String deviceId, String busName, short port, String deviceFriendlyName, AboutObjectDescription[] objectDescriptions, Map<String, Object> aboutMap) {
        SoftAPDetails oldDevice = m_devicesMap.get(deviceId);

        if (oldDevice != null) {// device already exist. update the fields that
                                // might have changed.
            if (!oldDevice.busName.equals(busName)) {
                // In case the bus name changed, the password should be reset
                oldDevice.password = SrpAnonymousKeyListener.DEFAULT_PINCODE;
            } else {
                // do not change password.
            }

            oldDevice.busName = busName;
            oldDevice.aboutMap = aboutMap;
            oldDevice.deviceFriendlyName = deviceFriendlyName;
            oldDevice.port = port;
            oldDevice.objectDescriptions = objectDescriptions;
            oldDevice.updateSupportedServices();

        } else {
            // add the device to the map
            SoftAPDetails sad = new SoftAPDetails(deviceId, busName, deviceFriendlyName, port, objectDescriptions, aboutMap, SrpAnonymousKeyListener.DEFAULT_PINCODE);
            m_devicesMap.put(deviceId, sad);
        }
        // notify the activity to come and get it
        Intent intent = new Intent(Keys.Actions.ACTION_DEVICE_FOUND);
        Bundle extras = new Bundle();
        extras.putString(Keys.Extras.EXTRA_DEVICE_ID, deviceId);
        intent.putExtras(extras);
        sendBroadcast(intent);
    }

    // ======================================================================

    // Remove an AllJoyn device from the application.
    private void removeDevice(String busName) {

        Collection<SoftAPDetails> devices = m_devicesMap.values();
        Object[] array = devices.toArray();
        for (int i = 0; i < array.length; i++) {
            SoftAPDetails d = (SoftAPDetails) array[i];
            if (d.busName.equals(busName)) {
                m_devicesMap.remove(d.appId);
            }
        }
        Intent intent = new Intent(Keys.Actions.ACTION_DEVICE_LOST);
        Bundle extras = new Bundle();
        extras.putString(Keys.Extras.EXTRA_BUS_NAME, busName);
        intent.putExtras(extras);
        sendBroadcast(intent);
    }

    // ======================================================================

    // Send an intent indicating an error has occurred.
    private void updateTheUiAboutError(String error) {

        Intent intent = new Intent(Keys.Actions.ACTION_ERROR);
        intent.putExtra(Keys.Extras.EXTRA_ERROR, error);
        sendBroadcast(intent);
    }

    // ======================================================================

    // Retrieve a device by its device is.
    public SoftAPDetails getDevice(String deviceId) {
        return m_devicesMap.get(deviceId);
    }

    // ======================================================================

    // Display a dialog with the given errorMsg and displays it.
    public void showAlert(Context context, String errorMsg) {

        AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setTitle("Error");
        alert.setMessage(errorMsg);

        alert.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
            }
        });
        alert.show();
    }

    // ======================================================================
    public void startSession(SoftAPDetails device) {

        m_currentPeer = device;
        m_onboardingClient = new OnboardingClientImpl(m_currentPeer.busName, m_Bus, null, m_currentPeer.port);

        if (m_onboardingClient != null) {
            Status status = m_onboardingClient.connect();
            m_logger.debug(TAG, "status of start session: " + status.toString());
        }
    }

    // ======================================================================
    public void endSession() {
        if (m_onboardingClient != null) {
            m_onboardingClient.disconnect();
        }
    }

    // ======================================================================
    /**
     * Return the onboarding service fields.
     * 
     * @return the onboarding service fields.
     */
    public Short getOnboardingVersion() {

        short onboardingVersion = -1;
        try {
            if (m_currentPeer.supportOnboarding) {

                if (!m_onboardingClient.isConnected()) {
                    m_onboardingClient.connect();
                }
                onboardingVersion = m_onboardingClient.getVersion();
            }
        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("GET ONBOARDING VERSION: Exception: " + e.toString());
        }
        return Short.valueOf(onboardingVersion);

    }

    // ======================================================================
    /**
     * Return the onboarding service last error
     * 
     * @return the onboarding service last error
     */
    public OBLastError getLastError() {

        OBLastError m_lastError = new OBLastError();
        m_lastError.setErrorCode((short) -1);
        m_lastError.setErrorMessage("unKnown");

        try {
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            m_lastError = m_onboardingClient.GetLastError();

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("GET LAST ERROR: Exception: " + e.toString());
        }
        return m_lastError;
    }

    // ======================================================================
    /**
     * Return the onboarding service state.
     * 
     * @return the onboarding service state.
     */
    public Short getState() {

        short state = -1;
        try {
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            state = m_onboardingClient.getState();

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("GET STATE: Exception: " + e.toString());
        }
        return state;
    }

    // ======================================================================
    /**
     * Return the onboarding service scan info.
     * 
     * @return the onboarding service scan info.
     */
    public ScanInfo getScanInfo() {

        ScanInfo scanInfo = null;
        try {
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            scanInfo = m_onboardingClient.getScanInfo();

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("GET SCAN INFO: Exception: " + e.toString());
        }
        return scanInfo;
    }

    // ======================================================================
    /**
     * Configure a network for the alljoyn device
     * 
     * @param networkName
     *            The name (ssid) of the configured network
     * @param networkPassword
     *            The password of the configured network
     * @param networkAuthType
     *            The auth type of the configured network (can get by
     *            AuthType.getTypeId()).
     */
    public void configureNetwork(String networkName, String networkPassword, short networkAuthType) {

        Log.d(TAG, "ONBOARDING: OnboardingClientImpl for " + m_currentPeer.busName);
        try {
            Log.d(TAG, "ONBOARDING: before connect");
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            Log.d(TAG, "ONBOARDING: connect() succeeded. before configureWiFi()");
            ConfigureWifiMode mode = m_onboardingClient.configureWiFi(networkName, networkPassword, AuthType.getAuthTypeById(networkAuthType));
            System.out.println(mode);
            Log.d(TAG, "ONBOARDING: configureWiFi() succeeded. before disconnect()");

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("CONFIGURE NETWORK: Exception: " + e.toString());
        }
    }

    // ======================================================================
    /**
     * Connect the alljoyn device to its last configured network.
     */
    public void connectNetwork() {

        Log.d(TAG, "ONBOARDING: OnboardingClientImpl for " + m_currentPeer.busName);
        try {
            Log.d(TAG, "ONBOARDING: before connect");
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            Log.d(TAG, "ONBOARDING: connect() succeeded. before connectWiFi()");
            m_onboardingClient.connectWiFi();
            Log.d(TAG, "ONBOARDING: connectWiFi() succeeded");

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("CONNECT NETWORK: Exception: " + e.toString());
        }
    }

    // ======================================================================
    /**
     * Offboard the alljoyn device from its last network.
     */
    public void offboard() {

        Log.d(TAG, "OFFBOARDING: OnboardingClientImpl for " + m_currentPeer.busName);
        try {
            Log.d(TAG, "OFFBOARDING: before connect");
            if (!m_onboardingClient.isConnected()) {
                m_onboardingClient.connect();
            }
            Log.d(TAG, "OFFBOARDING: connect() succeeded. before offboard()");
            m_onboardingClient.offboard();
            Log.d(TAG, "OFFBOARDING: offboard() succeeded");

        } catch (Exception e) {
            e.printStackTrace();
            updateTheUiAboutError("OFFBOARDING: " + e.getMessage());
        }
    }

    // ***************************** PASSWORD *******************************
    // ======================================================================
    /*
     * (non-Javadoc)
     * 
     * @see
     * org.alljoyn.services.android.security.AuthPasswordHandler#getPassword
     * (java.lang.String)
     */
    @Override
    public char[] getPassword(String peerName) {

        // Search the device by its peerName and return its password
        m_logger.info(TAG_PASSWORD, "AuthPasswordHandler asked for password");
        SoftAPDetails softAPDetails = null;
        if (peerName != null) {
            Collection<SoftAPDetails> devices = m_devicesMap.values();
            Iterator<SoftAPDetails> iterator = devices.iterator();
            for (; iterator.hasNext();) {
                softAPDetails = iterator.next();
                if (softAPDetails.busName.equals(peerName)) {
                    char[] password = softAPDetails.password;
                    m_logger.info(TAG_PASSWORD, "Client password = " + String.valueOf(password) + " for peerName " + peerName + ", Name=" + softAPDetails.deviceFriendlyName);
                    if (password != null)
                        return password;
                }
            }
        }
        m_logger.info(TAG_PASSWORD, "Client password = 000000+ for peerName " + peerName + ", Name=" + (softAPDetails != null ? softAPDetails.deviceFriendlyName : ""));
        return SrpAnonymousKeyListener.DEFAULT_PINCODE;
    }

    // ======================================================================
    /**
     * Sets a password on the client side. This password will be compared
     * against the alljoyn device password when needed.
     * 
     * @param peerName
     *            The device bus name.
     * @param password
     *            The device new password.
     */
    public void setPassword(String peerName, char[] password) {

        m_logger.info(TAG_PASSWORD, "setPassword was called on the client side");
        Collection<SoftAPDetails> devices = m_devicesMap.values();
        Iterator<SoftAPDetails> iterator = devices.iterator();
        for (; iterator.hasNext();) {
            SoftAPDetails softAPDetails = iterator.next();
            if (softAPDetails.busName.equals(peerName)) {
                m_logger.info(TAG_PASSWORD, "Set the password on the client side from: " + String.valueOf(softAPDetails.password) + " to: " + String.valueOf(password));
                softAPDetails.password = password;
            }
        }
    }

    // ======================================================================
    /*
     * (non-Javadoc)
     * 
     * @see
     * org.alljoyn.services.android.security.AuthPasswordHandler#completed(java
     * .lang.String, java.lang.String, boolean)
     */
    @Override
    public void completed(String mechanism, String authPeer, boolean authenticated) {
        if (!authenticated) {
            m_logger.info(TAG_PASSWORD, " ** " + authPeer + " failed to authenticate");
            Intent intent = new Intent(Keys.Actions.ACTION_PASSWORD_IS_INCORRECT);
            sendBroadcast(intent);
        } else
            m_logger.info(TAG_PASSWORD, " ** " + authPeer + " successfully authenticated");
    }
    // ======================================================================
}
