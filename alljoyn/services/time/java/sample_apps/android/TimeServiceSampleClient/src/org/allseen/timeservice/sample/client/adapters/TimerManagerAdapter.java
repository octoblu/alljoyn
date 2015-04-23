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
package org.allseen.timeservice.sample.client.adapters;

import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.dataobjects.TimerItem;

import android.content.Context;
import android.graphics.Paint;
import android.os.AsyncTask;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Switch;
import android.widget.TextView;

/**
 * Adapter to display {@link TimerItem}
 */
public class TimerManagerAdapter extends BaseAdapter {
    private static final String TAG = "TimerManagerAdapter";
    private List<TimerItem> values;
    private final LayoutInflater inflater;
    private final Context context;

    private final Map<String, FecthAsyncTimerData> updateMap = new Hashtable<String, FecthAsyncTimerData>();

    /**
     * Internal class to fetch the remote Timer data to display accurate remaining time
     */
    private class FecthAsyncTimerData extends AsyncTask<DataObjectHolder, Void, TimerItem> {

        private DataObjectHolder holder;
        private int position;

        @Override
        protected TimerItem doInBackground(DataObjectHolder... params) {
            holder = params[0];
            position = Integer.valueOf((String) holder.refresh_progressbar.getTag());
            TimerItem currentTimerItem = values.get(position);
            currentTimerItem.objectpath = currentTimerItem.timer.getObjectPath();
            try {
                currentTimerItem.description = currentTimerItem.timer.retrieveTitle();

                Period periodInterval = currentTimerItem.timer.retrieveInterval();
                Period periodRemaining = currentTimerItem.timer.retrieveTimeLeft();
                currentTimerItem.running = currentTimerItem.timer.retrieveIsRunning();
                currentTimerItem.repeat = currentTimerItem.timer.retrieveRepeat();
                currentTimerItem.interval = String.format("%02d:%02d:%02d", periodInterval.getHour(), periodInterval.getMinute(), periodInterval.getSeconds());
                currentTimerItem.remaining = String.format("%02d:%02d:%02d", periodRemaining.getHour(), periodRemaining.getMinute(), periodRemaining.getSeconds());
            } catch (TimeServiceException e) {
                Log.e(TAG, "Error fetching timer for [" + currentTimerItem.objectpath + "]", e);
                return null;
            }
            return currentTimerItem;

        }

        @Override
        protected void onPostExecute(TimerItem result) {
            holder.refresh_progressbar.setVisibility(View.GONE);
            holder.refresh_button.setVisibility(View.VISIBLE);
            // Only in case that we have a valid result and we are "seeing" the
            // correct holder than we update the UI.
            if (result != null) {
                if (holder.refresh_progressbar.getTag().equals(String.valueOf(position))) {
                    fillObjectHolder(result, holder);
                }
            }
            updateMap.remove(String.valueOf(position));

        }
    }

    static class DataObjectHolder {
        TextView remaining;
        ImageView lock;
        Switch running;
        ImageView refresh_button;
        ProgressBar refresh_progressbar;
        TextView interval;
        TextView repeat;
        ImageView register;
        TextView description;
        TextView objectpath;
    }

    public TimerManagerAdapter(Context context, List<TimerItem> values) {
        this.context = context;
        this.values = values;
        this.inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    @Override
    public int getCount() {
        return values.size();
    }

    @Override
    public Object getItem(int position) {
        return values.get(position);
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    public void update(List<TimerItem> values) {
        this.values = values;
        notifyDataSetChanged();
    }

    public void removeItem(int location) {
        values.remove(location);
        notifyDataSetChanged();
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        View row = convertView;
        DataObjectHolder holder = null;
        if (row == null) {
            row = inflater.inflate(R.layout.timer_row_layout, parent, false);
            holder = new DataObjectHolder();
            holder.remaining = (TextView) row.findViewById(R.id.remaining);
            holder.lock = (ImageView) row.findViewById(R.id.lock);
            holder.refresh_button = (ImageView) row.findViewById(R.id.refresh_button);
            holder.refresh_progressbar = (ProgressBar) row.findViewById(R.id.refresh_progressbar);
            holder.running = (Switch) row.findViewById(R.id.running);
            holder.interval = (TextView) row.findViewById(R.id.interval);
            holder.repeat = (TextView) row.findViewById(R.id.repeat_value);
            holder.description = (TextView) row.findViewById(R.id.description);
            holder.objectpath = (TextView) row.findViewById(R.id.objectpath);
            holder.register = (ImageView) row.findViewById(R.id.register);
            row.setTag(holder);
        } else {
            holder = (DataObjectHolder) row.getTag();
        }

        final DataObjectHolder workingHolder = holder;
        final TimerItem currentTimerItem = values.get(position);
        final String key = String.valueOf(position);

        if (currentTimerItem.running) {
            if (updateMap.containsKey(key)) {
                holder.refresh_button.setVisibility(View.GONE);
                holder.refresh_progressbar.setVisibility(View.VISIBLE);
            } else {
                holder.refresh_button.setVisibility(View.VISIBLE);
                holder.refresh_progressbar.setVisibility(View.GONE);
            }
            if (holder.refresh_button.getVisibility() == View.VISIBLE) {
                holder.refresh_button.setOnClickListener(new OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        workingHolder.refresh_button.setVisibility(View.GONE);
                        workingHolder.refresh_progressbar.setVisibility(View.VISIBLE);

                        workingHolder.refresh_progressbar.setTag(key);
                        FecthAsyncTimerData fecthAsyncTimerData = new FecthAsyncTimerData();
                        updateMap.put(key, fecthAsyncTimerData);
                        fecthAsyncTimerData.execute(workingHolder);
                    }
                });
            }
        } else {
            holder.refresh_progressbar.setVisibility(View.GONE);
            holder.refresh_button.setVisibility(View.GONE);
        }

        fillObjectHolder(currentTimerItem, workingHolder);
        return row;

    }

    private void fillObjectHolder(final TimerItem currentTimerItem, DataObjectHolder currentHolder) {

        currentHolder.remaining.setText(currentTimerItem.remaining);
        currentHolder.interval.setText(currentTimerItem.interval);
        currentHolder.running.setChecked(currentTimerItem.running);
        currentHolder.description.setPaintFlags(currentHolder.description.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);
        currentHolder.objectpath.setPaintFlags(currentHolder.objectpath.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);

        if (currentTimerItem.repeat == org.allseen.timeservice.ajinterfaces.Timer.REPEAT_FOREVER) {
            currentHolder.repeat.setText(context.getResources().getString(R.string.forever));
        } else if (currentTimerItem.repeat == 0) {
            currentHolder.repeat.setText(context.getResources().getString(R.string.none));
        } else {
            currentHolder.repeat.setText(String.valueOf(currentTimerItem.repeat));
        }
        if (currentTimerItem.description.isEmpty()) {
            currentHolder.description.setText(context.getResources().getString(R.string.none));
        } else {
            currentHolder.description.setText(currentTimerItem.description);
        }
        if (!currentTimerItem.isLocked) {
            currentHolder.lock.setVisibility(View.GONE);
        } else {
            currentHolder.lock.setVisibility(View.VISIBLE);
        }
        currentHolder.objectpath.setText(currentTimerItem.objectpath);

        if (currentTimerItem.timer.getTimerHandler() != null) {
            currentHolder.register.setVisibility(View.VISIBLE);
        } else {
            currentHolder.register.setVisibility(View.GONE);
        }
    }

}
