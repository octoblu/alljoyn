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
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public abstract class BaseActivity extends FragmentActivity{

    protected AlertDialog m_alertDialog = null;
    private static final String TAG = "BaseActivity";

    protected void launchBasicAlertDialog(String title, String message, String posButton, final Runnable onPosClicked, String negButton) {
        launchBasicAlertDialog(title, 0, message, posButton, onPosClicked, null, null, negButton, null, null, null);
    }

    protected void launchBasicAlertDialog(String title, String message, String posButton, final Runnable onPosClicked, String negButton, final Runnable onNegClicked) {
        launchBasicAlertDialog(title, 0, message, posButton, onPosClicked, null, null, negButton, onNegClicked, null, null);
    }

    protected void launchBasicAlertDialog(String title, String message, String posButton, final Runnable onPosClicked, String negButton, final Runnable onNegClicked, final OnCancelListener onCancel) {
        launchBasicAlertDialog(title, 0, message, posButton, onPosClicked, null, null, negButton, onNegClicked, onCancel, null);
    }

    protected void launchBasicAlertDialog(String title, int iconId, String message, String posButton, final Runnable onPosClicked, String negButton, final Runnable onNegClicked) {
        launchBasicAlertDialog(title, iconId, message, posButton, onPosClicked, null, null, negButton, onNegClicked, null, null);
    }

    protected void launchBasicAlertDialog(String title, int iconId, String message, String posButton, final Runnable onPosClicked, String neutralButton, final Runnable onNeutralClicked,
            String negButton, final Runnable onNegClicked) {
        launchBasicAlertDialog(title, iconId, message, posButton, onPosClicked, neutralButton, onNeutralClicked, negButton, onNegClicked, null, null);
    }

    protected void launchBasicAlertDialog(String title, int iconId, String message, String posButton, final Runnable onPosClicked, String neutralButton, final Runnable onNeutralClicked,
            String negButton, final Runnable onNegClicked, final OnCancelListener onCancel, final Runnable onBackClicked) {
        // get dialog layout
        final View dialogLayout = getLayoutInflater().inflate(R.layout.alert_dialog, null);
        final TextView titleView = (TextView) dialogLayout.findViewById(R.id.alert_title_text);
        final TextView messageView = (TextView) dialogLayout.findViewById(R.id.alert_msg_text);
        final Button posView = (Button) dialogLayout.findViewById(R.id.alert_button_pos);
        final Button neutralView = (Button) dialogLayout.findViewById(R.id.alert_button_neutral);
        final Button negView = (Button) dialogLayout.findViewById(R.id.alert_button_neg);

        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setView(dialogLayout);
        builder.setCancelable(true);

        if (title != null) {
            titleView.setText(title);
            if (iconId != 0) {
                titleView.setCompoundDrawablesWithIntrinsicBounds(iconId, 0, 0, 0);
                titleView.setPadding(titleView.getPaddingLeft(), titleView.getPaddingTop(), titleView.getPaddingLeft() * 4, titleView.getPaddingBottom());
            }
        } else {
            titleView.setVisibility(View.GONE);
        }

        if (message != null) {
            messageView.setText(message);
        } else {
            messageView.setVisibility(View.GONE);
        }

        if (posButton != null) {
            posView.setText(posButton);
            posView.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (onPosClicked != null) {
                        onPosClicked.run();
                    }

                    dismissAlertDialog();
                }
            });
        } else {
            posView.setVisibility(View.GONE);
        }

        if (neutralButton != null) {
            neutralView.setText(neutralButton);
            neutralView.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (onNeutralClicked != null) {
                        onNeutralClicked.run();
                    }

                    dismissAlertDialog();
                }
            });
        } else {
            neutralView.setVisibility(View.GONE);
        }

        if (negButton != null) {
            negView.setText(negButton);
            negView.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (onNegClicked != null) {
                        onNegClicked.run();
                    }

                    dismissAlertDialog();
                }
            });
        } else {
            negView.setVisibility(View.GONE);
        }

        if (onCancel != null) {
            builder.setOnCancelListener(onCancel);
        }

        if (onBackClicked != null) {
            builder.setOnKeyListener(new DialogInterface.OnKeyListener() {
                @Override
                public boolean onKey(DialogInterface arg0, int keyCode, KeyEvent arg2) {

                    if (keyCode == KeyEvent.KEYCODE_BACK) {
                        onBackClicked.run();
                        return true;
                    }
                    return false;
                }
            });
        }

        dismissAlertDialog();

        m_alertDialog = builder.create();

        m_alertDialog.show();

    }



    protected void dismissAlertDialog() {
        Log.d(TAG, "dismissAlertDialog");
        if (m_alertDialog != null && m_alertDialog.isShowing()) {
            Log.d(TAG, "dismissAlertDialog dismissing");
            m_alertDialog.dismiss();
            m_alertDialog = null;
        }
    }

    @Override
    protected void onPause() {
        Log.v(TAG, "onPause()");
        dismissAlertDialog();
        super.onPause();
    }
}
