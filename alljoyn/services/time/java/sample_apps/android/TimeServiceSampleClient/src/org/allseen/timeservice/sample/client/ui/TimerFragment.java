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

import java.util.Map;
import java.util.StringTokenizer;
import java.util.UUID;

import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.Timer;
import org.allseen.timeservice.client.TimerFactory;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.application.DeviceManager;
import org.allseen.timeservice.sample.client.application.DeviceManager.SignalObject;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.TimerItem;
import org.allseen.timeservice.sample.client.ui.TimerPickerDialog.OnCancelListener;
import org.allseen.timeservice.sample.client.ui.TimerPickerDialog.OnTimeSetListener;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.InputFilter;
import android.util.Log;
import android.util.Pair;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.NumberPicker;
import android.widget.NumberPicker.OnValueChangeListener;
import android.widget.Switch;
import android.widget.TextView;

/**
 * Enables the user to change the contents of the {@link Timer}.
 *
 */
public class TimerFragment extends Fragment {
    private static final String TAG = "TimerFragment";
    private UUID currentDeviceUUID;
    private Context context;

    private EditText editTime;

    private Button buttonCancel;
    private Button buttonOk;
    private TextView object_path;
    private EditText descriptionValue;
    private Switch enabled;
    private View rootView;
    private Timer currentTimer;
    private CheckBox checkbox;
    private TextView remaining;
    private CheckBox registerSignal;

    private EditText count;
    private BaseActivity baseActivity = null;
    private ProgressDialog progressDialog = null;
    private FragmentCommunicationInterface fragmentCommunication = null;

    private boolean fullTimerDetailsNeeded = false;

    /**
     *
     * Internal class to create a new {@link Timer} Assume that the session is connected
     *
     */
    private class CreateTimerAsyncTask extends AsyncTask<DeviceManager.Device, Void, Pair<String, Timer>> {

        private DeviceManager.Device currentDevice;

        @Override
        protected Pair<String, Timer> doInBackground(DeviceManager.Device... params) {
            currentDevice = params[0];
            if (currentDevice.timeServiceClient.isConnected()) {
                try {
                    TimerFactory timerFactory = currentDevice.timeServiceClient.getAnnouncedTimerFactoryList().get(0);
                    return new Pair<String, Timer>(null, timerFactory.newTimer());
                } catch (TimeServiceException e) {
                    Log.e(TAG, "Error in fetching timer", e);
                    return new Pair<String, Timer>(e.getMessage(), null);
                }
            }
            return new Pair<String, Timer>("Not connected", null);
        }

        @Override
        protected void onPostExecute(Pair<String, Timer> result) {
            progressDialog.dismiss();
            if (result.first == null) {
                currentTimer = result.second;
                object_path.setText(currentTimer.getObjectPath());
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
        rootView = inflater.inflate(R.layout.fragment_timer, container, false);
        currentDeviceUUID = (UUID) getArguments().getSerializable("UUID");

        buttonCancel = (Button) rootView.findViewById(R.id.left);
        buttonOk = (Button) rootView.findViewById(R.id.right);

        object_path = (TextView) rootView.findViewById(R.id.object_path_value);
        descriptionValue = (EditText) rootView.findViewById(R.id.description_value);
        descriptionValue.setFilters(new InputFilter[] { new InputFilter.LengthFilter(10) });

        editTime = (EditText) rootView.findViewById(R.id.interval_value);
        enabled = (Switch) rootView.findViewById(R.id.enabled);
        count = (EditText) rootView.findViewById(R.id.count);
        registerSignal = (CheckBox) rootView.findViewById(R.id.register);
        count.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                showCountDialog();
            }
        });

        descriptionValue.setText("");
        editTime.setText("");

        checkbox = (CheckBox) rootView.findViewById(R.id.infinite);

        remaining = (TextView) rootView.findViewById(R.id.remaining_value);
        final InputMethodManager imm = ((InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE));

        // in case the user presses the cancel button ,and we are not in "EDIT" mode,the application deletes the Timer using TimeService API.
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
                                    currentDevice.timeServiceClient.getAnnouncedTimerFactoryList().get(0).deleteTimer(currentTimer.getObjectPath());
                                } catch (TimeServiceException e) {
                                    Log.e(TAG, "Error in updating timer", e);
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

                final StringTokenizer tokenizer = new StringTokenizer(editTime.getText().toString(), ":");
                final Period periodInterval = new Period(Integer.valueOf((String) tokenizer.nextElement()), Byte.valueOf((String) tokenizer.nextElement()), Byte.valueOf((String) tokenizer
                        .nextElement()), (short) 0);
                short temp = 0;
                if (checkbox.isChecked()) {
                    temp = org.allseen.timeservice.ajinterfaces.Timer.REPEAT_FOREVER;
                } else {
                    temp = new Short(count.getText().toString()).shortValue();
                }

                final short repeat = temp;
                fullTimerDetailsNeeded = false;
                // check if we need to send the full details of the Timer.
                if (getArguments().containsKey("EDIT")) {
                    TimerItem currentTimerItem = (TimerItem) fragmentCommunication.getObject();
                    if (!currentTimerItem.interval.equals(editTime.getText().toString())) {
                        fullTimerDetailsNeeded = true;
                    } else if (currentTimerItem.repeat != repeat) {
                        fullTimerDetailsNeeded = true;
                    }
                } else {
                    fullTimerDetailsNeeded = true;
                }

                final boolean dostart = enabled.isChecked();

                progressDialog.setMessage(context.getResources().getString(R.string.wait_update_message));
                progressDialog.show();

                new AsyncTask<Void, Void, String>() {
                    @Override
                    protected String doInBackground(Void... params) {
                        try {
                            // calls to TimeService API.
                            if (fullTimerDetailsNeeded) {
                                currentTimer.reset();
                                currentTimer.setRepeat(repeat);
                                currentTimer.setInterval(periodInterval);
                            }
                            currentTimer.setTitle(descriptionValue.getText().toString());
                            if (dostart) {
                                currentTimer.start();
                            } else {
                                currentTimer.pause();
                            }

                        } catch (TimeServiceException e) {
                            Log.e(TAG, "Error in updating timer", e);
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
                                    SignalObject signalObject = currentDevice.getSignalObject(currentTimer.getObjectPath());
                                    if (registerSignal.isChecked() && signalObject == null) {

                                        DeviceManager.TimerHandlerImpl handler = new DeviceManager.TimerHandlerImpl(context, currentTimer, currentDevice.serviceName);
                                        currentTimer.registerTimerHandler(handler);
                                        currentDevice.registerSignalHandler(currentTimer.getObjectPath(), handler);
                                    }
                                    if (!registerSignal.isChecked() && signalObject != null) {
                                        currentTimer.unregisterTimerHandler();
                                        currentDevice.unRegisterSignalHandler(currentTimer.getObjectPath());
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

                final StringTokenizer tokenizer = new StringTokenizer(editTime.getText().toString(), ":");
                TimerPickerDialog mTimePicker = new TimerPickerDialog(context, new OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimerPickerWidget view, int hourOfDay, int minute, int seconds) {
                        editTime.setText(String.format("%02d:%02d:%02d", hourOfDay, minute, seconds));
                    }
                }, new OnCancelListener() {
                    @Override
                    public void onCancel() {
                    }
                }, context.getResources().getString(R.string.set_timer_title), false, Integer.valueOf((String) tokenizer.nextElement()), Integer.valueOf((String) tokenizer.nextElement()), Integer
                        .valueOf((String) tokenizer.nextElement()));

                mTimePicker.setOnKeyListener(new OnKeyListener() {
                    @Override
                    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                        progressDialog.dismiss();
                        return false;
                    }
                });
                mTimePicker.show();
            }
        });
        count.setText("1");
        if (checkbox.isChecked()) {
            count.setEnabled(false);
        }
        checkbox.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    count.setEnabled(false);
                } else {

                    count.setEnabled(true);
                }

            }
        });
        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.timer_edior_title));
        if (getArguments().containsKey("EDIT")) {
            TimerItem currentTimerItem = (TimerItem) fragmentCommunication.getObject();
            currentTimer = currentTimerItem.timer;

            registerSignal.setChecked(currentTimer.getTimerHandler() != null);
            if (currentTimerItem.description.isEmpty()) {
                descriptionValue.setText(currentTimerItem.description);
                descriptionValue.setHint(context.getResources().getString(R.string.timer_title_hint));
            } else {
                descriptionValue.setText(currentTimerItem.description);
            }
            editTime.setText(currentTimerItem.interval);
            remaining.setText(currentTimerItem.remaining);

            object_path.setText(currentTimerItem.objectpath);
            if (currentTimerItem.repeat == org.allseen.timeservice.ajinterfaces.Timer.REPEAT_FOREVER) {
                checkbox.setChecked(true);
                count.setEnabled(false);
            } else {
                checkbox.setChecked(false);
                count.setText(String.valueOf(currentTimerItem.repeat));
            }
            enabled.setChecked(currentTimerItem.running);
        } else {
            editTime.setText("00:00:00");
            registerSignal.setChecked(false);
            checkbox.setChecked(false);
            count.setText("1");

            count.setEnabled(true);

            descriptionValue.setHint(context.getResources().getString(R.string.timer_title_hint));
            enabled.setChecked(false);
            Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
            if (map != null) {
                DeviceManager.Device currentDevice = map.get(currentDeviceUUID);
                if (currentDevice != null) {
                    progressDialog.setMessage(context.getResources().getString(R.string.wait_while_creating_new_timer));
                    progressDialog.show();
                    new CreateTimerAsyncTask().execute(currentDevice);
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
        progressDialog.setCancelable(false);
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
        fragmentCommunication = (FragmentCommunicationInterface) activity;
    }

    private void showCountDialog() {

        final Dialog d = new Dialog(getActivity());
        d.setTitle("Repeat count");
        d.setContentView(R.layout.number_picker_dialog);
        Button set = (Button) d.findViewById(R.id.set);
        Button cancel = (Button) d.findViewById(R.id.cancel);

        final NumberPicker np = (NumberPicker) d.findViewById(R.id.numberPicker);
        np.setMinValue(1); // min value 1
        np.setMaxValue(100); // max value 100
        np.setWrapSelectorWheel(true);
        np.setOnValueChangedListener(new OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
            }
        });

        set.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                count.setText(String.valueOf(np.getValue()));
                d.dismiss();
            }
        });
        cancel.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                d.dismiss();
            }
        });
        d.show();

    }

}
