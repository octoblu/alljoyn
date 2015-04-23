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

import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObj;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.onboarding.OnboardingService.AuthType;
import org.alljoyn.onboarding.client.OnboardingClient;
import org.alljoyn.onboarding.client.OnboardingClientImpl;
import org.alljoyn.onboarding.sdk.OnboardingManager.DeviceResponse.ResponseCode;
import org.alljoyn.onboarding.transport.ConnectionResult;
import org.alljoyn.onboarding.transport.ConnectionResultListener;
import org.alljoyn.onboarding.transport.OnboardingTransport;
import org.alljoyn.onboarding.transport.OnboardingTransport.ConfigureWifiMode;
import org.alljoyn.services.common.ClientBase;
import org.alljoyn.services.common.ServiceAvailabilityListener;
import org.alljoyn.services.common.utils.TransportUtil;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.util.Pair;

/**
 * Streamlines the process of onboarding for Android application developers.<br>
 * The SDK encapsulates the Wi-Fi and AllJoyn sessions that are part of the
 * process.<br>
 * The Onboarding SDK API provides the following :
 * 
 * <ul>
 * <li>Discovery of potential target networks,Discovery of potential
 * onboardees,by calling {@link #scanWiFi()}
 * <li>A single call, {@link #runOnboarding(OnboardingConfiguration)}, for start
 * the flow of onboarding process
 * <li>A single call ,{@link #runOffboarding(OffboardingConfiguration)}, for
 * starting the flow of offboarding process
 * </ul>
 * 
 * <P>
 * The onboarding flow works as follows:
 * <P>
 * The SDK uses a state machine handled by Andorid's handler
 * {@link #stateHandler} using the {@link #onHandleCommandMessage} function to
 * handle state changes.
 * <ul>
 * <li>IDLE state moves to CONNECTING_TO_ONBOARDEE when starting the onboarding
 * process
 * <li>CONNECTING_TO_ONBOARDEE state moves to WAITING_FOR_ONBOARDEE_ANNOUNCEMENT
 * after Wi-Fi connection has been established with onboardee device.
 * <li>WAITING_FOR_ONBOARDEE_ANNOUNCEMENT state moves to state
 * ONBOARDEE_ANNOUNCEMENT_RECEIVED after a valid announce message has been
 * received.
 * <li>ONBOARDEE_ANNOUNCEMENT_RECEIVED state moves to CONFIGURING_ONBOARDEE if
 * the onboardee supports onboarding service.
 * <li>CONFIGURING_ONBOARDEE state moves to CONNECTING_TO_TARGET_WIFI_AP if
 * passing the target credentials to the onboardee was successful.
 * <li>CONFIGURING_ONBOARDEE_WAITING_FOR_SIGNAL_TIMEOUT state moves to
 * CONNECTING_TO_TARGET_WIFI_AP if passing the target credentials to the
 * onboardee was successful.
 * <li>CONNECTING_TO_TARGET_WIFI_AP moves to WAITING_FOR_TARGET_ANNOUNCE after
 * Wi-Fi connection has been established with target .
 * <li>WAITING_FOR_TARGET_ANNOUNCE moves to TARGET_ANNOUNCEMENT_RECEIVED after
 * after a valid announce message has been received.
 * </ul>
 * 
 * <P>
 * The SDK receives events from external resources
 * <ul>
 * <li>OnboardingSDKWifiManager {@link #onboardingClient} that broadcasts Wi-Fi
 * success and failure to connect to a desired Wi-Fi device.
 * <li>onAnnouncement callback which handles AllJoyn AboutService announcements.
 * </ul>
 * 
 * <P>
 * The SDK uses intents to report status changes and errors during
 * onboarding/offboarding process.
 * <ul>
 * <li> {@link #STATE_CHANGE_ACTION} action with extra
 * <ul>
 * <li>{@link #EXTRA_ONBOARDING_STATE} extra information of enum
 * {@link OnboardingState}
 * </ul>
 * <li>{@link #ERROR} action with extra
 * <ul>
 * <li>{@link #EXTRA_ERROR_DETAILS} extra information of enum
 * {@link OnboardingErrorType}
 * </ul>
 * </ul>
 * 
 * <P>
 * View sample code for SDK usage
 */

public class OnboardingManager implements AboutListener {

    /**
     * Activity Action:WIFI has been connected
     */
    static final String WIFI_CONNECTED_BY_REQUEST_ACTION = "org.alljoyn.onboardingsdk.wifi.connection_by_request";

    /**
     * Activity Action:WIFI connection has timed out
     */
    static final String WIFI_TIMEOUT_ACTION = "org.alljoyn.onboardingsdk.wifi.time_out";

    /**
     * Activity Action:WIFI authentication has occurred
     */
    static final String WIFI_AUTHENTICATION_ERROR = "org.alljoyn.onboardingsdk.wifi.authentication_error";

    /**
     * The lookup key for WifiConfiguration details after connection request.
     */
    static final String EXTRA_WIFI_WIFICONFIGURATION = "org.alljoyn.intent_keys.WifiConfiguration";

    /**
     * The lookup key for list of onboardee access points
     */
    public static final String EXTRA_ONBOARDEES_AP = "org.alljoyn.onboardingsdk.intent_keys.onboardeesAP";

    /**
     * The lookup key for list of target access points
     */
    public static final String EXTRA_TARGETS_AP = "org.alljoyn.onboardingsdk.intent_keys.targetsAP";

    /**
     * The lookup key for list of all access points
     */
    public static final String EXTRA_ALL_AP = "org.alljoyn.onboardingsdk.intent_keys.allAP";

    /**
     * The lookup key for Onboarding state reported by the SDK
     */
    public static final String EXTRA_ONBOARDING_STATE = "org.alljoyn.onboardingsdk.intent_keys.onboardingState";

    /**
     * The lookup key for ERROR details reported by the SDK
     */
    public static final String EXTRA_ERROR_DETAILS = "org.alljoyn.onboardingsdk.intent_keys.error";

    /**
     * The lookup key for EXTRA_DEVICE_BUS_NAME reported by the SDK ,used to
     * report device service name during call
     * {@link #offboardDevice(String, short)}
     */
    public static final String EXTRA_DEVICE_BUS_NAME = "org.alljoyn.onboardingsdk.intent_keys.device_bus_name";

    /**
     * The lookup key for EXTRA_DEVICE_ONBOARDEE_SSID reported by the SDK after
     * successful onboarding process ,reports the onboardee ssid name
     */
    public static final String EXTRA_DEVICE_ONBOARDEE_SSID = "org.alljoyn.onboardingsdk.intent_keys.device_onboardee_ssid";

    /**
     * The lookup key for EXTRA_DEVICE_TARGET_SSID reported by the SDK after
     * successful onboarding process ,reports the target ssid name
     */
    public static final String EXTRA_DEVICE_TARGET_SSID = "org.alljoyn.onboardingsdk.intent_keys.device_target_ssid";

    /**
     * The lookup key for EXTRA_DEVICE_APPID reported by the SDK after
     * successful onboarding process ,reports the application id
     */
    public static final String EXTRA_DEVICE_APPID = "org.alljoyn.onboardingsdk.intent_keys.device_appid";

    /**
     * The lookup key for EXTRA_DEVICE_DEVICEID reported by the SDK after
     * successful onboarding process ,reports the device id
     */
    public static final String EXTRA_DEVICE_DEVICEID = "org.alljoyn.onboardingsdk.intent_keys.device_deviceid";

    /**
     * Activity Action: indicates that the WIFI scan has been completed
     */
    public static final String WIFI_SCAN_RESULTS_AVAILABLE_ACTION = "org.alljoyn.onboardingsdk.scan_result_available";

    /**
     * Activity Action: indicates state changes in the SDK
     */
    public static final String STATE_CHANGE_ACTION = "org.alljoyn.onboardingsdk.state_change";

    /**
     * Activity Action: indicates error encountered by the SDK
     */
    public static final String ERROR = "org.alljoyn.onboardingsdk.error";

    /**
     * These enumeration values are used to indicate possible errors
     * 
     * see also {@link #EXTRA_ERROR_DETAILS}
     */
    public static enum OnboardingErrorType {

        /**
         * Wi-Fi is disabled.
         */
        WIFI_DISABLED(0),

        /**
         * Onboardee Wi-Fi authentication error.
         */
        ONBOARDEE_WIFI_AUTH(10),

        /**
         * Target Wi-Fi authentication error.
         */
        TARGET_WIFI_AUTH(11),

        /**
         * Onboardee Wi-Fi connection timeout.
         */
        ONBOARDEE_WIFI_TIMEOUT(12),

        /**
         * Target Wi-Fi connection timeout.
         */
        TARGET_WIFI_TIMEOUT(13),

        /**
         * Timeout while searching for onboardee.
         */
        FIND_ONBOARDEE_TIMEOUT(14),

        /**
         * Error while establishing AllJoyn session.
         */
        JOIN_SESSION_ERROR(15),

        /**
         * Timeout while establishing AllJoyn session.
         */
        JOIN_SESSION_TIMEOUT(16),

        /**
         * Error while configuring onboardee with target credentials.
         */
        ERROR_CONFIGURING_ONBOARDEE(17),

        /**
         * Timeout while waiting for signal in two state configuring.
         */
        CONFIGURING_ONBOARDEE_WAITING_FOR_SIGNAL_TIMEOUT(18),

        /**
         * Timeout while waiting to receive announcement from onboardee on
         * target network.
         */
        VERIFICATION_TIMEOUT(19),

        /**
         * Failed ot offboard a device from target.
         */
        OFFBOARDING_FAILED(20),

        /**
         * Announce data is inavalid.
         */
        INVALID_ANNOUNCE_DATA(21),

        /**
         * Wi-Fi connection timeout
         * {@link OnboardingManager#connectToNetwork(WiFiNetworkConfiguration, long)}
         */
        OTHER_WIFI_TIMEOUT(30),

        /**
         * Wi-Fi authentication error
         * {@link OnboardingManager#connectToNetwork(WiFiNetworkConfiguration, long)}
         */
        OTHER_WIFI_AUTH(31),

        /**
         * Wi-Fi connection timeout {@link OnboardingManager#abortOnboarding()}
         */
        ORIGINAL_WIFI_TIMEOUT(32),

        /**
         * Wi-Fi authentication error
         * {@link OnboardingManager#abortOnboarding()}
         */
        ORIGINAL_WIFI_AUTH(33),

        /**
         * SDK internal error
         */
        INTERNAL_ERROR(40);

        private int value;

        private OnboardingErrorType(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

        public static OnboardingErrorType getOnboardingErrorTypeByValue(int value) {
            OnboardingErrorType retType = null;
            for (OnboardingErrorType type : OnboardingErrorType.values()) {
                if (value == type.getValue()) {
                    retType = type;
                    break;
                }
            }
            return retType;
        }

        public static OnboardingErrorType getOnboardingErrorTypeByString(String str) {
            OnboardingErrorType retType = null;
            for (OnboardingErrorType type : OnboardingErrorType.values()) {
                if (type.name().equals(str)) {
                    retType = type;
                }
            }
            return retType;
        }

        @Override
        public String toString() {
            return this.name();
        }

    };

    /**
     * These enumeration values are used to indicate the current onboarding
     * state
     * 
     * see also {@link #EXTRA_ONBOARDING_STATE} {@link #STATE_CHANGE_ACTION}
     */
    public static enum OnboardingState {

        /**
         * Connecting to onboardee Wi-Fi
         */
        CONNECTING_ONBOARDEE_WIFI(0),

        /**
         * Connected to onboardee Wi-Fi
         */
        CONNECTED_ONBOARDEE_WIFI(1),

        /**
         * Waiting for announcement from onboardee
         */
        FINDING_ONBOARDEE(2),

        /**
         * Announcement received from onboardee
         */
        FOUND_ONBOARDEE(3),

        /**
         * Creating AllJoyn session with onboardee
         */
        JOINING_SESSION(4),

        /**
         * AllJoyn session established with onboardee
         */
        SESSION_JOINED(5),

        /**
         * Sending target credentials to onboardee
         */
        CONFIGURING_ONBOARDEE(6),

        /**
         * Waiting for signal from onboardee with two stage configuring
         */
        CONFIGURING_ONBOARDEE_WITH_SIGNAL(7),

        /**
         * Onboardee received target credentials
         */
        CONFIGURED_ONBOARDEE(8),

        /**
         * Connecting to WIFI target
         */
        CONNECTING_TARGET_WIFI(9),

        /**
         * Wi-Fi connection with target established
         */
        CONNECTED_TARGET_WIFI(10),

        /**
         * Wait for announcement from onboardee over target WIFI
         */
        VERIFYING_ONBOARDED(11),

        /**
         * Announcement from onboardee over target WIFI has been received
         */
        VERIFIED_ONBOARDED(12),

        /**
         * Connecting to the original network before calling
         * {@link OnboardingManager#runOnboarding(OnboardingConfiguration)}.
         */
        CONNECTING_ORIGINAL_WIFI(13),

        /**
         * Connected to the original network before calling
         * {@link OnboardingManager#runOnboarding(OnboardingConfiguration)}.
         */
        CONNECTED_ORIGINAL_WIFI(14),

        /**
         * Connecting to the selected network
         * {@link OnboardingManager#connectToNetwork(WiFiNetworkConfiguration, long)}
         * .
         */
        CONNECTING_OTHER_WIFI(15),

        /**
         * Connected to the selected network
         * {@link OnboardingManager#connectToNetwork(WiFiNetworkConfiguration, long)}
         * .
         */
        CONNECTED_OTHER_WIFI(16),

        /**
         * Aborting has been started.
         */
        ABORTING(20),

        /**
         * Aborting has been completed.
         */
        ABORTED(21);

        private int value;

        private OnboardingState(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

        public static OnboardingState getOnboardingStateByValue(int value) {
            OnboardingState retType = null;
            for (OnboardingState type : OnboardingState.values()) {
                if (value == type.getValue()) {
                    retType = type;
                    break;
                }
            }
            return retType;
        }

        public static OnboardingState getOnboardingStateByString(String str) {
            OnboardingState retType = null;
            for (OnboardingState type : OnboardingState.values()) {
                if (type.name().equals(str)) {
                    retType = type;
                    break;
                }
            }
            return retType;
        }

        @Override
        public String toString() {
            return this.name();
        }
    }

    /**
     * These enumeration values are used to filter Wi-Fi scan result
     * {@link OnboardingManager#scanWiFi()}
     */
    public static enum WifiFilter {

        /**
         * Wi-Fi access point name that contains the following prefix
         * {@link OnboardingSDKWifiManager#ONBOARDABLE_PREFIX} or suffix
         * {@link OnboardingSDKWifiManager#ONBOARDABLE_SUFFIX}
         */
        ONBOARDABLE,

        /**
         * Wi-Fi access point name that doesn't contain the following prefix
         * {@link OnboardingSDKWifiManager#ONBOARDABLE_PREFIX} or suffix
         * {@link OnboardingSDKWifiManager#ONBOARDABLE_SUFFIX}
         */
        TARGET,

        /**
         * Wi-Fi access point name .
         */
        ALL
    }

    /**
     * DeviceResponse is a class used internally to encapsulate possible errors
     * that may occur during AllJoyn transactions carried by the SDK
     */
    static class DeviceResponse {

        /**
         * enumeration of possible ResponseCodes
         */
        public enum ResponseCode {
            /**
             * AllJoyn transaction successful
             */
            Status_OK,
            /**
             * AllJoyn transaction general error
             */
            Status_ERROR,
            /**
             * AllJoyn session creation error
             */
            Status_ERROR_CANT_ESTABLISH_SESSION,
            /**
             * AllJoyn transaction successful
             */
            Status_OK_CONNECT_SECOND_PHASE,
        }

        /**
         * holds the response code.
         */
        private final ResponseCode status;

        /**
         * holds the description of the error.
         */
        private String description = null;

        /**
         * DeviceResponse Constructor
         * 
         * @param status
         *            {@link #status}
         * 
         */
        public DeviceResponse(ResponseCode status) {
            this.status = status;
        }

        /**
         * DeviceResponse Constructor
         * 
         * @param status
         *            {@link #status}
         * @param description
         *            {@link #description}
         * 
         */
        public DeviceResponse(ResponseCode status, String description) {
            this.status = status;
            this.description = description;
        }

        /**
         * Get {@link #status}
         * 
         * @return the status code {@link #status}
         */
        public ResponseCode getStatus() {
            return status;
        }

        /**
         * Get {@link #description}
         * 
         * @return the error description {@link #description}
         */
        public String getDescription() {
            return description;
        }

    }

    /**
     * TAG for debug information
     */
    private final static String TAG = "OnboardingManager";

    /**
     * Default timeout for Wi-Fi connection
     * {@value #DEFAULT_WIFI_CONNECTION_TIMEOUT} msec
     */
    public static final int DEFAULT_WIFI_CONNECTION_TIMEOUT = 20000;

    /**
     * Default timeout for waiting for
     * {@link AboutObj#announce(short, org.alljoyn.bus.AboutDataListener)}
     * {@value #DEFAULT_ANNOUNCEMENT_TIMEOUT} msec
     */
    public static final int DEFAULT_ANNOUNCEMENT_TIMEOUT = 25000;

    /**
     * OnboardingManager singleton
     */
    private static volatile OnboardingManager onboardingManager = null;

    /**
     * Application context
     */
    private Context context = null;

    /**
     * HandlerThread for the state machine looper
     */
    private static HandlerThread stateHandlerThread;

    /**
     * Handler for OnboardingManager state changing messages.
     */
    private static Handler stateHandler = null;

    /**
     * Stores the OnboardingsdkWifiManager object
     */
    private OnboardingSDKWifiManager onboardingSDKWifiManager = null;

    /**
     * Stores the OnboardingConfiguration object
     */
    private OnboardingConfiguration onboardingConfiguration = null;

    /**
     * IntentFilter used to filter out intents of WIFI messages received from
     * OnboardingsdkWifiManager
     */
    private final IntentFilter wifiIntentFilter = new IntentFilter();

    /**
     * BroadcastReceiver for intents from OnboardingSDKWifiManager while running
     * the onboarding process.
     */
    private BroadcastReceiver onboardingWifiBroadcastReceiver = null;

    /**
     * BroadcastReceiver for intents from OnboardingsdkWifiManager while running
     * {@link #connectToNetwork(WiFiNetworkConfiguration, long)}
     */
    private BroadcastReceiver connectToNetworkWifiBroadcastReceiver = null;

    /**
     * Stores the OnboardingClient object used to communicate with
     * OnboardingService.
     */
    private OnboardingClient onboardingClient = null;

    /**
     * Stores the BusAttachment needed for accessing Alljoyn framework.
     */
    private BusAttachment bus = null;

    /**
     * Timer used for managing announcement timeout
     */
    private Timer announcementTimeout = new Timer();

    /**
     * Indicates a special case when {@link #abortOnboarding()} is in state
     * {@link State#JOINING_SESSION}
     */
    private final int ABORTING_INTERRUPT_FLAG = 0x100;

    /**
     * Indicator flag to listen to incoming Announcements
     */
    private static volatile boolean listenToAnnouncementsFlag;

    /**
     * Stores the OnboardingManager state machine state
     */
    private State currentState = State.IDLE;

    /**
     * Stores the original network name before starting onboarding process.
     */
    private String originalNetwork;

    /**
     * Handles About Service announcements
     */
    @Override
    public void announced(final String serviceName, final int version, final short port, final AboutObjectDescription[] objectDescriptions, final Map<String, Variant> serviceMetadata) {

        synchronized (TAG) {
            if (!listenToAnnouncementsFlag) {
                Log.w(TAG, "AnnouncementHandler not in listeneing mode");
                return;
            }
        }
        Log.d(TAG, "onAnnouncement: received  state " + currentState.toString());

        Map<String, Object> announceDataMap = null;
        try {
            announceDataMap = TransportUtil.fromVariantMap(serviceMetadata);
            if (announceDataMap == null) {
                // ignoring error. will be handled by announcement timeout
                Log.e(TAG, "onAnnouncement: invalid announcement");
                return;
            }
        } catch (BusException e) {
            // ignoring error. will be handled by announcement timeout
            Log.e(TAG, "onAnnouncement: invalid announcement", e);
            return;
        }

        UUID uniqueId = (UUID) announceDataMap.get(AboutKeys.ABOUT_APP_ID);
        if (uniqueId == null) {
            Log.e(TAG, "onAnnouncement: received null device uuid!! ignoring.");
            return;
        } else {
            Log.d(TAG, "onAnnouncement: UUID=" + uniqueId + ", busName=" + serviceName + ", port=" + port);
        }

        switch (currentState) {
        case CONNECTING_TO_ONBOARDEE:
            context.unregisterReceiver(onboardingWifiBroadcastReceiver);
            // no need for break
        case WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            if (isSeviceSupported(objectDescriptions, OnboardingTransport.INTERFACE_NAME)) {
                setState(State.ONBOARDEE_ANNOUNCEMENT_RECEIVED, new AnnounceData(serviceName, port, objectDescriptions, serviceMetadata));
            } else {
                Log.e(TAG, "onAnnouncement: for device UUID " + deviceData.getAppUUID() + " doesn't support onboarding interface");
            }
            break;

        case ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            if (isSeviceSupported(objectDescriptions, OnboardingTransport.INTERFACE_NAME)) {
                setState(State.ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT, new AnnounceData(serviceName, port, objectDescriptions, serviceMetadata));
            } else {
                Log.e(TAG, "onAnnouncement: for device UUID " + deviceData.getAppUUID() + " doesn't support onboarding interface");
            }
            break;

        case CONNECTING_TO_TARGET_WIFI_AP:
            context.unregisterReceiver(onboardingWifiBroadcastReceiver);
            // no need for break
        case WAITING_FOR_TARGET_ANNOUNCE:
            if (deviceData != null && deviceData.getAnnounceData() != null && deviceData.getAppUUID() != null) {
                if (deviceData.getAppUUID().compareTo(uniqueId) == 0) {
                    setState(State.TARGET_ANNOUNCEMENT_RECEIVED, new AnnounceData(serviceName, port, objectDescriptions, serviceMetadata));
                }
            }
            break;

        case ERROR_WAITING_FOR_TARGET_ANNOUNCE:
            if (isSeviceSupported(objectDescriptions, OnboardingTransport.INTERFACE_NAME)) {
                setState(State.ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT, new AnnounceData(serviceName, port, objectDescriptions, serviceMetadata));
            } else {
                Log.e(TAG, "onAnnouncement: for device UUID " + deviceData.getAppUUID() + " doesn't support onboarding interface");
            }
            break;

        default:
            break;
        }
    }

    /**
     * Stores information about the device to be onboarded
     */
    private DeviceData deviceData = null;

    /**
     * Internal class that stores information about the device to be onboarded
     * AnnounceData,appUUID
     */
    private static class DeviceData {

        public AnnounceData getAnnounceData() {
            return announceData;
        }

        public void setAnnounceData(AnnounceData announceData) throws BusException {
            this.announceData = announceData;
            Map<String, Object> announceDataMap = TransportUtil.fromVariantMap(announceData.getServiceMetadata());
            appUUID = (UUID) announceDataMap.get(AboutKeys.ABOUT_APP_ID);
            deviceID = (String) announceDataMap.get(AboutKeys.ABOUT_DEVICE_ID);
        }

        private AnnounceData announceData = null;

        public UUID getAppUUID() {
            return appUUID;
        }

        public String getDeviceID() {
            return deviceID;
        }

        private UUID appUUID = null;

        private String deviceID = null;

    }

    /**
     * An internal class to store the Announcement received by the AboutService.
     */
    private static class AnnounceData {
        private final String serviceName;
        private final short port;
        private final AboutObjectDescription[] objectDescriptions;
        private final Map<String, Variant> serviceMetadata;

        public String getServiceName() {
            return serviceName;
        }

        public AboutObjectDescription[] getObjectDescriptions() {
            return objectDescriptions;
        }

        public Map<String, Variant> getServiceMetadata() {
            return serviceMetadata;
        }

        public short getPort() {
            return port;
        }

        public AnnounceData(String serviceName, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> serviceMetadata) {
            this.serviceName = serviceName;
            this.port = port;
            this.objectDescriptions = objectDescriptions;
            this.serviceMetadata = serviceMetadata;
        }
    }

    /**
     * These enumeration values are used to indicate the current internal state
     * of the OnboardingManager state machine.
     */
    private static enum State {
        /**
         * Start state
         */
        IDLE(0),

        /**
         * Connecting to onboardee device Wi-Fi
         */
        CONNECTING_TO_ONBOARDEE(10),

        /**
         * Waiting for announcement on onboardee Wi-Fi
         */
        WAITING_FOR_ONBOARDEE_ANNOUNCEMENT(11),

        /**
         * Announcement received on onboardee Wi-Fi
         */
        ONBOARDEE_ANNOUNCEMENT_RECEIVED(12),

        /**
         * Join session with onboardee
         */
        JOINING_SESSION(13),

        /**
         * Configuring onboardee with target credentials
         */
        CONFIGURING_ONBOARDEE(14),

        /**
         * Configuring onboardee with target credentials
         */
        CONFIGURING_ONBOARDEE_WITH_SIGNAL(15),

        /**
         * Connecting to target Wi-Fi AP
         */
        CONNECTING_TO_TARGET_WIFI_AP(20),

        /**
         * Waiting for announcement on target Wi-Fi from onboardee
         */
        WAITING_FOR_TARGET_ANNOUNCE(21),

        /**
         * Announcement received on target Wi-Fi from onboardee
         */
        TARGET_ANNOUNCEMENT_RECEIVED(22),

        /**
         * Aborting state ,temporary state used to Abort the onboarding process.
         */
        ABORTING(30),

        /**
         * Error connecting to onboardee device Wi-Fi
         */
        ERROR_CONNECTING_TO_ONBOARDEE(110),

        /**
         * Error waiting for announcement on onboardee Wi-Fi
         */
        ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT(111),

        /**
         * Error annnouncemnet has been received from onboardee after timeout
         * expired
         */
        ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT(112),

        /**
         * Error announcement received on onboardee Wi-Fi
         */
        ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED(113),

        /**
         * Error joining AllJoyn session
         */
        ERROR_JOINING_SESSION(114),

        /**
         * Error configuring onboardee with target credentials
         */
        ERROR_CONFIGURING_ONBOARDEE(115),

        /**
         * Error waiting for configure onboardee signal
         */
        ERROR_WAITING_FOR_CONFIGURE_SIGNAL(116),

        /**
         * Error connecting to target Wi-Fi AP
         */
        ERROR_CONNECTING_TO_TARGET_WIFI_AP(120),

        /**
         * Error waiting for announcement on target Wi-Fi from onboardee
         */
        ERROR_WAITING_FOR_TARGET_ANNOUNCE(121),

        /**
         * Error annnouncemnet has been received from target after timeout
         * expired
         */
        ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT(122);

        private int value;

        private State(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

        public static State getStateByValue(int value) {
            State retType = null;
            for (State type : State.values()) {
                if (value == type.getValue()) {
                    retType = type;
                    break;
                }
            }
            return retType;
        }
    }

    /**
     * @return instance of the OnboardingManager
     */
    public static OnboardingManager getInstance() {
        if (onboardingManager == null) {
            synchronized (TAG) {
                if (onboardingManager == null) {
                    onboardingManager = new OnboardingManager();
                }
            }
        }
        return onboardingManager;
    }

    private OnboardingManager() {
        wifiIntentFilter.addAction(WIFI_CONNECTED_BY_REQUEST_ACTION);
        wifiIntentFilter.addAction(WIFI_TIMEOUT_ACTION);
        wifiIntentFilter.addAction(WIFI_AUTHENTICATION_ERROR);
        stateHandlerThread = new HandlerThread("OnboardingManagerLooper");
        stateHandlerThread.start();
        stateHandler = new Handler(stateHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                onHandleCommandMessage(msg);
            }
        };
    }

    /**
     * Initialize the SDK singleton with the current application configuration.
     * Registers AnnouncementHandler to receive announcements
     * 
     * @param context
     *            The application context.
     * @param aboutService
     *            The user application's About service.
     * @param bus
     *            The user application's bus attachment.
     * @throws OnboardingIllegalArgumentException
     *             if either of the parameters is null.
     * @throws OnboardingIllegalStateException
     *             if already initialized.
     */
    public void init(Context context, BusAttachment bus) throws OnboardingIllegalArgumentException, OnboardingIllegalStateException {
        synchronized (TAG) {
            if (context == null || bus == null) {
                throw new OnboardingIllegalArgumentException();
            }
            if (this.context != null || this.bus != null) {
                throw new OnboardingIllegalStateException();
            }

            synchronized (TAG) {
                listenToAnnouncementsFlag = false;
            }

            this.bus = bus;
            this.bus.registerAboutListener(this);
            this.bus.whoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });
            this.context = context;
            this.onboardingSDKWifiManager = new OnboardingSDKWifiManager(this.context);
        }
    }

    /**
     * Terminate the SDK .
     * 
     * @throws OnboardingIllegalStateException
     *             if not in IDLE state ,need to abort first.
     */
    public void shutDown() throws OnboardingIllegalStateException {
        synchronized (TAG) {
            if (currentState == State.IDLE) {
                this.context = null;
                this.bus.cancelWhoImplements(new String[] { OnboardingTransport.INTERFACE_NAME });
                this.bus.unregisterAboutListener(this);
                this.bus = null;
                this.onboardingSDKWifiManager = null;

            } else {
                throw new OnboardingIllegalStateException("Not in IDLE state ,please Abort first");
            }
        }
    }

    /**
     * Handle the CONNECT_TO_ONBOARDEE state. Listen to WIFI intents from
     * OnboardingsdkWifiManager. Requests from OnboardingsdkWifiManager to
     * connect to the Onboardee. If successful moves to the next state otherwise
     * sends an error intent and returns to IDLE state.
     */
    private void handleConnectToOnboardeeState() {
        final Bundle extras = new Bundle();
        onboardingWifiBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {

                String action = intent.getAction();
                Log.d(TAG, "handleConnectToOnboardeeState onReceive action=" + action);

                if (action == null) {
                    return;
                }

                if (WIFI_CONNECTED_BY_REQUEST_ACTION.equals(action)) {
                    context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                    extras.clear();
                    extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTED_ONBOARDEE_WIFI.toString());
                    sendBroadcast(STATE_CHANGE_ACTION, extras);
                    setState(State.WAITING_FOR_ONBOARDEE_ANNOUNCEMENT);
                }

                if (WIFI_TIMEOUT_ACTION.equals(action)) {
                    context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ONBOARDEE_WIFI_TIMEOUT.toString());
                    sendBroadcast(ERROR, extras);
                    setState(State.ERROR_CONNECTING_TO_ONBOARDEE);
                }

                if (WIFI_AUTHENTICATION_ERROR.equals(action)) {
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ONBOARDEE_WIFI_AUTH.toString());
                    sendBroadcast(ERROR, extras);
                    context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                    setState(State.ERROR_CONNECTING_TO_ONBOARDEE);
                }
            }
        };
        context.registerReceiver(onboardingWifiBroadcastReceiver, wifiIntentFilter);
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTING_ONBOARDEE_WIFI.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);
        onboardingSDKWifiManager.connectToWifiAP(onboardingConfiguration.getOnboardee().getSSID(), onboardingConfiguration.getOnboardee().getAuthType(), onboardingConfiguration.getOnboardee()
                .getPassword(), onboardingConfiguration.getOnboardee().isHidden(), onboardingConfiguration.getOnboardeeConnectionTimeout());
    }

    /**
     * Handle the WAIT_FOR_ONBOARDEE_ANNOUNCE state. Set a timer using
     * {@link #startAnnouncementTimeout()} Wait for an announcement which should
     * arrive from the onAnnouncement handler.
     */
    private void handleWaitForOnboardeeAnnounceState() {
        Bundle extras = new Bundle();
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.FINDING_ONBOARDEE.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);
        if (!startAnnouncementTimeout()) {
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.INTERNAL_ERROR.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT);
        }
    }

    /**
     * Handle the ERROR_TARGETR_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT state.
     * Verifies that Announcement is valid if so stay on
     * ERROR_TARGER_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT state ,otherwise moves
     * to state ERROR_WAITING_FOR_TARGET_ANNOUNCE. This state is only for
     * continuing from state ERROR_WAITING_FOR_TARGET_ANNOUNCEMENT if the
     * Announcement has been received while the timer has expired.
     * 
     * @param announceData
     *            contains the information of the Announcement .
     */
    private void handleErrorTargetAnnouncementReceivedAfterTimeoutState(AnnounceData announceData) {
        deviceData = new DeviceData();
        try {
            Log.d(TAG, "handleErrorTargetAnnouncementReceivedAfterTimeoutState" + announceData.getServiceName() + " " + announceData.getPort());
            deviceData.setAnnounceData(announceData);
        } catch (BusException e) {
            Log.e(TAG, "handleErrorTargetAnnouncementReceivedAfterTimeoutState DeviceData.setAnnounceObject failed with BusException. ", e);
            Bundle extras = new Bundle();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.INVALID_ANNOUNCE_DATA.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_WAITING_FOR_TARGET_ANNOUNCE);
        }
    }

    /**
     * Handle the ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT state.
     * Verifies that Announcement is valid if so stay on
     * ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT state ,otherwise
     * moves to state ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED. This state is only
     * for continuing from state ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT if the
     * Announcement has been received while the timer has expired.
     * 
     * @param announceData
     *            contains the information of the Announcement .
     */
    private void handleErrorOnboardeeAnnouncementReceivedAfterTimeoutState(AnnounceData announceData) {
        Bundle extras = new Bundle();
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.FOUND_ONBOARDEE.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        deviceData = new DeviceData();
        try {
            deviceData.setAnnounceData(announceData);
        } catch (BusException e) {
            Log.e(TAG, "handleOnboardeeAnnouncementReceivedState DeviceData.setAnnounceObject failed with BusException. ", e);
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.INVALID_ANNOUNCE_DATA.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED);
        }
    }

    /**
     * Handle the ONBOARDEE_ANNOUNCEMENT_RECEIVED state. Stop the announcement
     * timeout timer, Verifies that Announcement is valid if so moves to state
     * JOINING_SESSION,otherwise moves to state
     * ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED
     * 
     * @param announceData
     *            contains the information of the Announcement .
     */
    private void handleOnboardeeAnnouncementReceivedState(AnnounceData announceData) {
        stopAnnouncementTimeout();

        Bundle extras = new Bundle();
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.FOUND_ONBOARDEE.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        deviceData = new DeviceData();
        try {
            deviceData.setAnnounceData(announceData);
            setState(State.JOINING_SESSION, announceData);
        } catch (BusException e) {
            Log.e(TAG, "handleOnboardeeAnnouncementReceivedState DeviceData.setAnnounceObject failed with BusException. ", e);
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.INVALID_ANNOUNCE_DATA.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED);
        }
    }

    /**
     * Handle the JOINING_SESSION state. Handle AllJoyn session establishment
     * with the device.
     * 
     * @param announceData
     *            contains the information of the announcement
     */
    private void handleJoiningSessionState(AnnounceData announceData) {
        Bundle extras = new Bundle();
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.JOINING_SESSION.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        ResponseCode response = establishSessionWithDevice(announceData).getStatus();
        if (response == ResponseCode.Status_OK) {
            extras.clear();
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.SESSION_JOINED.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);
            setState(State.CONFIGURING_ONBOARDEE);
        } else {
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.JOIN_SESSION_ERROR.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_JOINING_SESSION);
        }
    }

    /**
     * Handle the CONFIGURE_ONBOARDEE state. Call onboardDevice to send target
     * information to the board. in case successful moves to next step else move
     * to ERROR_CONFIGURING_ONBOARDEE state.
     */
    private void handleConfigureOnboardeeState() {
        Bundle extras = new Bundle();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURING_ONBOARDEE.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);
        ResponseCode responseCode = onboardDevice().getStatus();
        if (responseCode == ResponseCode.Status_OK) {
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURED_ONBOARDEE.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);
            setState(State.CONNECTING_TO_TARGET_WIFI_AP);
        } else if (responseCode == ResponseCode.Status_OK_CONNECT_SECOND_PHASE) {
            extras.clear();
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURING_ONBOARDEE_WITH_SIGNAL.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);
            setState(State.CONFIGURING_ONBOARDEE_WITH_SIGNAL);
        } else {
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ERROR_CONFIGURING_ONBOARDEE.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_CONFIGURING_ONBOARDEE);
        }
    }

    /**
     * Handle the CONFIGURING_ONBOARDEE_WITH_SIGNAL state. Register to receive
     * oboarding siganl , start timeout if signal arrives in time call
     * {@link OnboardingClient#connectWiFi()} otherwise move to
     * ERROR_WAITING_FOR_CONFIGURE_SIGNAL state .
     */
    private void handleConfigureWithSignalOnboardeeState() {

        final Bundle extras = new Bundle();
        final Timer configWifiSignalTimeout = new Timer();
        final ConnectionResultListener listener = new ConnectionResultListener() {
            @Override
            public void onConnectionResult(ConnectionResult connectionResult) {
                Log.d(TAG, "onConnectionResult recevied " + connectionResult.getConnectionResponseType() + " " + connectionResult.getMessage());
                configWifiSignalTimeout.cancel();
                configWifiSignalTimeout.purge();
                try {
                    onboardingClient.unRegisterConnectionResultListener(this);
                    if (connectionResult.getConnectionResponseType() == ConnectionResult.ConnectionResponseType.VALIDATED) {
                        onboardingClient.connectWiFi();
                        extras.clear();
                        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURED_ONBOARDEE.toString());
                        sendBroadcast(STATE_CHANGE_ACTION, extras);
                        setState(State.CONNECTING_TO_TARGET_WIFI_AP);
                    } else {
                        extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ERROR_CONFIGURING_ONBOARDEE.toString());
                        sendBroadcast(ERROR, extras);
                        setState(State.ERROR_CONFIGURING_ONBOARDEE);
                        return;
                    }
                } catch (Exception e) {
                    Log.e(TAG, "onConnectionResult error", e);
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ERROR_CONFIGURING_ONBOARDEE.toString());
                    sendBroadcast(ERROR, extras);
                    setState(State.ERROR_CONFIGURING_ONBOARDEE);
                }
            }
        };

        try {
            configWifiSignalTimeout.schedule(new TimerTask() {
                @Override
                public void run() {
                    Log.e(TAG, "configWifiSignalTimeout  expired");
                    onboardingClient.unRegisterConnectionResultListener(listener);
                    configWifiSignalTimeout.cancel();
                    configWifiSignalTimeout.purge();
                    onboardingClient.unRegisterConnectionResultListener(listener);
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.CONFIGURING_ONBOARDEE_WAITING_FOR_SIGNAL_TIMEOUT.toString());
                    sendBroadcast(ERROR, extras);
                    setState(State.ERROR_WAITING_FOR_CONFIGURE_SIGNAL);
                }
            }, 30 * 1000);
            onboardingClient.registerConnectionResultListener(listener);
        } catch (Exception e) {
            Log.e(TAG, "registerConnectionResultListener", e);
            configWifiSignalTimeout.cancel();
            configWifiSignalTimeout.purge();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ERROR_CONFIGURING_ONBOARDEE.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_CONFIGURING_ONBOARDEE); // send error
        }

    }

    /**
     * Handle the CONNECT_TO_TARGET state. Listen to WIFI intents from
     * OnboardingsdkWifiManager Requests from OnboardingsdkWifiManager to
     * connect to the Target. if successful moves to the next state otherwise
     * send error intent and returns to IDLE state.
     */
    private void handleConnectToTargetState() {
        final Bundle extras = new Bundle();
        onboardingWifiBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {

                String action = intent.getAction();
                Log.d(TAG, "onReceive action=" + action);
                if (action != null) {

                    if (WIFI_CONNECTED_BY_REQUEST_ACTION.equals(action)) {
                        context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTED_TARGET_WIFI.toString());
                        sendBroadcast(STATE_CHANGE_ACTION, extras);
                        setState(State.WAITING_FOR_TARGET_ANNOUNCE);
                    }

                    if (WIFI_TIMEOUT_ACTION.equals(action)) {
                        context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                        extras.clear();
                        extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.TARGET_WIFI_TIMEOUT.toString());
                        sendBroadcast(ERROR, extras);
                        setState(State.ERROR_CONNECTING_TO_TARGET_WIFI_AP);
                    }
                    if (WIFI_AUTHENTICATION_ERROR.equals(action)) {
                        context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                        extras.clear();
                        extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.TARGET_WIFI_AUTH.toString());
                        sendBroadcast(ERROR, extras);
                        setState(State.ERROR_CONNECTING_TO_TARGET_WIFI_AP);
                    }
                }
            }
        };// receiver
        context.registerReceiver(onboardingWifiBroadcastReceiver, wifiIntentFilter);
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTING_TARGET_WIFI.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);
        onboardingSDKWifiManager.connectToWifiAP(onboardingConfiguration.getTarget().getSSID(), onboardingConfiguration.getTarget().getAuthType(), onboardingConfiguration.getTarget().getPassword(),
                onboardingConfiguration.getTarget().isHidden(), onboardingConfiguration.getTargetConnectionTimeout());
    }

    /**
     * Handle the WAIT_FOR_TARGET_ANNOUNCE state. Set a timer with using
     * startAnnouncementTimeout. waits for an announcement which should arrive
     * from the onAnnouncement handler.
     */
    private void handleWaitForTargetAnnounceState() {
        Bundle extras = new Bundle();
        extras.clear();
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.VERIFYING_ONBOARDED.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);
        if (!startAnnouncementTimeout()) {
            extras.clear();
            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.INTERNAL_ERROR.toString());
            sendBroadcast(ERROR, extras);
            setState(State.ERROR_WAITING_FOR_TARGET_ANNOUNCE);
        }
    }

    /**
     * Handls the TARGET_ANNOUNCEMENT_RECEIVED state.
     * <ul>
     * <li>Call {@link #stopAnnouncementTimeout()}
     * <li>Send a success broadcast.
     * <li>Move state machine to IDLE state (onboarding process finished)
     * <li>Enable all Wi-Fi access points that were disabled in the onboarding
     * process.
     * </ul>
     * 
     * @param announceData
     *            contains the information of the announcement .
     */
    private void handleTargetAnnouncementReceivedState(AnnounceData announceData) {
        stopAnnouncementTimeout();

        Bundle extras = new Bundle();
        extras.putString(EXTRA_DEVICE_ONBOARDEE_SSID, onboardingConfiguration.getOnboardee().getSSID());
        extras.putString(EXTRA_DEVICE_TARGET_SSID, onboardingConfiguration.getTarget().getSSID());
        deviceData = new DeviceData();
        try {
            deviceData.setAnnounceData(announceData);
            extras.putSerializable(EXTRA_DEVICE_APPID, deviceData.getAppUUID());
            extras.putString(EXTRA_DEVICE_DEVICEID, deviceData.getDeviceID());
        } catch (BusException e) {
            Log.e(TAG, "handleTargetAnnouncementReceivedState unable to retrieve AppID/DeviceID ", e);
        }
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.VERIFIED_ONBOARDED.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        synchronized (TAG) {
            listenToAnnouncementsFlag = false;
        }
        setState(State.IDLE);
        onboardingSDKWifiManager.enableAllWifiNetworks();
    }

    /**
     * Handle the the ABORTING state. Due to the implementation of the state
     * machine via hander. Messages are received into the handler's queue ,and
     * are handled serially , when ABORTING message is received
     * handleAbortingState deletes all messages in the handlers queue .In case
     * there are any messages there . Due to the blocking nature of
     * JOINING_SESSION state the ABORTING message can be handled only after it
     * has completed!.
     */
    private void handleAbortingState() {
        // store current State in initalState
        State initalState = currentState;

        // set state to State.ABORTING
        currentState = State.ABORTING;

        // in case State.JOINING_SESSION push ABORTING_INTERRUPT_FLAG into the
        // stateHandler queue.
        if (initalState == State.JOINING_SESSION) {
            stateHandler.sendMessageDelayed(stateHandler.obtainMessage(ABORTING_INTERRUPT_FLAG), 60 * 60 * 10000);
            return;
        }

        // remove all queued up messages in the stateHandler
        for (State s : State.values()) {
            stateHandler.removeMessages(s.value);
        }

        // note ABORTING state can't be handled during JOINING_SESSION it is
        // blocking!
        switch (initalState) {

        case CONNECTING_TO_ONBOARDEE:
            context.unregisterReceiver(onboardingWifiBroadcastReceiver);
            abortStateCleanUp();
            break;

        case WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            stopAnnouncementTimeout();
            abortStateCleanUp();
            break;

        case ONBOARDEE_ANNOUNCEMENT_RECEIVED:
            abortStateCleanUp();
            break;

        case CONFIGURING_ONBOARDEE:
        case CONFIGURING_ONBOARDEE_WITH_SIGNAL:
            abortStateCleanUp();
            break;

        case ERROR_CONNECTING_TO_ONBOARDEE:
        case ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
        case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
        case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED:
        case ERROR_JOINING_SESSION:
        case ERROR_CONFIGURING_ONBOARDEE:
        case ERROR_WAITING_FOR_CONFIGURE_SIGNAL:
        case ERROR_CONNECTING_TO_TARGET_WIFI_AP:
            abortStateCleanUp();
            break;

        case WAITING_FOR_TARGET_ANNOUNCE:
            stopAnnouncementTimeout();
            // no need for break
        case ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
            // no need for break
        case ERROR_WAITING_FOR_TARGET_ANNOUNCE:
            Bundle extras = new Bundle();
            onboardingSDKWifiManager.enableAllWifiNetworks();

            synchronized (TAG) {
                listenToAnnouncementsFlag = false;
            }

            setState(State.IDLE);
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.ABORTED.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);
            break;

        default:
            break;
        }
    }

    /**
     * Handle the state machine transition.
     * 
     * @param msg
     */
    private void onHandleCommandMessage(Message msg) {

        if (msg == null) {
            return;
        }
        // in case ABORTING_INTERRUPT_FLAG has been found in the queue remove it
        // and call abortStateCleanUp
        if (stateHandler.hasMessages(ABORTING_INTERRUPT_FLAG)) {
            stateHandler.removeMessages(ABORTING_INTERRUPT_FLAG);
            abortStateCleanUp();
            return;
        }
        State stateByValue = State.getStateByValue(msg.what);
        if (stateByValue == null) {
            return;
        }
        Log.d(TAG, "onHandleCommandMessage " + stateByValue);
        switch (stateByValue) {

        case IDLE:
            currentState = State.IDLE;
            break;

        case CONNECTING_TO_ONBOARDEE:
            currentState = State.CONNECTING_TO_ONBOARDEE;
            handleConnectToOnboardeeState();
            break;

        case WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            currentState = State.WAITING_FOR_ONBOARDEE_ANNOUNCEMENT;
            handleWaitForOnboardeeAnnounceState();
            break;

        case ONBOARDEE_ANNOUNCEMENT_RECEIVED:
            currentState = State.ONBOARDEE_ANNOUNCEMENT_RECEIVED;
            handleOnboardeeAnnouncementReceivedState((AnnounceData) msg.obj);
            break;

        case JOINING_SESSION:
            currentState = State.JOINING_SESSION;
            handleJoiningSessionState((AnnounceData) msg.obj);
            break;

        case CONFIGURING_ONBOARDEE:
            currentState = State.CONFIGURING_ONBOARDEE;
            handleConfigureOnboardeeState();
            break;

        case CONFIGURING_ONBOARDEE_WITH_SIGNAL:
            currentState = State.CONFIGURING_ONBOARDEE_WITH_SIGNAL;
            handleConfigureWithSignalOnboardeeState();
            break;

        case CONNECTING_TO_TARGET_WIFI_AP:
            currentState = State.CONNECTING_TO_TARGET_WIFI_AP;
            handleConnectToTargetState();
            break;

        case WAITING_FOR_TARGET_ANNOUNCE:
            currentState = State.WAITING_FOR_TARGET_ANNOUNCE;
            handleWaitForTargetAnnounceState();
            break;

        case TARGET_ANNOUNCEMENT_RECEIVED:
            currentState = State.TARGET_ANNOUNCEMENT_RECEIVED;
            handleTargetAnnouncementReceivedState((AnnounceData) msg.obj);
            break;

        case ERROR_CONNECTING_TO_ONBOARDEE:
            currentState = State.ERROR_CONNECTING_TO_ONBOARDEE;
            break;

        case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
            currentState = State.ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT;
            handleErrorOnboardeeAnnouncementReceivedAfterTimeoutState((AnnounceData) msg.obj);
            break;

        case ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            currentState = State.ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT;
            break;

        case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED:
            currentState = State.ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED;
            break;

        case ERROR_JOINING_SESSION:
            currentState = State.ERROR_JOINING_SESSION;
            break;

        case ERROR_CONFIGURING_ONBOARDEE:
            currentState = State.ERROR_CONFIGURING_ONBOARDEE;
            break;

        case ERROR_WAITING_FOR_CONFIGURE_SIGNAL:
            currentState = State.ERROR_WAITING_FOR_CONFIGURE_SIGNAL;
            break;

        case ERROR_CONNECTING_TO_TARGET_WIFI_AP:
            currentState = State.ERROR_CONNECTING_TO_TARGET_WIFI_AP;
            break;

        case ERROR_WAITING_FOR_TARGET_ANNOUNCE:
            currentState = State.ERROR_WAITING_FOR_TARGET_ANNOUNCE;
            break;

        case ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
            currentState = State.ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT;
            handleErrorTargetAnnouncementReceivedAfterTimeoutState((AnnounceData) msg.obj);
            break;

        case ABORTING:
            handleAbortingState();
            break;

        default:
            break;

        }
    }

    /**
     * Move the state machine to a new state.
     * 
     * @param state
     */
    private void setState(State state) {
        Message msg = stateHandler.obtainMessage(state.getValue());
        stateHandler.sendMessage(msg);
    }

    /**
     * Move the state machine to a new state.
     * 
     * @param state
     * @param data
     *            metadata to pass to the new state
     * 
     */
    private void setState(State state, Object data) {
        Message msg = stateHandler.obtainMessage(state.getValue());
        msg.obj = data;
        stateHandler.sendMessage(msg);
    }

    /**
     * Establish an AllJoyn session with the device.
     * 
     * @param announceData
     *            the Announcement data.
     * @return status of operation.
     */
    private DeviceResponse establishSessionWithDevice(final AnnounceData announceData) {

        try {
            if (announceData.serviceName == null) {
                return new DeviceResponse(ResponseCode.Status_ERROR, "announceData.serviceName == null");
            }
            if (announceData.getPort() == 0) {
                return new DeviceResponse(ResponseCode.Status_ERROR, "announceData.getPort() == 0");
            }

            if (onboardingClient != null) {
                onboardingClient.disconnect();
                onboardingClient = null;
            }

            synchronized (TAG) {
                onboardingClient = new OnboardingClientImpl(announceData.getServiceName(), bus, new ServiceAvailabilityListener() {
                    @Override
                    public void connectionLost() {
                        // expected. we are onboarding the device, hence sending
                        // it the another network.
                        Log.d(TAG, "establishSessionWithDevice connectionLost");
                    }
                }, announceData.getPort());
            }
        } catch (Exception e) {
            Log.e(TAG, "establishSessionWithDevice Exception: ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        }

        try {
            ResponseCode connectToDeviceStatus = connectToDevice(onboardingClient).getStatus();
            if (connectToDeviceStatus != ResponseCode.Status_OK) {
                return new DeviceResponse(ResponseCode.Status_ERROR_CANT_ESTABLISH_SESSION, connectToDeviceStatus.name());
            }
            return new DeviceResponse(ResponseCode.Status_OK);
        } catch (Exception e) {
            Log.e(TAG, "establishSessionWithDevice ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        }
    }

    /**
     * Call the OnboardingService API for passing the onboarding configuration
     * to the device.
     * 
     * @return status of operation.
     */
    private DeviceResponse onboardDevice() {
        try {
            AuthType authType = onboardingConfiguration.getTarget().getAuthType();
            boolean isPasswordHex = false;
            String passForConfigureNetwork = onboardingConfiguration.getTarget().getPassword();
            if (authType == AuthType.WEP) {
                Pair<Boolean, Boolean> wepCheckResult = OnboardingSDKWifiManager.checkWEPPassword(passForConfigureNetwork);
                isPasswordHex = wepCheckResult.second;
            }
            Log.d(TAG, "onBoardDevice OnboardingClient isPasswordHex " + isPasswordHex);
            if (!isPasswordHex) {
                passForConfigureNetwork = toHexadecimalString(onboardingConfiguration.getTarget().getPassword());
                Log.i(TAG, "convert pass to hex: from " + onboardingConfiguration.getTarget().getPassword() + " -> to " + passForConfigureNetwork);
            }
            Log.i(TAG, "before configureWiFi networkName = " + onboardingConfiguration.getTarget().getSSID() + " networkPass = " + passForConfigureNetwork + " selectedAuthType = "
                    + onboardingConfiguration.getTarget().getAuthType().getTypeId());
            ConfigureWifiMode res = onboardingClient.configureWiFi(onboardingConfiguration.getTarget().getSSID(), passForConfigureNetwork, onboardingConfiguration.getTarget().getAuthType());
            Log.i(TAG, "configureWiFi result=" + res);
            switch (res) {
            case REGULAR:
                onboardingClient.connectWiFi();
                return new DeviceResponse(ResponseCode.Status_OK);
            case FAST_CHANNEL_SWITCHING:
                // wait for a ConnectionResult signal from the device before
                // calling connectWifi
                return new DeviceResponse(ResponseCode.Status_OK_CONNECT_SECOND_PHASE);
            default:
                Log.e(TAG, "configureWiFi returned an unexpected result: " + res);
                return new DeviceResponse(ResponseCode.Status_ERROR);
            }
        } catch (BusException e) {
            Log.e(TAG, "onboarddDevice ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        } catch (Exception e) {
            Log.e(TAG, "onboarddDevice ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        }
    }

    /**
     * Call the offboardDevice API for offboarding the device.
     * 
     * @param serviceName
     *            device's service name
     * @param port
     *            device's application port
     * @return result of action
     */
    private DeviceResponse offboardDevice(String serviceName, short port) {
        Log.d(TAG, "offboardDevice serviceName:[" + serviceName + "] port:[" + port + "]");
        Bundle extras = new Bundle();
        extras.putString(EXTRA_DEVICE_BUS_NAME, serviceName);
        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.JOINING_SESSION.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        if (onboardingClient != null) {
            onboardingClient.disconnect();
            onboardingClient = null;
        }

        try {
            synchronized (TAG) {
                onboardingClient = new OnboardingClientImpl(serviceName, bus, new ServiceAvailabilityListener() {
                    @Override
                    public void connectionLost() {
                        // expected. we are offboarding the device...
                        Log.d(TAG, "offboardDevice connectionLost");
                    }
                }, port);
            }
        } catch (Exception e) {
            Log.e(TAG, "offboardDevice Exception: ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        }

        try {
            ResponseCode connectToDeviceStatus = connectToDevice(onboardingClient).getStatus();
            if (connectToDeviceStatus != ResponseCode.Status_OK) {
                return new DeviceResponse(ResponseCode.Status_ERROR_CANT_ESTABLISH_SESSION, connectToDeviceStatus.name());
            }

            extras.clear();
            extras.putString(EXTRA_DEVICE_BUS_NAME, serviceName);
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.SESSION_JOINED.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);

            extras.clear();
            extras.putString(EXTRA_DEVICE_BUS_NAME, serviceName);
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURING_ONBOARDEE.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);

            onboardingClient.offboard();

            extras.clear();
            extras.putString(EXTRA_DEVICE_BUS_NAME, serviceName);
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONFIGURED_ONBOARDEE.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);

            return new DeviceResponse(ResponseCode.Status_OK);
        } catch (BusException e) {
            Log.e(TAG, "offboardDevice ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        } catch (Exception e) {
            Log.e(TAG, "offboardDevice ", e);
            return new DeviceResponse(ResponseCode.Status_ERROR);
        }
    }

    /**
     * Start an AllJoyn session with another Alljoyn device.
     * 
     * @param client
     * @return status of operation.
     */
    private DeviceResponse connectToDevice(ClientBase client) {
        if (client == null) {
            return new DeviceResponse(ResponseCode.Status_ERROR, "fail connect to device, client == null");
        }
        if (client.isConnected()) {
            return new DeviceResponse(ResponseCode.Status_OK);
        }

        Status status = client.connect();
        switch (status) {
        case OK:
            Log.d(TAG, "connectToDevice. Join Session OK");
            return new DeviceResponse(ResponseCode.Status_OK);
        case ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED:
            Log.d(TAG, "connectToDevice: Join Session returned ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED. Ignoring");
            return new DeviceResponse(ResponseCode.Status_OK);
        case ALLJOYN_JOINSESSION_REPLY_FAILED:
        case ALLJOYN_JOINSESSION_REPLY_UNREACHABLE:
            Log.e(TAG, "connectToDevice: Join Session returned ALLJOYN_JOINSESSION_REPLY_FAILED.");
            return new DeviceResponse(ResponseCode.Status_ERROR_CANT_ESTABLISH_SESSION, "device unreachable");
        default:
            Log.e(TAG, "connectToDevice: Join session returned error: " + status.name());
            return new DeviceResponse(ResponseCode.Status_ERROR, "Failed connecting to device");
        }
    }

    /**
     * Convert a string in ASCII format to HexAscii.
     * 
     * @param pass
     *            password to convert
     * @return HexAscii of the input.
     */
    private static String toHexadecimalString(String pass) {
        char[] HEX_CODE = "0123456789ABCDEF".toCharArray();
        byte[] data;
        try {
            data = pass.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            Log.e(TAG, "Failed getting bytes of passcode by UTF-8", e);
            data = pass.getBytes();
        }
        StringBuilder r = new StringBuilder(data.length * 2);
        for (byte b : data) {
            r.append(HEX_CODE[(b >> 4) & 0xF]);
            r.append(HEX_CODE[(b & 0xF)]);
        }
        return r.toString();
    }

    /**
     * Start a wifi scan. Sends the following possible intents
     * <ul>
     * <li>{@link #WIFI_SCAN_RESULTS_AVAILABLE_ACTION} action with this extra
     * <ul>
     * <li>{@link #EXTRA_ONBOARDEES_AP} extra information containg ArrayList of
     * {@link WiFiNetwork}
     * <li>{@link #EXTRA_TARGETS_AP} extra information containg ArrayList of
     * {@link WiFiNetwork}
     * <li>{@link #EXTRA_ALL_AP} extra information containg ArrayList of
     * {@link WiFiNetwork}
     * </ul>
     * <ul>
     * 
     * @throws WifiDisabledException
     *             if Wi-Fi is not enabled.
     */
    public void scanWiFi() throws WifiDisabledException {
        if (!onboardingSDKWifiManager.isWifiEnabled()) {
            throw new WifiDisabledException();
        }
        onboardingSDKWifiManager.scan();
    }

    /**
     * Retrieves list of access points after scan was complete.
     * 
     * @param filter
     *            of Wi-Fi list type {@link WifiFilter}
     * @return list of Wi-Fi access points {@link #scanWiFi()}
     */
    public List<WiFiNetwork> getWifiScanResults(WifiFilter filter) {
        if (filter == WifiFilter.ALL) {
            return onboardingSDKWifiManager.getAllAccessPoints();
        } else if (filter == WifiFilter.TARGET) {
            return onboardingSDKWifiManager.getNonOnboardableAccessPoints();
        } else {
            return onboardingSDKWifiManager.getOnboardableAccessPoints();
        }
    }

    /**
     * @return the current Wi-Fi network that the Android device is connected
     *         to.
     * @throws WifiDisabledException
     *             in case Wi-Fi is disabled.
     */
    public WiFiNetwork getCurrentNetwork() throws WifiDisabledException {
        if (!onboardingSDKWifiManager.isWifiEnabled()) {
            throw new WifiDisabledException();
        }
        return onboardingSDKWifiManager.getCurrentConnectedAP();
    }

    /**
     * Connect the Android device to a WIFI network. Sends the following
     * possible intents
     * <ul>
     * <li>{@link #STATE_CHANGE_ACTION} action with this extra
     * <ul>
     * <li>{@link #EXTRA_ONBOARDING_STATE} extra information of enum
     * {@link OnboardingState}
     * </ul>
     * <li> {@link #ERROR} action with this extra
     * <ul>
     * <li>{@link #EXTRA_ERROR_DETAILS} extra information of enum
     * {@link OnboardingErrorType}
     * </ul>
     * <ul>
     * 
     * @param network
     *            contains detailed data how to connect to the WIFI network.
     * @param connectionTimeout
     *            timeout in Msec to complete the task of connecting to a Wi-Fi
     *            network
     * @throws WifiDisabledException
     *             in case Wi-Fi is disabled
     * @throws OnboardingIllegalArgumentException
     *             in case WiFiNetworkConfiguration is invalid or
     *             connectionTimeout is invalid
     */
    public void connectToNetwork(final WiFiNetworkConfiguration network, long connectionTimeout) throws WifiDisabledException, OnboardingIllegalArgumentException {

        if (!onboardingSDKWifiManager.isWifiEnabled()) {
            throw new WifiDisabledException();
        }

        if (network == null || network.getSSID() == null || network.getSSID().isEmpty()
                || (network.getAuthType() != AuthType.OPEN && (network.getPassword() == null || network.getPassword().isEmpty()))) {
            throw new OnboardingIllegalArgumentException();
        }

        connectToNetworkWifiBroadcastReceiver = new BroadcastReceiver() {

            @Override
            public void onReceive(Context arg0, Intent intent) {
                Bundle extras = new Bundle();
                String action = intent.getAction();
                Log.d(TAG, "onReceive action=" + action);

                if (WIFI_CONNECTED_BY_REQUEST_ACTION.equals(action)) {
                    context.unregisterReceiver(connectToNetworkWifiBroadcastReceiver);
                    if (intent.hasExtra(EXTRA_WIFI_WIFICONFIGURATION)) {
                        WifiConfiguration config = (WifiConfiguration) intent.getParcelableExtra(EXTRA_WIFI_WIFICONFIGURATION);

                        if (OnboardingSDKWifiManager.normalizeSSID(network.getSSID()).equals(OnboardingSDKWifiManager.normalizeSSID(config.SSID))) {
                            extras.clear();
                            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTED_OTHER_WIFI.toString());
                            sendBroadcast(STATE_CHANGE_ACTION, extras);
                            return;
                        }
                        extras.clear();
                        extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.OTHER_WIFI_TIMEOUT.toString());
                        sendBroadcast(ERROR, extras);
                    }
                }

                if (WIFI_TIMEOUT_ACTION.equals(action)) {
                    context.unregisterReceiver(connectToNetworkWifiBroadcastReceiver);
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.OTHER_WIFI_TIMEOUT.toString());
                    sendBroadcast(ERROR, extras);
                }
                if (WIFI_AUTHENTICATION_ERROR.equals(action)) {
                    context.unregisterReceiver(connectToNetworkWifiBroadcastReceiver);
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.OTHER_WIFI_AUTH.toString());
                    sendBroadcast(ERROR, extras);
                }

            }

        };
        context.registerReceiver(connectToNetworkWifiBroadcastReceiver, wifiIntentFilter);
        Bundle extras = new Bundle();

        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTING_OTHER_WIFI.toString());
        sendBroadcast(STATE_CHANGE_ACTION, extras);

        if (connectionTimeout <= 0) {
            connectionTimeout = DEFAULT_WIFI_CONNECTION_TIMEOUT;
        }

        onboardingSDKWifiManager.connectToWifiAP(network.getSSID(), network.getAuthType(), network.getPassword(), network.isHidden(), connectionTimeout);
    }

    /**
     * Validate OnboardingConfiguration .
     * 
     * @param config
     *            information for onboarding process.
     * @return true if OnboardingConfiguration is valid else false.
     */
    private boolean validateOnboardingConfiguration(OnboardingConfiguration config) {

        if (config == null || config.getOnboardee() == null || config.getTarget() == null) {
            return false;
        }

        if (config.getOnboardeeAnnoucementTimeout() <= 0 || config.getOnboardeeConnectionTimeout() <= 0 || config.getTargetAnnoucementTimeout() <= 0 || config.getTargetConnectionTimeout() <= 0) {
            return false;
        }

        // in case authtype isn't OPEN verify that a password exists.
        if (config.getOnboardee().getAuthType() != AuthType.OPEN && (config.getOnboardee().getPassword() == null || config.getOnboardee().getPassword().isEmpty())) {
            return false;
        }

        // in case authtype isn't OPEN verify that a password exists.
        if (config.getTarget().getAuthType() != AuthType.OPEN && (config.getTarget().getPassword() == null || config.getTarget().getPassword().isEmpty())) {
            return false;
        }

        if (config.getTarget().getSSID() == null || config.getTarget().getSSID().isEmpty() || config.getOnboardee().getSSID() == null || config.getOnboardee().getSSID().isEmpty()) {
            return false;
        }

        return true;
    }

    /**
     * Start and resume the onboarding process. Send these possible intents
     * <ul>
     * <li>{@link #STATE_CHANGE_ACTION} action with this extra
     * <ul>
     * <li>{@link #EXTRA_ONBOARDING_STATE} extra information of enum
     * {@link OnboardingState}
     * </ul>
     * <li> {@link #ERROR} action with this extra
     * <ul>
     * <li>{@link #EXTRA_ERROR_DETAILS} extra information of enum
     * {@link OnboardingErrorType}
     * </ul>
     * </ul>
     * 
     * 
     * can resume the onboarding process in case one of the internal errors has
     * occoured
     * <ul>
     * <li>ERROR_CONNECTING_TO_ONBOARDEE
     * <li>ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT
     * <li>ERROR_JOINING_SESSION
     * <li>ERROR_CONFIGURING_ONBOARDEE
     * <li>ERROR_CONNECTING_TO_TARGET_WIFI_AP
     * <li>ERROR_WAITING_FOR_TARGET_ANNOUNCE
     * </ul>
     * 
     * @param config
     *            containing information about onboardee and target networks.
     * @throws OnboardingIllegalStateException
     *             in case onboarding is arleady running or trying to resume
     *             from internal state ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED.
     * @throws OnboardingIllegalArgumentException
     *             in case OnboardingConfiguration is invalid.
     * @throws WifiDisabledException
     *             in case Wi-Fi is disabled.
     */
    public void runOnboarding(OnboardingConfiguration config) throws OnboardingIllegalStateException, OnboardingIllegalArgumentException, WifiDisabledException {
        if (!onboardingSDKWifiManager.isWifiEnabled()) {
            throw new WifiDisabledException();
        }
        if (!validateOnboardingConfiguration(config)) {
            throw new OnboardingIllegalArgumentException();
        }

        synchronized (TAG) {
            onboardingConfiguration = config;
            listenToAnnouncementsFlag = true;

            if (currentState == State.IDLE) {

                WiFiNetwork currentAP = onboardingSDKWifiManager.getCurrentConnectedAP();
                if (currentAP != null) {
                    originalNetwork = currentAP.getSSID();
                }

                setState(State.CONNECTING_TO_ONBOARDEE);
            } else if (currentState.getValue() >= State.ERROR_CONNECTING_TO_ONBOARDEE.getValue()) {

                switch (currentState) {

                case ERROR_CONNECTING_TO_ONBOARDEE:
                    setState(State.CONNECTING_TO_ONBOARDEE);
                    break;

                case ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
                    setState(State.WAITING_FOR_ONBOARDEE_ANNOUNCEMENT);
                    break;

                case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED:
                    throw new OnboardingIllegalStateException("The device doesn't comply with onboarding service");

                case ERROR_ONBOARDEE_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
                case ERROR_JOINING_SESSION:
                    setState(State.JOINING_SESSION, deviceData.getAnnounceData());
                    break;

                case ERROR_CONFIGURING_ONBOARDEE:
                    setState(State.CONFIGURING_ONBOARDEE);
                    break;

                case ERROR_WAITING_FOR_CONFIGURE_SIGNAL:
                    setState(State.CONFIGURING_ONBOARDEE_WITH_SIGNAL);
                    break;

                case ERROR_CONNECTING_TO_TARGET_WIFI_AP:
                    setState(State.CONNECTING_TO_TARGET_WIFI_AP);
                    break;

                case ERROR_WAITING_FOR_TARGET_ANNOUNCE:
                    setState(State.WAITING_FOR_TARGET_ANNOUNCE);
                    break;

                case ERROR_TARGET_ANNOUNCEMENT_RECEIVED_AFTER_TIMEOUT:
                    setState(State.TARGET_ANNOUNCEMENT_RECEIVED, deviceData.getAnnounceData());
                    break;

                default:
                    break;
                }
            } else {
                throw new OnboardingIllegalStateException("onboarding process is already running");
            }
        }
    }

    /**
     * Abort the onboarding process.
     * 
     * <ul>
     * <li>Send the following intents.
     * <li>{@link #STATE_CHANGE_ACTION} action with this extra
     * <ul>
     * <li> {@link OnboardingState#ABORTING} when starting the abort process.
     * </ul>
     * </ul>
     * <p>
     * see also {@link #abortStateCleanUp()}
     * 
     * @throws OnboardingIllegalStateException
     *             in case the state machine is in state IDLE,ABORTING (No need
     *             to Abort) in case the state machine is in state
     *             CONNECTING_TO_TARGET_WIFI_AP
     *             ,WAITING_FOR_TARGET_ANNOUNCE,TARGET_ANNOUNCEMENT_RECEIVED
     *             (can't abort ,in final stages of onboarding)
     */
    public void abortOnboarding() throws OnboardingIllegalStateException {
        synchronized (TAG) {
            if (currentState == State.IDLE || currentState == State.ABORTING) {
                throw new OnboardingIllegalStateException("Can't abort ,already ABORTED");
            }

            if (currentState == State.CONNECTING_TO_TARGET_WIFI_AP || currentState == State.TARGET_ANNOUNCEMENT_RECEIVED || currentState == State.CONFIGURING_ONBOARDEE
                    || currentState == State.CONFIGURING_ONBOARDEE_WITH_SIGNAL) {
                throw new OnboardingIllegalStateException("Can't abort");
            }
            Bundle extras = new Bundle();
            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.ABORTING.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);

            setState(State.ABORTING);
        }

    }

    /**
     * Prepare the state machine for the onboarding process after abort has
     * beeen requested.
     * 
     * Try restoring the connection to the original Wi-Fi access point prior to
     * calling {@link OnboardingManager#runOnboarding(OnboardingConfiguration)}
     * <p>
     * Does the following :
     * <ul>
     * <li>Stop listening to About Service announcements.
     * <li>Remove onboardee Wi-Fi access point from Android's Wi-Fi configured
     * netwroks.
     * <li>Enable Android's Wi-Fi manager to select a suitable Wi-Fi access
     * point.
     * <li>Move state maching to IDLE state.
     * <li>Send the following intents.
     * <ul>
     * <li>{@link #STATE_CHANGE_ACTION} action with this extra
     * <ul>
     * <li> {@link OnboardingState#CONNECTING_ORIGINAL_WIFI} when trying to
     * connect to orignal network if exists.
     * <li> {@link OnboardingState#CONNECTING_ORIGINAL_WIFI} when connected to
     * orignal network if exists.
     * <li> {@link OnboardingState#ABORTED} when abort process complete.
     * </ul>
     * <li>{@link #ERROR} action with this extra
     * <ul>
     * <li> {@link OnboardingErrorType#ORIGINAL_WIFI_TIMEOUT} when trying to
     * connect to orignal and get timeout.
     * <li> {@link OnboardingErrorType#ORIGINAL_WIFI_AUTH} when trying to connect
     * to orignal and get authentication error.
     * </ul>
     * </ul>
     * </ul>
     */
    private void abortStateCleanUp() {

        // in case ABORTING_INTERRUPT_FLAG found in stateHandler remove it.
        if (stateHandler.hasMessages(ABORTING_INTERRUPT_FLAG)) {
            stateHandler.removeMessages(ABORTING_INTERRUPT_FLAG);
        }

        if (onboardingConfiguration != null && onboardingConfiguration.getOnboardee() != null && onboardingConfiguration.getOnboardee().getSSID() != null) {
            onboardingSDKWifiManager.removeWifiAP(onboardingConfiguration.getOnboardee().getSSID());
        }
        final Bundle extras = new Bundle();

        synchronized (TAG) {
            listenToAnnouncementsFlag = false;
        }

        // Try to connect to orginal access point if existed.
        if (originalNetwork != null) {
            try {
                onboardingSDKWifiManager.connectToWifiBySSID(originalNetwork, DEFAULT_WIFI_CONNECTION_TIMEOUT);
            } catch (OnboardingIllegalArgumentException e) {
                Log.e(TAG, "abortStateCleanUp " + e.getMessage());

                onboardingSDKWifiManager.enableAllWifiNetworks();
                setState(State.IDLE);

                extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.ABORTED.toString());
                sendBroadcast(STATE_CHANGE_ACTION, extras);
                return;
            }

            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTING_ORIGINAL_WIFI.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);

            onboardingWifiBroadcastReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {

                    String action = intent.getAction();
                    Log.d(TAG, "onReceive action=" + action);
                    if (action != null) {

                        context.unregisterReceiver(onboardingWifiBroadcastReceiver);
                        onboardingSDKWifiManager.enableAllWifiNetworks();
                        setState(State.IDLE);

                        if (WIFI_CONNECTED_BY_REQUEST_ACTION.equals(action)) {
                            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.CONNECTED_ORIGINAL_WIFI.toString());
                            sendBroadcast(STATE_CHANGE_ACTION, extras);
                        }

                        if (WIFI_TIMEOUT_ACTION.equals(action)) {
                            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ORIGINAL_WIFI_TIMEOUT.toString());
                            sendBroadcast(ERROR, extras);
                        }

                        if (WIFI_AUTHENTICATION_ERROR.equals(action)) {
                            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ORIGINAL_WIFI_AUTH.toString());
                            sendBroadcast(ERROR, extras);
                        }

                        extras.clear();
                        extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.ABORTED.toString());
                        sendBroadcast(STATE_CHANGE_ACTION, extras);
                    }
                }
            };// receiver
            context.registerReceiver(onboardingWifiBroadcastReceiver, wifiIntentFilter);

        } else {
            // Probably onboarding started when the device wasn't connected to a
            // Wi-Fi. So nothing to restore.
            // Just return to IDLE and enable all the networks that were
            // disabled by the onboarding process.
            onboardingSDKWifiManager.enableAllWifiNetworks();
            setState(State.IDLE);

            extras.putString(EXTRA_ONBOARDING_STATE, OnboardingState.ABORTED.toString());
            sendBroadcast(STATE_CHANGE_ACTION, extras);
        }

    }

    /**
     * 
     * Offboard a device that is on the current Wi-Fi network.
     * 
     * @param config
     *            contains the offboarding information needed to complete the
     *            task.
     * @throws OnboardingIllegalStateException
     *             is thrown when not in internal IDLE state.
     * @throws OnboardingIllegalArgumentException
     *             is thrown when config is not valid
     * @throws WifiDisabledException
     *             in case Wi-Fi is disabled.
     */
    public void runOffboarding(final OffboardingConfiguration config) throws OnboardingIllegalStateException, OnboardingIllegalArgumentException, WifiDisabledException {

        if (!onboardingSDKWifiManager.isWifiEnabled()) {
            throw new WifiDisabledException();
        }
        // verify that the OffboardingConfiguration has valid data
        if (config == null || config.getServiceName() == null || config.getServiceName().isEmpty() || config.getPort() == 0) {
            throw new OnboardingIllegalArgumentException();
        }

        synchronized (TAG) {
            // in case the SDK is in onboarding mode the runOffboarding can't
            // continue
            if (currentState != State.IDLE) {
                throw new OnboardingIllegalStateException("onboarding process is already running");
            }
            Log.d(TAG, "runOffboarding serviceName" + config.getServiceName() + " port" + config.getPort());
            new Thread() {
                @Override
                public void run() {
                    DeviceResponse deviceResponse = offboardDevice(config.getServiceName(), config.getPort());
                    if (deviceResponse.getStatus() != ResponseCode.Status_OK) {
                        Bundle extras = new Bundle();
                        if (deviceResponse.getStatus() == ResponseCode.Status_ERROR_CANT_ESTABLISH_SESSION) {
                            extras.putString(EXTRA_DEVICE_BUS_NAME, config.getServiceName());
                            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.JOIN_SESSION_ERROR.toString());
                        } else {
                            extras.putString(EXTRA_DEVICE_BUS_NAME, config.getServiceName());
                            extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.ERROR_CONFIGURING_ONBOARDEE.toString());
                        }
                        sendBroadcast(ERROR, extras);
                    }
                }
            }.start();
        }
    }

    /**
     * Check if the device supports the given service.
     * 
     * @param objectDescriptions
     *            the list of supported services as announced by the device.
     * @param service
     *            name of the service to check
     * @return true if supported else false
     */
    private boolean isSeviceSupported(final AboutObjectDescription[] objectDescriptions, String service) {
        if (objectDescriptions != null) {
            for (int i = 0; i < objectDescriptions.length; i++) {
                String[] interfaces = objectDescriptions[i].interfaces;
                for (int j = 0; j < interfaces.length; j++) {
                    String currentInterface = interfaces[j];
                    if (currentInterface.startsWith(service)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * Start a timeout announcement to arrive from a device. Takes the timeout
     * interval from the {@link OnboardingConfiguration} that stores the data.
     * If timeout expires, moves the state machine to idle state and sends
     * timeout intent.
     * 
     * @return true if in correct state else false.
     */
    private boolean startAnnouncementTimeout() {
        long timeout = 0;
        switch (currentState) {

        case WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
            timeout = onboardingConfiguration.getOnboardeeAnnoucementTimeout();
            break;

        case WAITING_FOR_TARGET_ANNOUNCE:
            timeout = onboardingConfiguration.getTargetAnnoucementTimeout();
            break;

        default:
            Log.e(TAG, "startAnnouncementTimeout has been intialized in bad state abort");
            return false;

        }

        announcementTimeout.schedule(new TimerTask() {

            Bundle extras = new Bundle();

            @Override
            public void run() {

                Log.e(TAG, "Time out expired  " + currentState.toString());
                switch (currentState) {

                case WAITING_FOR_ONBOARDEE_ANNOUNCEMENT:
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.FIND_ONBOARDEE_TIMEOUT.toString());
                    sendBroadcast(ERROR, extras);
                    setState(State.ERROR_WAITING_FOR_ONBOARDEE_ANNOUNCEMENT);
                    break;

                case WAITING_FOR_TARGET_ANNOUNCE:
                    extras.clear();
                    extras.putString(EXTRA_ERROR_DETAILS, OnboardingErrorType.VERIFICATION_TIMEOUT.toString());
                    sendBroadcast(ERROR, extras);
                    setState(State.ERROR_WAITING_FOR_TARGET_ANNOUNCE);
                    break;

                default:
                    break;

                }
            }
        }, timeout);
        return true;
    }

    /**
     * Stop the announcement timeout that was activated by
     * {@link #startAnnouncementTimeout()}
     */
    private void stopAnnouncementTimeout() {
        announcementTimeout.cancel();
        announcementTimeout.purge();
        announcementTimeout = new Timer();
    }

    /**
     * A wrapper method that sends intent broadcasts with extra data
     * 
     * @param action
     *            an action for the intent
     * @param extras
     *            extras for the intent
     */
    private void sendBroadcast(String action, Bundle extras) {
        Intent intent = new Intent(action);
        if (extras != null && !extras.isEmpty()) {
            intent.putExtras(extras);
        }
        context.sendBroadcast(intent);
    }

}
