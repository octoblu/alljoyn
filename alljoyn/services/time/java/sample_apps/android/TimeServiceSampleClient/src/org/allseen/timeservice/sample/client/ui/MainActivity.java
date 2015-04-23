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

import java.util.UUID;

import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.MenuItemCompat;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.TextView;

public class MainActivity extends BaseActivity implements FragmentCommunicationInterface {
    private static final String TAG = "MainActivity";
    private final AlarmManagerFragment alarmMangerFragment = new AlarmManagerFragment();
    private final TimerManagerFragment timerMangerFragment = new TimerManagerFragment();
    private final ClientServiceFragment clientServiceFragment = new ClientServiceFragment();
    private final AlarmFragment alarmFragment = new AlarmFragment();
    private final TimerFragment timerFragment = new TimerFragment();
    private final SignalMessagesFragment singalMessagesFragment = new SignalMessagesFragment();
    private FragmentManager manager;
    private Object storageObject;
    private TextView signalMessagesCounterTextView;

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        if (intent.getAction() == TimeSampleClient.NEW_SIGNAL_MESSAGE_ACTION) {
            final Integer count = intent.getIntExtra(TimeSampleClient.NEW_SIGNAL_COUNT_KEY, 0);
            Log.d(TAG, "Received message count " + count);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    signalMessagesCounterTextView.setTextColor(getResources().getColor(R.color.red));
                    if (count > 99) {
                        signalMessagesCounterTextView.setText("!99");
                    } else {
                        signalMessagesCounterTextView.setText(count.toString());
                    }
                }
            });
        }
        if (intent.getAction() == TimeSampleClient.CLEAR_SIGNAL_MESSAGE_ACTION) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    signalMessagesCounterTextView.setText("");
                }
            });
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Set up the action bar.
        final ActionBar actionBar = getActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_STANDARD);

        manager = getSupportFragmentManager();
        FragmentTransaction transaction = manager.beginTransaction();

        transaction.replace(R.id.fragment_container, clientServiceFragment);
        transaction.addToBackStack(null);
        transaction.commit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        MenuItem timerItem2 = menu.findItem(R.id.notification_action);
        FrameLayout layout = (FrameLayout) MenuItemCompat.getActionView(timerItem2);
        signalMessagesCounterTextView = (TextView) layout.getChildAt(1);
        layout.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                FragmentTransaction transaction = manager.beginTransaction();
                transaction.replace(R.id.fragment_container, singalMessagesFragment);
                transaction.addToBackStack(null);
                transaction.commit();
            }
        });
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case R.id.notification_action: {
            FragmentTransaction transaction = manager.beginTransaction();
            transaction.replace(R.id.fragment_container, singalMessagesFragment);
            transaction.addToBackStack(null);
            transaction.commit();
        }
            break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void displayFragment(int fragmentID, Object extraData) {
        FragmentTransaction transaction = manager.beginTransaction();
        Bundle args = new Bundle();
        switch (fragmentID) {

        case FragmentCommunicationInterface.ALARMS_MANAGER:
            args.putSerializable("UUID", (UUID) extraData);
            alarmMangerFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, alarmMangerFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;

        case FragmentCommunicationInterface.TIMERS_MANAGER:
            args.putSerializable("UUID", (UUID) extraData);
            timerMangerFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, timerMangerFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;

        case FragmentCommunicationInterface.ALARM_NEW:
            args.putSerializable("UUID", (UUID) extraData);
            args.putString("NEW", "YES");
            alarmFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, alarmFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;

        case FragmentCommunicationInterface.ALARM_EDIT:
            args.putSerializable("UUID", (UUID) extraData);
            args.putString("EDIT", "YES");
            alarmFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, alarmFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;

        case FragmentCommunicationInterface.TIMER_NEW:
            args.putSerializable("UUID", (UUID) extraData);
            args.putString("NEW", "YES");
            timerFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, timerFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;

        case FragmentCommunicationInterface.TIMER_EDIT:
            args.putSerializable("UUID", (UUID) extraData);
            args.putString("EDIT", "YES");
            timerFragment.setArguments(args);
            transaction.replace(R.id.fragment_container, timerFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            break;
        }
    }

    @Override
    public void setObject(Object obj) {
        storageObject = obj;
    }

    @Override
    public Object getObject() {
        return storageObject;
    }

}
