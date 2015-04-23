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

package org.alljoyn.onboarding.sdk.sampleApp;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.alljoyn.onboarding.OnboardingService.AuthType;
import org.alljoyn.onboarding.sdk.OffboardingConfiguration;
import org.alljoyn.onboarding.sdk.OnboardingConfiguration;
import org.alljoyn.onboarding.sdk.OnboardingIllegalArgumentException;
import org.alljoyn.onboarding.sdk.OnboardingIllegalStateException;
import org.alljoyn.onboarding.sdk.OnboardingManager;
import org.alljoyn.onboarding.sdk.OnboardingManager.OnboardingState;
import org.alljoyn.onboarding.sdk.OnboardingManager.WifiFilter;
import org.alljoyn.onboarding.sdk.WiFiNetwork;
import org.alljoyn.onboarding.sdk.WiFiNetworkConfiguration;
import org.alljoyn.onboarding.sdk.WifiDisabledException;
import org.alljoyn.onboarding.sdk.sampleApp.ProtocolManager.Device;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;

/**
 * Test the onboardingSDK functionality.
 */
public class MainActivity extends Activity {

    private final String TAG = this.getClass().getSimpleName();

    // can be chosen from get Onboardable Devices alert dialog and Will be used
    // as default for RunOnboarding alert dialog.
    WiFiNetwork selectedOnboardDevice;
    // can be chosen from get Candidate Target Networks alert dialog and Will be
    // used as default for RunOnboarding alert dialog.
    WiFiNetwork selectedTargetNetwork;

    // List and List adapter used in the log list.
    ListView logList;
    ArrayAdapter<String> logListAdapter;
    private boolean isOffboarding = false;

    private IntentFilter mainFilter;

    /**
     * Listen to the two main OnboardingManager Intents. Log intents with the
     * action "OnboardingManager.STATE_CHANGE_ACTION" in a list Display alert
     * dialog for intents with the action "OnboardingManager.ERROR".
     */
    private final BroadcastReceiver mainReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Bundle extras = intent.getExtras();
            String intentString = "Action = " + intent.getAction() + (extras != null ? bundleToString(extras) : "");
            Log.i(TAG, intentString);
            if (intent.getAction().equals(OnboardingManager.STATE_CHANGE_ACTION)) {

                logList.setAdapter(logListAdapter);
                logListAdapter.add(intentString);

                if (extras != null && extras.containsKey(OnboardingManager.EXTRA_ONBOARDING_STATE)) {
                    String value = extras.getString(OnboardingManager.EXTRA_ONBOARDING_STATE);
                    if (value != null && !value.isEmpty()) {
                        if (OnboardingState.ABORTED.toString().equals(value)) {
                            showSuccessMessage("Success", "Abort has been completed");
                        } else if (OnboardingState.VERIFIED_ONBOARDED.toString().equals(value)) {
                            showSuccessMessage("Success", "Onboarding process completed");
                        } else if (OnboardingState.CONFIGURED_ONBOARDEE.toString().equals(value) && isOffboarding) {
                            showSuccessMessage("Success", "Offboarding process completed");                          
                        }
                    }
                }

            } else if (intent.getAction().equals(OnboardingManager.ERROR)) {

                showErrorMessage(getString(R.string.error), intentString);
            }
        }
    };

    static {
        try {
            // load alljoyn lib.
            System.loadLibrary("alljoyn_java");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Display alert dialog informing error message.
     */
    private void showErrorMessage(String title, String message) {
        showMessage(title, message, R.drawable.error);
    }

    private void showSuccessMessage(String title, String message) {
        showMessage(title, message, R.drawable.success);
    }

    private void showMessage(String title, String message, int resourceID) {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setIcon(resourceID);
        builder.setTitle(title);
        builder.setMessage(message);
        builder.setNegativeButton(R.string.dismiss, null);
        builder.show();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ProtocolManager.getInstance().init(getApplicationContext());

        setContentView(R.layout.main_layout);

        Button scanWifiButton = (Button) findViewById(R.id.scan_wifi);
        scanWifiButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleScanWifiOnClick();
            }
        });

        Button getAllNetworksButton = (Button) findViewById(R.id.get_all_networks);
        getAllNetworksButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleGetAllNetworksOnClick();
            }
        });

        Button getCandidateTargetNetworksButton = (Button) findViewById(R.id.get_candidate_target_networks);
        getCandidateTargetNetworksButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleGetCandidateTargetOnClick();
            }
        });

        Button getOnboardableDevicesButton = (Button) findViewById(R.id.get_onboardable_devices);
        getOnboardableDevicesButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleGetOnboardableOnClick();
            }
        });

        Button connectToNetworkButton = (Button) findViewById(R.id.connect_to_network);
        connectToNetworkButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleConnectToNetworkOnClick();
            }
        });

        Button getCurrentNetworkButton = (Button) findViewById(R.id.get_current_network);
        getCurrentNetworkButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleGetCurrentNetworkOnClick();
            }
        });

        Button runOnboardingButton = (Button) findViewById(R.id.run_onboarding);
        runOnboardingButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleRunOnboardingOnClick();
            }

        });

        Button RunOffboardingButton = (Button) findViewById(R.id.run_offboarding);
        RunOffboardingButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleRunOffboardingOnClick();
            }
        });

        Button abortOnboardingButton = (Button) findViewById(R.id.abort_onboarding);
        abortOnboardingButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                handleAbortOnboardingOnClick();
            }

        });

        logList = (ListView) findViewById(R.id.main_log_list);
        logListAdapter = new ArrayAdapter<String>(MainActivity.this, android.R.layout.simple_list_item_1);

        // Creates new IntentFilter and add two main OnboardingManager Actions
        // to
        // it.
        mainFilter = new IntentFilter();
        mainFilter.addAction(OnboardingManager.STATE_CHANGE_ACTION);
        mainFilter.addAction(OnboardingManager.ERROR);
    }

    @Override
    public void onPause() {
        super.onPause();
        unregisterReceiver(mainReceiver);
    }

    @Override
    public void onResume() {
        super.onResume();
        registerReceiver(mainReceiver, mainFilter);
    }

    /**
     * shutdown the OnboardingManager
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();

        try {

            OnboardingManager.getInstance().shutDown();
        } catch (OnboardingIllegalStateException e) {
            try {
                OnboardingManager.getInstance().abortOnboarding();
                OnboardingManager.getInstance().shutDown();
            } catch (OnboardingIllegalStateException e1) {
                e1.printStackTrace();
            }
            e.printStackTrace();
        }
        ProtocolManager.getInstance().disconnectFromBus();
    }

    /**
     * Perform Wi-Fi scanning using OnboardingManager and display alert when
     * done
     */
    private void handleScanWifiOnClick() {
        final ProgressDialog progressDialog = new ProgressDialog(MainActivity.this);
        progressDialog.setMessage(getString(R.string.wifi_activity_scanning));

        BroadcastReceiver wifireceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                unregisterReceiver(this);
                progressDialog.dismiss();
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setTitle(R.string.alert_title_scan_wifi_completed);
                builder.setMessage(String.format(getString(R.string.alert_message_scan_wifi_completed), getString(R.string.get_all_networks), getString(R.string.get_candidate_target_networks),
                        getString(R.string.get_onboardable_devices)));
                builder.setNegativeButton(R.string.dismiss, null);
                builder.show();
            }
        };

        IntentFilter wifiFilter = new IntentFilter(OnboardingManager.WIFI_SCAN_RESULTS_AVAILABLE_ACTION);
        registerReceiver(wifireceiver, wifiFilter);

        try {
            OnboardingManager.getInstance().scanWiFi();
            progressDialog.show();
        } catch (WifiDisabledException e) {
            showErrorMessage(getString(R.string.alert_title_wifi_error), getString(R.string.alert_msg_wifi_disabled));
            progressDialog.dismiss();
            e.printStackTrace();
        }
    }

    /**
     * Display alert dialog that will receive data and call the connectToNetwork
     * function.
     */
    private void handleConnectToNetworkOnClick() {

        // Clear the log list
        logListAdapter.clear();

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.alert_title_connect_to_network);
        LayoutInflater inflater = MainActivity.this.getLayoutInflater();

        // Initialize view fields
        View networkCredentialsLayout = inflater.inflate(R.layout.connect_to_network_layout, null);
        final EditText ssidEditText = (EditText) networkCredentialsLayout.findViewById(R.id.network_credentials_ssid_editText);
        final EditText passwordEditText = (EditText) networkCredentialsLayout.findViewById(R.id.network_credentials_password_editText);
        final Spinner authTypeSpinner = (Spinner) networkCredentialsLayout.findViewById(R.id.network_credentials_layout_authType_spinner);
        final EditText timeoutEditText = (EditText) networkCredentialsLayout.findViewById(R.id.network_credentials_timeout_editText);
        timeoutEditText.setText(String.valueOf(OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT));

        // prepare authentication type spinner
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this, R.array.auth_types, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        authTypeSpinner.setAdapter(adapter);

        builder.setView(networkCredentialsLayout);
        builder.setNegativeButton(R.string.cancel, null);
        builder.setPositiveButton(R.string.connect, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                // Read the parameters from the view fields and call
                // connectToNetwork. We don't verify the arguments
                // legality, to
                // verify it's done properly by the SDK.
                String ssid = ssidEditText.getText().toString();
                String password = passwordEditText.getText().toString();
                AuthType authType = AuthType.valueOf((String) authTypeSpinner.getSelectedItem());
                int timeout = Integer.valueOf(timeoutEditText.getText().toString());

                WiFiNetworkConfiguration wifi = new WiFiNetworkConfiguration(ssid, authType, password);
                try {
                    OnboardingManager.getInstance().connectToNetwork(wifi, timeout);
                } catch (WifiDisabledException e) {
                    showErrorMessage(getString(R.string.alert_title_wifi_error), getString(R.string.alert_msg_wifi_disabled));
                    e.printStackTrace();
                } catch (OnboardingIllegalArgumentException e) {
                    showErrorMessage("ConnectToNetwork error", "invalid wifi data");
                    e.printStackTrace();
                }
            }
        });
        builder.show();

    }

    /**
     * Display list of all available networks.
     */
    private void handleGetAllNetworksOnClick() {
        final List<WiFiNetwork> allNetworks = OnboardingManager.getInstance().getWifiScanResults(WifiFilter.ALL);
        String[] ssid = new String[allNetworks.size()];
        for (int i = 0; i < ssid.length; i++) {
            WiFiNetwork network = allNetworks.get(i);
            ssid[i] = String.format(getString(R.string.alert_generic_network_item), network.getSSID(), network.getAuthType(), network.getLevel());
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.get_all_networks);
        builder.setItems(ssid, null);
        builder.setNegativeButton(R.string.dismiss, null);
        builder.show();
    }

    /**
     * Display list of the candidate onboarding target network (Wi-Fi access
     * point name that doesn't contain the following prefix AJ_ or suffix _AJ
     */

    private void handleGetCandidateTargetOnClick() {
        final List<WiFiNetwork> candidateTargetNetworks = OnboardingManager.getInstance().getWifiScanResults(WifiFilter.TARGET);
        String[] ssid = new String[candidateTargetNetworks.size()];
        for (int i = 0; i < ssid.length; i++) {
            WiFiNetwork network = candidateTargetNetworks.get(i);
            ssid[i] = String.format(getString(R.string.alert_generic_network_item), network.getSSID(), network.getAuthType(), network.getLevel());
        }

        int selectedTargetIndex = selectedTargetNetwork == null ? -1 : candidateTargetNetworks.indexOf(selectedTargetNetwork);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.candidate_targets);
        builder.setSingleChoiceItems(ssid, selectedTargetIndex, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                selectedTargetNetwork = candidateTargetNetworks.get(which);
            }
        });
        builder.setNegativeButton(R.string.dismiss, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                selectedTargetNetwork = null;
            }
        });
        builder.setPositiveButton(R.string.select, null);
        builder.show();
    }

    /**
     * Display AlertDialog with info about the current connected network.
     */
    private void handleGetCurrentNetworkOnClick() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        WiFiNetwork current = null;
        try {
            current = OnboardingManager.getInstance().getCurrentNetwork();
        } catch (WifiDisabledException e) {
            showErrorMessage(getString(R.string.alert_title_wifi_error), getString(R.string.alert_msg_wifi_disabled));
            e.printStackTrace();
            return;
        }
        builder.setTitle(R.string.alert_title_get_current_network);

        if (current == null) {
            builder.setMessage(getString(R.string.getcurrentnetwork_returned_null));
        } else {
            builder.setMessage(String.format(getString(R.string.alert_msg_get_current_network), current.getSSID(), current.getAuthType(), current.getLevel()));
        }
        builder.setNegativeButton(R.string.dismiss, null);
        builder.show();
    }

    /**
     * Display list of onboardable access points (Wi-Fi access point name that
     * contains the following prefix AJ_ or suffix _AJ
     */
    private void handleGetOnboardableOnClick() {
        final List<WiFiNetwork> onboardableDevices = OnboardingManager.getInstance().getWifiScanResults(WifiFilter.ONBOARDABLE);
        String[] ssid = new String[onboardableDevices.size()];
        for (int i = 0; i < ssid.length; i++) {
            WiFiNetwork network = onboardableDevices.get(i);
            ssid[i] = String.format(getString(R.string.alert_generic_network_item), network.getSSID(), network.getAuthType(), network.getLevel());
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.onboardable_devices);
        int selectedOnboardeeIndex = selectedOnboardDevice == null ? -1 : onboardableDevices.indexOf(selectedOnboardDevice);
        builder.setSingleChoiceItems(ssid, selectedOnboardeeIndex, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                selectedOnboardDevice = onboardableDevices.get(which);
            }
        });

        builder.setNegativeButton(R.string.dismiss, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                selectedOnboardDevice = null;
            }
        });
        builder.setPositiveButton(R.string.select, null);
        builder.show();
    }

    /**
     * Display AlertDialog Pre-filled with selected device from
     * "Get Onboardable Devices" list, Selected target from
     * "Get Candidate Terget" list and default timeout from OnboardingManager.
     * Call runOnboarding with the given parameters.
     */
    private void handleRunOnboardingOnClick() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.run_onboarding);
        LayoutInflater inflater = MainActivity.this.getLayoutInflater();

        View networkCredentialsLayout = inflater.inflate(R.layout.run_onboarding_layout, null);
        // Initialize Onboardee view fields
        final EditText onboardeeSsidEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_onboardee_ssid_editText);
        final EditText onboardeePasswordEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_onboardee_password_editText);
        final Spinner onboardeeAuthTypeSpinner = (Spinner) networkCredentialsLayout.findViewById(R.id.run_onboarding_onboardee_authType_spinner);
        final EditText onboardeeAnnouncementTimeoutEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_onboardeeAnnoucementTimeoutMsec);
        final EditText onboardeeConnectionTimeoutEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_onboardeeConnectionTimeoutMsec);

        // Initialize target view fields
        final EditText targetSsidEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_target_ssid_editText);
        final EditText targetPasswordEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_target_password_editText);
        final Spinner targetAuthTypeSpinner = (Spinner) networkCredentialsLayout.findViewById(R.id.run_onboarding_target_authType_spinner);
        final EditText targetAnnouncementTimeoutEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_targrt_anoucement_timeout);
        final EditText targetConnectionTimeoutEditText = (EditText) networkCredentialsLayout.findViewById(R.id.run_onboarding_targrt_connection_timeout);

        final CheckBox onboardeeHidden = (CheckBox) networkCredentialsLayout.findViewById(R.id.onboardee_hidden);
        final CheckBox targetHidden = (CheckBox) networkCredentialsLayout.findViewById(R.id.target_hidden);

        // prepare adapter for the Authentication type spinners
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this, R.array.auth_types, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        onboardeeAuthTypeSpinner.setAdapter(adapter);
        targetAuthTypeSpinner.setAdapter(adapter);

        // Set default values for onboardee using the selectedOnboardDevice
        if (selectedOnboardDevice != null) {
            onboardeeSsidEditText.setText(selectedOnboardDevice.getSSID());
            AuthType authType = selectedOnboardDevice.getAuthType();
            onboardeeAuthTypeSpinner.setSelection(getIndexForAuthType(authType));
        }
        onboardeeConnectionTimeoutEditText.setText(String.valueOf(OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT));
        onboardeeAnnouncementTimeoutEditText.setText(String.valueOf(OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT));

        // Set default values for onboardee using the selectedTargetNetwork
        if (selectedTargetNetwork != null) {
            targetSsidEditText.setText(selectedTargetNetwork.getSSID());
            AuthType authType = selectedTargetNetwork.getAuthType();
            targetAuthTypeSpinner.setSelection(getIndexForAuthType(authType));
        }
        targetConnectionTimeoutEditText.setText(String.valueOf(OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT));
        targetAnnouncementTimeoutEditText.setText(String.valueOf(OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT));

        builder.setView(networkCredentialsLayout);
        builder.setNegativeButton(R.string.cancel, null);
        builder.setPositiveButton(R.string.onboard, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                // Clear the log list
                logListAdapter.clear();

                // Read the onboardee parameters from the view fields.
                String onboardeeSsid = (onboardeeSsidEditText.getText() != null ? onboardeeSsidEditText.getText().toString() : "");

                String onboardeePassword = (onboardeePasswordEditText.getText() != null ? onboardeePasswordEditText.getText().toString() : "");
                AuthType onboardeeAuthType = AuthType.valueOf((String) onboardeeAuthTypeSpinner.getSelectedItem());
                int onboardeeWifiTimeout = (onboardeeConnectionTimeoutEditText.getText() != null && onboardeeConnectionTimeoutEditText.getText().length() > 0 ? Integer
                        .valueOf(onboardeeConnectionTimeoutEditText.getText().toString()) : OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT);
                int onboardeeAnnouncementTimeout = (onboardeeAnnouncementTimeoutEditText.getText() != null && onboardeeAnnouncementTimeoutEditText.getText().length() > 0 ? Integer
                        .valueOf(onboardeeAnnouncementTimeoutEditText.getText().toString()) : OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT);

                // Read the target parameters from the view fields.
                String targetSsid = (targetSsidEditText.getText() != null ? targetSsidEditText.getText().toString() : "");
                String targetPassword = (targetPasswordEditText.getText() != null ? targetPasswordEditText.getText().toString() : "");
                AuthType targetAuthType = AuthType.valueOf((String) targetAuthTypeSpinner.getSelectedItem());
                int targetWifiTimeout = (targetConnectionTimeoutEditText.getText() != null && targetConnectionTimeoutEditText.getText().length() > 0 ? Integer.valueOf(targetConnectionTimeoutEditText
                        .getText().toString()) : OnboardingManager.DEFAULT_WIFI_CONNECTION_TIMEOUT);
                int targetAnnouncementTimeout = (targetAnnouncementTimeoutEditText.getText() != null && targetAnnouncementTimeoutEditText.getText().length() > 0 ? Integer
                        .valueOf(targetAnnouncementTimeoutEditText.getText().toString()) : OnboardingManager.DEFAULT_ANNOUNCEMENT_TIMEOUT);

                // Call the runOnboarding function with the relevant
                // parameters.
                // We don't verify the arguments legality, to verify
                // it's done
                // properly by the SDK.
                WiFiNetworkConfiguration onboardee = new WiFiNetworkConfiguration(onboardeeSsid, onboardeeAuthType, onboardeePassword, onboardeeHidden.isChecked());
                WiFiNetworkConfiguration target = new WiFiNetworkConfiguration(targetSsid, targetAuthType, targetPassword, targetHidden.isChecked());
                OnboardingConfiguration config = new OnboardingConfiguration(onboardee, onboardeeWifiTimeout, onboardeeAnnouncementTimeout, target, targetWifiTimeout, targetAnnouncementTimeout);

                try {
                    OnboardingManager.getInstance().runOnboarding(config);
                    isOffboarding = false;
                } catch (OnboardingIllegalArgumentException e) {
                    showErrorMessage(getString(R.string.alert_title_runonboarding_error), getString(R.string.alert_msg_invalid_configuration));
                    e.printStackTrace();
                } catch (OnboardingIllegalStateException e) {
                    showErrorMessage(getString(R.string.alert_title_runonboarding_error), getString(R.string.alert_msg_runonboarding_invalid_state));
                    e.printStackTrace();
                } catch (WifiDisabledException e) {
                    showErrorMessage(getString(R.string.alert_title_wifi_error), getString(R.string.alert_msg_wifi_disabled));
                    e.printStackTrace();
                }
            }
        });
        AlertDialog alertDialog = builder.create();
        alertDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
        alertDialog.show();
    }

    /**
     * Display a list represent all the available onboarded devices on the
     * current network. Select item will allow the user to offboard the selected
     * device.
     */
    private void handleRunOffboardingOnClick() {       
        final List<Device> devicesList = new ArrayList<Device>(ProtocolManager.getInstance().getDeviceList()); // clone the list, so that UI has an immutable copy.
        String[] deviceArray = new String[devicesList.size()];
        for (int i = 0; i < deviceArray.length; i++) {
            Device currDevice = devicesList.get(i);
            deviceArray[i] = String.format(getString(R.string.alert_item_onboarded_devices), currDevice.name, currDevice.serviceName, String.valueOf(currDevice.port));
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.alert_title_onboarded_devices);

        LayoutInflater inflater = MainActivity.this.getLayoutInflater();
        View offboardinglayout = inflater.inflate(R.layout.run_offboarding_layout, null);
        final EditText busNameEditText = (EditText) offboardinglayout.findViewById(R.id.offboarding_service_name_edit_text);
        final EditText portEditText = (EditText) offboardinglayout.findViewById(R.id.offboarding_port_edit_text);
        builder.setView(offboardinglayout);
        builder.setSingleChoiceItems(deviceArray, -1, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                Device selectedOffboardDevice = devicesList.get(which);
                busNameEditText.setText(selectedOffboardDevice.serviceName);
                portEditText.setText(String.valueOf(selectedOffboardDevice.port));
            }
        });
        builder.setNegativeButton(R.string.dismiss, null);
        builder.setPositiveButton(R.string.offboard, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (busNameEditText.getText() != null && portEditText.getText() != null && portEditText.getText().length() > 0) {
                    // Clear the log list
                    logListAdapter.clear();

                    // We don't verify the arguments legality, to verify
                    // it's
                    // done properly by the SDK.
                    OffboardingConfiguration config = new OffboardingConfiguration(busNameEditText.getText().toString(), Short.valueOf(portEditText.getText().toString()));
                    try {
                        OnboardingManager.getInstance().runOffboarding(config);
                        isOffboarding = true;
                    } catch (OnboardingIllegalArgumentException e) {
                        showErrorMessage(getString(R.string.alert_title_runoffboarding_error), getString(R.string.alert_msg_invalid_configuration));
                        e.printStackTrace();
                    } catch (OnboardingIllegalStateException e) {
                        showErrorMessage(getString(R.string.alert_title_runoffboarding_error), getString(R.string.alert_msg_runonboarding_invalid_state));
                        e.printStackTrace();
                    } catch (WifiDisabledException e) {
                        showErrorMessage(getString(R.string.alert_title_wifi_error), getString(R.string.alert_msg_wifi_disabled));
                        e.printStackTrace();
                    }
                   
                } else {
                    showErrorMessage(getString(R.string.alert_title_runoffboarding_error), getString(R.string.alert_msg_invalid_configuration));
                }
            }
        });
        builder.show();
    }

    /**
     * Call abortOnboarding at the OnboardingManager
     */
    private void handleAbortOnboardingOnClick() {

        try {
            OnboardingManager.getInstance().abortOnboarding();

            // Clear the log list
            logListAdapter.clear();
        } catch (OnboardingIllegalStateException e) {
            showErrorMessage(getString(R.string.alert_title_abort_error), e.getMessage());
            e.printStackTrace();
        }
    }

    /**
     * receive AuthType and return this AuthType index in the authTypeNames
     * array.
     *
     * @param authType
     * @return AuthType index
     */
    private int getIndexForAuthType(AuthType authType) {
        if (authType == null) {
            return -1;
        }

        String[] authTypeNames = getResources().getStringArray(R.array.auth_types);
        for (int i = 0; i < authTypeNames.length; i++) {
            if (authType.toString().equals(authTypeNames[i])) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Utility to flatten bundle to String. Required for the log list.
     */
    private String bundleToString(Bundle bundle) {
        if (bundle == null) {
            return "";
        }
        Set<String> keys = bundle.keySet();
        if (keys == null || keys.size() == 0) {
            return "";
        }
        StringBuilder s = new StringBuilder();
        for (String key : keys) {
            s.append("  " + key + " = " + bundle.get(key));
        }
        return s.toString();
    }
}
