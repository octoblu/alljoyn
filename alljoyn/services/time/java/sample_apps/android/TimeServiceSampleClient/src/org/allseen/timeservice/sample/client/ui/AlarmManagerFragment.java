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
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.Time;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.Alarm;
import org.allseen.timeservice.client.AlarmFactory;
import org.allseen.timeservice.client.SessionListenerHandler;
import org.allseen.timeservice.client.TimeServiceClient;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.adapters.AlarmManagerAdapter;
import org.allseen.timeservice.sample.client.application.DeviceManager;
import org.allseen.timeservice.sample.client.application.DeviceManager.SignalObject;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.AlarmItem;

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
 * Displays {@link Alarm} retrieved from the remote service.
 * <ul>
 * <li>Join a session with the Device.
 * <li>If {@see TimeServiceClient#isAlarmAnnounced()} fetch list of {@link Alarm} for each one get all properties.
 * <li>If {@see TimeServiceClient#isAlarmFactoryAnnounced()} fetch list of {@link AlarmFactory} for each fetch list of {@link Alarm} for each one get all properties.
 * <li>Fill UI elements.
 * </ul>
 */
public class AlarmManagerFragment extends Fragment {
    private static final String TAG = "AlarmMangerFragment";
    private static final int EDIT_ALARM = 1;
    private static final int DELETE_ALARM = 2;
    private static final int REGISTER_ALARM_SIGNAL = 3;
    private static final int UNREGISTER_ALARM_SIGNAL = 4;

    private AlarmManagerAdapter alarmManagerAdapter;
    private Context context;
    private UUID currentDeviceUUID;
    private int selectedItem = -1;
    private FragmentCommunicationInterface fragmentCommunication;

    private ProgressDialog progressDialog = null;
    private BaseActivity baseActivity = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    private class FetchListAsyncTask extends AsyncTask<TimeServiceClient, Void, Pair<String, List<AlarmItem>>> {

        @Override
        protected Pair<String, List<AlarmItem>> doInBackground(TimeServiceClient... params) {
            TimeServiceClient timeServiceClient = params[0];
            Pair<String, List<AlarmItem>> result;
            try {
                result = new Pair<String, List<AlarmItem>>(null, retrieveList(timeServiceClient));
            } catch (TimeServiceException e) {
                Log.e(TAG, "Error in FetchListAsyncTask ", e);
                result = new Pair<String, List<AlarmItem>>(e.getMessage(), null);
            }
            return result;
        }

        @Override
        protected void onPostExecute(Pair<String, List<AlarmItem>> result) {
            progressDialog.dismiss();
            if (result.first != null) {
                // this means we have some error.
            } else {
                alarmManagerAdapter.update(result.second);
            }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.alarm_manager));
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_alarm_manager, container, false);

        View buttonView = rootView.findViewById(R.id.button_layout);
        currentDeviceUUID = (UUID) getArguments().getSerializable("UUID");
        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        if (map != null) {
            DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
            if (currentDevice != null) {
                if (currentDevice.timeServiceClient.isAlarmFactoryAnnounced()) {
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
                fragmentCommunication.displayFragment(FragmentCommunicationInterface.ALARM_NEW, currentDeviceUUID);

            }
        });

        ListView listView = (ListView) rootView.findViewById(R.id.alarm_list);

        List<AlarmItem> temp = new ArrayList<AlarmItem>();
        alarmManagerAdapter = new AlarmManagerAdapter(context, temp);
        listView.setAdapter(alarmManagerAdapter);

        listView.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {

            @Override
            public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
                AdapterContextMenuInfo info = (AdapterContextMenuInfo) menuInfo;
                selectedItem = info.position;
                AlarmItem item = (AlarmItem) alarmManagerAdapter.getItem(selectedItem);
                menu.setHeaderTitle(item.description + " " + context.getResources().getString(R.string.options));
                menu.add(0, EDIT_ALARM, 0, context.getResources().getString(R.string.manage_alarms_edit_alarm));
                if (!item.isLocked) {
                    menu.add(0, DELETE_ALARM, 0, context.getResources().getString(R.string.manage_alarms_delete_alarm));
                }
                if (item.alarm.getAlarmHandler() == null) {
                    menu.add(0, REGISTER_ALARM_SIGNAL, 0, context.getResources().getString(R.string.manage_alarms_register_alarm_signal));
                } else {
                    menu.add(0, UNREGISTER_ALARM_SIGNAL, 0, context.getResources().getString(R.string.manage_alarms_unregister_alarm_signal));
                }
            }
        });
        progressDialog.show();
        generateAlarmItemList();
        return rootView;
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        final AlarmItem alarmItem = (AlarmItem) alarmManagerAdapter.getItem(selectedItem);
        switch (item.getItemId()) {

        case EDIT_ALARM:
            fragmentCommunication.setObject(alarmItem);
            fragmentCommunication.displayFragment(FragmentCommunicationInterface.ALARM_EDIT, currentDeviceUUID);
            return true;

        case DELETE_ALARM: {
            // delete an Alarm by using the AlarmFactory to delete it.
            if (alarmItem.alarmFactory != null) {
                progressDialog.setMessage(context.getResources().getString(R.string.wait_delete_message));
                progressDialog.show();
                new AsyncTask<Void, Void, String>() {
                    @Override
                    protected String doInBackground(Void... params) {
                        try {
                            //call to TimeService API.
                            alarmItem.alarmFactory.deleteAlarm(alarmItem.alarm.getObjectPath());
                        } catch (TimeServiceException e) {
                            Log.e(TAG, "Error in deleting Alarm ", e);
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
                            alarmManagerAdapter.removeItem(selectedItem);
                        }
                    };
                }.execute();
            }
            return true;
        }

        // register to receive Alarm signal.
        case REGISTER_ALARM_SIGNAL: {
            final DeviceManager.Device currentDevice = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(currentDeviceUUID);
            if (currentDevice != null) {
                registerSignals( currentDevice,alarmItem.alarm);
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        alarmItem.isSignalRegistered = true;
                        alarmManagerAdapter.notifyDataSetChanged();
                    }
                });
            }
            return true;
        }

        // unregister from receiving Alarm signal.
        case UNREGISTER_ALARM_SIGNAL: {
            // unregister from remote party to receive signals to current Alarm.
            alarmItem.alarm.unregisterAlarmHandler();
            final DeviceManager.Device currentDevice = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(currentDeviceUUID);
            if (currentDevice != null) {
                currentDevice.unRegisterSignalHandler(alarmItem.alarm.getObjectPath());
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        alarmItem.isSignalRegistered = false;
                        alarmManagerAdapter.notifyDataSetChanged();
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
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        context = activity;
        baseActivity = (BaseActivity) activity;
        progressDialog = new ProgressDialog(getActivity());
        progressDialog.setMessage(context.getResources().getString(R.string.wait_message));
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
        progressDialog.setCancelable(false);
        fragmentCommunication = (FragmentCommunicationInterface) activity;

    }

    /**
     * Fetch the Alarms list by first checking if the app has a session established with the remote device. After establishing remote session retrieve the Alarm items by traversing the alarm factories
     * and the alarm announced list. for each Alarm fetch it's properties.
     */
    private void generateAlarmItemList() {

        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        if (map == null) {

            return;
        }
        final DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
        if (currentDevice == null) {
            progressDialog.dismiss();
            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), context.getResources().getString(R.string.alert_message_error_device_notfound), context
                    .getResources().getString(R.string.alert_dissmis_button), null, null);
            return;
        }

        if (!(currentDevice.timeServiceClient.isAlarmAnnounced() || currentDevice.timeServiceClient.isAlarmFactoryAnnounced())) {
            progressDialog.dismiss();
            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), context.getResources().getString(R.string.alert_message_error_alarm_not_supported),
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
                    if (progressDialog.isShowing()) {
                        progressDialog.dismiss();
                    }
                    if (status != Status.OK) {
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
     * Read information from the Alarm on the remote party,fill the AlarmItem object.
     * @param currentAlarm
     * @param alarmFactory
     * @return
     * @throws TimeServiceException
     */
    public AlarmItem generateAlarmItem(Alarm currentAlarm, AlarmFactory alarmFactory) throws TimeServiceException {
        AlarmItem alarmItem = new AlarmItem();

        //calls to TimeService API.
        Schedule schedule = currentAlarm.retrieveSchedule();
        alarmItem.enabled = currentAlarm.retrieveIsEnabled();
        alarmItem.description = currentAlarm.retrieveTitle();

        alarmItem.objectpath = currentAlarm.getObjectPath();
        for (Schedule.WeekDay currentDay : schedule.getWeekDays()) {
            alarmItem.arrayDays[currentDay.ordinal()] = true;
        }
        Time time = schedule.getTime();
        alarmItem.hour = time.getHour();
        alarmItem.minutes = time.getMinute();
        alarmItem.seconds = time.getSeconds();
        alarmItem.alarm = currentAlarm;
        alarmItem.alarmFactory = alarmFactory;

        return alarmItem;
    }

    /**
     * Generate a list of the Alarms in the remote party,using TimeService API.
     * In case the Alarm is registered to receive signal (from the device's internal storage) register it.
     *
     * @param timeServiceClient
     * @return List of AlarmItems
     * @throws TimeServiceException
     */
    private List<AlarmItem> retrieveList(TimeServiceClient timeServiceClient) throws TimeServiceException {
        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        DeviceManager.Device currentDevice = null;
        if (map != null) {
            currentDevice = map.get(currentDeviceUUID);
        }

        final List<AlarmItem> temp = new ArrayList<AlarmItem>();
        if (timeServiceClient.isAlarmAnnounced()) {
            for (Alarm currentAlarm : timeServiceClient.getAnnouncedAlarmList()) {
                AlarmItem item = generateAlarmItem(currentAlarm, null);
                item.isLocked = true;
                currentAlarm.unregisterAlarmHandler();
                if (currentDevice != null) {
                    SignalObject signalObject = currentDevice.getSignalObject(currentAlarm.getObjectPath());
                    if (signalObject != null) {
                        item.isSignalRegistered = true;
                        registerSignals( currentDevice,currentAlarm);
                    }
                }
                temp.add(item);

            }
        }

        if (timeServiceClient.isAlarmFactoryAnnounced()) {
            for (AlarmFactory currentAlarmFactory : timeServiceClient.getAnnouncedAlarmFactoryList()) {
                List<Alarm> currentAlarmFactoryList = currentAlarmFactory.retrieveAlarmList();
                for (Alarm currentAlarm : currentAlarmFactoryList) {
                    AlarmItem item = generateAlarmItem(currentAlarm, currentAlarmFactory);
                    if (currentDevice != null) {
                        SignalObject signalObject = currentDevice.getSignalObject(currentAlarm.getObjectPath());
                        if (signalObject != null) {
                            item.isSignalRegistered = true;
                            registerSignals( currentDevice,currentAlarm);
                        }
                    }
                    temp.add(item);
                }
            }
        }
        return temp;
    }

    /**
     * Register Alarm signals locally for the device internal storage using {@link org.allseen.timeservice.sample.client.application.DeviceManager.Device#registerSignalHandler(String, SignalObject)}
     * Register to receive signal from remote party using {@link Alarm#registerAlarmHandler(org.allseen.timeservice.client.Alarm.AlarmHandler)}
     * @param currentDevice
     * @param currentAlarm
     */
    private void registerSignals(DeviceManager.Device currentDevice,Alarm currentAlarm){
        currentDevice.unRegisterSignalHandler(currentAlarm.getObjectPath());
        DeviceManager.AlarmHandlerImpl handler = new DeviceManager.AlarmHandlerImpl(context, currentAlarm, currentDevice.serviceName);
        // register locally in the device storage.
        currentDevice.registerSignalHandler(currentAlarm.getObjectPath(), handler);
        // register to the remote party to receive signals.
        currentAlarm.registerAlarmHandler(handler);
    }

}
