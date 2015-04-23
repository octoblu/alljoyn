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

/**
 * Holder of onboarding data: onboardee, target, and timeouts
 */
public class OnboardingConfiguration {

    /**
     * Stores the onboardee WIFI credentials.
     */
    private WiFiNetworkConfiguration onboardee = null;

    /**
     * Stores the target WIFI credentials.
     */
    private WiFiNetworkConfiguration target = null;

    /**
     * Stores the timeout of establishing WIFI connection with the onboardee in
     * msec.
     */
    private long onboardeeConnectionTimeout = OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT;

    /**
     * Stores the timeout of waiting for announcement from the onboardee after
     * establishing WIFI connection with the onboardee in msec.
     */
    private long onboardeeAnnoucementTimeout = OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT;

    /**
     * Stores the timeout of establishing WIFI connection with the target in
     * msec.
     */
    private long targetConnectionTimeout = OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT;

    /**
     * Stores the timeout of waiting for announcement from the onboardee after
     * establishing WIFI connection with the target in msec.
     */
    private long targetAnnoucementTimeout = OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT;

    /**
     * Constructor of OnboardingConfiguration that receives all parameters
     * including WIFI credentials and timeouts.
     *
     * @param onboardee
     *            {@link #onboardee}
     * @param onboardeeConnectionTimeout
     *            {@link #onboardeeConnectionTimeout}
     * @param onboardeeAnnoucementTimeout
     *            {@link #onboardeeAnnoucementTimeout}
     * @param target
     *            {@link #target}
     * @param targetConnectionTimeout
     *            {@link #targetConnectionTimeout}
     * @param targetAnnoucementTimeout
     *            {@link #targetAnnoucementTimeout}
     */
    public OnboardingConfiguration(WiFiNetworkConfiguration onboardee, long onboardeeConnectionTimeout, long onboardeeAnnoucementTimeout, WiFiNetworkConfiguration target,
            long targetConnectionTimeout, long targetAnnoucementTimeout) {
        this.onboardee = onboardee;
        this.target = target;
        this.onboardeeConnectionTimeout = onboardeeConnectionTimeout;
        this.onboardeeAnnoucementTimeout = onboardeeAnnoucementTimeout;
        this.targetConnectionTimeout = targetConnectionTimeout;
        this.targetAnnoucementTimeout = targetAnnoucementTimeout;
    }

    /**
     * Constructor of OnboardingConfiguration that receives WIFI credentials and
     * uses default timeout values
     *
     * @param onboardee
     *            {@link #onboardee}
     * @param target
     *            {@link #target}
     */
    public OnboardingConfiguration(WiFiNetworkConfiguration onboardee, WiFiNetworkConfiguration target) {
        this.onboardee = onboardee;
        this.target = target;
    }

    /**
     * Get {@link #onboardee}
     *
     * @return the onboardee of the configuration {@link #onboardee}
     */
    public WiFiNetworkConfiguration getOnboardee() {
        return onboardee;
    }


    /**
     * Get {@link #target}
     *
     * @return the target of the configuration {@link #target}
     */
    public WiFiNetworkConfiguration getTarget() {
        return target;
    }


    /**
     * Get {@link #onboardeeConnectionTimeout}
     *
     * @return the onboardee Wi-Fi timeout {@link #onboardeeConnectionTimeout}
     */
    public long getOnboardeeConnectionTimeout() {
        return onboardeeConnectionTimeout;
    }


    /**
     * Get {@link #onboardeeAnnoucementTimeout}
     *
     * @return the onboardee announcement timeout
     *         {@link #onboardeeAnnoucementTimeout}
     */
    public long getOnboardeeAnnoucementTimeout() {
        return onboardeeAnnoucementTimeout;
    }



    /**
     * Get {@link #targetConnectionTimeout}
     *
     * @return the target Wi-Fi timeout {@link #targetConnectionTimeout}
     */
    public long getTargetConnectionTimeout() {
        return targetConnectionTimeout;
    }



    /**
     * Get {@link #targetAnnoucementTimeout}
     *
     * @return the target announcement timeout {@link #targetAnnoucementTimeout}
     */
    public long getTargetAnnoucementTimeout() {
        return targetAnnoucementTimeout;
    }


}
