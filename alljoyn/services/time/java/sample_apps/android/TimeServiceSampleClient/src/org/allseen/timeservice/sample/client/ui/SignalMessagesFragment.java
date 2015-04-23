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
import java.util.List;

import org.allseen.timeservice.sample.client.R;
import org.allseen.timeservice.sample.client.adapters.SignalMessagesAdapter;
import org.allseen.timeservice.sample.client.application.TimeSampleClient;
import org.allseen.timeservice.sample.client.dataobjects.SignalMessageItem;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ListView;

/**
 *
 * Display Signal Messages sent by internal broadcast {@link TimeSampleClient#getSignalMessages()}
 */
public class SignalMessagesFragment extends Fragment {
    private SignalMessagesAdapter signalMessagesAdapter;
    private Context context;
    private ProgressDialog progressDialog = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_alarm_manager, container, false);

        ListView listView = (ListView) rootView.findViewById(R.id.alarm_list);

        rootView.findViewById(R.id.button_layout).setVisibility(View.VISIBLE);
        rootView.findViewById(R.id.add).setVisibility(View.GONE);
        rootView.findViewById(R.id.dismiss).setVisibility(View.VISIBLE);
        rootView.findViewById(R.id.dismiss).setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                ((TimeSampleClient) context.getApplicationContext()).getSignalMessages().clear();
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        signalMessagesAdapter.update(((TimeSampleClient) context.getApplicationContext()).getSignalMessages());
                    }
                });
            }
        });

        signalMessagesAdapter = new SignalMessagesAdapter(context, new ArrayList<SignalMessageItem>());
        listView.setAdapter(signalMessagesAdapter);

        new AsyncTask<Void, Void, Void>() {
            List<SignalMessageItem> list;
            @Override
            protected Void doInBackground(Void... params) {
                list = ((TimeSampleClient) context.getApplicationContext()).getSignalMessages();
                for (SignalMessageItem item : list) {
                    item.seen = true;
                }
                return null;
            }
            @Override
            protected void onPostExecute(Void result) {
                signalMessagesAdapter.update(list);
                Intent sendIntent = new Intent(context, MainActivity.class);
                sendIntent.setAction(TimeSampleClient.CLEAR_SIGNAL_MESSAGE_ACTION);
                sendIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(sendIntent);
            }
        }.execute();

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(context.getResources().getString(R.string.notification_title));
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        context = activity;
        progressDialog = new ProgressDialog(getActivity());
        activity.setTitle(context.getResources().getString(R.string.timer_manager));
        progressDialog.setMessage(context.getResources().getString(R.string.wait_message));
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setIndeterminate(true);
    }

}
