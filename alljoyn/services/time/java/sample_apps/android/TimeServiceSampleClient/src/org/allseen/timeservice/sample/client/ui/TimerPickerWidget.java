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

import java.util.Locale;

import org.allseen.timeservice.sample.client.R;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.FrameLayout;
import android.widget.NumberPicker;
import android.widget.NumberPicker.Formatter;
import android.widget.TextView;

/**
 *
 * Widget to pick time or timer information
 *
 */
public class TimerPickerWidget extends FrameLayout {

    private final NumberPicker hourPicker;
    private final NumberPicker minutesPicker;
    private final NumberPicker secondsPicker;
    private final TextView hourTitle;
    private boolean isClockDisplay = false;
    private OnTimeChangedListener mOnTimeChangedListener;

    private int currentHour = 0;
    private int currentMinutes = 0;
    private int currentSeconds = 0;

    private static final NumberPicker.Formatter TIMER_FORMATTER = new Formatter() {
        @Override
        public String format(int value) {
            return String.format(Locale.US, "%02d", value);
        }
    };

    public interface OnTimeChangedListener {
        void onTimeChanged(TimerPickerWidget view, int hourOfDay, int minute, int seconds);
    }

    public TimerPickerWidget(Context context) {
        this(context, null);
    }

    public TimerPickerWidget(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public TimerPickerWidget(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.timer_picker_widget, this, true);
        hourTitle = (TextView) findViewById(R.id.hour_title);

        hourPicker = (NumberPicker) findViewById(R.id.hour);
        hourPicker.setMinValue(0);
        hourPicker.setMaxValue(99);
        hourPicker.setFormatter(TIMER_FORMATTER);

        hourPicker.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
                currentHour = newVal;
                onTimeChanged();
            }
        });

        minutesPicker = (NumberPicker) findViewById(R.id.minutes);
        minutesPicker.setMinValue(0);
        minutesPicker.setMaxValue(59);
        minutesPicker.setFormatter(TIMER_FORMATTER);
        minutesPicker.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker spinner, int oldVal, int newVal) {
                currentMinutes = newVal;
                onTimeChanged();
            }
        });

        secondsPicker = (NumberPicker) findViewById(R.id.seconds);
        secondsPicker.setMinValue(0);
        secondsPicker.setMaxValue(59);
        secondsPicker.setFormatter(TIMER_FORMATTER);
        secondsPicker.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
                currentSeconds = newVal;
                onTimeChanged();
            }
        });

        setOnTimeChangedListener(new OnTimeChangedListener() {
            @Override
            public void onTimeChanged(TimerPickerWidget view, int hourOfDay, int minute, int seconds) {
            }
        });

        setCurrentHour(0);
        setCurrentMinutes(0);
        setCurrentSecond(0);

        if (!isEnabled()) {
            setEnabled(false);
        }
    }

    public void updateClockDisplay() {
        if (isClockDisplay) {
            hourPicker.setMaxValue(23);
            hourTitle.setText(getResources().getString(R.string.timer_picker_hour));
        } else {
            hourTitle.setText(getResources().getString(R.string.timer_picker_hours));
            hourPicker.setMaxValue(99);
        }
    }

    public void setClockDisplay(boolean value) {
        isClockDisplay = value;
        updateClockDisplay();
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        minutesPicker.setEnabled(enabled);
        hourPicker.setEnabled(enabled);
        secondsPicker.setEnabled(enabled);
    }

    public void setOnTimeChangedListener(OnTimeChangedListener onTimeChangedListener) {
        mOnTimeChangedListener = onTimeChangedListener;
    }

    public Integer getCurrentHour() {
        return currentHour;
    }

    public void setCurrentHour(Integer currentHour) {
        this.currentHour = currentHour;
        hourPicker.setValue(currentHour);
        onTimeChanged();
    }

    public Integer getCurrentMinutes() {
        return currentMinutes;
    }

    public void setCurrentMinutes(Integer currentMinutes) {
        this.currentMinutes = currentMinutes;
        minutesPicker.setValue(currentMinutes);
        mOnTimeChangedListener.onTimeChanged(this, getCurrentHour(), getCurrentMinutes(), getCurrentSeconds());
    }

    public Integer getCurrentSeconds() {
        return currentSeconds;
    }

    public void setCurrentSecond(Integer currentSecond) {
        this.currentSeconds = currentSecond;
        secondsPicker.setValue(currentSeconds);
        mOnTimeChangedListener.onTimeChanged(this, getCurrentHour(), getCurrentMinutes(), getCurrentSeconds());
    }

    private void onTimeChanged() {
        mOnTimeChangedListener.onTimeChanged(this, getCurrentHour(), getCurrentMinutes(), getCurrentSeconds());
    }

}
