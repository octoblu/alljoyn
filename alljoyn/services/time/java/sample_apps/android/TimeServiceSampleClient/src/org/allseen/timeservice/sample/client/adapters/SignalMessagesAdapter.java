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

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Locale;

import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.dataobjects.SignalMessageItem;

import android.content.Context;
import android.graphics.Paint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

/**
 * Adapter to display {@link SignalMessageItem}
 */
public class SignalMessagesAdapter extends BaseAdapter {

    private List<SignalMessageItem> values;
    private final LayoutInflater inflater;

    private final SimpleDateFormat simpleDateFormat = new SimpleDateFormat("HH:mm:ss", Locale.US);

    static class DataObjectHolder {
        TextView servicename;
        TextView timestamp;
        TextView object_path;
        TextView message;

    }

    public SignalMessagesAdapter(Context context, List<SignalMessageItem> values) {
        this.values = values;
        this.inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public void update(List<SignalMessageItem> values) {
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

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View row = convertView;
        DataObjectHolder holder = null;
        if (row == null) {
            row = inflater.inflate(R.layout.signal_message_row_layout, parent, false);
            holder = new DataObjectHolder();
            holder.timestamp = (TextView) row.findViewById(R.id.timestamp);
            holder.object_path = (TextView) row.findViewById(R.id.object_path);
            holder.message = (TextView) row.findViewById(R.id.message);
            holder.servicename = (TextView) row.findViewById(R.id.servicename);
            row.setTag(holder);
        } else {
            holder = (DataObjectHolder) row.getTag();
        }

        SignalMessageItem current = values.get(position);
        holder.object_path.setText(current.objectpath);
        holder.message.setText(current.message);
        holder.timestamp.setText(simpleDateFormat.format(current.timestamp));
        holder.servicename.setText(current.servicename);

        setUnderline(holder.servicename);
        setUnderline(holder.object_path);
        setUnderline(holder.message);

        return row;

    }

    private void setUnderline(TextView textView) {
        textView.setPaintFlags(textView.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);
    }

}
