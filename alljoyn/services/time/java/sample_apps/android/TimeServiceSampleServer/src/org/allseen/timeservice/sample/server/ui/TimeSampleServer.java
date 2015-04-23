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

import java.util.ArrayList;
import java.util.List;

import org.allseen.timeservice.sample.server.logic.PreferencesManager;
import org.allseen.timeservice.sample.server.logic.ProtocolManager;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

/**
 * TimeServiceSampleServerApplication listen to incoming messages via broadcast receiver and updates the messageItemsList with them.
 *
 *
 */
public class TimeSampleServer extends Application {

    private static final String TAG = "TimeServiceSampleServerApplication";

    public static final String TIMER_EVENT_ACTION = "TimerEvent";
    public static final String ALARM_EVENT_ACTION = "AlarmEvent";
    public static final String CLOCK_EVENT_ACTION = "ClockEvent";
    public static final String GENERAL_ACTION = "GeneralEvent";

    public static final String MESSAGE_KEY = "message";
    public static final String OBJECT_PATH_KEY = "objectpath";

    public static class MessageItem {
        public long timestamp;
        public String type;
        public String objectPath;
        public String message;
    }

    private final ProtocolManager protocolManager = new ProtocolManager();
    static {
        try {
            System.loadLibrary("alljoyn_java");
        } catch (Exception e) {
            Log.d("TimeServiceSampleServerApplication", "Failed to load alljoyn_java.so", e);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        IntentFilter filter = new IntentFilter();
        filter.addAction(TIMER_EVENT_ACTION);
        filter.addAction(ALARM_EVENT_ACTION);
        filter.addAction(CLOCK_EVENT_ACTION);
        filter.addAction(GENERAL_ACTION);
        LocalBroadcastManager.getInstance(this).registerReceiver(messagesReceiver, filter);
        PreferencesManager.getInstance().init(this);
        protocolManager.init(this);

    }

    private final BroadcastReceiver messagesReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            synchronized (TAG) {
                Bundle bundle = intent.getExtras();
                if (bundle != null) {
                    MessageItem item = new MessageItem();
                    item.message = bundle.getString(MESSAGE_KEY);
                    item.objectPath = bundle.getString(OBJECT_PATH_KEY);
                    item.type = intent.getAction();
                    item.timestamp = System.currentTimeMillis();
                    Log.d(TAG, intent.getAction() + "  received from [" + item.objectPath + "] " + item.message);
                    messageItemsList.add(item);
                }
            }
        }
    };

    private final List<MessageItem> messageItemsList = new ArrayList<MessageItem>();

    public List<MessageItem> getMessagesList() {
        synchronized (TAG) {
            return messageItemsList;
        }
    }

    public static void sendMessage(Context context, String type, String ObjectPath, String message) {
        Intent intent = new Intent(type);
        intent.putExtra(TimeSampleServer.MESSAGE_KEY, message);
        intent.putExtra(TimeSampleServer.OBJECT_PATH_KEY, ObjectPath);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    public ProtocolManager getProtocolManager() {
        return protocolManager;
    }
}
