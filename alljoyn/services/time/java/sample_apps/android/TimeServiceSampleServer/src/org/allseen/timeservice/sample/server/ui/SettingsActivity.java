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
package org.allseen.timeservice.sample.server.ui;

import java.util.Timer;
import java.util.TimerTask;

import org.allseen.timeservice.sample.server.R;
import org.allseen.timeservice.sample.server.logic.PreferencesManager;

import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.util.Pair;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class SettingsActivity extends PreferenceActivity {
    private static final String TAG = "SettingsActivity";
    private ProgressDialog progressDialog = null;

    /**
     *
     * Sends a sync signal asynchronously
     *
     */
    private class SyncNowTask extends AsyncTask<Void, Void, Pair<String, Boolean>> {

        @Override
        protected Pair<String, Boolean> doInBackground(Void... params) {
            try {
                return new Pair<String, Boolean>(null, ((TimeSampleServer) getApplication()).getProtocolManager().sendTimeSync());
            } catch (Exception e) {
                Log.e(TAG, "Error sending time sync signal", e);
                return new Pair<String, Boolean>(e.getMessage(), null);
            }
        }

        @Override
        protected void onPostExecute(Pair<String, Boolean> result) {
            new Timer().schedule(new TimerTask() {
                @Override
                public void run() {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            progressDialog.dismiss();
                        }
                    });
                }
            }, 1000);

        }
    }

    @Override
    @SuppressWarnings("deprecation")
    public void onCreate(Bundle savedInstanceState) {

        progressDialog = new ProgressDialog(this);
        progressDialog.setMessage(getResources().getString(R.string.wait_message));
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
        progressDialog.setCancelable(false);

        super.onCreate(savedInstanceState);
        this.setTitle("Settings");
        addPreferencesFromResource(R.xml.settings);
        setContentView(R.layout.activity_settings);

        final CheckBoxPreference clocksetable = (CheckBoxPreference) findPreference(getString(R.string.settings_clock_key));
        final CheckBoxPreference timeAuthority = (CheckBoxPreference) findPreference(getString(R.string.settings_autherity_key));
        final Preference timesync = findPreference(getString(R.string.settings_time_sync_key));

        final CheckBoxPreference timerFactory = (CheckBoxPreference) findPreference(getString(R.string.settings_timers_factory_key));
        final CheckBoxPreference timer1 = (CheckBoxPreference) findPreference(getString(R.string.settings_timers_timer1_key));

        final CheckBoxPreference alarmFactory = (CheckBoxPreference) findPreference(getString(R.string.settings_alarm_factory_key));
        final CheckBoxPreference alarm1 = (CheckBoxPreference) findPreference(getString(R.string.settings_alarm_alarm1_key));

        Button commit = (Button) findViewById(R.id.commit);
        commit.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                PreferencesManager.AppPreferences appPrefrences = new PreferencesManager.AppPreferences();
                appPrefrences.clockSetable = clocksetable.isChecked();
                appPrefrences.timeAuthority = timeAuthority.isChecked();
                appPrefrences.alarmFactory = alarmFactory.isChecked();
                appPrefrences.alarm1 = alarm1.isChecked();
                appPrefrences.timerFactory = timerFactory.isChecked();
                appPrefrences.timer1 = timer1.isChecked();
                PreferencesManager.getInstance().setPrefrences(appPrefrences);
                ((TimeSampleServer) getApplication()).getProtocolManager().initiateTimeServer();
                finish();
            }
        });

        Button defaults = (Button) findViewById(R.id.defaults);
        defaults.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                clocksetable.setChecked(true);
                timeAuthority.setChecked(true);
                timerFactory.setChecked(true);
                timer1.setChecked(true);
                alarmFactory.setChecked(true);
                alarm1.setChecked(true);
            }
        });
        PreferencesManager.AppPreferences appPrefrences = PreferencesManager.getInstance().getPrefrences();
        clocksetable.setChecked(appPrefrences.clockSetable);

        timeAuthority.setChecked(appPrefrences.timeAuthority);
        if (!timeAuthority.isChecked()) {
            timesync.setEnabled(false);
        }

        timerFactory.setChecked(appPrefrences.timerFactory);
        timer1.setChecked(appPrefrences.timer1);

        alarmFactory.setChecked(appPrefrences.alarmFactory);
        alarm1.setChecked(appPrefrences.alarm1);

        timesync.setOnPreferenceClickListener(new OnPreferenceClickListener() {
            @Override
            public boolean onPreferenceClick(Preference preference) {
                if (timesync.isEnabled()) {
                    progressDialog.show();
                    new SyncNowTask().execute();
                }
                return false;
            }
        });

        timeAuthority.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                if (newValue.toString().equals("true")) {
                    timesync.setEnabled(true);

                } else {
                    timesync.setEnabled(false);
                }
                return true;
            }
        });

    }

}
