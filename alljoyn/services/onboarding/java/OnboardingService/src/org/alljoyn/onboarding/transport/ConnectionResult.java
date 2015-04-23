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

/**
 * Wraps the signal that is emitted when the connection attempt against the personal AP is completed.
 * This signal will be received only if the fast channel switching feature is supported by the device.
 */
public class ConnectionResult {

    /**
     * ConnectionResult message
     */
    private final String message;

    /**
     * ConnectionResult response code
     */
    private final ConnectionResponseType connectionResponseType;

    /**
     * These enumeration values are returned from a device that supports fast channel switching .It is reported after the device tries to verify
     * the validity of the Wi-Fi parameters received via {@link OnboardingTransport#ConfigureWiFi(String, String, short)}
     */
    public static enum ConnectionResponseType {

        /**
         * Wi-Fi validated
         */
        VALIDATED((short) 0),

        /**
         * Wi-Fi unreachable
         */
        UNREACHABLE((short) 1),

        /**
         * Wi-Fi AP doesn't support the authentication received
         */
        UNSUPPORTED_PROTOCOL((short) 2),

        /**
         * Wi-Fi authentication error
         */
        UNAUTHORIZED((short) 3),

        /**
         * Misc error
         */
        ERROR_MESSAGE((short) 4);

        private short value;

        /**
         *
         * @param value
         */
        private ConnectionResponseType(short value) {
            this.value = value;
        }

        /**
         * Returns the id of authentication type
         *
         * @return id of authentication type
         */
        public short getValue() {
            return value;
        }

        public static ConnectionResponseType getConnectionResponseTypeByValue(short value) {
            ConnectionResponseType ret = null;
            for (ConnectionResponseType responseType : ConnectionResponseType.values()) {
                if (value == responseType.getValue()) {
                    ret = responseType;
                    break;
                }
            }
            return ret;
        }
    }

    public ConnectionResult(ConnectionResponseType connectionResponseType, String message) {
        this.message = message;
        this.connectionResponseType = connectionResponseType;
    }

    public String getMessage() {
        return message;
    }

    public ConnectionResponseType getConnectionResponseType() {
        return connectionResponseType;
    }
}
