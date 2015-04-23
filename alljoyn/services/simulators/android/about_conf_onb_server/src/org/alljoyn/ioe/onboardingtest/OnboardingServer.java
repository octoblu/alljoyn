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
package org.alljoyn.ioe.onboardingtest;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Map;

import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.config.ConfigServiceImpl;
import org.alljoyn.config.server.ConfigChangeListener;
import org.alljoyn.config.server.FactoryResetHandler;
import org.alljoyn.config.server.PassphraseChangedListener;
import org.alljoyn.config.server.RestartHandler;
import org.alljoyn.config.server.SetPasswordHandler;
import org.alljoyn.config.transport.ConfigTransport;
import org.alljoyn.onboarding.OnboardingService;
import org.alljoyn.onboarding.OnboardingServiceImpl;
import org.alljoyn.onboarding.transport.OnboardingTransport;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.android.storage.PropertyStoreImpl;
import org.alljoyn.services.android.utils.AndroidLogger;
import org.alljoyn.services.common.PropertyStore;
import org.alljoyn.services.common.utils.GenericLogger;
import org.apache.http.util.ByteArrayBuffer;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

/**
 * The OnboardingServer is a board simulator written for Android.
 * It acts as a board supporting the following AllJoyn services: Onboarding, About, Config
 */
public class OnboardingServer extends Service implements AuthPasswordHandler, SetPasswordHandler, OnboardingServerCallback {
    private static final String TAG = "ioe" + OnboardingServer.class.getSimpleName();

    // Reference to the AllJoyn bus
    BusAttachment m_bus;
    GenericLogger m_logger = new AndroidLogger();

    // Reference to AboutConfigService
    OnboardingService m_onboardingService;
    private AsyncHandler m_asyncHandler;

    // the property store used by About and Config services
    private PropertyStore m_propertyStore;

    public String m_keyStoreFileName;

    // listener for bus security requests. Both Config and Onboarding services are secure
    public SrpAnonymousKeyListener m_authListener;

    // the password for secured sessions
    private char[] m_myPass;
    
    //Supported Authentication mechanisms
    private static final String[] AUTH_MECHANISMS = new String[]{"ALLJOYN_SRP_KEYX", "ALLJOYN_PIN_KEYX", "ALLJOYN_ECDHE_PSK"};
    
    // load the native alljoyn_java library.
    static {
        System.loadLibrary("alljoyn_java");
    }

    public OnboardingServer()
    {
    }

    /* (non-Javadoc)
     * @see android.app.Service#onBind(android.content.Intent)
     */
    @Override
    public IBinder onBind(Intent intent)
    {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    /* (non-Javadoc)
     * @see android.app.Service#onCreate()
     */
    @Override
    public void onCreate() {
        m_logger.debug(TAG, "onCreate");
        super.onCreate();

        // create a handler
        HandlerThread busThread = new HandlerThread("BusHandler");
        busThread.start();
        m_asyncHandler = new AsyncHandler(busThread.getLooper());

        m_onboardingService = OnboardingServiceImpl.getInstance();
        ((OnboardingServiceImpl) m_onboardingService).init(this, this);

        // initialize the state machine
        m_asyncHandler.sendEmptyMessage(AsyncHandler.CONNECT);
    }

    /* (non-Javadoc)
     * @see android.app.Service#onStartCommand(android.content.Intent, int, int)
     */
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // Continue running until explicitly being stopped.
        return START_STICKY;
    }

    /* (non-Javadoc)
     * @see android.app.Service#onDestroy()
     */
    @Override
    public void onDestroy()
    {
        m_logger.debug(TAG, "onDestroy");

        // Disconnect to prevent any resource leaks
        m_asyncHandler.shutdown();
        m_asyncHandler.getLooper().quit();

        super.onDestroy();

    }

    @Override
    public void connect() {
        m_asyncHandler.sendEmptyMessage(AsyncHandler.CONNECT);

    }

    @Override
    public void disconnect() {
        m_asyncHandler.sendEmptyMessage(AsyncHandler.DISCONNECT);
    }

    /**
     * Handles all callbacks from UI and AllJoyn
     */
    class AsyncHandler extends Handler implements RestartHandler, ConfigChangeListener, FactoryResetHandler, PassphraseChangedListener
    {

        public static final int CONNECT = 1;
        public static final int DISCONNECT = 2;

        public AsyncHandler(Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {

            // connect to the bus and start our service
            case CONNECT: {
                connect();
                break;
            }

            // release all resources acquired in connect
            case DISCONNECT: {
                disconnect();
                break;
            }

            default:
                break;
            }
        }

        // ------------------------ Connect --------------------------------

        /**
         * Connect to the bus and start our services.
         */
        private void connect()
        {
            m_logger.debug(TAG, "connect");
            // All Service use the same AllJoyn BusAttachment
            m_bus = new BusAttachment(getPackageName(), BusAttachment.RemoteMessage.Receive);
            m_bus.connect();

            // Pump up the daemon debug level
            /*
			m_Bus.setDaemonDebug("ALL", 7);
			m_Bus.setLogLevels("ALL=7");
			m_Bus.useOSLogging(true);
             */

            // initialize the PropertyStore
            m_propertyStore = new PropertyStoreImpl(getApplicationContext());

            // initialize AboutService
            // it will expose the board's properties to clients
            try {
                AboutServiceImpl.getInstance().startAboutServer((short)1080, m_propertyStore, m_bus);
            }
            catch (Exception e)
            {
                m_logger.error(TAG, "AboutService failed, Error: " + e.getMessage());
            }

            // initialize ConfigService
            // it will expose the board's configurable properties to clients
            try{
                ConfigServiceImpl.getInstance().startConfigServer(m_propertyStore, this, this, this, this, m_bus);
                ConfigServiceImpl.getInstance().setSetPasswordHandler(OnboardingServer.this);
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

            // initialize IconService
            // it will expose the board's icon to clients
            try{

                // serialize a png file into a byte array

                InputStream ims = getAssets().open("img-alljoyn-logo.png");
                BufferedInputStream bis = new BufferedInputStream(ims);
                int offset = 0;
                int count = 0;
                int bufferSize = 1024*10;
                byte[] buffer = new byte[bufferSize];

                ByteArrayBuffer tempArray = new ByteArrayBuffer(bufferSize);
                try {
                    while ((count = bis.read(buffer, offset, bufferSize)) != -1) {
                        tempArray.append(buffer, offset, count);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                int size = tempArray.length();
                byte[] resultBytes = new byte[size];
                System.arraycopy(tempArray.buffer(), 0, resultBytes, 0, size);

                // register the byte array as the board's icon. The AboutService will expose it
                AboutServiceImpl.getInstance().registerIcon("image/png",
                        "https://www.alljoyn.org/sites/all/themes/at_alljoyn/images/img-alljoyn-logo.png",
                        resultBytes);
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }


            // initialize OnboardingService
            // it will let clients onboard/offboard this board with the home
            // network
            // m_onboardingService = OnboardingServiceImpl.getInstance();
            // ((OnboardingServiceImpl)
            // m_onboardingService).initContext(OnboardingServer.this);
            try{
                m_onboardingService.startOnboardingServer(m_bus);
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

            // register authentication listener. This is needed as Config and Onboarding services are secure
            m_keyStoreFileName = getFileStreamPath("alljoyn_keystore").getAbsolutePath();
            m_authListener = new SrpAnonymousKeyListener(OnboardingServer.this, m_logger, AUTH_MECHANISMS);
            Status authStatus = m_bus.registerAuthListener(m_authListener.getAuthMechanismsAsString(), m_authListener, m_keyStoreFileName);
            m_logger.debug(TAG, "BusAttachment.registerAuthListener status = " + authStatus);
            if (authStatus != Status.OK)
            {
                m_logger.debug(TAG, "Failed to register Auth listener status = " + authStatus.toString());
            }

            // Add configService to About announcement.
            AboutServiceImpl.getInstance().addObjectDescription(OnboardingTransport.OBJ_PATH, new String[] {OnboardingTransport.INTERFACE_NAME});
            AboutServiceImpl.getInstance().addObjectDescription(ConfigTransport.OBJ_PATH, new String[] {ConfigTransport.INTERFACE_NAME});

            // this is not a TCP/IP port. It's AllJoyn specific
            bindSessionPort((short)1080);

            // send an announcement to notify clients of the existence of this board, and let them know the services that it supports
            AboutServiceImpl.getInstance().announce();

        }

        // ------------------------ Disconnect --------------------------------

        /**
         * Release all resources acquired in connect.
         */
        private void disconnect()
        {

            m_logger.debug(TAG, "disconnect");
            try
            {
                OnboardingServiceImpl.getInstance().stopOnboardingServer();
                AboutServiceImpl.getInstance().unregisterIcon();
                ConfigServiceImpl.getInstance().stopConfigServer();
                AboutServiceImpl.getInstance().stopAboutServer();
            } catch (Exception e)
            {
                m_logger.error(TAG, "disconnect failed: " + e.getMessage());
            }
            m_bus.disconnect();
            m_bus.release();
            m_bus = null;
        }

        // ------------------------ Shutdown --------------------------------

        private void shutdown()
        {
            try
            {
                AboutServiceImpl.getInstance().stopAboutServer();
                ConfigServiceImpl.getInstance().stopConfigServer();
                AboutServiceImpl.getInstance().unregisterIcon();
                m_bus.disconnect();
                m_bus.release();
            } catch (Exception e)
            {
                m_logger.error(TAG, "Shutdown failed to stop service, Error: " + e.getMessage());
            }
        }

        // ------------------------ ConfigService callbacks --------------------------------

        /* (non-Javadoc)
         * @see org.alljoyn.config.server.PassphraseChangedListener#onPassphraseChanged(byte[])
         */
        @Override
        public void onPassphraseChanged(byte[] passphrase)
        {
            showToast(TAG + ": Passphrase Changed");
        }

        /* (non-Javadoc)
         * @see org.alljoyn.config.server.FactoryResetHandler#doFactoryReset()
         */
        @Override
        public void doFactoryReset()
        {
            m_myPass = SrpAnonymousKeyListener.DEFAULT_PINCODE; // restart the password to default.
            m_bus.clearKeyStore();
            ((OnboardingServiceImpl)m_onboardingService).reset(); // go back to soft AP.
            showToast(TAG + ": Factory Reset");
        }

        /* (non-Javadoc)
         * @see org.alljoyn.config.server.ConfigChangeListener#onConfigChanged(java.util.Map, java.lang.String)
         */
        @Override
        public void onConfigChanged(Map<String, Variant> configuration, String languageTag)
        {
            showToast(TAG + ": Configuration Changed");
            AboutServiceImpl.getInstance().announce();
        }

        /* (non-Javadoc)
         * @see org.alljoyn.config.server.RestartHandler#restart()
         */
        @Override
        public void restart()
        {

            try {
                m_asyncHandler.sendEmptyMessage(AsyncHandler.DISCONNECT);
                m_asyncHandler.sendEmptyMessage(AsyncHandler.CONNECT);
                showToast(TAG + ": Restart. AJ disconnected");

            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        /* (non-Javadoc)
         * @see org.alljoyn.config.server.ConfigChangeListener#onResetConfiguration(java.lang.String, java.lang.String[])
         */
        @Override
        public void onResetConfiguration(String language, String[] fieldsToRemove) {
            showToast(TAG + ": onResetConfiguration was called");
            AboutServiceImpl.getInstance().announce();
        }
    }

    public void showToast(String text)
    {
        System.out.println(text);
        //		Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
    }

    private String bindSessionPort(final short port)
    {
        /*
         * Create a new session listening on the contact port of the about/config service.
         */
        Mutable.ShortValue contactPort = new Mutable.ShortValue(port);

        SessionOpts sessionOpts = new SessionOpts();
        sessionOpts.traffic = SessionOpts.TRAFFIC_MESSAGES;
        sessionOpts.isMultipoint = true;
        sessionOpts.proximity = SessionOpts.PROXIMITY_ANY;
        sessionOpts.transports = SessionOpts.TRANSPORT_ANY;

        Status status = m_bus.bindSessionPort(contactPort, sessionOpts, new SessionPortListener() {
            @Override
            public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts sessionOpts) {
                if (sessionPort == port) {
                    return true;
                } else {
                    return false;
                }
            }
            @Override
            public void sessionJoined(short sessionPort, int id, String joiner){
                Log.i(TAG, String.format("SessionPortListener.sessionJoined(%d, %d, %s)", sessionPort, id, joiner));
                ((OnboardingServiceImpl) OnboardingServiceImpl.getInstance()).setSessionParameters(id, joiner);
            }
        });

        String logMessage = String.format("BusAttachment.bindSessionPort(%d, %s): %s",
                contactPort.value, sessionOpts.toString(),status);
        Log.d(TAG, logMessage);
        if (status != Status.OK) {
            return "";
        }

        String serviceName = m_bus.getUniqueName();

        status = m_bus.advertiseName(serviceName, SessionOpts.TRANSPORT_ANY);
        Log.d(TAG, String.format("BusAttachement.advertiseName(%s): %s", serviceName, status));

        return serviceName;
    }

    // ------------------------ Authentication callbacks --------------------------------
    // Called by AllJoyn when we access secure interfaces like the Config and Onboarding services

    /* (non-Javadoc)
     * @see org.alljoyn.login.dashboard.security.AuthPasswordHandler#getPassword(java.lang.String)
     */
    @Override
    public char[] getPassword(String peerName) {
        return m_myPass != null ?  m_myPass : SrpAnonymousKeyListener.DEFAULT_PINCODE;
    }

    /* (non-Javadoc)
     * @see org.alljoyn.config.server.SetPasswordHandler#setPassword(java.lang.String, char[])
     */
    @Override
    public void setPassword(String realmName, char[] password) {
        m_myPass = password;
        m_bus.clearKeyStore(); //remove all encryption keys.
    }

    /* (non-Javadoc)
     * @see org.alljoyn.login.dashboard.security.AuthPasswordHandler#completed(java.lang.String, java.lang.String, boolean)
     */
    @Override
    public void completed(String mechanism, String authPeer, boolean authenticated) {

        if(!authenticated) {
            m_logger.info(TAG, " ** " + authPeer + " failed to authenticate");
        } else {
            m_logger.info(TAG, " ** " + authPeer + " successfully authenticated");
        }
    }

}
