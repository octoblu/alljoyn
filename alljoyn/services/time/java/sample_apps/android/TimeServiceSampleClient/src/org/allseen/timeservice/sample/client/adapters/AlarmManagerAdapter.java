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

import java.util.List;

import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.dataobjects.AlarmItem;

import android.content.Context;
import android.graphics.Paint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

/**
 * Adapter to display {@link AlarmItem}
 */
public class AlarmManagerAdapter extends BaseAdapter {

    private List<AlarmItem> values;
    private final LayoutInflater inflater;
    private final Context context;

    static class DataObjectHolder {
        TextView time;
        ImageView lock;
        Switch enabled;
        TextView days[] = new TextView[7];
        ImageView register;
        TextView description;
        TextView objectpath;
    }

    public AlarmManagerAdapter(Context context, List<AlarmItem> values) {
        this.context = context;
        this.values = values;
        this.inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public void update(List<AlarmItem> values) {
        this.values = values;
        notifyDataSetChanged();
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

    public void removeItem(int location) {
        values.remove(location);
        notifyDataSetChanged();
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View row = convertView;
        DataObjectHolder holder = null;
        if (row == null) {
            row = inflater.inflate(R.layout.alarm_row_layout, parent, false);
            holder = new DataObjectHolder();
            holder.time = (TextView) row.findViewById(R.id.time);
            holder.enabled = (Switch) row.findViewById(R.id.enabled);
            holder.description = (TextView) row.findViewById(R.id.description);
            holder.objectpath = (TextView) row.findViewById(R.id.objectpath);
            holder.lock = (ImageView) row.findViewById(R.id.lock);
            holder.register = (ImageView) row.findViewById(R.id.register);
            for (int i = 0; i < holder.days.length; i++) {
                holder.days[i] = (TextView) row.findViewById(R.id.day0 + i);
            }
            row.setTag(holder);
        } else {
            holder = (DataObjectHolder) row.getTag();
        }
        AlarmItem currentAlarmItem = values.get(position);
        holder.time.setText(String.format("%02d:%02d:%02d", currentAlarmItem.hour, currentAlarmItem.minutes, currentAlarmItem.seconds));
        holder.enabled.setChecked(currentAlarmItem.enabled);
        holder.description.setText(currentAlarmItem.description);

        if (currentAlarmItem.description.isEmpty()) {
            holder.description.setText(context.getResources().getString(R.string.none));
        } else {
            holder.description.setText(currentAlarmItem.description);
        }

        if (!currentAlarmItem.isLocked) {
            holder.lock.setVisibility(View.GONE);
        } else {
            holder.lock.setVisibility(View.VISIBLE);
        }

        holder.objectpath.setText(currentAlarmItem.objectpath);
        holder.description.setPaintFlags(holder.description.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);
        holder.objectpath.setPaintFlags(holder.objectpath.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);

        if (currentAlarmItem.isSignalRegistered) {
            holder.register.setVisibility(View.VISIBLE);
        } else {
            holder.register.setVisibility(View.GONE);
        }

        for (int i = 0; i < holder.days.length; i++) {
            if (!currentAlarmItem.arrayDays[i]) {
                holder.days[i].setTextColor(context.getResources().getColor(R.color.dark_gray));
            } else {
                holder.days[i].setTextColor(context.getResources().getColor(R.color.green));
            }
        }
        return row;

    }

}
