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
import org.allseen.timeservice.sample.client.dataobjects.ClientServiceItem;

import android.content.Context;
import android.graphics.Paint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;


/**
 * Adapter to display {@link ClientServiceItem}
 */
public class ClientServiceAdapter extends BaseAdapter {

    private List<ClientServiceItem> values;
    private final LayoutInflater inflater;
    private final Context context;

    static class DataObjectHolder {
        TextView name;
        TextView busname;
        TextView alarms;
        TextView timers;
        TextView clock;
        TextView timeauthority;
        ImageView sync;
    }

    public ClientServiceAdapter(Context context, List<ClientServiceItem> values) {
        this.context = context;
        this.values = values;
        this.inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }



    public void update(List<ClientServiceItem> values ){
        this.values=values;
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

    private void colorText(boolean value, TextView view) {
        if (value) {
            view.setTextColor(context.getResources().getColor(R.color.green));
        } else {
            view.setTextColor(context.getResources().getColor(R.color.gray));
        }
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View row = convertView;
        DataObjectHolder holder = null;
        if (row == null) {
            row = inflater.inflate(R.layout.client_service_row_layout, parent, false);
            holder = new DataObjectHolder();
            holder.name = (TextView) row.findViewById(R.id.name_value);
            holder.busname = (TextView) row.findViewById(R.id.busname_value);
            holder.alarms = (TextView) row.findViewById(R.id.alarms_label);
            holder.timers = (TextView) row.findViewById(R.id.timers_label);
            holder.clock = (TextView) row.findViewById(R.id.clock_label);
            holder.timeauthority = (TextView) row.findViewById(R.id.timeauthority_label);
            holder.sync = (ImageView) row.findViewById(R.id.sync_image);
            row.setTag(holder);
        } else {
            holder = (DataObjectHolder) row.getTag();
        }

        ClientServiceItem current = values.get(position);
        holder.name.setText(current.name);
        holder.busname.setText(current.busName);

        colorText(current.alarms, holder.alarms);
        colorText(current.timers, holder.timers);
        colorText(current.clock, holder.clock);
        colorText(current.timeAuthority, holder.timeauthority);
        holder.busname.setPaintFlags( holder.busname.getPaintFlags()|Paint.UNDERLINE_TEXT_FLAG);
        holder.name.setPaintFlags( holder.name.getPaintFlags()|Paint.UNDERLINE_TEXT_FLAG);

        if (current.syncTimeRegistered) {
            holder.sync.setVisibility(View.VISIBLE);
        } else {
            holder.sync.setVisibility(View.GONE);
        }

        return row;

    }

}
