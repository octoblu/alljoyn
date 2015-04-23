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
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.bus.Status;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.Clock;
import org.allseen.timeservice.client.SessionListenerHandler;
import org.allseen.timeservice.client.TimeServiceClient;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.adapters.ClientServiceAdapter;
import org.allseen.timeservice.sample.client.application.DeviceManager;
import org.allseen.timeservice.sample.client.application.DeviceManagerNotifier;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.ClientServiceItem;
import org.allseen.timeservice.sample.client.ui.TimerPickerDialog.OnCancelListener;
import org.allseen.timeservice.sample.client.ui.TimerPickerDialog.OnTimeSetListener;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.util.Pair;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnCreateContextMenuListener;
import android.view.ViewGroup;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.ListView;

/**
 *
 * Displays the current devices and their supported capabilities of TimeService
 */
public class ClientServiceFragment extends Fragment {
    private static final String TAG = "ClientServiceFragment";
    private static final int MANAGE_ALARMS = 1;
    private static final int MANAGE_TIMERS = 2;
    private static final int SET_CLOCK = 3;
    private static final int GET_CLOCK = 4;
    private static final int REGISTER_SYNC = 5;
    private static final int UNREGISTER_SYNC = 6;

    private ProgressDialog progressDialog = null;
    private Context context;
    private ClientServiceAdapter clientServiceAdapter;

    private UUID selecteItemUUID = null;

    private FragmentCommunicationInterface fragmentCommunication;
    private BaseActivity baseActivity = null;

    private class SetClockAsyncTask extends AsyncTask<Pair<DeviceManager.Device, DateTime>, Void, String> {

        @Override
        protected String doInBackground(Pair<DeviceManager.Device, DateTime>... params) {
            Pair<DeviceManager.Device, DateTime> pair = params[0];
            try {
                for (Clock clock : pair.first.timeServiceClient.getAnnouncedClockList()) {
                    clock.setDateTime(pair.second);
                }
            } catch (TimeServiceException e) {
                Log.e(TAG, "Error in SetClockAsyncTask ", e);
                return e.getMessage();
            }
            return null;
        }

        @Override
        protected void onPostExecute(String result) {
            progressDialog.dismiss();
            if (result != null) {
                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result, context.getResources().getString(R.string.alert_dissmis_button), null, null);
            } else {
                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_success_title), context.getResources().getString(R.string.alert_message_success_updating_clock),
                        context.getResources().getString(R.string.alert_dissmis_button), null, null);
            }
        }
    }

    private class GetClockAsyncTask extends AsyncTask<DeviceManager.Device, Void, Pair<String, DateTime>> {

        @Override
        protected Pair<String, DateTime> doInBackground(DeviceManager.Device... params) {
            try {

                for (Clock clock : params[0].timeServiceClient.getAnnouncedClockList()) {
                    DateTime dataTime = clock.retrieveDateTime();
                    return new Pair<String, DateTime>(null, dataTime);
                }
            } catch (TimeServiceException e) {
                Log.e(TAG, "Error in GetClockAsyncTask ", e);
                return new Pair<String, DateTime>(e.getMessage(), null);
            }
            return null;
        }

        @Override
        protected void onPostExecute(Pair<String, DateTime> result) {
            progressDialog.dismiss();
            if (result.first != null) {

                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result.first, context.getResources().getString(R.string.alert_dissmis_button), null,
                        null);
            } else {

                Log.d(TAG, "Recviced:" + result.second.getTime().toString() + " " + result.second.getDate().toString());
                String message = "Received the following " + "\n"
                        + String.format("%02d:%02d:%02d", result.second.getTime().getHour(), result.second.getTime().getMinute(), result.second.getTime().getSeconds()) + "\t\t"
                        + String.format("%02d/%02d/%04d", result.second.getDate().getDay(), result.second.getDate().getMonth(), result.second.getDate().getYear());

                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_success_title), message, context.getResources().getString(R.string.alert_dissmis_button), null,
                        null);
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View rootView = inflater.inflate(R.layout.fragment_alarm_manager, container, false);
        ListView listView = (ListView) rootView.findViewById(R.id.alarm_list);
        View buttonView = rootView.findViewById(R.id.button_layout);
        buttonView.setVisibility(View.GONE);

        clientServiceAdapter = new ClientServiceAdapter(context, generateClientServiceItemList());

        listView.setAdapter(clientServiceAdapter);
        listView.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {

            @Override
            public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
                AdapterContextMenuInfo info = (AdapterContextMenuInfo) menuInfo;
                int position = info.position;
                ClientServiceItem item = (ClientServiceItem) clientServiceAdapter.getItem(position);
                selecteItemUUID = item.uuid;
                menu.setHeaderTitle(item.name + " " + context.getResources().getString(R.string.options));
                if (item.alarms) {
                    menu.add(0, MANAGE_ALARMS, 0, context.getResources().getString(R.string.client_service_manage_alarms));
                }
                if (item.timers) {
                    menu.add(0, MANAGE_TIMERS, 0, context.getResources().getString(R.string.client_service_manage_timers));
                }
                if (item.clock) {
                    menu.add(0, SET_CLOCK, 0, context.getResources().getString(R.string.client_service_set_clock));
                    menu.add(0, GET_CLOCK, 0, context.getResources().getString(R.string.client_service_get_clock));
                }
                if (item.timeAuthority) {
                    if (item.syncTimeRegistered) {
                        menu.add(0, UNREGISTER_SYNC, 0, context.getResources().getString(R.string.client_service_unregister_time_authority_sync));
                    } else {
                        menu.add(0, REGISTER_SYNC, 0, context.getResources().getString(R.string.client_service_register_time_authority_sync));
                    }
                }

            }
        });

        return rootView;
    }

    private void handleGetClock() {

        progressDialog.setMessage(context.getResources().getString(R.string.wait_getting_clock));
        progressDialog.show();
        final DeviceManager.Device device = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(selecteItemUUID);
        if (device != null) {
            if (!device.timeServiceClient.isConnected()) {
                device.timeServiceClient.joinSessionAsync(new SessionListenerHandler() {
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
                            new GetClockAsyncTask().execute(device);
                        }
                    }
                });
            } else {
                new GetClockAsyncTask().execute(device);
            }
        }
    }

    private void handleSetClock() {
        final DeviceManager.Device device = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(selecteItemUUID);
        if (device != null) {
            progressDialog.setMessage(context.getResources().getString(R.string.wait_setting_clock));
            progressDialog.show();
            final Calendar calander = Calendar.getInstance();
            TimerPickerDialog mTimePicker = new TimerPickerDialog(context, new OnTimeSetListener() {
                @Override
                public void onTimeSet(TimerPickerWidget view, int hour, int minutes, int seconds) {

                    org.allseen.timeservice.Date date = new org.allseen.timeservice.Date((short) calander.get(Calendar.YEAR), (byte) ((calander.get(Calendar.MONTH) + 1)),
                            (byte) (calander.get(Calendar.DAY_OF_MONTH)));

                    org.allseen.timeservice.Time time = new org.allseen.timeservice.Time((byte) hour, (byte) minutes, (byte) seconds, (byte) 0);

                    Log.d(TAG, "setting the following data" + date.toString() + " time " + time);
                    final DateTime dateTime = new DateTime(date, time, (short) 0);
                    if (!device.timeServiceClient.isConnected()) {
                        device.timeServiceClient.joinSessionAsync(new SessionListenerHandler() {
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
                                                    context.getResources().getString(R.string.alert_message_error_join_session), context.getResources().getString(R.string.alert_dissmis_button), null,
                                                    null);
                                        }
                                    });
                                } else {
                                    new SetClockAsyncTask().execute(new Pair<DeviceManager.Device, DateTime>(device, dateTime));
                                }
                            }
                        });
                    } else {

                        new SetClockAsyncTask().execute(new Pair<DeviceManager.Device, DateTime>(device, dateTime));
                    }
                }
            }, new OnCancelListener() {
                @Override
                public void onCancel() {
                    progressDialog.dismiss();
                }
            }, context.getResources().getString(R.string.set_clock_title), true, calander.get(Calendar.HOUR_OF_DAY), calander.get(Calendar.MINUTE), calander.get(Calendar.SECOND));
            mTimePicker.setOnKeyListener(new OnKeyListener() {
                @Override
                public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                    progressDialog.dismiss();
                    return false;
                }
            });
            mTimePicker.show();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.app_name) + " Devices");
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MANAGE_ALARMS:
            fragmentCommunication.displayFragment(FragmentCommunicationInterface.ALARMS_MANAGER, selecteItemUUID);
            return true;

        case MANAGE_TIMERS:
            fragmentCommunication.displayFragment(FragmentCommunicationInterface.TIMERS_MANAGER, selecteItemUUID);
            return true;

        case SET_CLOCK:
            handleSetClock();
            return true;

        case GET_CLOCK:
            handleGetClock();

            return true;
        case REGISTER_SYNC: {

            DeviceManager.Device device = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(selecteItemUUID);
            if (device != null) {
                for (Clock clock : device.timeServiceClient.getAnnouncedClockList()) {
                    if (clock.isAuthority()) {
                        DeviceManager.TimeAuthorityHandlerImpl handler = new DeviceManager.TimeAuthorityHandlerImpl(context, clock, device.serviceName);
                        clock.registerTimeAuthorityHandler(handler);

                        DeviceManager.Device currentDevice = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(selecteItemUUID);
                        if (currentDevice != null) {
                            currentDevice.registerSignalHandler(clock.getObjectPath(), handler);
                        }

                    }
                }
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        clientServiceAdapter.update(generateClientServiceItemList());
                        clientServiceAdapter.notifyDataSetChanged();
                    }
                });
            }

        }
            return true;
        case UNREGISTER_SYNC: {
            DeviceManager.Device device = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap().get(selecteItemUUID);
            if (device != null) {
                for (Clock clock : device.timeServiceClient.getAnnouncedClockList()) {
                    if (clock.isAuthority()) {
                        clock.unregisterTimeAuthorityHandler();
                        device.unRegisterSignalHandler(clock.getObjectPath());
                        break;
                    }
                }

                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        clientServiceAdapter.update(generateClientServiceItemList());
                        clientServiceAdapter.notifyDataSetChanged();
                    }
                });

            }
        }
            return true;
        default:
            return false;
        }
    }

    @Override
    public void onAttach(Activity activity) {
        // TODO Auto-generated method stub
        super.onAttach(activity);
        context = activity;
        baseActivity = (BaseActivity) activity;

        fragmentCommunication = (FragmentCommunicationInterface) activity;
        ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().registerDeviceManagerNotifier(new DeviceManagerNotifier() {

            @Override
            public void devicesHaveBeenChanged() {

                final List<ClientServiceItem> temp = generateClientServiceItemList();
                ((Activity) context).runOnUiThread(new Runnable() {

                    @Override
                    public void run() {

                        clientServiceAdapter.update(temp);
                        clientServiceAdapter.notifyDataSetChanged();
                    }
                });

            }
        });

        progressDialog = new ProgressDialog(getActivity());
        progressDialog.setMessage(context.getResources().getString(R.string.wait_update_message));
        progressDialog.setCancelable(false);
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);

    }

    private List<ClientServiceItem> generateClientServiceItemList() {

        List<ClientServiceItem> temp = new ArrayList<ClientServiceItem>();

        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();

        for (Map.Entry<UUID, DeviceManager.Device> entry : map.entrySet()) {
            UUID key = entry.getKey();
            DeviceManager.Device currentDevice = entry.getValue();
            TimeServiceClient timeServiceClient = currentDevice.timeServiceClient;
            ClientServiceItem clientServiceItem = new ClientServiceItem(currentDevice.deviceName, currentDevice.serviceName);

            if (timeServiceClient.isAlarmAnnounced() || timeServiceClient.isAlarmFactoryAnnounced()) {
                clientServiceItem.alarms = true;
            }

            if (timeServiceClient.isTimerAnnounced() || timeServiceClient.isTimerFactoryAnnounced()) {
                clientServiceItem.timers = true;
            }
            if (timeServiceClient.isClockAnnounced()) {
                clientServiceItem.clock = true;
                for (Clock clock : timeServiceClient.getAnnouncedClockList()) {
                    if (clock.isAuthority()) {
                        clientServiceItem.timeAuthority = true;
                        if (clock.getTimeAuthorityHandler() != null) {
                            clientServiceItem.syncTimeRegistered = true;
                        }
                        break;
                    }
                }
            }
            clientServiceItem.uuid = currentDevice.appId;
            clientServiceItem.lastSeen = currentDevice.lastSeen;
            temp.add(clientServiceItem);
            Collections.sort(temp, new Comparator<ClientServiceItem>() {
                @Override
                public int compare(ClientServiceItem o1, ClientServiceItem o2) {
                    if (o1.lastSeen == o2.lastSeen)
                        return 0;
                    return o1.lastSeen < o2.lastSeen ? -1 : 1;
                }
            });

        }
        return temp;
    }

    @Override
    public void onDetach() {
        super.onDetach();
        ((MainActivity) context).finish();
    }

}
