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
package org.allseen.timeservice.sample.server.logic;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

/**
 * Singleton that manages the preferences of the application. Store the preferences of the application using SharedPreferences
 *
 */

public class PreferencesManager {

    private Context context;

    private static final PreferencesManager instance = new PreferencesManager();

    private PreferencesManager() {
    }

    public static final String PREFERENCES = "prfrences";

    private static final  String PREFERENCE_CLOCKSETABLE = "clockStable";
    private static final  String PREFERENCE_TIMEAUTHORITY = "timeAuthority";
    private static final  String PREFERENCE_ALARMFACTORY = "alarmFactory";
    private static final  String PREFERENCE_ALRAM1 = "alarm1";
    private static final  String PREFERENCE_TIMERFACTORY = "timerFactory";
    private static final  String PREFERENCE_TIMER1 = "timer1";

    public static PreferencesManager getInstance() {
        return instance;
    }

    public void init(Context context) {
        this.context = context;
    }

    public void setPrefrences(AppPreferences appPrefrences) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(PREFERENCES, Context.MODE_PRIVATE);
        Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREFERENCE_CLOCKSETABLE, appPrefrences.clockSetable);
        editor.putBoolean(PREFERENCE_TIMEAUTHORITY, appPrefrences.timeAuthority);
        editor.putBoolean(PREFERENCE_ALARMFACTORY, appPrefrences.alarmFactory);
        editor.putBoolean(PREFERENCE_ALRAM1, appPrefrences.alarm1);
        editor.putBoolean(PREFERENCE_TIMERFACTORY, appPrefrences.timerFactory);
        editor.putBoolean(PREFERENCE_TIMER1, appPrefrences.timer1);
        editor.commit();
    }

    public AppPreferences getPrefrences() {
        SharedPreferences sharedPreferences = context.getSharedPreferences(PREFERENCES, Context.MODE_PRIVATE);
        AppPreferences appPrefrences = new AppPreferences();
        appPrefrences.clockSetable = sharedPreferences.getBoolean(PREFERENCE_CLOCKSETABLE, true);
        appPrefrences.timeAuthority = sharedPreferences.getBoolean(PREFERENCE_TIMEAUTHORITY, true);
        appPrefrences.alarmFactory = sharedPreferences.getBoolean(PREFERENCE_ALARMFACTORY, true);
        appPrefrences.alarm1 = sharedPreferences.getBoolean(PREFERENCE_ALRAM1, true);
        appPrefrences.timerFactory = sharedPreferences.getBoolean(PREFERENCE_TIMERFACTORY, true);
        appPrefrences.timer1 = sharedPreferences.getBoolean(PREFERENCE_TIMER1, true);
        return appPrefrences;
    }

    public static class AppPreferences {
        public boolean clockSetable;
        public boolean timeAuthority;
        public boolean alarmFactory;
        public boolean alarm1;
        public boolean timerFactory;
        public boolean timer1;
    }

}
