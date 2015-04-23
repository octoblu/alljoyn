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

import org.allseen.timeservice.sample.client.R;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;

/**
 *
 * Dialog to display the {@link TimerPickerWidget} widget.
 *
 */
public class TimerPickerDialog extends AlertDialog implements OnClickListener {

    public interface OnTimeSetListener {
        void onTimeSet(TimerPickerWidget view, int hour, int minutes, int seconds);
    }

    public interface OnCancelListener {
        void onCancel();
    }

    private final TimerPickerWidget timerPicker;
    private final OnTimeSetListener onTimeSetListener;

    public TimerPickerDialog(Context context, OnTimeSetListener callBack, OnCancelListener onCancelListener, String title, boolean isClockDisplay, int hour, int minutes, int seconds) {
        this(context, 0, callBack, onCancelListener, title, isClockDisplay, hour, minutes, seconds);
    }

    public TimerPickerDialog(Context context, int theme, OnTimeSetListener onTimeSetListener, final OnCancelListener onCancelListener, String title, boolean isClockDisplay, int hour, int minutes,
            int seconds) {
        super(context, theme);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setTitle(title);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.timer_picker_dialog, null);
        timerPicker = (TimerPickerWidget) view.findViewById(R.id.timePicker);

        setButton(DialogInterface.BUTTON_POSITIVE, context.getResources().getString(R.string.set), this);
        setButton(DialogInterface.BUTTON_NEGATIVE, context.getResources().getString(R.string.cancel), new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                onCancelListener.onCancel();
            }
        });
        this.onTimeSetListener = onTimeSetListener;

        timerPicker.setCurrentHour(hour);
        timerPicker.setCurrentMinutes(minutes);
        timerPicker.setCurrentSecond(seconds);
        timerPicker.setClockDisplay(isClockDisplay);
        setView(view);
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        if (onTimeSetListener != null) {
            timerPicker.clearFocus();
            onTimeSetListener.onTimeSet(timerPicker, timerPicker.getCurrentHour(), timerPicker.getCurrentMinutes(), timerPicker.getCurrentSeconds());
        }
    }

}
