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

import java.util.Calendar;
import java.util.HashSet;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.UUID;

import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.Schedule.WeekDay;
import org.allseen.timeservice.Time;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.Alarm;
import org.allseen.timeservice.client.AlarmFactory;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.application.DeviceManager;
import org.allseen.timeservice.sample.client.application.DeviceManager.SignalObject;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.AlarmItem;

import android.app.Activity;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.ProgressDialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.InputFilter;
import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.TimePicker;

/**
 * Enables the user to change the contents of the {@link Alarm}.
 *
 */
public class AlarmFragment extends Fragment {
    private static final String TAG = "AlarmFragment";
    private UUID currentDeviceUUID;
    private Context context;
    private boolean daysChosenArray[] = new boolean[7];
    private EditText editTime;
    private Button buttonCancel;
    private Button buttonOk;
    private TextView object_path;
    private EditText descriptionValue;
    private Switch enabled;
    private View rootView;
    private Alarm currentAlarm;
    private FragmentCommunicationInterface fragmentCommunication = null;
    private BaseActivity baseActivity = null;
    private ProgressDialog progressDialog = null;
    private CheckBox registerSignal;

    /**
     *
     * Internal class to create a new {@link Alarm} Assume that the session is connected
     *
     */
    private class CreateAlarmAsyncTask extends AsyncTask<DeviceManager.Device, Void, Pair<String, Alarm>> {

        private DeviceManager.Device currentDevice;

        @Override
        protected Pair<String, Alarm> doInBackground(DeviceManager.Device... params) {
            currentDevice = params[0];
            if (currentDevice.timeServiceClient.isConnected()) {
                try {
                    AlarmFactory alarmFactory = currentDevice.timeServiceClient.getAnnouncedAlarmFactoryList().get(0);
                    return new Pair<String, Alarm>(null, alarmFactory.newAlarm());
                } catch (TimeServiceException e) {
                    Log.e(TAG, "Error in creating Alarm", e);
                    return new Pair<String, Alarm>(e.getMessage(), null);
                }
            }
            return new Pair<String, Alarm>("Not connected", null);
        }

        @Override
        protected void onPostExecute(Pair<String, Alarm> result) {
            progressDialog.dismiss();
            if (result.first == null) {
                currentAlarm = result.second;
                object_path.setText(currentAlarm.getObjectPath());
            } else {
                baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result.first, context.getResources().getString(R.string.alert_dissmis_button), null,
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
        getActivity().getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
        rootView = inflater.inflate(R.layout.fragment_alarm, container, false);
        currentDeviceUUID = (UUID) getArguments().getSerializable("UUID");

        buttonCancel = (Button) rootView.findViewById(R.id.left);
        buttonOk = (Button) rootView.findViewById(R.id.right);

        object_path = (TextView) rootView.findViewById(R.id.object_path_value);
        descriptionValue = (EditText) rootView.findViewById(R.id.description_value);
        descriptionValue.setFilters(new InputFilter[] { new InputFilter.LengthFilter(10) });
        editTime = (EditText) rootView.findViewById(R.id.time_value);
        enabled = (Switch) rootView.findViewById(R.id.enabled);

        descriptionValue.setText("");
        editTime.setText("");
        registerSignal = (CheckBox) rootView.findViewById(R.id.register);

        final InputMethodManager imm = ((InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE));

        // in case the user presses the cancel button ,and we are not in "EDIT" mode,the application deletes the alarm using TimeService API.
        buttonCancel.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                imm.hideSoftInputFromWindow(descriptionValue.getWindowToken(), 0);
                if (getArguments().containsKey("EDIT")) {
                    getFragmentManager().popBackStack();
                } else {
                    Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
                    if (map != null) {
                        final DeviceManager.Device currentDevice = map.get(currentDeviceUUID);

                        progressDialog.setMessage(context.getResources().getString(R.string.wait_update_message));
                        progressDialog.show();
                        new AsyncTask<Void, Void, String>() {
                            @Override
                            protected String doInBackground(Void... params) {
                                try {
                                    // call to TimeService API.
                                    currentDevice.timeServiceClient.getAnnouncedAlarmFactoryList().get(0).deleteAlarm(currentAlarm.getObjectPath());
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
                                    baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result,
                                            context.getResources().getString(R.string.alert_dissmis_button), new Runnable() {
                                                @Override
                                                public void run() {
                                                    getFragmentManager().popBackStack();
                                                }
                                            }, null);
                                } else {
                                    getFragmentManager().popBackStack();
                                }
                            };
                        }.execute();

                    }

                }
            }
        });

        // in case the user presses the OK button , the application gathers the information from the UI and commits it to the remote party via TimeService API.
        buttonOk.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                imm.hideSoftInputFromWindow(descriptionValue.getWindowToken(), 0);
                StringTokenizer tokenizer = new StringTokenizer(editTime.getText().toString(), ":");
                Set<WeekDay> weekDays = new HashSet<WeekDay>();
                WeekDay[] weekDaysArrayDays = WeekDay.values();
                for (int i = 0; i < daysChosenArray.length; i++) {
                    if (daysChosenArray[i]) {
                        weekDays.add(weekDaysArrayDays[i]);
                    }
                }
                final Schedule schedule = new Schedule(new Time(Byte.valueOf((String) tokenizer.nextElement()), Byte.valueOf((String) tokenizer.nextElement()), (byte) 0, (short) 0), weekDays);
                progressDialog.setMessage(context.getResources().getString(R.string.wait_update_message));
                progressDialog.show();

                new AsyncTask<Void, Void, String>() {
                    @Override
                    protected String doInBackground(Void... params) {
                        try {
                            // calls to TimeService API.
                            currentAlarm.setSchedule(schedule);
                            currentAlarm.setTitle(descriptionValue.getText().toString());
                            currentAlarm.setEnabled(enabled.isChecked());
                        } catch (TimeServiceException e) {
                            Log.e(TAG, "Error in updaing Alarm ", e);
                            return e.getMessage();
                        }
                        return null;
                    }

                    @Override
                    protected void onPostExecute(String result) {
                        progressDialog.dismiss();
                        if (result != null) {
                            baseActivity.launchBasicAlertDialog(context.getResources().getString(R.string.alert_error_title), result, context.getResources().getString(R.string.alert_dissmis_button),
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            getFragmentManager().popBackStack();
                                        }
                                    }, null);
                        } else {

                            Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
                            if (map != null) {
                                final DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
                                if (currentDevice != null) {
                                    SignalObject signalObject = currentDevice.getSignalObject(currentAlarm.getObjectPath());
                                    if (registerSignal.isChecked() && signalObject == null) {
                                        DeviceManager.AlarmHandlerImpl handler = new DeviceManager.AlarmHandlerImpl(context, currentAlarm, currentDevice.serviceName);
                                        currentAlarm.registerAlarmHandler(handler);
                                        currentDevice.registerSignalHandler(currentAlarm.getObjectPath(), handler);
                                    }
                                    if (!registerSignal.isChecked() && signalObject != null) {
                                        currentAlarm.unregisterAlarmHandler();
                                        currentDevice.unRegisterSignalHandler(currentAlarm.getObjectPath());
                                    }
                                }
                            }
                            getFragmentManager().popBackStack();
                        }
                    };

                }.execute();

            }
        });

        editTime.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                TimePickerFragment newFragment = new TimePickerFragment();
                newFragment.setEditText(editTime);
                newFragment.show(((Activity) context).getFragmentManager(), "alarmPicker");
            }
        });

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.alarm_editor));
        final TextView[] days = new TextView[7];
        for (int i = 0; i < days.length; i++) {
            days[i] = (TextView) rootView.findViewById(R.id.day0 + i);
            days[i].setTextColor(context.getResources().getColor(R.color.dark_gray));
            days[i].setTag(i);
            daysChosenArray[i] = false;
            days[i].setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    int t = ((Integer) v.getTag()).intValue();
                    if (daysChosenArray[t]) {
                        days[t].setTextColor(context.getResources().getColor(R.color.dark_gray));
                        daysChosenArray[t] = false;
                    } else {
                        days[t].setTextColor(context.getResources().getColor(R.color.green));
                        daysChosenArray[t] = true;
                    }
                }
            });
        }

        if (getArguments().containsKey("EDIT")) {
            AlarmItem currentAlarmItem = (AlarmItem) fragmentCommunication.getObject();
            currentAlarm = currentAlarmItem.alarm;
            String time = String.format(Locale.US, "%02d:%02d:%02d", currentAlarmItem.hour, currentAlarmItem.minutes, currentAlarmItem.seconds);
            editTime.setText(time);
            registerSignal.setChecked(currentAlarm.getAlarmHandler() != null);
            if (currentAlarmItem.description.isEmpty()) {
                descriptionValue.setText(currentAlarmItem.description);
                descriptionValue.setHint(context.getResources().getString(R.string.alarm_title_hint));
            } else {
                descriptionValue.setText(currentAlarmItem.description);
            }
            object_path.setText(currentAlarmItem.objectpath);
            daysChosenArray = currentAlarmItem.arrayDays;
            for (int i = 0; i < daysChosenArray.length; i++) {
                if (daysChosenArray[i]) {
                    days[i].setTextColor(context.getResources().getColor(R.color.green));
                } else {
                    days[i].setTextColor(context.getResources().getColor(R.color.dark_gray));
                }
            }
            ((Switch) rootView.findViewById(R.id.enabled)).setChecked(currentAlarmItem.enabled);
        } else {
            editTime.setText("00:00:00");
            descriptionValue.setText("");
            registerSignal.setChecked(false);
            descriptionValue.setHint(context.getResources().getString(R.string.alarm_title_hint));
            enabled.setChecked(false);
            Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
            if (map != null) {
                DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
                if (currentDevice != null) {
                    progressDialog.setMessage(context.getResources().getString(R.string.wait_while_creating_new_alarm));
                    progressDialog.show();
                    new CreateAlarmAsyncTask().execute(currentDevice);
                }
            }
        }
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        context = activity;
        baseActivity = (BaseActivity) activity;
        progressDialog = new ProgressDialog(getActivity());
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
        progressDialog.setCancelable(false);
        fragmentCommunication = (FragmentCommunicationInterface) activity;
    }

    public static class TimePickerFragment extends DialogFragment implements TimePickerDialog.OnTimeSetListener {
        private EditText editTextview;

        public void setEditText(EditText editTextview) {
            this.editTextview = editTextview;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            // Use the current time as the default values for the picker
            final Calendar c = Calendar.getInstance();
            int hour = c.get(Calendar.HOUR_OF_DAY);
            int minute = c.get(Calendar.MINUTE);
            // Create a new instance of TimePickerDialog and return it
            return new TimePickerDialog(getActivity(), this, hour, minute, true);
        }

        @Override
        public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
            editTextview.setText(String.format("%02d:%02d:%02d", hourOfDay, minute, 0));
        }
    }

}
