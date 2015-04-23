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

package org.alljoyn.onboarding.sdk.sampleApp;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.onboarding.sdk.OnboardingIllegalArgumentException;
import org.alljoyn.onboarding.sdk.OnboardingIllegalStateException;
import org.alljoyn.onboarding.sdk.OnboardingManager;
import org.alljoyn.onboarding.transport.OnboardingTransport;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.android.utils.AndroidLogger;
import org.alljoyn.services.common.utils.TransportUtil;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

/**
 * Initializes an Alljoyn daemon and manages a list of AllJoyn devices the
 * daemon is announced on. This class will also enable the user to connect
 * Alljoyn bus attachment and disconnect from it.
 */
public class ProtocolManager implements AboutListener {

    private final String TAG = this.getClass().getSimpleName();

    /**
     * Default password for daemon and the Alljoyn devices
     */
    private final static String DEFAULT_PINCODE = "000000";

    /**
     * List of AllJoyn devices the daemon is announced on.
     */
    List<Device> deviceList;

    /**
     * Unique prefix indicated that this daemon will be advertised quietly.
     */
    private static final String DAEMON_QUIET_PREFIX = "quiet@";

    /**
     * AllJoynManager singleton.
     */
    private static ProtocolManager instance = null;

    /**
     * Android application context.
     */
    private Context context = null;

    /**
     * Alljoyn bus attachment.
     */
    private static BusAttachment busAttachment = null;

    /**
     * String for Alljoyn daemon to be advertised with.
     */
    private static String daemonName = null;

    /**
     * Class represents onboarded device.
     */
    public class Device {

        /**
         * Bus attachment name
         */
        public String serviceName;

        /**
         * Alljoyn port for this device
         */
        public short port;

        /**
         * This device unique application id, taken from metadataMap map.
         */
        public UUID appId;

        /**
         * This device friendly name, taken from metadataMap map.
         */
        public String name;

        public Device(String serviceName, short port, UUID appId, String name) {
            this.serviceName = serviceName;
            this.port = port;
            this.appId = appId;
            this.name = name;
        }

        public void update(String serviceName, short port, String name) {
            this.serviceName = serviceName;
            this.port = port;
            this.name = name;
        }
    }

    public static ProtocolManager getInstance() {
        if (instance == null) {
            instance = new ProtocolManager();
        }
        return instance;
    }

    /**
     * Initialize the device list and starts the Alljoyn daemon.
     * 
     * @param context
     *            Android application context
     */
    protected void init(Context context) {
        Log.i(TAG, "init");
        this.context = context;
        deviceList = new ArrayList<Device>();
        boolean prepareDaemonResult = DaemonInit.PrepareDaemon(context.getApplicationContext());
        Log.i(TAG, "PrepareDaemon returned " + prepareDaemonResult);
        connectToBus();
    }

    /**
     * Listen to aboutService Announcement call and manage the DeviceList
     * accordingly.
     */
    @Override
    public void announced(final String serviceName, final int version, final short port, final AboutObjectDescription[] objectDescriptions, final Map<String, Variant> serviceMetadata) {
        UUID appId;
        String deviceName;

        // serviceMetadata map analysis
        try {
            Map<String, Object> fromVariantMap = TransportUtil.fromVariantMap(serviceMetadata);
            if (fromVariantMap == null) {
                Log.e(TAG, "onAnnouncement: serviceMetadata map = null !! ignoring.");
                return;
            }
            appId = (UUID) fromVariantMap.get(AboutKeys.ABOUT_APP_ID);
            deviceName = (String) fromVariantMap.get(AboutKeys.ABOUT_DEVICE_NAME);
            Log.i(TAG, "onAnnouncement: ServiceName = " + serviceName + " port = " + port + " deviceId = " + appId.toString() + " deviceName = " + deviceName);
        } catch (BusException e) {
            e.printStackTrace();
            return;
        }

        // update device list
        Device oldDevice = null;
        boolean deviceAdded = false;
        for (int i = 0; i < deviceList.size(); i++) {
            oldDevice = deviceList.get(i);
            if (oldDevice.appId.equals(appId)) {
                deviceList.remove(oldDevice);
                deviceList.add(new Device(serviceName, port, appId, deviceName));
                deviceAdded = true;
            }
        }
        if (!deviceAdded) {
            deviceList.add(new Device(serviceName, port, appId, deviceName));
        }
    }

    /**
     * Listen to aboutService onDeviceLost call and manage the DeviceList
     * accordingly.
     */
    public void onDeviceLost(String serviceName) {
        Log.d(TAG, "onDeviceLost serviceName = " + serviceName);
        Device device = null;
        for (int i = 0; i < deviceList.size(); i++) {
            device = deviceList.get(i);
            if (device.serviceName.equals(serviceName)) {
                Log.i(TAG, "remove device from list, friendly name = " + device.name);
                deviceList.remove(device);
                break;
            }
        }
    }

    public List<Device> getDeviceList() {
        return deviceList;
    }

    /**
     * Creates new busAttachment, connect and register authListener. Starts
     * about service. Update the OnboardingManager with the new busAttachment
     * aboutClient objects.
     */
    public void connectToBus() {
        Log.i(TAG, "connectToBus");
        if (context == null) {
            Log.e(TAG, "Failed to connect AJ, m_context == null !!");
            return;
        }
        // prepare bus attachment
        busAttachment = new BusAttachment(context.getPackageName(), BusAttachment.RemoteMessage.Receive);
        busAttachment.connect();

        // request the name for the daemon and advertise it.
        daemonName = "org.alljoyn.BusNode.d" + busAttachment.getGlobalGUIDString();
        int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;
        Status reqStatus = busAttachment.requestName(daemonName, flag);
        if (reqStatus == Status.OK) {
            // advertise the name with a quite prefix for devices to find it
            Status adStatus = busAttachment.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);
            if (adStatus != Status.OK) {
                busAttachment.releaseName(daemonName);
                Log.w(TAG, "failed to advertise daemon name " + daemonName);
            } else {
                Log.d(TAG, "Succefully advertised daemon name " + daemonName);
            }
        }

        try {
            busAttachment.registerAboutListener(this);
            busAttachment.whoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });

            // Add auth listener - needed for OnboardingService secure calls
            String keyStoreFileName = context.getFileStreamPath("alljoyn_keystore").getAbsolutePath();
            SrpAnonymousKeyListener m_authListener = new SrpAnonymousKeyListener(new AuthPasswordHandler() {
                private final String TAG = "AlljoynOnAuthPasswordHandler";

                @Override
                public char[] getPassword(String peerName) {
                    return DEFAULT_PINCODE.toCharArray();
                }

                @Override
                public void completed(String mechanism, String authPeer, boolean authenticated) {
                    Log.d(TAG, "Auth completed: mechanism = " + mechanism + " authPeer= " + authPeer + " --> " + authenticated);
                    if (!authenticated) {
                        Intent AuthErrorIntent = new Intent(OnboardingManager.ERROR);
                        Bundle extra = new Bundle();
                        extra.putString(OnboardingManager.EXTRA_ERROR_DETAILS, String.format(context.getString(R.string.auth_failed_msg), mechanism, authPeer));
                        AuthErrorIntent.putExtras(extra);
                        context.sendBroadcast(AuthErrorIntent);
                    }
                }

            }, new AndroidLogger(), new String[] { "ALLJOYN_SRP_KEYX", "ALLJOYN_ECDHE_PSK", "ALLJOYN_PIN_KEYX" });
            Log.i(TAG, "m_authListener.getAuthMechanismsAsString: " + m_authListener.getAuthMechanismsAsString());
            Status authStatus = busAttachment.registerAuthListener(m_authListener.getAuthMechanismsAsString(), m_authListener, keyStoreFileName);
            if (authStatus != Status.OK) {
                Log.e(TAG, "Failed to connectToBus");
            }
        } catch (Exception e) {
            Log.e(TAG, "fail to connectToBus", e);
        }

        try {
            OnboardingManager.getInstance().init(context, busAttachment);
        } catch (OnboardingIllegalArgumentException e) {
            e.printStackTrace();
        } catch (OnboardingIllegalStateException e) {
            e.printStackTrace();
        }

        Log.i(TAG, " connectToBus Done");
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
                busAttachment.cancelWhoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });
                busAttachment.unregisterAboutListener(this);
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
        deviceList.clear();
    }

    public boolean isConnectedToBus() {
        if (busAttachment == null) {
            return false;
        }
        boolean isConnected = busAttachment.isConnected();
        Log.i(TAG, "isConnectToBus = " + isConnected);
        return isConnected;
    }

}
