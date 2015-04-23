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

package org.allseen.timeservice.test;

import org.alljoyn.bus.AuthListener;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.android.utils.AndroidLogger;

import android.content.Context;
import android.util.Log;

/**
 * Registers {@link AuthListener}. The default passcode is defined as in
 * {@link SrpAnonymousKeyListener#DEFAULT_PINCODE}
 */
public class AuthManager implements AuthPasswordHandler {
    private static final String TAG = "ajts" + AuthManager.class.getSimpleName();

    /**
     * Current pass code
     */
    private char[] passCode;

    /**
     * The authentication mechanisms that this application supports
     */
    private static final String[] AUTH_MECHANISMS = new String[] { "ALLJOYN_SRP_KEYX", "ALLJOYN_PIN_KEYX", "ALLJOYN_ECDHE_PSK" };

    /**
     * Constructor
     */
    public AuthManager() {

        this.passCode = SrpAnonymousKeyListener.DEFAULT_PINCODE;
    }

    /**
     * Set authentication passcode
     *
     * @param passCode
     * @throws IllegalArgumentException
     *             if the received passCode is undefined
     */
    public void setPassCode(String passCode) {

        if (passCode == null) {
            throw new IllegalArgumentException("passCode is undefined");
        }

        this.passCode = passCode.toCharArray();
    }

    /**
     * Register the AuthManager
     *
     * @param bus {@link BusAttachment} to be used for the registration
     * @param context {@link Context}
     * @return {@link Status}
     */
    public Status register(BusAttachment bus, Context context) {

        SrpAnonymousKeyListener authListener = new SrpAnonymousKeyListener(this, new AndroidLogger(), AUTH_MECHANISMS);

        String keyStoreFileName = context.getFileStreamPath("alljoyn_keystore").getAbsolutePath();
        Status status           = bus.registerAuthListener(authListener.getAuthMechanismsAsString(), authListener,
                                                               keyStoreFileName);

        Log.d(TAG, "AuthListener has registered, Status: '" + status + "'");
        return status;
    }

    /**
     * @see org.alljoyn.services.android.security.AuthPasswordHandler#getPassword(java.lang.String)
     */
    @Override
    public char[] getPassword(String busName) {

        return passCode;
    }

    /**
     * @see org.alljoyn.services.android.security.AuthPasswordHandler#completed(java.lang.String,
     *      java.lang.String, boolean)
     */
    @Override
    public void completed(String mechanism, String peerName, boolean authenticated) {

        if (authenticated) {
            Log.i(TAG, "The authentication process has been completed successfully. Mechanism: '" + mechanism +
                            "' ,peerName: '" + peerName + "'");
            return;
        }

        Log.e(TAG, "The authentication process has FAILED . Mechanism: '" + mechanism + "' ,peerName: '" +
                    peerName + "'");
    }
}