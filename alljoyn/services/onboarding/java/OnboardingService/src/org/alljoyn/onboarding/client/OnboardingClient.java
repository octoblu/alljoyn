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

package org.alljoyn.onboarding.client;

import org.alljoyn.bus.BusException;
import org.alljoyn.onboarding.OnboardingService.AuthType;
import org.alljoyn.onboarding.transport.ConnectionResultListener;
import org.alljoyn.onboarding.transport.OBLastError;
import org.alljoyn.onboarding.transport.OnboardingTransport.ConfigureWifiMode;
import org.alljoyn.onboarding.transport.ScanInfo;
import org.alljoyn.services.common.ClientBase;
/**
 * An interface for onboarding a remote IoE device (onboardee). Encapsulates the
 * OnboardingTransport BusInterface
 */
public interface OnboardingClient extends ClientBase {
    /**
     * Returns the configured state of the onboardee
     *
     * @return the onboarding state: 0 - Personal AP Not Configured 1 - Personal
     *         AP Configured/Not Validated 2 - Personal AP Configured/Validating
     *         3 - Personal AP Configured/Validated 4 - Personal AP
     *         Configured/Error 5 - Personal AP Configured/Retry
     * @throws BusException
     */
    public short getState() throws BusException;

    /**
     * Get the last error. During most of the onboarding process, the onboarder
     * and onboardee are on different networks so returning a sync result isn't
     * possible. Especially when there was an error and the onboardee did not
     * connect to the onboarder's network. The onboardee then opens a soft AP to
     * which the onboarder connects, and this method is how the onboarder can
     * query about what happened.
     *
     * @return last error
     * @throws BusException
     */
    public OBLastError GetLastError() throws BusException;



    /**
     * Send the personal AP info to the onboardee. The onboardee doesn't try to
     * connect yet. It waits for {@link #connectWiFi()} When the authType is
     * equal to "any", the onboardee needs to try out all the possible
     * authentication types it supports to connect to the AP.
     *
     * @param ssid
     *            the personal AP SSID
     * @param passphrase
     *            the personal AP passphrase
     * @param authType
     *            the authentication type of the AP
     * @return Wifi mode regular or fast channel
     * @throws BusException
     *             If authType parameter is invalid then the AllJoyn error code
     *             org.alljoyn.Error.OutOfRange will be returned
     */
    public ConfigureWifiMode configureWiFi(String ssid, String passphrase, AuthType  authType) throws BusException;

    /**
     * Tell the onboardee to connect to the Personal AP. The onboardee is
     * recommended to use channel switching feature if it is available. That is,
     * keep the soft AP open with the onboarder, while trying to connect to the
     * personal AP.
     *
     * @throws BusException
     */
    public void connectWiFi() throws BusException;

    /**
     * Tell the onboardee to disconnect from the personal AP, clear the personal
     * AP configuration fields, and start the soft AP mode.
     *
     * @throws BusException
     */
    public void offboard() throws BusException;

    /**
     * Scan all the WiFi access points in the onboardee's proximity, so that the
     * onboarder can select the proper one. Some devices may not support this
     * feature.
     *
     * @return ScanInfo an array of scan results.
     * @throws BusException
     *             If device doesn't support the feature, the AllJoyn error code
     *             org.alljoyn.Error.FeatureNotAvailable will be returned in the
     *             AllJoyn response.
     */
    public ScanInfo getScanInfo() throws BusException;

    /***
     * Register to receive ConnectionResult signal data.
     * This is relevant in fast channel switching mode.
     *
     * @param listener callback class to receive data.
     * @throws BusException if registration fails
     */
    public void registerConnectionResultListener(ConnectionResultListener listener) throws BusException;

    /**
     * Unregister from receiving ConnectionResult signal data.
     * @param listener
     */
    public void unRegisterConnectionResultListener(ConnectionResultListener listener);

}
