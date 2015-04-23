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

package org.alljoyn.onboarding.sdk;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;

import org.alljoyn.onboarding.OnboardingService.AuthType;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.os.Bundle;
import android.util.Log;
import android.util.Pair;

/**
 * Wraps the interaction with {@link WifiManager}.
 */
class OnboardingSDKWifiManager {

    private final static String TAG = "OnboardingSDKWifiManager";

    /**
     * Android application context.
     */
    private final Context context;

    /**
     * WiFi manager instance.
     */
    private final WifiManager wifi;

    /**
     * Broadcast receiver for WifiManager intents.
     */
    private BroadcastReceiver wifiBroadcastReceiver;

    /**
     * Stores the details of the target Wi-Fi. Uses volatile to verify that the
     * broadcast receiver reads its content each time onReceive is called, thus
     * "knowing" if the an API call to connect has been made
     */
    private volatile WifiConfiguration targetWifiConfiguration = null;

    /**
     * Stores all Wi-Fi access points that are not considered AllJoyn devices.
     */
    private final ArrayList<WiFiNetwork> nonOnboardableAPlist = new ArrayList<WiFiNetwork>();

    /**
     * Stores all Wi-Fi access points that are considered onboardee devices.
     */
    private final ArrayList<WiFiNetwork> onboardableAPlist = new ArrayList<WiFiNetwork>();

    /**
     * Stores all Wi-Fi access points that were found in the scan
     */
    private final ArrayList<WiFiNetwork> allAPlist = new ArrayList<WiFiNetwork>();

    /**
     * SSID prefix for onboardable devices.
     */
    static final private String ONBOARDABLE_PREFIX = "AJ_";

    /**
     * SSID suffix for onboardable devices.
     */
    static final private String ONBOARDABLE_SUFFIX = "_AJ";

    /**
     * WEP HEX password pattern.
     */
    static final String WEP_HEX_PATTERN = "[\\dA-Fa-f]+";

    /**
     * Timer for checking completion of Wi-Fi tasks.
     */
    private Timer wifiTimeoutTimer = new Timer();

    /**
     * AJ daemon discovery relies on multicast. Normally the Android filters
     * out packets not explicitly addressed to this device. Acquiring a
     * MulticastLock will cause the stack to receive packets addressed to
     * multicast addresses.
     */
    private MulticastLock multicastLock;

    /**
     * Initialize the WifiManager and the Wi-Fi BroadcastReceiver.
     *
     * @param context
     *            the application Context
     */
    public OnboardingSDKWifiManager(Context context) {
        this.context = context;
        wifi = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        registerWifiBroadcastReceiver();
    }


    /**
     * Register a BroadcastReciver to receive intents from {@link WifiManager}.
     *
     * <ul>
     *   <li>{@link WifiManager#SCAN_RESULTS_AVAILABLE_ACTION} - Wi-Fi scan is complete. handled by {@link #processScanResults(List)}
     *    <li>{@link WifiManager#NETWORK_STATE_CHANGED_ACTION} - the handset connected to a Wi-Fi access point. handled by {@link #processChangeOfNetwork(WifiInfo)}
     *    <li>{@link WifiManager#SUPPLICANT_STATE_CHANGED_ACTION} - authentication error. Will fire a {@link OnboardingManager#WIFI_AUTHENTICATION_ERROR} intent.
     * </ul>
     * using synchronized to protect against concurrent onRecive(..) calls that set targetWifiConfiguration in a non steady state.
     */
    private void registerWifiBroadcastReceiver() {
        Log.d(TAG, "registerWifiBroadcastReceiver");

        wifiBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {

                Log.d(TAG, "WiFi BroadcastReceiver onReceive: " + intent.getAction());
                if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION.equals(intent.getAction())) {
                    // Wi-Fi scan is complete
                    List<ScanResult> scans = wifi.getScanResults();
                    if (scans != null) {
                        processScanResults(scans);
                    } else {
                        Log.i(TAG, "WiFi BroadcastReceiver onReceive wifi.getScanResults() == null");
                    }
                } else if (WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())) {
                    NetworkInfo networkInfo = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                    if (networkInfo != null && networkInfo.getState() != null && networkInfo.isConnected()) {
                        // the state of Wi-Fi connectivity has changed.
                        if (getCurrentConnectedAP() != null) {
                            Log.d(TAG, "WiFi BroadcastReceiver onReceive success in connecting to " + getCurrentConnectedAP().getSSID());
                        }
                        // the wifiInfo extra indicates that the new state is
                        // CONNECTED, and provides the SSID.
                        WifiInfo wifiInfo = intent.getParcelableExtra(WifiManager.EXTRA_WIFI_INFO);
                        if (wifiInfo == null) {
                            wifiInfo = wifi.getConnectionInfo();
                        }
                        if (wifiInfo != null) {
                            processChangeOfNetwork(wifiInfo);
                        }
                    } else {
                        Log.i(TAG, "WiFi BroadcastReceiver onReceive not a connected networkInfo: " + networkInfo);
                    }

                } else if (WifiManager.SUPPLICANT_STATE_CHANGED_ACTION.equals(intent.getAction()) && intent.hasExtra(WifiManager.EXTRA_SUPPLICANT_ERROR)
                        && intent.getIntExtra(WifiManager.EXTRA_SUPPLICANT_ERROR, 0) == WifiManager.ERROR_AUTHENTICATING) {
                    // Wi-Fi authentication error
                    synchronized (this) {
                        if (targetWifiConfiguration != null) {
                            Log.e(TAG, "Network Listener ERROR_AUTHENTICATING when trying to connect to " + normalizeSSID(targetWifiConfiguration.SSID));

                            // it was the SDK that initiated the Wi-Fi change,
                            // hence the timer should be cancelled
                            stopWifiTimeoutTimer();

                            Bundle extras = new Bundle();
                            extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, targetWifiConfiguration);
                            sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
                            targetWifiConfiguration=null;
                        }
                    }
                }

            }
        };
        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        context.registerReceiver(wifiBroadcastReceiver, filter);
    }


    /**
     * Handle a list of {@link ScanResult}.
     * Filter out empty or duplicate access
     * point names, and normalize access point names, then split the results
     * between the onboardableAPlist and the nonOnboardableAPlist lists. After
     * completion send {@link OnboardingManager#WIFI_SCAN_RESULTS_AVAILABLE_ACTION}
     * intent with onboardableAPlist,nonOnboardableAPlist as extra data.
     *
     * @param scans
     *            List of ScanResults
     */
    private void processScanResults(List<ScanResult> scans) {
        Log.d(TAG, "processScanResults");
        if (scans != null) {
            synchronized (wifi) {
                onboardableAPlist.clear();
                nonOnboardableAPlist.clear();
                allAPlist.clear();

                Set<String> tempset = new HashSet<String>(); // avoid duplicates
                StringBuffer buff = new StringBuffer(); // for logging
                for (ScanResult scan : scans) {

                    // remove extra quotes from network SSID
                    scan.SSID = normalizeSSID(scan.SSID);
                    // a scan may contain results with no SSID
                    if (scan.SSID == null || scan.SSID.isEmpty()) {
                        Log.i(TAG, "processScanResults currentScan was empty,skipping");
                        continue;
                    }
                    // avoid duplicates
                    if (tempset.contains(scan.SSID)) {
                        Log.i(TAG, "processScanResults currentScan " + scan.SSID + " already seen ,skipping");
                        continue;
                    }

                    // store the new SSID
                    tempset.add(scan.SSID);
                    WiFiNetwork wiFiNetwork = new WiFiNetwork(scan.SSID, scan.capabilities, scan.level);
                    if (scan.SSID.startsWith(ONBOARDABLE_PREFIX) || scan.SSID.endsWith(ONBOARDABLE_SUFFIX)) {
                        onboardableAPlist.add(wiFiNetwork);
                    } else {
                        nonOnboardableAPlist.add(wiFiNetwork);
                    }
                    allAPlist.add(wiFiNetwork);
                    buff.append(scan.SSID).append(",");
                }

                Log.i(TAG, "processScanResults " + (buff.length() > 0 ? buff.toString().substring(0, buff.length() - 1) : " empty"));

                // broadcast a WIFI_SCAN_RESULTS_AVAILABLE_ACTION intent
                Bundle extras = new Bundle();
                extras.putParcelableArrayList(OnboardingManager.EXTRA_ONBOARDEES_AP, onboardableAPlist);
                extras.putParcelableArrayList(OnboardingManager.EXTRA_TARGETS_AP, nonOnboardableAPlist);
                extras.putParcelableArrayList(OnboardingManager.EXTRA_ALL_AP, allAPlist);
                sendBroadcast(OnboardingManager.WIFI_SCAN_RESULTS_AVAILABLE_ACTION, extras);
            }
        }
    }


    /**
     * Handle the information that the device has connected to a Wi-Fi network.
     * It's important to know that {@link WifiManager} API doesn't fully
     * guarantee that the connected network is the one you asked for. So the new
     * network is compared with the one that we asked for.
     *
     * <P>
     * If the connection was successful the
     * {@link OnboardingManager#WIFI_CONNECTED_BY_REQUEST_ACTION} intent will be
     * sent
     *
     * @param wifiInfo
     *            WifiInfo of the access point to which we have just switched
     */
    private void processChangeOfNetwork(WifiInfo wifiInfo) {
        Log.d(TAG, "processChangeOfNetwork");

        synchronized (this) {

            // This is needed so that boards can discover the bundled AJ daemon.
            // On some devices, the multicast lock must be acquired on every
            // Wi-Fi network switch.
            acquireMulticastLock();

            if (targetWifiConfiguration != null) {
                // check if it's the network that we tried to connect to
                WiFiNetwork currentConnectedAP = getCurrentConnectedAP();
                if (((wifiInfo != null) && isSsidEquals(targetWifiConfiguration.SSID, wifiInfo.getSSID())) ||
                        (currentConnectedAP != null && isSsidEquals(targetWifiConfiguration.SSID, currentConnectedAP.getSSID()))) {
                    Bundle extras = new Bundle();
                    // it was the SDK that initiated the Wi-Fi change, hence the
                    // timer should be cancelled
                    stopWifiTimeoutTimer();

                    String intentAction = OnboardingManager.WIFI_CONNECTED_BY_REQUEST_ACTION;
                    extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, targetWifiConfiguration);
                    sendBroadcast(intentAction, extras);
                    targetWifiConfiguration = null;
                }
            }
        }
    }


    /**
     * Stops the wifiTimeoutTimer
     */
    private void stopWifiTimeoutTimer() {
        Log.d(TAG, "stopWifiTimeoutTimer");
        wifiTimeoutTimer.cancel();
        wifiTimeoutTimer.purge();
        wifiTimeoutTimer = new Timer();
    }


    /**
     * @return list of Wi-Fi access points whose SSID starts with "AJ_" or ends
     *         with "_AJ"
     */
    List<WiFiNetwork> getOnboardableAccessPoints() {
        synchronized (wifi) {
            return onboardableAPlist;
        }
    }


    /**
     * @return list of Wi-Fi access points whose SSID doesn't start with "AJ_"
     *         and doesn't end with "_AJ".
     */
    List<WiFiNetwork> getNonOnboardableAccessPoints() {
        synchronized (wifi) {
            return nonOnboardableAPlist;
        }
    }

    /**
     * @return list of all the access points found by the Wi-Fi scan.
     */
    List<WiFiNetwork> getAllAccessPoints() {
        synchronized (wifi) {
            return allAPlist;
        }
    }


    /**
     * @return current connected AP  if connected to Wi-Fi else returns null.
     */
    WiFiNetwork getCurrentConnectedAP() {
        Log.d(TAG, "getCurrentSSID: SupplicantState =  " + wifi.getConnectionInfo().getSupplicantState());
        synchronized (wifi) {

            // Check if we're still negotiating with the access point
            if (wifi.getConnectionInfo().getSupplicantState() != SupplicantState.COMPLETED) {
                Log.w(TAG, "getCurrentSSID: SupplicantState not COMPLETED, return null");
                return null;
            }
            if (wifi.getConnectionInfo() == null || wifi.getConnectionInfo().getSSID() == null) {
                return null;
            }

            String currentSSID = normalizeSSID(wifi.getConnectionInfo().getSSID());
            Log.d(TAG, "getCurrentSSID =" + currentSSID);

            for (int i = 0; i < onboardableAPlist.size(); i++) {
                if (onboardableAPlist.get(i).SSID.equals(currentSSID)) {
                    return onboardableAPlist.get(i);
                }
            }
            for (int i = 0; i < nonOnboardableAPlist.size(); i++) {
                if (nonOnboardableAPlist.get(i).SSID.equals(currentSSID)) {
                    return nonOnboardableAPlist.get(i);
                }
            }
            return new WiFiNetwork(currentSSID);
        }
    }


    /**
     * Extracts AuthType from a SSID by retrieving its capabilities via WifiManager.
     *
     * @param ssid
     * @return AuthType of SSID or null if not found
     */
    private AuthType getSSIDAuthType(String ssid) {
        Log.d(TAG, "getSSIDAuthType SSID = " + ssid);
        if (ssid == null || ssid.length() == 0) {
            Log.w(TAG, "getSSIDAuthType given string was null");
            return null;
        }
        List<ScanResult> networks = wifi.getScanResults();
        for (ScanResult scan : networks) {
            if (ssid.equalsIgnoreCase(scan.SSID)) {
                AuthType res = getScanResultAuthType(scan.capabilities);
                return res;
            }
        }
        return null;
    }


    /**
     * Parses ScanResult 'capabilities' string into an AuthType object.
     *
     * @param capabilities
     *            {@link ScanResult#capabilities}
     * @return AuthType object. {@link AuthType}
     */
    private AuthType getScanResultAuthType(String capabilities) {
        Log.d(TAG, "getScanResultAuthType capabilities = " + capabilities);

        if (capabilities.contains("WPA2")) {
            return AuthType.WPA2_AUTO;
        }
        if (capabilities.contains("WPA")) {
            return AuthType.WPA_AUTO;
        }
        if (capabilities.contains(AuthType.WEP.name())) {
            return AuthType.WEP;
        }
        return AuthType.OPEN;
    }

    /**
     * Use {@link WifiManager} to connect to a Wi-Fi access point.
     * <ul>
     *  <li> In case an access point with the same SSID exists, delete it.
     *  <li> Create a new access point with SSID name ,
     *  <li> Verify that it is a valid one .
     *  <li> Call {@link #connect(WifiConfiguration, int, long)}.
     *</ul>
     *
     * @param ssid Wi-Fi access point
     * @param authType
     * @param password
     * @param connectionTimeout in milliseconds
     * @param isHidden
     *
     */
    void connectToWifiAP(String ssid, AuthType authType, String password, boolean isHidden,long connectionTimeout) {
        //21 = android.os.Build.VERSION_CODES.LOLLIPOP
        if (android.os.Build.VERSION.SDK_INT >= 21) {
            lollipop_connectToWifiAP(ssid, authType, password, isHidden, connectionTimeout);
            return;
        }
        Log.d(TAG, "connectToWifiAP SSID = " + ssid + " authtype = " + authType.toString()+ " is hidden = "+ isHidden);

        // if networkPass is null set it to ""
        if (password == null) {
            password = "";
        }

        // the configured Wi-Fi networks
        final List<WifiConfiguration> wifiConfigs = wifi.getConfiguredNetworks();

        // log the list
        StringBuffer buff = new StringBuffer();
        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null) {
                w.SSID = normalizeSSID(w.SSID);
                if (w.SSID.length() > 1) {
                    buff.append(w.SSID).append(",");
                }
            }
        }
        Log.i(TAG, "connectToWifiAP ConfiguredNetworks " + (buff.length() > 0 ? buff.toString().substring(0, buff.length() - 1) : " empty"));

        int networkId = -1;

        // delete any existing WifiConfiguration that has the same SSID as the
        // new one
        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null && isSsidEquals(w.SSID, ssid)) {
                networkId = w.networkId;
                Log.i(TAG, "connectToWifiAP found " + ssid + " in ConfiguredNetworks. networkId = " + networkId);
                boolean res = wifi.removeNetwork(networkId);
                Log.i(TAG, "connectToWifiAP delete  " + networkId + "? " + res);
                res = wifi.saveConfiguration();
                Log.i(TAG, "connectToWifiAP saveConfiguration  res = " + res);
                break;
            }
        }

        WifiConfiguration wifiConfiguration = new WifiConfiguration();

        // check the AuthType of the SSID against the WifiManager
        // if null use the one given by the API
        // else use the result from getSSIDAuthType
        AuthType verrifiedWifiAuthType = getSSIDAuthType(ssid);
        if (verrifiedWifiAuthType != null) {
            authType = verrifiedWifiAuthType;
        }

        if (isHidden){
            wifiConfiguration.hiddenSSID=true;
        }

        Log.i(TAG, "connectToWifiAP selectedAuthType = " + authType);

        // set the WifiConfiguration parameters
        switch (authType) {
        case OPEN:
            wifiConfiguration.SSID = "\"" + ssid + "\"";
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            networkId = wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "connectToWifiAP [OPEN] add Network returned " + networkId);
            break;

        case WEP:
            wifiConfiguration.SSID = "\"" + ssid + "\"";

            // check the validity of a WEP password
            Pair<Boolean, Boolean> wepCheckResult = checkWEPPassword(password);
            if (!wepCheckResult.first) {
                Log.i(TAG, "connectToWifiAP  auth type = WEP: password " + password + " invalid length or charecters");
                Bundle extras = new Bundle();
                extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfiguration);
                sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
                return;
            }
            Log.i(TAG, "connectToWifiAP [WEP] using " + (!wepCheckResult.second ? "ASCII" : "HEX"));
            if (!wepCheckResult.second) {
                wifiConfiguration.wepKeys[0] = "\"" + password + "\"";
            } else {
                wifiConfiguration.wepKeys[0] = password;
            }
            wifiConfiguration.priority = 40;
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            wifiConfiguration.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
            wifiConfiguration.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP104);
            wifiConfiguration.wepTxKeyIndex = 0;
            networkId = wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "connectToWifiAP [WEP] add Network returned " + networkId);
            break;

        case WPA_AUTO:
        case WPA_CCMP:
        case WPA_TKIP:
        case WPA2_AUTO:
        case WPA2_CCMP:
        case WPA2_TKIP: {
            wifiConfiguration.SSID = "\"" + ssid + "\"";
            // handle special case when WPA/WPA2 and 64 length password that can
            // be HEX
            if (password.length() == 64 && password.matches(WEP_HEX_PATTERN)) {
                wifiConfiguration.preSharedKey = password;
            } else {
                wifiConfiguration.preSharedKey = "\"" + password + "\"";
            }
            wifiConfiguration.hiddenSSID = true;
            wifiConfiguration.status = WifiConfiguration.Status.ENABLED;
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            networkId = wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "connectToWifiAP  [WPA..WPA2] add Network returned " + networkId);
            break;
        }
        default:
            networkId = -1;
            break;
        }
        if (networkId < 0) {
            Log.d(TAG, "connectToWifiAP networkId <0  WIFI_AUTHENTICATION_ERROR");
            Bundle extras = new Bundle();
            extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfiguration);
            sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
            return;
        }
        Log.d(TAG, "connectToWifiAP calling connect");
        connect(wifiConfiguration, networkId, connectionTimeout);
    }


    /**
     * Connect to an already configured Wi-Fi access point.
     * @param SSID
     * @param timeout
     * @throws OnboardingIllegalArgumentException in case SSID not found in configured access points.
     */
    void connectToWifiBySSID(String SSID,long timeout) throws OnboardingIllegalArgumentException{

        final List<WifiConfiguration> wifiConfigs = wifi.getConfiguredNetworks();

        WifiConfiguration wifiConfig=null;

        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null && isSsidEquals(w.SSID, SSID)) {
                wifiConfig=w;
                break;
            }
        }
        if (wifiConfig!=null){
            // 21 = android.os.Build.VERSION_CODES.LOLLIPOP
            if (android.os.Build.VERSION.SDK_INT >= 21) {
                lillipop_connect(wifiConfig,wifiConfig.networkId,timeout);
            } else {
                connect(wifiConfig,wifiConfig.networkId,timeout);
            }
        }else{
            throw new OnboardingIllegalArgumentException("unable to find "+ SSID +" in list of configured networks");
        }
    }


    /**
     * Make the actual connection to the requested Wi-Fi target.
     *
     * @param wifiConfig
     *            details of the Wi-Fi access point used by the WifiManger
     * @param networkId
     *            id of the Wi-Fi configuration
     * @param timeoutMsec
     *            period of time in Msec to complete Wi-Fi connection task
     */
    private void connect(final WifiConfiguration wifiConfig, final int networkId, final long timeoutMsec) {
        Log.i(TAG, "connect  SSID=" + wifiConfig.SSID + " within " + timeoutMsec);
        boolean res;


        synchronized (this) {
            targetWifiConfiguration = wifiConfig;
        }

        // this is the application's Wi-Fi connection timeout
        wifiTimeoutTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.e(TAG, "Network Listener WIFI_TIMEOUT  when trying to connect to " + normalizeSSID(targetWifiConfiguration.SSID));
                Bundle extras = new Bundle();
                extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfig);
                sendBroadcast(OnboardingManager.WIFI_TIMEOUT_ACTION, extras);
            }
        }, timeoutMsec);

        if (wifi.getConnectionInfo().getSupplicantState() == SupplicantState.DISCONNECTED) {
            wifi.disconnect();
        }

        wifi.saveConfiguration();

        res = wifi.enableNetwork(networkId, false);
        Log.d(TAG, "connect enableNetwork [false] status=" + res);
        res = wifi.disconnect();
        Log.d(TAG, "connect disconnect  status=" + res);

        // enabling a network doesn't guarantee that it's the one that Android
        // will connect to.
        // Selecting a particular network is achieved by passing 'true' here to
        // disable all other networks.
        // the side effect is that all other user's Wi-Fi networks become
        // disabled.
        // The recovery for that is enableAllWifiNetworks method.
        res = wifi.enableNetwork(networkId, true);
        Log.d(TAG, "connect enableNetwork [true] status=" + res);
        res = wifi.reconnect();
        wifi.setWifiEnabled(true);
    }


    /**
     * Remove the WiFi access point using {@link WifiManager} Api.
     *
     * @param ssid name of the Wi-Fi network to remove
     */
    void removeWifiAP(String ssid) {
        final List<WifiConfiguration> wifiConfigs = wifi.getConfiguredNetworks();
        // disconnect from Wi-Fi
        wifi.disconnect();
        // delete WifiConfiguration that has the same SSID as the supplied name.
        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null && isSsidEquals(w.SSID, ssid)) {
                int networkId = w.networkId;
                Log.i(TAG, "connectToWifiAP found " + ssid + " in ConfiguredNetworks. networkId = " + networkId);
                boolean res = wifi.removeNetwork(networkId);
                Log.i(TAG, "connectToWifiAP delete  " + networkId + "? " + res);
                res = wifi.saveConfiguration();
                Log.i(TAG, "connectToWifiAP saveConfiguration  res = " + res);
                break;
            }
        }
    }


    /**
     * Enable all the disabled Wi-Fi access points. This method tries to restore
     * the Wi-Fi environment that the user had prior to onboarding. Because
     * While running the onboarding process, the SDK disables all the user's
     * Wi-Fi networks other than the onboardee access point and the target.
     */
    void enableAllWifiNetworks() {
        Log.d(TAG, "enableAllWifiNetworks start");
        List<WifiConfiguration> configuredNetworks = wifi.getConfiguredNetworks();
        for (WifiConfiguration wifiConfiguration : configuredNetworks) {
            if (wifiConfiguration.status == WifiConfiguration.Status.DISABLED) {
                Log.d(TAG, "enableAllWifiNetworks enabling " + wifiConfiguration.SSID);
                wifi.enableNetwork(wifiConfiguration.networkId, false);
            }
        }
        wifi.reconnect();
    }


    /**
     * Start a Wi-Fi scan.
     */
    void scan() {
        Log.d(TAG, "scan");
        wifi.startScan();
    }


    /**
     * Acquire Android Multicast lock.
     * AllJoyn daemon discovery relies on multicast ,and Android, by default, filters out multicast packets unless
     * an application acquires a lock.
     */
    private void acquireMulticastLock() {
        Log.d(TAG, "acquireMulticastLock");

        // On some devices the multicast lock must be renewed every time,
        // because after Wi-Fi switches,
        // the lock is still held, but no longer works - multicast packets no
        // longer come through.
        if (multicastLock != null) {
            // keep the environment clean. Dispose the old lock
            Log.d(TAG, "MulticastLock != null. releasing MulticastLock");
            multicastLock.release();
        }

        multicastLock = wifi.createMulticastLock("multicastLock");
        multicastLock.setReferenceCounted(false);
        multicastLock.acquire();
    }




    /**
     * A wrapper function to send broadcast intent messages.
     *
     * @param action
     *            the name of the action
     * @param extras
     *            contains extra information bundled with the intent
     */
    private void sendBroadcast(String action, Bundle extras) {
        Intent intent = new Intent(action);
        if (extras != null && !extras.isEmpty()) {
            intent.putExtras(extras);
        }
        context.sendBroadcast(intent);
    }


    /**
     * A utility method for retrieving Android's Wi-Fi status.
     * @return  true if enabled else false
     */
    boolean isWifiEnabled(){
        return wifi.isWifiEnabled();
    }


    /**
     * A utility method for comparing two SSIDs. Some Android devices return an
     * SSID surrounded with quotes. For the sake of comparison and readability,
     * remove those.
     *
     * @param ssid1
     * @param ssid2
     * @return true if equals else false
     */
    static boolean isSsidEquals(String ssid1, String ssid2) {
        if (ssid1 == null || ssid1.length() == 0 || ssid2 == null || ssid2.length() == 0) {
            return false;
        }
        return normalizeSSID(ssid1).equals(normalizeSSID(ssid2));
    }


    /**
     * A utility method for removing wrapping quotes from SSID name. Some
     * Android devices return an SSID surrounded with quotes. For the sake of
     * comparison and readability, remove those.
     *
     * @param ssid
     *            could be AJ_QA but also "AJ_QA" (with quotes).
     * @return normalized SSID: AJ_QA
     */
    static String normalizeSSID(String ssid) {
        if (ssid != null && ssid.length() > 2 && ssid.startsWith("\"") && ssid.endsWith("\"")) {
            ssid = ssid.substring(1, ssid.length() -1);
        }
        return ssid;
    }


    /**
     * A utility method that checks if a password complies with WEP password
     * rules, and if it's in HEX format.
     *
     * @param password
     * @return {@link Pair} of two {@link Boolean} is it a valid WEP password
     *         and is it a HEX password.
     */
    static Pair<Boolean, Boolean> checkWEPPassword(String password) {
        Log.d(TAG, "checkWEPPassword");

        if (password == null || password.isEmpty()) {
            Log.w(TAG, "checkWEPPassword empty password");
            return new Pair<Boolean, Boolean>(false, false);
        }

        int length = password.length();
        switch (length) {
        // valid ASCII keys length
        case 5:
        case 13:
        case 16:
        case 29:
            Log.d(TAG, "checkWEPPassword valid WEP ASCII password");
            return new Pair<Boolean, Boolean>(true, false);
            // valid hex keys length
        case 10:
        case 26:
        case 32:
        case 58:
            if (password.matches(WEP_HEX_PATTERN)) {
                Log.d(TAG, "checkWEPPassword valid WEP password length, and HEX pattern match");
                return new Pair<Boolean, Boolean>(true, true);
            }
            Log.w(TAG, "checkWEPPassword valid WEP password length, but HEX pattern matching failed: " + WEP_HEX_PATTERN);
            return new Pair<Boolean, Boolean>(false, false);
        default:
            Log.w(TAG, "checkWEPPassword invalid WEP password length: " + length);
            return new Pair<Boolean, Boolean>(false, false);
        }
    }

    //////////////////////////////////////////////////////////////////////
    //NOTE: The below code was done to allow onboarding to function on
    //       Android Lollipop.  The change has been minimally tested for
    //       other Android platforms and although it functions, previous
    //       solution has been stress tested on multiple platforms and
    //       validated avoid complications the code has been explicitly
    //       split in order to avoid regression testing.
    //////////////////////////////////////////////////////////////////////
    /**
     * Look through the supplicant and find a configuration that matches
     * the supplied ssid if one exists.
     *
     * @param ssid
     *            name of the Wireless SSID that is to be found
     * @return WiFiConfiguration for supplied ssid if found, else null
     */
    private WifiConfiguration findConfiguration(String ssid) {
        // the configured Wi-Fi networks
        final List<WifiConfiguration> wifiConfigs = wifi.getConfiguredNetworks();

        // for debugging purposes only log the list
        StringBuffer buff = new StringBuffer();
        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null) {
                w.SSID = normalizeSSID(w.SSID);
                if (w.SSID.length() > 1) {
                    buff.append(w.SSID).append(",");
                }
            }
        }
        Log.i(TAG, "connectToWifiAP ConfiguredNetworks " + (buff.length() > 0 ? buff.toString().substring(0, buff.length() - 1) : " empty"));

        // find any existing WifiConfiguration that has the same SSID as the
        // supplied one and return it if found
        for (WifiConfiguration w : wifiConfigs) {
            if (w.SSID != null && isSsidEquals(w.SSID, ssid)) {
                Log.i(TAG, "connectToWifiAP found " + ssid + " in ConfiguredNetworks. networkId = " + w.networkId);
                return w;
            }
        }

        return null;
    }


    /**
     * Use {@link WifiManager} to connect to a Wi-Fi access point.
     * <ul>
     *  <li> In case an access point with the same SSID exists, delete it.
     *  <li> Create a new access point with SSID name ,
     *  <li> Verify that it is a valid one .
     *  <li> Call {@link #connect(WifiConfiguration, int, long)}.
     *</ul>
     *
     * @param ssid Wi-Fi access point
     * @param authType
     * @param password
     * @param connectionTimeout in milliseconds
     * @param isHidden
     *
     */
    private void lollipop_connectToWifiAP(String ssid, AuthType authType, String password, boolean isHidden,long connectionTimeout) {
        Log.d(TAG, "lollipop_connectToWifiAP SSID = " + ssid + " authtype = " + authType.toString()+ " is hidden = "+ isHidden);

        // if networkPass is null set it to ""
        if (password == null) {
            password = "";
        }

        int networkId = -1;
        boolean shouldUpdate = false;

        WifiConfiguration wifiConfiguration = findConfiguration(ssid);

        if(wifiConfiguration == null) {
            wifiConfiguration = new WifiConfiguration();
        } else {
            shouldUpdate = true;
        }

        // check the AuthType of the SSID against the WifiManager
        // if null use the one given by the API
        // else use the result from getSSIDAuthType
        AuthType verrifiedWifiAuthType = getSSIDAuthType(ssid);
        if (verrifiedWifiAuthType != null) {
            authType = verrifiedWifiAuthType;
        }

        if (isHidden){
            wifiConfiguration.hiddenSSID=true;
        }

        Log.i(TAG, "lollipop_connectToWifiAP selectedAuthType = " + authType);

        // set the priority to something high so that the network we are entering should be used
        wifiConfiguration.priority = 140;

        // set the WifiConfiguration parameters
        switch (authType) {
        case OPEN:
            wifiConfiguration.SSID = "\"" + ssid + "\"";
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            networkId = shouldUpdate ? wifi.updateNetwork(wifiConfiguration) : wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "lollipop_connectToWifiAP [OPEN] add Network returned " + networkId);
            break;

        case WEP:
            wifiConfiguration.SSID = "\"" + ssid + "\"";

            // check the validity of a WEP password
            Pair<Boolean, Boolean> wepCheckResult = checkWEPPassword(password);
            if (!wepCheckResult.first) {
                Log.i(TAG, "lollipop_connectToWifiAP  auth type = WEP: password " + password + " invalid length or charecters");
                Bundle extras = new Bundle();
                extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfiguration);
                sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
                return;
            }
            Log.i(TAG, "lollipop_connectToWifiAP [WEP] using " + (!wepCheckResult.second ? "ASCII" : "HEX"));
            if (!wepCheckResult.second) {
                wifiConfiguration.wepKeys[0] = "\"" + password + "\"";
            } else {
                wifiConfiguration.wepKeys[0] = password;
            }
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            wifiConfiguration.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
            wifiConfiguration.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP104);
            wifiConfiguration.wepTxKeyIndex = 0;
            networkId = shouldUpdate ? wifi.updateNetwork(wifiConfiguration) : wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "lollipop_connectToWifiAP [WEP] add Network returned " + networkId);
            break;

        case WPA_AUTO:
        case WPA_CCMP:
        case WPA_TKIP:
        case WPA2_AUTO:
        case WPA2_CCMP:
        case WPA2_TKIP: {
            wifiConfiguration.SSID = "\"" + ssid + "\"";
            // handle special case when WPA/WPA2 and 64 length password that can
            // be HEX
            if (password.length() == 64 && password.matches(WEP_HEX_PATTERN)) {
                wifiConfiguration.preSharedKey = password;
            } else {
                wifiConfiguration.preSharedKey = "\"" + password + "\"";
            }
            wifiConfiguration.status = WifiConfiguration.Status.ENABLED;
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            networkId = shouldUpdate ? wifi.updateNetwork(wifiConfiguration) : wifi.addNetwork(wifiConfiguration);
            Log.d(TAG, "lollipop_connectToWifiAP  [WPA..WPA2] add Network returned " + networkId);
            break;
        }
        default:
            networkId = -1;
            break;
        }
        if (networkId < 0) {
            Log.d(TAG, "lollipop_connectToWifiAP networkId <0  WIFI_AUTHENTICATION_ERROR");
            Bundle extras = new Bundle();
            extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfiguration);
            sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
            return;
        }
        Log.d(TAG, "lollipop_connectToWifiAP calling connect");

        //We will now save the configuration and then look back up the networkId
        //saveConfiguration may cause networkId to change
        boolean res = wifi.saveConfiguration();
        Log.d(TAG, "lollipop_connectToWifiAP saveConfiguration status=" + res);
        wifiConfiguration = findConfiguration(ssid);
        if(wifiConfiguration == null) {
            Log.d(TAG, "lollipop_connectToWifiAP Could not find configuration after adding");
            Bundle extras = new Bundle();
            extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfiguration);
            sendBroadcast(OnboardingManager.WIFI_AUTHENTICATION_ERROR, extras);
            return;
        }

        lillipop_connect(wifiConfiguration, wifiConfiguration.networkId, connectionTimeout);
    }


    /**
     * Make the actual connection to the requested Wi-Fi target.
     *
     * @param wifiConfig
     *            details of the Wi-Fi access point used by the WifiManger
     * @param networkId
     *            id of the Wi-Fi configuration
     * @param timeoutMsec
     *            period of time in Msec to complete Wi-Fi connection task
     */
    private void lillipop_connect(final WifiConfiguration wifiConfig, final int networkId, final long timeoutMsec) {
        Log.i(TAG, "lillipop_connect  SSID=" + wifiConfig.SSID + " within " + timeoutMsec);
        boolean res;


        synchronized (this) {
            targetWifiConfiguration = wifiConfig;
        }

        // this is the application's Wi-Fi connection timeout
        wifiTimeoutTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.e(TAG, "lillipop_connect Network Listener WIFI_TIMEOUT  when trying to connect to " + normalizeSSID(targetWifiConfiguration.SSID));
                Bundle extras = new Bundle();
                extras.putParcelable(OnboardingManager.EXTRA_WIFI_WIFICONFIGURATION, wifiConfig);
                sendBroadcast(OnboardingManager.WIFI_TIMEOUT_ACTION, extras);
            }
        }, timeoutMsec);

        res = wifi.disconnect();
        Log.d(TAG, "lillipop_connect disconnect  status=" + res);

        if ( !wifi.isWifiEnabled() ) {
            wifi.setWifiEnabled(true);
        }

        // enabling a network doesn't guarantee that it's the one that Android
        // will connect to.
        // Selecting a particular network is achieved by passing 'true' here to
        // disable all other networks.
        // the side effect is that all other user's Wi-Fi networks become
        // disabled.
        // The recovery for that is enableAllWifiNetworks method.
        res = wifi.enableNetwork(networkId, true);
        Log.d(TAG, "lillipop_connect enableNetwork [true] status=" + res);
        // Wait a few for the WiFi to do something and try again just in case
        // Android has decided that the network we configured is not "good enough"
        try{ Thread.sleep(500); } catch(Exception e) {}
        res = wifi.enableNetwork(networkId, true);
        Log.d(TAG, "lillipop_connect enableNetwork [true] status=" + res);
    }
}
