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

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.test.CommandsReceiver.TSAction;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

/**
 * Test Application
 */
public class TimeServiceTestApp extends Application {
    private static final String TAG = "ajtsapp" + TimeServiceTestApp.class.getSimpleName();

    static {
        System.loadLibrary("alljoyn_java");
    }

    /**
     * The daemon should advertise itself "quietly" (directly to the calling
     * port) This is to reply directly to a TC looking for a daemon
     */
    private static final String DAEMON_NAME_PREFIX  = "org.alljoyn.BusNode.TimeService";

    /**
     * The daemon should advertise itself "quietly" (directly to the calling
     * port) This is to reply directly to a TC looking for a daemon
     */
    private static final String DAEMON_QUIET_PREFIX = "quiet@";

    /**
     * The {@link BusAttachment} to be used by the Time Service
     */
    private BusAttachment bus;

    /**
     * Server side functionality
     */
    private TestServer tsServer;

    /**
     * Client side functionality;
     */
    private TestClient tsClient;

    /**
     * @see android.app.Application#onCreate()
     */
    @Override
    public void onCreate() {

        super.onCreate();
        DaemonInit.PrepareDaemon(this);

        try {

            prepareAJ();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to startup AJ", tse);
            return;
        }

        AuthManager authMgr = new AuthManager();
        Status status       = authMgr.register(bus, this);
        if ( status != Status.OK ) {

            Log.e(TAG, "Failed to register AuthenticationListener");
        }

        registerCommandsReceiver();
    }

    /**
     * @return {@link BusAttachment}
     */
    public BusAttachment getBusAttachment() {

        return bus;
    }

    /**
     * @return {@link TestClient}
     */
    public TestClient getTestClient() {

        return tsClient;
    }

    /**
     * @return {@link TestServer}
     */
    public TestServer getTestServer() {

        return tsServer;
    }

    /**
     * Starts TimeService in the server mode
     */
    public void startServer() {

        if ( tsServer == null ) {

            tsServer = new TestServer(this);
        }

        Log.i(TAG, "Starting TimeService server");

        try {

            tsServer.init();
        } catch (Exception e) {

            Log.e(TAG, "Failed to start TimeService in the server mode", e);
        }
    }

    /**
     * Starts TimeService in the client mode
     */
    public void startClient() {

        if ( tsClient == null ) {

            tsClient = new TestClient(this);
        }

        Log.i(TAG, "Starting TimeService client");

        try {

            tsClient.init();
        } catch (Exception e) {

            Log.e(TAG, "Failed to start TimeService in the client mode", e);
        }
    }

    /**
     * Stops the TS server
     */
    public void stopServer() {

        if ( tsServer == null ) {

            return;
        }

        Log.d(TAG, "Stopping TS Server");
        tsServer.shutdown();
    }

    /**
     * Stopps TS client
     */
    public void stopClient() {

        if ( tsClient == null ) {

            return;
        }

        Log.d(TAG, "Stopping TS Client");
        tsClient.shutdown();
    }

    /**
     * Performs all the preparation before starting the service
     */
    private void prepareAJ() throws TimeServiceException {

        Log.d(TAG, "Create the BusAttachment");
        bus = new BusAttachment("TimeService", BusAttachment.RemoteMessage.Receive);

        //For verbose AJ logging use the following lines
        //bus.setDaemonDebug("ALL", 7);
        //bus.setLogLevels("ALLJOYN=7");
        //bus.setLogLevels("SESSIONLESS=2");
        //bus.setLogLevels("ALL=7");
        //bus.useOSLogging(true);

        Status conStatus = bus.connect();
        if ( conStatus != Status.OK ) {
            Log.e(TAG, "Failed connect to bus, Error: '" + conStatus + "'");
            throw new TimeServiceException("Failed connect to bus, Error: '" + conStatus + "'");
        }

        Log.d(TAG, "The TimeService BusAttachment: '" + bus.getUniqueName() + "', created and connected successfully");

        //Advertise the daemon so that the thin client can find it
        advertiseDaemon();
    }//prepareAJ

    /**
     * Advertise the daemon so that the thin client can find it
     * @param logger
     */
    private void advertiseDaemon() throws TimeServiceException {
        int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;

        String daemonName = DAEMON_NAME_PREFIX + ".G" + bus.getGlobalGUIDString();

        // request the name
        Status reqStatus = bus.requestName(daemonName, flag);
        if (reqStatus == Status.OK) {

            // advertise the name with a quite prefix for TC to find it
            Status adStatus = bus.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);

            if (adStatus != Status.OK) {

                bus.releaseName(daemonName);
                Log.e(TAG, "Failed to advertise daemon name " + daemonName + ", Error: '" + adStatus + "'");
                throw new TimeServiceException("Failed to advertise daemon name '" + daemonName + "', Error: '" + adStatus + "'");
            } else {
                Log.d(TAG, "Succefully advertised daemon name: '" + daemonName + "', BusName: '" + bus.getUniqueName() + "'");
            }
        } else {
            Log.d(TAG, "Failed to request the daemon name: '" + daemonName + "', Error: '" + reqStatus + "'");
            throw new TimeServiceException("Failed to request the DaemonName: '" + daemonName + "', Error: '" + reqStatus + "'");
        }

    }// advertiseDaemon

    /**
     * Register {@link BroadcastReceiver} that will receive the {@link Intent} commands
     */
    private void registerCommandsReceiver() {

        IntentFilter filter = new IntentFilter();
        for ( TSAction action : CommandsReceiver.TSAction.values() ) {

            filter.addAction(action.name());
        }

        BroadcastReceiver br = new CommandsReceiver();
        registerReceiver(br, filter);
    }
}
