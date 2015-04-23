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

package org.alljoyn.onboarding.transport;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.annotation.BusAnnotation;
import org.alljoyn.bus.annotation.BusAnnotations;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.BusSignal;
import org.alljoyn.bus.annotation.Secure;

/**
 * Definition of the Onboarding BusInterface
 */
@BusInterface(name = OnboardingTransport.INTERFACE_NAME, announced = "true")
@Secure
public interface OnboardingTransport extends BusObject {
    public static final String INTERFACE_NAME = "org.alljoyn.Onboarding";
    public final static String OBJ_PATH = "/Onboarding";

    /**
     * These enumeration values are used to indicate the ConfigureWifi possible
     * modes
     */
    public static enum ConfigureWifiMode {

        /**
         * Wi-Fi standard mode
         */
        REGULAR((short) 1),

        /**
         * Wi-Fi fast channel switching mode
         */
        FAST_CHANNEL_SWITCHING((short) 2);

        private final short value;

        private ConfigureWifiMode(short value) {
            this.value = value;
        }

        public short getValue() {
            return value;
        }

        public static ConfigureWifiMode getConfigureWifiModeByValue(short value) {
            ConfigureWifiMode retState = null;
            for (ConfigureWifiMode responseType : ConfigureWifiMode.values()) {
                if (value == responseType.getValue()) {
                    retState = responseType;
                    break;
                }
            }
            return retState;
        }
    }

    /**
     * @return the interface version
     * @throws BusException
     */
    @BusProperty(signature = "q")
    public short getVersion() throws BusException;

    /**
     * @return the state - one of the following values: 0 - Personal AP Not
     *         Configured 1 - Personal AP Configured/Not Validated 2 - Personal
     *         AP Configured/Validating 3 - Personal AP Configured/Validated 4 -
     *         Personal AP Configured/Error 5 - Personal AP Configured/Retry
     * @throws BusException
     */
    @BusProperty(signature = "n")
    public short getState() throws BusException;

    /**
     * @return the last error 0 - Validated 1 - Unreachable 2 -
     *         Unsupported_protocol 3 - Unauthorized 4 - Error_message
     * @throws BusException
     */
    @BusProperty(signature = "(ns)")
    public OBLastError getLastError() throws BusException;

    /**
     * Tell the onboardee to connect to the Personal AP. The onboardee is
     * recommended to use channel switching feature if it is available.
     * 
     * @throws BusException
     */
    @BusMethod()
    @BusAnnotations({ @BusAnnotation(name = "org.freedesktop.DBus.Method.NoReply", value = "true") })
    public void Connect() throws BusException;

    /**
     * Send the personal AP info to the onboardee. When the authType is equal to
     * "any", the onboardee needs to try out all the possible authentication
     * types it supports to connect to the AP. If authType parameter is invalid
     * then the AllJoyn error code org.alljoyn.Error.OutOfRange will be returned
     * 
     * @param ssid
     * @param passphrase
     * @param authType
     *            - one of the following values WPA2_AUTO = -3, WPA_AUTO = -2,
     *            any = -1, Open = 0 , WEP = 1, WPA_TKIP =2, WPA_CCMP = 3,
     *            WPA2_TKIP = 4, WPA2_CCMP = 5, WPS = 6
     * @throws BusException
     */
    @BusMethod(signature = "ssn", replySignature = "n")
    public short ConfigureWiFi(String ssid, String passphrase, short authType) throws BusException;

    /**
     * Tell the onboardee to disconnect from the personal AP, clear the personal
     * AP configuration fields, and start the soft AP mode.
     */
    @BusMethod()
    @BusAnnotations({ @BusAnnotation(name = "org.freedesktop.DBus.Method.NoReply", value = "true") })
    public void Offboard() throws BusException;

    /**
     * Scan all the WiFi access points in the onboardee's proximity. Some device
     * may not support this feature.
     * 
     * @throws BusException
     *             if feature is unsupported, AllJoyn error code
     *             org.alljoyn.Error.FeatureNotAvailable will be returned in the
     *             AllJoyn response.
     */
    @BusMethod(replySignature = "qa(sn)")
    public ScanInfo GetScanInfo() throws BusException;

    /**
     * Signal received after ConfigureWiFi status is 2
     * 
     * @param connectionResultAJ
     */
    @BusSignal(signature = "(ns)")
    public void ConnectionResult(ConnectionResultAJ connectionResultAJ);
}
