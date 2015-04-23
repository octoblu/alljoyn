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

package org.allseen.timeservice.test;

import org.allseen.timeservice.AuthorityType;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class CommandsReceiver extends BroadcastReceiver {

    public static enum TSAction {

        //=================== SERVER =====================//
        TS_SERVER_START,
        TS_SERVER_STOP,
        TS_SERVER_ANNOUNCE,

        //Clock
        TS_SERVER_CREATE_CLOCK,
        TS_SERVER_CREATE_TA_CLOCK,
        TS_SERVER_TIME_SYNC,
        TS_SERVER_SHOW_CLOCKS,

        //Alarm
        TS_SERVER_CREATE_ALARM,
        TS_SERVER_CREATE_CS_ALARM,
        TS_SERVER_SHOW_ALARMS,
        TS_SERVER_ALARM_REACHED,

        //Alarm Factory
        TS_SERVER_CREATE_ALARM_FACTORY,
        TS_SERVER_SHOW_ALARM_FACTORY,

        //Timer
        TS_SERVER_CREATE_TIMER,
        TS_SERVER_SHOW_TIMERS,

        //Timer Factory
        TS_SERVER_CREATE_TIMER_FACTORY,
        TS_SERVER_SHOW_TIMER_FACTORY,

        //=================== CLIENT =====================//

        TS_CLIENT_START,
        TS_CLIENT_STOP,
        TS_CLIENT_CONNECT,
        TS_CLIENT_DISCONNECT,
        TS_CLIENT_PRINT_DEVICES,

        //Clock
        TS_CLIENT_PRINT_CLOCK,
        TS_CLIENT_SET_CLOCK,
        TS_CLIENT_TIME_SYNC,
        TS_CLIENT_TIME_SYNC_STOP,

        //Alarm
        TS_CLIENT_PRINT_ALARM,
        TS_CLIENT_PRINT_CS_ALARM,
        TS_CLIENT_SET_ALARM,
        TS_CLIENT_ALARM_REACHED,
        TS_CLIENT_ALARM_REACHED_STOP,

        //Alarm Factory
        TS_CLIENT_FACTORY_NEW_ALARM,
        TS_CLIENT_FACTORY_DELETE_ALARM,
        TS_CLIENT_FACTORY_GET_ALARMS,

        //Timer
        TS_CLIENT_PRINT_TIMER,
        TS_CLIENT_SET_TIMER,
        TS_CLIENT_TIMER_SIGNAL,
        TS_CLIENT_TIMER_SIGNAL_STOP,
        TS_CLIENT_TIMER_START,
        TS_CLIENT_TIMER_PAUSE,
        TS_CLIENT_TIMER_RESET,

        //Timer Factory
        TS_CLIENT_FACTORY_NEW_TIMER,
        TS_CLIENT_FACTORY_DELETE_TIMER,
        TS_CLIENT_FACTORY_GET_TIMERS,
        ;
    }

    @Override
    public void onReceive(Context context, Intent intent) {

        String action     = intent.getAction();
        TSAction tsAction = TSAction.valueOf(TSAction.class, action);

        TimeServiceTestApp app = (TimeServiceTestApp) context.getApplicationContext();

        switch(tsAction) {

            case TS_SERVER_START: {

                app.startServer();
                break;
            }
            case TS_SERVER_STOP: {

                app.stopServer();
                break;
            }
            case TS_SERVER_ANNOUNCE: {

                app.getTestServer().announceServer();
                break;
            }
            case TS_SERVER_CREATE_CLOCK: {

                app.getTestServer().createClock();
                break;
            }
            case TS_SERVER_CREATE_TA_CLOCK: {

                String typeStr     = intent.getStringExtra("TYPE");
                typeStr            = (typeStr == null) ? "0" : typeStr;
                AuthorityType type = AuthorityType.getType(Byte.parseByte(typeStr));
                app.getTestServer().createTimeAuthorityClock(type);
                break;
            }
            case TS_SERVER_TIME_SYNC: {

                String objPath  = intent.getStringExtra("OBJ");
                app.getTestServer().sendTimeSync(objPath);
                break;
            }
            case TS_SERVER_SHOW_CLOCKS: {

                app.getTestServer().showClocks();
                break;
            }
            case TS_SERVER_CREATE_ALARM: {

                String desc  = intent.getStringExtra("DESC");
                app.getTestServer().createAlarm(desc);
                break;
            }
            case TS_SERVER_CREATE_CS_ALARM: {

                String desc  = intent.getStringExtra("DESC");
                app.getTestServer().createCustomAlarm(desc);
                break;
            }
            case TS_SERVER_SHOW_ALARMS: {

                app.getTestServer().showAlarms();
                break;
            }
            case TS_SERVER_ALARM_REACHED: {

                String objPath  = intent.getStringExtra("OBJ");
                app.getTestServer().alarmReached(objPath);
                break;
            }
            case TS_SERVER_CREATE_ALARM_FACTORY: {

                String desc  = intent.getStringExtra("DESC");
                app.getTestServer().createAlarmFactory(desc);
                break;
            }
            case TS_SERVER_SHOW_ALARM_FACTORY: {

                app.getTestServer().showAlarmFactories();
                break;
            }
            case TS_SERVER_CREATE_TIMER: {

                String desc  = intent.getStringExtra("DESC");
                app.getTestServer().createTimer(desc);
                break;
            }
            case TS_SERVER_SHOW_TIMERS: {

                app.getTestServer().showTimers();
                break;
            }
            case TS_SERVER_CREATE_TIMER_FACTORY: {

                String desc  = intent.getStringExtra("DESC");
                app.getTestServer().createTimerFactory(desc);
                break;
            }
            case TS_SERVER_SHOW_TIMER_FACTORY: {

                app.getTestServer().showTimerFactories();
                break;
            }

            //=================== CLIENT ==================//

            case TS_CLIENT_START: {

                app.startClient();
                break;
            }
            case TS_CLIENT_STOP: {

                app.stopClient();
                break;
            }
            case TS_CLIENT_CONNECT: {

                String key = intent.getStringExtra("KEY");
                app.getTestClient().connect(key);
                break;
            }
            case TS_CLIENT_DISCONNECT: {

                String key = intent.getStringExtra("KEY");
                app.getTestClient().disconnect(key);
                break;
            }
            case TS_CLIENT_PRINT_DEVICES: {

                app.getTestClient().printDevices();
                break;
            }
            case TS_CLIENT_PRINT_CLOCK: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().printClock(key, objPath);
                break;
            }
            case TS_CLIENT_SET_CLOCK: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().setClock(key, objPath);
                break;
            }
            case TS_CLIENT_TIME_SYNC: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().regTimeSyncHandler(key, objPath);
                break;
            }
            case TS_CLIENT_TIME_SYNC_STOP: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().unRegTimeSyncHandler(key, objPath);
                break;
            }
            case TS_CLIENT_PRINT_ALARM: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().printAlarm(key, objPath);
                break;
            }
            case TS_CLIENT_PRINT_CS_ALARM: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().printCsAlarm(key, objPath);
                break;
            }
            case TS_CLIENT_SET_ALARM: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                String days     = intent.getStringExtra("DAYS");
                String enabled  = intent.getStringExtra("ENA");
                String title    = intent.getStringExtra("TITLE");
                app.getTestClient().setAlarm(key, objPath, days, title, enabled);
                break;
            }
            case TS_CLIENT_ALARM_REACHED: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().regAlarmHandler(key, objPath);
                break;
            }
            case TS_CLIENT_ALARM_REACHED_STOP: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().unRegAlarmHandler(key, objPath);
                break;
            }
            case TS_CLIENT_FACTORY_NEW_ALARM: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().newAlarm(key, objPath);
                break;
            }
            case TS_CLIENT_FACTORY_DELETE_ALARM: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                String alarm    = intent.getStringExtra("ALARM");
                app.getTestClient().deleteAlarm(key, objPath, alarm);
                break;
            }
            case TS_CLIENT_FACTORY_GET_ALARMS: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().getAlarms(key, objPath);
                break;
            }
            case TS_CLIENT_PRINT_TIMER: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().printTimer(key, objPath);
                break;
            }
            case TS_CLIENT_SET_TIMER: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                String interval = intent.getStringExtra("INT");
                String repeat   = intent.getStringExtra("REP");
                String title    = intent.getStringExtra("TITLE");
                app.getTestClient().setTimer(key, objPath, interval, repeat, title);
                break;
            }
            case TS_CLIENT_TIMER_SIGNAL: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().regTimerHandler(key, objPath);
                break;
            }
            case TS_CLIENT_TIMER_SIGNAL_STOP: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().unRegTimerHandler(key, objPath);
                break;
            }
            case TS_CLIENT_TIMER_START: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().timerStart(key, objPath);
                break;
            }
            case TS_CLIENT_TIMER_PAUSE: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().timerPause(key, objPath);
                break;
            }
            case TS_CLIENT_TIMER_RESET: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().timerReset(key, objPath);
                break;
            }
            case TS_CLIENT_FACTORY_NEW_TIMER: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().newTimer(key, objPath);
                break;
            }
            case TS_CLIENT_FACTORY_DELETE_TIMER: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                String timer    = intent.getStringExtra("TIMER");
                app.getTestClient().deleteTimer(key, objPath, timer);
                break;
            }
            case TS_CLIENT_FACTORY_GET_TIMERS: {

                String key      = intent.getStringExtra("KEY");
                String objPath  = intent.getStringExtra("OBJ");
                app.getTestClient().getTimers(key, objPath);
                break;
            }

        }
    }

}
