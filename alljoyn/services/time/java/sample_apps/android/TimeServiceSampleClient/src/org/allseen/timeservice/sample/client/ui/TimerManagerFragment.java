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
package org.allseen.timeservice.sample.client.ui;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.bus.Status;
import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.SessionListenerHandler;
import org.allseen.timeservice.client.TimeServiceClient;
import org.allseen.timeservice.client.Timer;
import org.allseen.timeservice.client.TimerFactory;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.adapters.TimerManagerAdapter;
import org.allseen.timeservice.sample.client.application.DeviceManager;
import org.allseen.timeservice.sample.client.application.DeviceManager.SignalObject;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.TimerItem;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.util.Pair;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnCreateContextMenuListener;
import android.view.ViewGroup;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.ImageView;
import android.widget.ListView;

/**
 * Displays {@link Timer} retrieved from the remote service.
 * <ul>
 * <li>Join a session with the Device.
 * <li>If {@see TimeServiceClient#isTimerAnnounced()} fetch list of
 * {@link Timer} for each one get all properties.
 * <li>If {@see TimeServiceClient#isTimerFactoryAnnounced()} fetch list of
 * {@link TimerFactory} for each fetch list of {@link Timer} for each one get
 * all properties.
 * <li>Fill UI elements.
 * </ul>
 */

public class TimerManagerFragment extends Fragment {
    private static final String TAG = "TimerMangerFragment";
    private static final int EDIT_TIMER = 1;
    private static final int DELETE_TIMER = 2;

    private static final int REGISTER_TIMER_SIGNALS = 3;
    private static final int UNREGISTER_TIMER_SIGNALS = 4;

    private TimerManagerAdapter timerManagerAdapter;
    private Context context;

    private UUID currentDeviceUUID;
    private int selectedItem = -1;
    private FragmentCommunicationInterface fragmentCommunication;
    private BaseActivity baseActivity = null;
    private ProgressDialog progressDialog = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    private class FetchListAsyncTask extends AsyncTask<TimeServiceClient, Void, Pair<String, List<TimerItem>>> {

        @Override
        protected Pair<String, List<TimerItem>> doInBackground(TimeServiceClient... params) {
            TimeServiceClient timeServiceClient = params[0];
            Pair<String, List<TimerItem>> result;
            try {
                result = new Pair<String, List<TimerItem>>(null, retrieveList(timeServiceClient));
            } catch (TimeServiceException e) {
                result = new Pair<String, List<TimerItem>>(e.getMessage(), null);
                Log.e(TAG, "Error in FecthListAsyncTask", e);
            }
            return result;
        }

        @Override
        protected void onPostExecute(Pair<String, List<TimerItem>> result) {
            progressDialog.dismiss();
            if (result.first != null) {
                // this means we have some error.
            } else {
                timerManagerAdapter.update(result.second);
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_alarm_manager, container, false);
        getActivity().setTitle(context.getResources().getString(R.string.timer_manager));
        ListView listView = (ListView) rootView.findViewById(R.id.alarm_list);

        View buttonView = rootView.findViewById(R.id.button_layout);
        currentDeviceUUID = (UUID) getArguments().getSerializable("UUID");

        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        if (map != null) {
            DeviceManager.Device currentDevice = map.get(currentDeviceUUID);

            if (currentDevice != null) {
                if (currentDevice.timeServiceClient.isTimerFactoryAnnounced()) {
                    buttonView.setVisibility(View.VISIBLE);
                } else {
                    buttonView.setVisibility(View.GONE);
                }
            }
        }

        ImageView addImage = (ImageView) rootView.findViewById(R.id.add);
        addImage.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                fragmentCommunication.displayFragment(FragmentCommunicationInterface.TIMER_NEW, currentDeviceUUID);
            }
        });

        List<TimerItem> temp = new ArrayList<TimerItem>();
        timerManagerAdapter = new TimerManagerAdapter(context, temp);

        listView.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {

            @Override
            public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
                AdapterContextMenuInfo info = (AdapterContextMenuInfo) menuInfo;
                selectedItem = info.position;
                TimerItem item = (TimerItem) timerManagerAdapter.getItem(selectedItem);

                menu.setHeaderTitle(item.description + " " + context.getResources().getString(R.string.options));
                menu.add(0, EDIT_TIMER, 0, context.getResources().getString(R.string.manage_timers_edit_timer));
                if (!item.isLocked) {
                    menu.add(0, DELETE_TIMER, 0, context.getResources().getString(R.string.manage_timers_delete_timer));
                }

                if (item.timer.getTimerHandler() == null) {
                    menu.add(0, REGISTER_TIMER_SIGNALS, 0, context.getResources().getString(R.string.manage_timers_register_timer_signals));
                } else {
                    menu.add(0, UNREGISTER_TIMER_SIGNALS, 0, context.getResources().getString(R.string.manage_timers_unregister_timer_signals));
                }
            }
        });
        listView.setAdapter(timerManagerAdapter);
        progressDialog.show();
        generateTimerItemList();
        return rootView;
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        final TimerItem timerItem = (TimerItem) timerManagerAdapter.getItem(selectedItem);

        switch (item.getItemId()) {
        case EDIT_TIMER:
            fragmentCommunication.setObject(timerItem);
            fragmentCommunication.displayFragment(FragmentCommunicationInterface.TIMER_EDIT, currentDeviceUUID);
            return true;

        case DELETE_TIMER: {
            // delete timer by using the TimerFactory to delete it.
            if (timerItem.timerFactory != null) {
                progressDialog.setMessage(context.getResources().getString(R.string.wait_delete_message));
                progressDialog.show();
                new AsyncTask<Void, Void, String>() {
                    @Override
                    protected String doInBackground(Void... params) {
                        try {
                            // call to TimeService API.
                            timerItem.timerFactory.deleteTimer(timerItem.timer.getObjectPath());
                        } catch (TimeServiceException e) {
                            Log.e(TAG, "Error in Deleting timer", e);
                            return e.getMessage();
                        }
                        return null;
                    }

                    @Override
                    protected void onPostExecute(String result) {
                        progressDialog.dismiss();
                        if (result != null) {
                            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result, context.getResources().getString(R.string.alert_dissmis_button),
                                    null, null);
                        } else {
                            timerManagerAdapter.removeItem(selectedItem);
                        }
                    };
                }.execute();
            }
            return true;
        }
        // register to receive Timer signal.
        case REGISTER_TIMER_SIGNALS: {
            // register to receive Timer signals.
            final DeviceManager.Device currentDevice = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(currentDeviceUUID);
            if (currentDevice != null) {
                registerSignals(currentDevice, timerItem.timer);
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        timerItem.isSignalRegistered = true;
                        timerManagerAdapter.notifyDataSetChanged();
                    }
                });
            }
            return true;
        }
        // unregister from receiving Alarm signal.
        case UNREGISTER_TIMER_SIGNALS: {
            // unregister from remote party to receive signals to current Timer.
            timerItem.timer.unregisterTimerHandler();
            final DeviceManager.Device currentDevice = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(currentDeviceUUID);
            if (currentDevice != null) {
                currentDevice.unRegisterSignalHandler(timerItem.timer.getObjectPath());
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        timerItem.isSignalRegistered = false;
                        timerManagerAdapter.notifyDataSetChanged();
                    }
                });
            }
            return true;
        }

        default:
            return false;
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.timer_manager));
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        context = activity;
        baseActivity = (BaseActivity) activity;
        progressDialog = new ProgressDialog(getActivity());
        progressDialog.setCancelable(false);
        progressDialog.setMessage(context.getResources().getString(R.string.wait_message));
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
        fragmentCommunication = (FragmentCommunicationInterface) activity;
    }

    /**
     * Fetch the Timers list by first checking if the app has a session
     * established with the remote device. After establishing remote session
     * retrieve the Timer items by traversing the timer factories and the timer
     * announced list. for each Timer fetch it's properties.
     */
    private void generateTimerItemList() {
        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        if (map == null) {

            progressDialog.dismiss();
            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), context.getResources().getString(R.string.alert_message_error_device_notfound), context
                    .getResources().getString(R.string.alert_dissmis_button), null, null);
            return;
        }
        final DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
        if (currentDevice == null) {
            progressDialog.dismiss();
            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), context.getResources().getString(R.string.alert_message_error_device_notfound), context
                    .getResources().getString(R.string.alert_dissmis_button), null, null);
            return;
        }

        if (!(currentDevice.timeServiceClient.isTimerAnnounced() || currentDevice.timeServiceClient.isTimerFactoryAnnounced())) {
            progressDialog.dismiss();
            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), context.getResources().getString(R.string.alert_message_error_timers_not_supported),
                    context.getResources().getString(R.string.alert_dissmis_button), null, null);
            return;
        }

        if (!currentDevice.timeServiceClient.isConnected()) {
            currentDevice.timeServiceClient.joinSessionAsync(new SessionListenerHandler() {
                @Override
                public void sessionLost(int reason, TimeServiceClient timeServiceClient) {
                }

                @Override
                public void sessionJoined(TimeServiceClient timeServiceClient, Status status) {
                    if (status != Status.OK) {
                        if (progressDialog.isShowing()) {
                            progressDialog.dismiss();
                        }
                        ((Activity) context).runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title),
                                        context.getResources().getString(R.string.alert_message_error_join_session), context.getResources().getString(R.string.alert_dissmis_button), null, null);
                            }
                        });
                    } else {
                        new FetchListAsyncTask().execute(timeServiceClient);
                    }
                }
            });
        } else {
            new FetchListAsyncTask().execute(currentDevice.timeServiceClient);
        }
    }

    /**
     * Read information from the Timer on the remote party,fill the TimerItem
     * object.
     * 
     * @param currentTimer
     * @param factory
     * @return
     * @throws TimeServiceException
     */
    private TimerItem generateTimerItem(Timer currentTimer, TimerFactory factory) throws TimeServiceException {
        TimerItem timerItem = new TimerItem();
        timerItem.description = currentTimer.retrieveTitle();
        timerItem.objectpath = currentTimer.getObjectPath();
        Period periodInterval = currentTimer.retrieveInterval();
        Period periodRemaining = currentTimer.retrieveTimeLeft();
        timerItem.running = currentTimer.retrieveIsRunning();
        timerItem.repeat = currentTimer.retrieveRepeat();
        timerItem.interval = String.format("%02d:%02d:%02d", periodInterval.getHour(), periodInterval.getMinute(), periodInterval.getSeconds());
        timerItem.remaining = String.format("%02d:%02d:%02d", periodRemaining.getHour(), periodRemaining.getMinute(), periodRemaining.getSeconds());
        timerItem.timer = currentTimer;
        timerItem.timerFactory = factory;
        timerItem.repeat = currentTimer.retrieveRepeat();
        return timerItem;
    }

    /**
     * Generate a list of the Timers in the remote party,using TimeService API.
     * In case the Timer is registered to receive signal (from the device's
     * internal storage) register it.
     * 
     * @param timeServiceClient
     * @return
     * @throws TimeServiceException
     */
    private List<TimerItem> retrieveList(TimeServiceClient timeServiceClient) throws TimeServiceException {
        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        DeviceManager.Device currentDevice = null;
        if (map != null) {
            currentDevice = map.get(currentDeviceUUID);
        }
        List<TimerItem> temp = new ArrayList<TimerItem>();
        if (timeServiceClient.isTimerAnnounced()) {
            for (Timer currentTimer : timeServiceClient.getAnnouncedTimerList()) {
                TimerItem item = generateTimerItem(currentTimer, null);
                item.isLocked = true;
                currentTimer.unregisterTimerHandler();
                SignalObject signalObject = currentDevice.getSignalObject(currentTimer.getObjectPath());
                if (signalObject != null) {
                    item.isSignalRegistered = true;
                    registerSignals(currentDevice, currentTimer);
                }
                temp.add(item);
            }
        }
        if (timeServiceClient.isTimerFactoryAnnounced()) {
            for (TimerFactory currentTimerFactory : timeServiceClient.getAnnouncedTimerFactoryList()) {
                List<Timer> currentTimerFactoryList = currentTimerFactory.retrieveTimerList();
                for (Timer currentTimer : currentTimerFactoryList) {
                    TimerItem item = generateTimerItem(currentTimer, currentTimerFactory);
                    SignalObject signalObject = currentDevice.getSignalObject(currentTimer.getObjectPath());
                    if (signalObject != null) {
                        item.isSignalRegistered = true;
                        registerSignals(currentDevice, currentTimer);
                    }
                    temp.add(item);
                }
            }
        }
        return temp;
    }

/**
     * Register Timer signals locally for the device internal storage using {@link org.allseen.timeservice.sample.client.application.DeviceManager.Device#registerSignalHandler(String, SignalObject)}
     * Register to receive signal from remote party using {@link Timer#registerTimerHandler(org.allseen.timeservice.client.Timer.TimerHandler)
     * @param currentDevice
     * @param currentAlarm
     */
    private void registerSignals(DeviceManager.Device currentDevice, Timer currentTimer) {
        currentDevice.unRegisterSignalHandler(currentTimer.getObjectPath());
        DeviceManager.TimerHandlerImpl handler = new DeviceManager.TimerHandlerImpl(context, currentTimer, currentDevice.serviceName);
        // register locally in the device storage.
        currentDevice.registerSignalHandler(currentTimer.getObjectPath(), handler);
        // register to the remote party to receive signals.
        currentTimer.registerTimerHandler(handler);
    }

}
