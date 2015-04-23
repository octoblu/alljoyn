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

import org.alljoyn.bus.annotation.Position;
import org.alljoyn.onboarding.OnboardingService.AuthType;

/**
 * A struct that represents a WiFi scan result: SSID and authType.
 */
public class MyScanResult
{
	/**
	 * The personal AP SSID
	 */
    @Position(0) public String m_ssid;
    
    /**
     * The personal AP authentication type
     * -3 - wpa2_auto, onboardee should try WPA2_TKIP and WPA2_CCMP
	 * -2 - wpa_auto, onboardee should try WPA_TKIP and WPA_CCMP
	 * -1 - any, The onboardee chooses the authentication
	 * 0 - Open Personal AP is open 
	 * 1 - WEP
	 * 2 - WPA_TKIP
	 * 3 - WPA_CCMP
	 * 4 - WPA2_TKIP
	 * 5 - WPA2_CCMP
	 * 6 - WPS
     * @see {@link AuthType}
     */
    @Position(1) public short m_authType;
}
