package org.alljoyn.ioe.controlpanelbrowser;

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

import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.ioe.controlpanelbrowser.DeviceList.DeviceContext;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ControlPanel;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.HTTPControl;
import org.alljoyn.services.android.security.AuthPasswordHandler;
import org.alljoyn.services.android.security.SrpAnonymousKeyListener;
import org.alljoyn.services.common.utils.GenericLogger;
import org.alljoyn.services.common.utils.TransportUtil;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.support.v4.app.ListFragment;
import android.text.InputType;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

/**
 * A list fragment representing a list of Appliances. This fragment also
 * supports tablet devices by allowing list items to be given an 'activated'
 * state upon selection. This helps indicate which item is currently being
 * viewed in a {@link DeviceDetailFragment}.
 * <p>
 * Activities containing this fragment MUST implement the
 * {@link DeviceListCallback} interface.
 */
public class DeviceListFragment extends ListFragment {

    /**
     * The serialization (saved instance state) Bundle key representing the
     * activated item position. Only used on tablets.
     */
    private static final String STATE_ACTIVATED_POSITION = "activated_position";

    /**
     * The fragment's current callback object, which is notified of list item
     * clicks.
     */
    private DeviceListCallback mCallbacks = sDummyCallbacks;

    /**
     * The current activated item position. Only used on tablets.
     */
    private int mActivatedPosition = ListView.INVALID_POSITION;

    /**
     * A callback interface that all activities containing this fragment must
     * implement. This mechanism allows activities to be notified of item
     * selections.
     */
    public interface DeviceListCallback {
        /**
         * Callback for when an item has been selected.
         */
        public void onItemSelected(DeviceContext context);
    }

    /**
     * A dummy implementation of the {@link DeviceListCallback} interface that
     * does nothing. Used only when this fragment is not attached to an
     * activity.
     */
    private static DeviceListCallback sDummyCallbacks = new DeviceListCallback() {
        @Override
        public void onItemSelected(DeviceContext context) {
        }
    };

    /**
     * For logging
     */
    private final static String TAG = "cpappApplianceListFragment";

    /**
     * A Handler for handling AllJoyn connection and disconnection on a
     * separated thread.
     */
    private AsyncHandler handler;

    /**
     * A device registry
     */
    private DeviceList deviceRegistry;

    /**
     * The AllJoyn bus attachment
     */
    private BusAttachment bus;

    /**
     * The AllJoyn daemon advertises itself so thin clients can connect to it.
     * This is the known prefix of the daemon advertisement.
     */
    private static final String DAEMON_NAME_PREFIX = "org.alljoyn.BusNode";

    /**
     * The daemon should advertise itself "quietly" (directly to the calling
     * port) This is to reply directly to a TC looking for a daemon
     */
    private static final String DAEMON_QUIET_PREFIX = "quiet@";

    /**
     * The password for authentication with a remote secured Interface
     */
    private static final String PREFS_NAME = "MyPrefsFile";
    private static final String PREFS_PASSWORD = "CPB_PASS";
    private static final String DEFAULT_SECURED_SRP_PASSWORD = "000000";
    private String srpPassword = DEFAULT_SECURED_SRP_PASSWORD;

    /**
     * Supported Authentication mechanisms
     */
    private final String[] AUTH_MECHANISMS = new String[] { "ALLJOYN_SRP_KEYX", "ALLJOYN_PIN_KEYX", "ALLJOYN_ECDHE_PSK" };

    private static final String[] ANNOUNCE_IFACES = new String[] { ControlPanel.IFNAME, HTTPControl.IFNAME };

    /**
     * Load the native alljoyn_java library.
     */
    static {
        System.loadLibrary("alljoyn_java");
    }

    /**
     * Reference to Logger
     */
    private final GenericLogger logger = new GenericLogger() {
        @Override
        public void debug(String TAG, String msg) {
            Log.d(TAG, msg);
        }

        @Override
        public void info(String TAG, String msg) {
            Log.i(TAG, msg);
        }

        @Override
        public void warn(String TAG, String msg) {
            Log.w(TAG, msg);
        }

        @Override
        public void error(String TAG, String msg) {
            Log.e(TAG, msg);
        }

        @Override
        public void fatal(String TAG, String msg) {
            Log.wtf(TAG, msg);
        }
    };

    /**
     * Mandatory empty constructor for the fragment manager to instantiate the
     * fragment (e.g. upon screen orientation changes).
     */
    public DeviceListFragment() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setHasOptionsMenu(true);

        deviceRegistry = new DeviceList();

        HandlerThread busThread = new HandlerThread("BusHandler");
        busThread.start();
        handler = new AsyncHandler(busThread.getLooper());
        handler.sendEmptyMessage(AsyncHandler.CONNECT);
    }

    @Override
    public void onDestroy() {
        // Disconnect to prevent any resource leaks.
        handler.disconnect();
        handler.getLooper().quit();
        super.onDestroy();
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        // Restore the previously serialized activated item position.
        if (savedInstanceState != null && savedInstanceState.containsKey(STATE_ACTIVATED_POSITION)) {
            setActivatedPosition(savedInstanceState.getInt(STATE_ACTIVATED_POSITION));
        }
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        // Activities containing this fragment must implement its callbacks.
        if (!(activity instanceof DeviceListCallback)) {
            throw new IllegalStateException("Activity must implement fragment's callbacks.");
        }

        mCallbacks = (DeviceListCallback) activity;
    }

    @Override
    public void onDetach() {
        super.onDetach();

        // Reset the active callbacks interface to the dummy implementation.
        mCallbacks = sDummyCallbacks;
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        // TODO Add your menu entries here
        super.onCreateOptionsMenu(menu, inflater);
        // Inflate the menu; this adds items to the action bar if it is present.
        inflater.inflate(R.menu.activity_control_panel, menu);

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        if (item.getItemId() == R.id.menu_set_password) {
            showSetPasswordDialog();
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public void onListItemClick(ListView listView, View view, int position, long id) {
        super.onListItemClick(listView, view, position, id);

        // Notify the active callbacks interface (the activity, if the
        // fragment is attached to one) that an item has been selected.
        mCallbacks.onItemSelected(deviceRegistry.getContexts().get(position));
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mActivatedPosition != ListView.INVALID_POSITION) {
            // Serialize and persist the activated item position.
            outState.putInt(STATE_ACTIVATED_POSITION, mActivatedPosition);
        }
    }

    /**
     * Turns on activate-on-click mode. When this mode is on, list items will be
     * given the 'activated' state when touched.
     */
    public void setActivateOnItemClick(boolean activateOnItemClick) {
        // When setting CHOICE_MODE_SINGLE, ListView will automatically
        // give items the 'activated' state when touched.
        getListView().setChoiceMode(activateOnItemClick ? ListView.CHOICE_MODE_SINGLE : ListView.CHOICE_MODE_NONE);
    }

    private void setActivatedPosition(int position) {
        if (position == ListView.INVALID_POSITION) {
            getListView().setItemChecked(mActivatedPosition, false);
        } else {
            getListView().setItemChecked(position, true);
        }

        mActivatedPosition = position;
    }

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class AsyncHandler extends Handler implements AboutListener, AuthPasswordHandler {

        public static final int CONNECT = 1;
        public static final int DISCONNECT = 2;

        public AsyncHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {

            /* Connect to the bus and start our service. */
            case CONNECT: {
                connect();
                break;
            }

            /* Release all resources acquired in connect. */
            case DISCONNECT: {
                disconnect();
                break;
            }

            }
        }

        // =================================== Connect
        // ============================================

        /**
         * Connect to the bus and start our services.
         */
        private void connect() {
            Log.d(TAG, "connect()");

            /* initialize AllJoyn BusAttachment. */
            bus = new BusAttachment("ControlPanelBrowser", BusAttachment.RemoteMessage.Receive);

            // Pump up the daemon debug level

            // bus.setDaemonDebug("ALL", 7);
            // bus.setLogLevels("ALLJOYN=7");
            // bus.useOSLogging(true);

            // load the password for accessing secured interfaces on the board
            SharedPreferences settings = getActivity().getSharedPreferences(PREFS_NAME, 0);
            srpPassword = settings.getString(PREFS_PASSWORD, DEFAULT_SECURED_SRP_PASSWORD);

            Log.d(TAG, "Setting the AuthListener");

            SrpAnonymousKeyListener authListener = new SrpAnonymousKeyListener(this, logger, AUTH_MECHANISMS);
            Status authStatus = bus.registerAuthListener(authListener.getAuthMechanismsAsString(), authListener, getKeyStoreFileName());

            if (authStatus != Status.OK) {
                Log.e(TAG, "Failed to register AuthListener");
            }

            Status status = bus.connect();
            if (Status.OK == status) {
                Log.d(TAG, "BusAttachment.connect(): ok. BusUniqueName: " + bus.getUniqueName());

                // request the name
                String daemonName = DAEMON_NAME_PREFIX + ".ControlPanelBrowser.G" + bus.getGlobalGUIDString();
                int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;
                Status reqStatus = bus.requestName(daemonName, flag);
                if (reqStatus == Status.OK) {

                    // advertise the name with a quite prefix for thin clients
                    // to find it
                    Status adStatus = bus.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);
                    if (adStatus != Status.OK) {
                        bus.releaseName(daemonName);
                        Log.e(TAG, "Failed to advertise daemon name: '" + daemonName + "', Error: '" + status + "'");
                    } else {
                        Log.d(TAG, "Succefully advertised daemon name: '" + daemonName + "'");
                    }
                } else {
                    Log.e(TAG, "Failed to request daemon name: '" + daemonName + "', Error: '" + status + "'");
                }
            }

            // Initialize AboutService
            bus.registerAboutListener(this);
            for (String iface : ANNOUNCE_IFACES) {
                bus.whoImplements(new String[] { iface });
            }

            // Initialize ControlPanelService
            ControlPanelService controlPanelService = ControlPanelService.getInstance();
            try {
                controlPanelService.init(bus);
                Log.d(TAG, "Initialized ControlPanelService with BusAttachment " + bus.getUniqueName());
            } catch (ControlPanelException e) {
                Log.e(TAG, "Unable to start ControlPanelService, Error: " + e.getMessage());
            }

            Toast.makeText(getActivity(), "Initialized", Toast.LENGTH_SHORT).show();

            // update the list
            refreshListView();

        }

        // =================================== Disconnect
        // ============================================

        /**
         * Release all resources acquired in connect.
         */
        private void disconnect() {
            ControlPanelService.getInstance().shutdown();
            for (String iface : ANNOUNCE_IFACES) {
                bus.cancelWhoImplements(new String[] { iface });
            }
            bus.unregisterAboutListener(this);
            bus.disconnect();
            bus.release();
        }

        // =================================== onAnnouncement
        // ============================================

        /*
         * A callback where About service notifies listeners about a new
         * announcement.
         * 
         * @see
         * org.alljoyn.services.common.AnnouncementHandler#onAnnouncement(java
         * .lang.String, short,
         * org.alljoyn.services.common.BusObjectDescription[], java.util.Map)
         */
        @Override
        public void announced(String busName, int version, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> aboutMap) {

            String deviceId;
            String deviceFriendlyName;
            UUID appId;
            DeviceContext deviceContext = null;

            try {

                Variant varDeviceId = aboutMap.get(AboutKeys.ABOUT_DEVICE_ID);
                String devIdSig = (varDeviceId != null) ? varDeviceId.getSignature() : "";

                if (!devIdSig.equals("s")) {
                    Log.e(TAG, "Received '" + AboutKeys.ABOUT_DEVICE_ID + "', that has an unexpected signature: '" + devIdSig + "', the expected signature is: 's'");
                    return;
                }
                deviceId = varDeviceId.getObject(String.class);

                Variant varFriendlyName = aboutMap.get(AboutKeys.ABOUT_DEVICE_NAME);
                String friendlyNameSig = (varFriendlyName != null) ? varFriendlyName.getSignature() : "";

                if (!friendlyNameSig.equals("s")) {
                    Log.e(TAG, "Received '" + AboutKeys.ABOUT_DEVICE_NAME + "', that has an unexpected signature: '" + friendlyNameSig + "', the expected signature is: 's'");
                    return;
                }
                deviceFriendlyName = varFriendlyName.getObject(String.class);

                Variant varAppId = aboutMap.get(AboutKeys.ABOUT_APP_ID);
                String appIdSig = (varAppId != null) ? varAppId.getSignature() : "";

                if (!appIdSig.equals("ay")) {
                    Log.e(TAG, "Received '" + AboutKeys.ABOUT_APP_ID + "', that has an unexpected signature: '" + appIdSig + "', the expected signature is: 'ay'");
                    return;
                }
                byte[] rawAppId = varAppId.getObject(byte[].class);
                appId = TransportUtil.byteArrayToUUID(rawAppId);

                if (appId == null) {
                    Log.e(TAG, "Received a bad AppId, failed to convert it to the UUID");
                    return;
                }
            } catch (BusException be) {
                Log.e(TAG, "Failed to retreive an Announcement properties, Error: '" + be.getMessage() + "'", be);
                return;
            }

            // scan the object descriptions, pick those who implement a control
            // panel interface
            for (int i = 0; i < objectDescriptions.length; ++i) {

                AboutObjectDescription description = objectDescriptions[i];
                String[] supportedInterfaces = description.interfaces;

                for (int j = 0; j < supportedInterfaces.length; ++j) {

                    if (supportedInterfaces[j].startsWith(ControlPanelService.INTERFACE_PREFIX)) {

                        // found a control panel interface
                        Log.d(TAG, "Adding BusObjectDesciption: " + description);
                        if (deviceContext == null) {
                            deviceContext = new DeviceContext(deviceId, busName, deviceFriendlyName, appId);
                        }

                        deviceContext.addObjectInterfaces(description.path, supportedInterfaces);
                    }
                }
            }

            // add the device context
            if (deviceContext != null) {
                deviceRegistry.addItem(deviceContext);
            }

            // update the list
            refreshListView();
        }

        public void onDeviceLost(String busName) {

            Log.d(TAG, "Received DeviceLost for '" + busName + "'");
            deviceRegistry.onDeviceOffline(busName);

            // update the list
            refreshListView();
        }

        private void refreshListView() {

            getActivity().runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    ArrayAdapter<DeviceContext> arrayAdapter = new ArrayAdapter<DeviceList.DeviceContext>(getActivity(), android.R.layout.simple_list_item_activated_1, android.R.id.text1,
                            deviceRegistry.getContexts());
                    setListAdapter(arrayAdapter);
                    arrayAdapter.notifyDataSetChanged();
                }
            });
        }

        /**
         * @see org.alljoyn.services.android.security.AuthPasswordHandler#getPassword(java.lang.String)
         */
        @Override
        public char[] getPassword(String peer) {

            return srpPassword.toCharArray();
        }

        /**
         * @see org.alljoyn.bus.AuthListener#completed(java.lang.String,
         *      java.lang.String, boolean)
         */
        @Override
        public void completed(String authMechanism, String authPeer, final boolean authenticated) {
            Log.d(TAG, "Authentication completed. peer: '" + authPeer + "', authenticated: " + authenticated + " using mechanism: '" + authMechanism + "'");

            if (authenticated) {
                Log.d(TAG, "The peer: '" + authPeer + "', authenticated successfully for authMechanism: '" + authMechanism + "'");
            } else {
                Log.w(TAG, "The peer: '" + authPeer + "', WAS NOT authenticated for authMechanism: '" + authMechanism + "'");
            }

            if (getActivity() != null)
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getActivity(), "Authenticated: " + authenticated, Toast.LENGTH_SHORT).show();
                    }
                });
        }

        /**
         * Persistent authentication and encryption data is stored at this
         * location.
         * 
         * This uses the private file area associated with the application
         * package.
         */
        public String getKeyStoreFileName() {
            return getActivity().getFileStreamPath("alljoyn_keystore").getAbsolutePath();
        }// getKeyStoreFileName

    }

    private void showSetPasswordDialog() {
        AlertDialog.Builder alert = new AlertDialog.Builder(getActivity());
        alert.setTitle(R.string.enter_device_password);
        alert.setCancelable(false);

        View view = getActivity().getLayoutInflater().inflate(R.layout.enter_password_popup, null);
        final EditText input = (EditText) view.findViewById(R.id.passwordEditText);
        input.setText(srpPassword);
        final CheckBox showPassword = (CheckBox) view.findViewById(R.id.showPasswordCheckBox);
        alert.setView(view);

        showPassword.setOnCheckedChangeListener(new OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

                if (isChecked) {
                    input.setInputType(InputType.TYPE_CLASS_TEXT);
                } else {
                    input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
                }
            }
        });

        alert.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int whichButton) {
                srpPassword = input.getText().toString();
                // store the new password
                SharedPreferences settings = getActivity().getSharedPreferences(PREFS_NAME, 0);
                SharedPreferences.Editor editor = settings.edit();
                editor.putString(PREFS_PASSWORD, srpPassword);
                editor.commit();
            }
        });

        alert.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int whichButton) {
            }
        });
        alert.show();

    }
}
