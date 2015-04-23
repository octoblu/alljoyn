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

import java.util.Calendar;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import org.allseen.timeservice.sample.server.R;
import org.allseen.timeservice.sample.server.logic.TimeChangeListener;
import org.allseen.timeservice.sample.server.logic.TimeOffsetManager;

import android.app.ActionBar;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v4.view.MenuItemCompat;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

public class MainActivity extends Activity {
    private TextView timerText;
    private Timer timer = new Timer();
    private MessagesAdapter adapter;
    private ViewGroup mainLayout = null;
    private ListView messageListView;
    private final IntentFilter filter = new IntentFilter();

    /**
     * Create a TimerTask to update timerText.
     *
     * @return
     */
    private TimerTask generateUpdateTimeDisplayTask() {
        return new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        timerText.setText(TimeOffsetManager.getInstance().getTimeDisplayFormat().format(TimeOffsetManager.getInstance().getCurrentServerTime().getTime()));
                    }
                });
            }
        };
    }

    /**
     * Listens to incoming messages ,updates the adapter .
     */
    private final BroadcastReceiver messagesReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            Bundle bundle = intent.getExtras();
            if (bundle != null) {
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        adapter.update(((TimeSampleServer) getApplication()).getMessagesList());
                    }
                });
            }

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final ActionBar actionBar = getActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_STANDARD);

        mainLayout = (ViewGroup) getLayoutInflater().inflate(R.layout.activity_main, null);
        messageListView = (ListView) mainLayout.findViewById(R.id.main_log_list);

        TextView logtitle = (TextView) mainLayout.findViewById(R.id.logtilte);
        logtitle.setText(getResources().getString(R.string.logTitle, ((TimeSampleServer) getApplication()).getProtocolManager().getBusAttachment().getUniqueName()));

        Button clearList = (Button) mainLayout.findViewById(R.id.clearlist);
        clearList.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                ((TimeSampleServer) getApplication()).getMessagesList().clear();
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        adapter.update(((TimeSampleServer) getApplication()).getMessagesList());
                        messageListView.smoothScrollToPosition(0);
                    }
                });
            }
        });

        filter.addAction(TimeSampleServer.TIMER_EVENT_ACTION);
        filter.addAction(TimeSampleServer.ALARM_EVENT_ACTION);
        filter.addAction(TimeSampleServer.CLOCK_EVENT_ACTION);
        filter.addAction(TimeSampleServer.GENERAL_ACTION);

        adapter = new MessagesAdapter(this, ((TimeSampleServer) getApplication()).getMessagesList());
        messageListView.setAdapter(adapter);

        TimeOffsetManager.getInstance().registerTimeChangeListener(new TimeChangeListener() {
            @Override
            public void timeHasChanged(long newTime) {
                timer.cancel();
                timer.purge();
                timer = new Timer();
                timer.scheduleAtFixedRate(generateUpdateTimeDisplayTask(), 0, 1000);
            }
        });

        LocalBroadcastManager.getInstance(this).registerReceiver(messagesReceiver, filter);
        setContentView(mainLayout);
    }

    @Override
    public void onResume() {
        super.onResume();
        adapter.update(((TimeSampleServer) getApplication()).getMessagesList());
        messageListView.smoothScrollToPosition(adapter.getCount() - 1);
        LocalBroadcastManager.getInstance(this).registerReceiver(messagesReceiver, filter);

    }

    @Override
    public void onPause() {
        super.onPause();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(messagesReceiver);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        MenuItem timerItem = menu.findItem(R.id.break_timer);
        timerText = (TextView) MenuItemCompat.getActionView(timerItem);
        timerText.setTextSize(20);
        timer.scheduleAtFixedRate(generateUpdateTimeDisplayTask(), 0, 1000);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case R.id.action_settings: {
            Intent launchIntent = new Intent(MainActivity.this, SettingsActivity.class);
            startActivity(launchIntent);
        }
            break;
        }
        return super.onOptionsItemSelected(item);
    }

    private static class MessagesAdapter extends BaseAdapter {

        private List<TimeSampleServer.MessageItem> values;
        private final LayoutInflater inflater;
        private final Context context;
        private final Calendar calendar = Calendar.getInstance();

        @Override
        public int getCount() {
            if (values != null) {
                return values.size();
            }
            return 0;

        }

        @Override
        public Object getItem(int position) {
            if (values != null) {
                return values.get(position);
            }
            return null;
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            View row = convertView;
            DataObjectHolder holder = null;
            if (row == null) {
                row = inflater.inflate(R.layout.message_row_layout, parent, false);
                holder = new DataObjectHolder();
                holder.title = (TextView) row.findViewById(R.id.type_tilte);
                holder.timestamp = (TextView) row.findViewById(R.id.timestamp);
                holder.message = (TextView) row.findViewById(R.id.message);
                holder.objectpath = (TextView) row.findViewById(R.id.objectpath);
                row.setTag(holder);
            } else {
                holder = (DataObjectHolder) row.getTag();
            }
            TimeSampleServer.MessageItem item = values.get(position);
            if (item.type.equals(TimeSampleServer.ALARM_EVENT_ACTION)) {
                holder.title.setBackgroundColor(context.getResources().getColor(R.color.green));
            } else if (item.type.equals(TimeSampleServer.CLOCK_EVENT_ACTION)) {
                holder.title.setBackgroundColor(context.getResources().getColor(R.color.red));
            } else if (item.type.equals(TimeSampleServer.TIMER_EVENT_ACTION)) {
                holder.title.setBackgroundColor(context.getResources().getColor(R.color.holo_light_blue));
            } else if (item.type.equals(TimeSampleServer.GENERAL_ACTION)) {
                holder.title.setBackgroundColor(context.getResources().getColor(R.color.gray));
            }
            calendar.setTimeInMillis(item.timestamp);

            holder.title.setText(item.type);
            holder.timestamp.setText(TimeOffsetManager.getInstance().getDateAndTimeDisplayFormat().format(calendar.getTime()));
            holder.message.setText(item.message);
            if (item.objectPath == null) {
                row.findViewById(R.id.objectpath_layout).setVisibility(View.GONE);
            } else {
                row.findViewById(R.id.objectpath_layout).setVisibility(View.VISIBLE);
                holder.objectpath.setText(item.objectPath);
            }

            return row;
        }

        public MessagesAdapter(Context context, List<TimeSampleServer.MessageItem> values) {
            this.context = context;
            this.values = values;
            this.inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void update(List<TimeSampleServer.MessageItem> values) {
            this.values = values;
            notifyDataSetChanged();
        }

        static class DataObjectHolder {
            TextView title;
            TextView timestamp;
            TextView message;
            TextView objectpath;
        }

    }

}
