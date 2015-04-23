package org.alljoyn.ioe.controlpaneladapter;

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

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;
import java.util.concurrent.Callable;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.ui.ActionWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ActionWidgetHintsType;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget.DialogButton;
import org.alljoyn.ioe.controlpanelservice.ui.ContainerWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ErrorWidget;
import org.alljoyn.ioe.controlpanelservice.ui.LabelWidget;
import org.alljoyn.ioe.controlpanelservice.ui.LayoutHintsType;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ConstrainToValues;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.RangeConstraint;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ValueType;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidgetHintsType;
import org.alljoyn.ioe.controlpanelservice.ui.UIElement;
import org.alljoyn.ioe.controlpanelservice.ui.UIElementType;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.TypedArray;
import android.text.InputFilter;
import android.text.InputType;
import android.text.Layout;
import android.text.Spanned;
import android.text.format.DateFormat;
import android.util.Log;
import android.util.SparseArray;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.HorizontalScrollView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.TimePicker;

public class ControlPanelAdapter {
    private final static String TAG = "cpapp" + ControlPanelAdapter.class.getSimpleName();

    private static final Object PROPERTY_VALUE      = "property_value";
    private static final Object PROPERTY_EDITOR     = "property_editor";

    /* a context for creating Views */
    private final Context uiContext;

    /* mapping of UIElements to Views that were created by this adapter */
    private final Map<String, View> uiElementToView            = new WeakHashMap<String, View>(10);

    /*
     * mapping of AlertDialogWidgets to AlertDialogs that were created by this
     * adapter
     */
    private final Map<String, AlertDialog> alertWidgetToDialog = new WeakHashMap<String, AlertDialog>(3);

    /* App handler for control panel exceptions */
    private final ControlPanelExceptionHandler exceptionHandler;

    /*Time out value to call the ControlPanelService to retrieve a value*/
    private long timeoutValue    = 4;

    /*Time out measurement unit to call the ControlPanelService to retrieve a value*/
    private TimeUnit timeoutUnit = TimeUnit.SECONDS;

    // =====================================================================================================================

    /**
     * Constructor
     * @param uiContext The context of the {@link Activity} hosting the {@link ControlPanelAdapter}
     * @param exceptionHandler Exceptions listener
     */
    public ControlPanelAdapter(Context uiContext, ControlPanelExceptionHandler exceptionHandler) {

        if ( uiContext == null || exceptionHandler == null) {
            throw new IllegalArgumentException("uiContext or exceptionHandler are undefined");
        }

        if ( !(uiContext instanceof Activity) ) {
            throw new IllegalArgumentException("uiContext should be instance of Activity");
        }

        this.uiContext        = uiContext;
        this.exceptionHandler = exceptionHandler;
    }

    // =====================================================================================================================

    /**
     * The maximum time to wait before timing out retrieving the control panel element's current value.
     * @param timeoutValue The value of the timeout
     * @param timeoutUnit The {@link TimeUnit} of the timeout value
     */
    public void setTimeout(long timeoutValue, TimeUnit timeoutUnit) {

        this.timeoutValue = timeoutValue;
        this.timeoutUnit  = timeoutUnit;
    }

    // =====================================================================================================================

   /**
    * Creates a {@link Layout} that corresponds with the given ContainerWidget.
    * Then for each contained widget the corresponded {@link View} is created.
    * @param container input widget defining a container.
    * @return a resulting Layout.
    * @deprecated Use the {@link ControlPanelAdapter#createContainerViewAsync(ContainerWidget, ContainerCreatedListener)}
    * instead
    */
    @Deprecated
    public View createContainerView(ContainerWidget container) {

        if (container == null) {
            Log.e(TAG, "createContainerView(container==null)");
            return null;
        }

        Log.w(TAG, "The deprecated createContainerView() methdod has been called, handling");
        return createContainerViewImpl(container, new HashMap<String, Object>());
    }

    // =====================================================================================================================

    /**
     * Creates a {@link Layout} that corresponds with the given ContainerWidget.
     * Then for each contained widget the corresponded {@link View} is created.
     * The thread that calls this method is released immediately. Once the container {@link Layout} is created
     * the caller is notified via the {@link ContainerCreatedListener#onContainerViewCreated(View)} method.
     * @param container input widget defining a container.
     * @return a resulting Layout.
     */
    public void createContainerViewAsync(final ContainerWidget container, final ContainerCreatedListener contCreateListener) {

        if (container == null || contCreateListener == null) {
            throw new IllegalArgumentException("Received an undefined argument");
        }

        Log.i(TAG, "createContainerViewAsync() method has been called, handling");

        Runnable task = new Runnable() {
            @Override
            public void run() {

                Map<String, Object> initialData = new HashMap<String, Object>();
                ServiceTasksExecutor exec       = ServiceTasksExecutor.createExecutor();
                traverseContainerElements(container, exec, initialData);
                retrieveElementsValues(initialData);
                exec.shutdown();
                submitCreateContainerViewTask(container, contCreateListener, initialData);
            }
        };

        Thread t = new Thread(task);
        t.start();
    }

    // =====================================================================================================================

    /**
     * Creates a Layout that corresponds with the given ContainerWidget by
     * iterating the contained widgets and creating the corresponding inner {@link View}s.
     * The container elements are initialized with the given {@link Map} of the initialData
     * @param container input widget defining a container.
     * @param initialData Key: element's object path; Value: element's initial value or NULL
     * @return a resulting Layout.
     */
    private View createContainerViewImpl(ContainerWidget container, Map<String, Object> initialData) {
        if (container == null) {
            Log.e(TAG, "createContainerView(container==null)");
            return null;
        }

        Log.d(TAG, "Received main container, objPath: '" + container.getObjectPath() + "'");

        // the returned Layout object
        ViewGroup scrollView;
        LinearLayout containerLayout;
        ViewGroup.LayoutParams layoutParams;

        // initialize the container by type
        List<LayoutHintsType> layoutHints = container.getLayoutHints();
        Log.d(TAG, "Container has LayoutHints: " + layoutHints);

        LayoutHintsType layoutHintsType = (layoutHints.size() == 0) ? LayoutHintsType.VERTICAL_LINEAR : layoutHints.get(0);
        // set the layout
        switch (layoutHintsType) {
        case HORIZONTAL_LINEAR:
            scrollView = new HorizontalScrollView(uiContext);
            LinearLayout linearLayout = new LinearLayout(uiContext);
            linearLayout.setOrientation(LinearLayout.HORIZONTAL);
            linearLayout.setGravity(Gravity.CENTER_VERTICAL);
            containerLayout = linearLayout;
            LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            hLinearLayoutParams.setMargins(10, 10, 10, 10);
            layoutParams = hLinearLayoutParams;
            break;
        case VERTICAL_LINEAR:
        default:
            scrollView = new ScrollView(uiContext);
            containerLayout = new LinearLayout(uiContext);
            containerLayout.setOrientation(LinearLayout.VERTICAL);
            containerLayout.setGravity(Gravity.LEFT);
            LinearLayout.LayoutParams vLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
            vLinearLayoutParams.setMargins(10, 10, 10, 10);
            layoutParams = vLinearLayoutParams;
            // set the inner label
            if (container.getLabel() != null && container.getLabel().trim().length() > 0) {
                Log.d(TAG, "Setting container label to: " + container.getLabel());
                TextView titleTextView = new TextView(uiContext);
                titleTextView.setText(container.getLabel());
                LinearLayout.LayoutParams textLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                textLayoutParams.setMargins(20, 0, 20, 0);
                containerLayout.addView(titleTextView, textLayoutParams);
            }
            break;
        }
        Log.d(TAG, "Initialized Layout of class: " + containerLayout.getClass().getSimpleName());

        Log.d(TAG, "Container bgColor:  " + container.getBgColor());
        // if (container.getBgColor() != null)
        // containerLayout.setBackgroundColor(container.getBgColor());

        // recursively layout the items
        List<UIElement> elements = container.getElements();
        Log.d(TAG, String.format("Laying out %d elements", elements.size()));

        int i = 0;
        for (UIElement element : elements) {
            i++;
            View childView = createInnerView(element, initialData);
            containerLayout.addView(childView, layoutParams);
            if (layoutHintsType == LayoutHintsType.VERTICAL_LINEAR && i < elements.size()) {
                // add a divider
                containerLayout.addView(createDividerView(), layoutParams);
            }
        }// for :: elements

        LinearLayout.LayoutParams linearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        scrollView.addView(containerLayout, linearLayoutParams);

        boolean enabled = container.isEnabled();
        Log.d(TAG, "Container is enabled: '" + enabled + "'");
        if (!enabled) {
            enableViewGroup(scrollView, false); // Only if container is created
                                                // as disabled, its children
                                                // should be disabled
        }

        uiElementToView.put(container.getObjectPath(), scrollView);
        return scrollView;
    }

    // =====================================================================================================================

    /**
     * Traverse container elements and use {@link ControlPanelService} to retrieve data required
     * for creating the element's {@link View}s.
     * @param container {@link ContainerWidget} containing the elements
     * @param exec {@link ServiceTasksExecutor} which is responsible to retrieve the element's data and populate the
     * {@link Future} object
     * @param initialData The map that is populated with the element's object path as a key and {@link Future} as a value
     */
    private void traverseContainerElements(ContainerWidget container, ServiceTasksExecutor exec, Map<String, Object> initialData) {

        List<UIElement> elements = container.getElements();

        for (UIElement element : elements) {

            UIElementType elType = element.getElementType();

            //Currently only PropertyWidgets need initial data retrieved by the ControlPanelService via AllJoyn
            if ( elType == UIElementType.PROPERTY_WIDGET ) {

                Log.d(TAG, "Found a PropertyWidget objPath: '" + element.getObjectPath() + "', retrieving Future value");
                initialData.put(element.getObjectPath(), submitGetPropertyCurrentValueTask(exec, (PropertyWidget)element));
            }
            else if ( elType == UIElementType.CONTAINER ) {

                traverseContainerElements((ContainerWidget)element, exec, initialData);
            }
        }
    }

    // =====================================================================================================================

    /**
     * Retrieves initial values of the elements from the {@link Future} object stored in the given initialData {@link Map}.
     * The values are retrieved by the call {@link Future#get(long, TimeUnit)}.
     * If an {@link Exception} is thrown while retrieving the value, NULL is stored in the initialData instead of the
     * real value.
     */
    private void retrieveElementsValues(Map<String, Object> initialData) {

        Log.d(TAG, "Retrieving initial values of the elements with timeout: '" + timeoutValue + " " + timeoutUnit + "'");
        for ( String objPath : initialData.keySet() ) {

            Object fo = initialData.get(objPath);
            if ( !(fo instanceof Future<?>) ) {

                Log.w(TAG, "The objPath: '" + objPath + "' doesn't have a Future object");
                initialData.put(objPath, null);
                continue;
            }

            @SuppressWarnings("unchecked")
            Future<Object> futureVal = (Future<Object>) fo;
            Object realValue         = null;

            try {
                //Blocking call with the given timeout
                realValue = futureVal.get(timeoutValue, timeoutUnit);
                Log.d(TAG, "Found an initial value of the element: '" + objPath + "', value: '" + realValue + "'");
            }
            catch(Exception e) {
                Log.e(TAG, "Failed to retrieve the element's initial value objPath: '" + objPath + "'. Exception: ", e);
                futureVal.cancel(true);
            }

            initialData.put(objPath, realValue);
        }
    }

    // =====================================================================================================================

    /**
     * Submits the {@link PropertyWidget#getCurrentValue()} task to the {@link ServiceTasksExecutor}.
     * @param exec The executor that executes the task
     * @param property {@link PropertyWidget} to retrieve the current value
     * @return {@link Future} of the initial {@link PropertyWidget} value or NULL if failed to submit the retrieval task
     */
    private Future<Object> submitGetPropertyCurrentValueTask(ServiceTasksExecutor exec, final PropertyWidget property) {

        Log.d(TAG, "Prepare a task to call PropertyWidget.getCurrentValue(), property: '" + property.getObjectPath() + "'");

        Callable<Object> task = new Callable<Object>() {
            @Override
            public Object call() throws Exception {

                return property.getCurrentValue();
            }
        };

        Future<Object> future = null;

        try {
            future = exec.submit(task);
        }
        catch (Exception e){

            Log.d(TAG, "Failed to submit the task to call PropertyWidget.getCurrentValue(), property: '" +
                        property.getObjectPath() + "'", e);
        }

        return future;
    }

    // =====================================================================================================================

    /**
     * Submit the method {@link ControlPanelAdapter#createContainerViewImpl(ContainerWidget, Map)} to run on UI thread
     * @param container
     * @param contCreateListener
     * @param initialData
     */
    private void submitCreateContainerViewTask(final ContainerWidget container, final ContainerCreatedListener contCreateListener,
                                               final Map<String, Object> initialData) {

        ((Activity)uiContext).runOnUiThread( new Runnable() {
            @Override
            public void run() {

                View containerView = createContainerViewImpl(container, initialData);
                contCreateListener.onContainerViewCreated(containerView);
            }
        });
    }

    // =====================================================================================================================

    /**
     * Creates a list divider for vertical layout
     * @return A list divider
     */
    private View createDividerView() {
        View listDivider = new View(uiContext);
        TypedArray listDividerAttrs = uiContext.getTheme().obtainStyledAttributes(new int[] { android.R.attr.listDivider });

        if (listDividerAttrs.length() > 0) {
            // don't change this to setBackground() it'll fly on run time
            listDivider.setBackgroundDrawable(listDividerAttrs.getDrawable(0));
        }

        listDividerAttrs.recycle();
        return listDivider;
    }

    // =====================================================================================================================

    /**
     * Creates an Android View that corresponds with the given UIElement using the given initialData
     * @param element The {@link UIElement} to create the corresponding {@link View}I
     * @param initialData
     * @return an Android View that corresponds with the given abstract UIElement
     */
    private View createInnerView(UIElement element, Map<String, Object> initialData) {
        UIElementType elementType = element.getElementType();
        Log.d(TAG, "Creating an Android View for element of type: '" + elementType + "'");
        View returnView = new TextView(uiContext);

        switch (elementType) {
        case ACTION_WIDGET: {
            returnView = createActionView((ActionWidget) element);
            break;
        }// ACTION_WIDGET
        case CONTAINER: {
            returnView = createContainerViewImpl((ContainerWidget) element, initialData);
            break;
        }// CONTAINER
         // case LIST_PROPERTY_WIDGET: {
         // break;
         // }//LIST_PROPERTY_WIDGET
        case PROPERTY_WIDGET: {

            //Currently only Properties require initial data for the creation
            returnView = createPropertyViewImpl((PropertyWidget) element, initialData);
            break;
        }// PROPERTY_WIDGET
        case LABEL_WIDGET: {
            returnView = createLabelView((LabelWidget) element);
            break;
        }// PROPERTY_WIDGET
        case ERROR_WIDGET: {
            returnView = createErrorView((ErrorWidget) element);
            break;
        }// ERROR_WIDGET
        default:
            break;
        }// switch :: elementType

        return returnView;
    }

    // =====================================================================================================================

    /**
     * Creates an AlertDialog that corresponds with the given AlertDialogWidget
     * @param alertDialogWidget input abstract widget defining a dialog.
     * @return an AlertDialog with up to 3 actions. Each action executes a different DialogButton UIElement.
     */
    public AlertDialog createAlertDialog(final AlertDialogWidget alertDialogWidget) {
        final SparseArray<DialogButton> buttonToAction = new SparseArray<AlertDialogWidget.DialogButton>(3);

        List<DialogButton> execButtons = alertDialogWidget.getExecButtons();
        int numOfButtons = alertDialogWidget.getNumActions();
        if (numOfButtons > 0) {
            buttonToAction.put(DialogInterface.BUTTON_POSITIVE, execButtons.get(0));
            if (numOfButtons > 1) {
                buttonToAction.put(DialogInterface.BUTTON_NEGATIVE, execButtons.get(1));
                if (numOfButtons > 2) {
                    buttonToAction.put(DialogInterface.BUTTON_NEUTRAL, execButtons.get(2));
                }
            }
        }

        // create a confirmation dialog
        DialogInterface.OnClickListener confirmationListener = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, final int which) {
                // handle user choice
                final DialogButton actionButton = buttonToAction.get(which);
                Log.d(TAG, "User selected: " + actionButton.getLabel());

                // execute the action in a background task
                ExecuteActionAsyncTask asyncTask = new ExecuteActionAsyncTask() {

                    @Override
                    protected void onFailure(ControlPanelException e) {
                        if (exceptionHandler != null) {
                            exceptionHandler.handleControlPanelException(e);
                        }
                    }

                    @Override
                    protected void onSuccess() {
                    }

                };
                asyncTask.execute(actionButton);
            }
        };

        // show the confirmation dialog
        Builder builder = new AlertDialog.Builder(uiContext).setMessage(alertDialogWidget.getMessage()).setTitle(alertDialogWidget.getLabel());

        if (numOfButtons > 0) {
            builder.setPositiveButton(buttonToAction.get(DialogInterface.BUTTON_POSITIVE).getLabel(), confirmationListener);
            if (numOfButtons > 1) {
                builder.setNegativeButton(buttonToAction.get(DialogInterface.BUTTON_NEGATIVE).getLabel(), confirmationListener);
                if (numOfButtons > 2) {
                    builder.setNeutralButton(buttonToAction.get(DialogInterface.BUTTON_NEUTRAL).getLabel(), confirmationListener);
                }
            }
        }

        AlertDialog alertDialog = builder.create();
        alertWidgetToDialog.put(alertDialogWidget.getObjectPath(), alertDialog);
        return alertDialog;

    }

    // =====================================================================================================================

    /**
     * Creates a Button for an ActionWidget
     * @param action the UIElement to be represented by this button.
     * @return a Button with 2 optional flows corresponding with the ActionWidget definition:
     * A button that executes an action. Or a button that popsup an "Are You Sure?" dialog.
     */
    public View createActionView(final ActionWidget action) {

        List<ActionWidgetHintsType> hints = action.getHints();
        String label = action.getLabel();
        Integer bgColor = action.getBgColor();
        boolean isEnabled = action.isEnabled();

        Log.d(TAG, "Create Action: " + label + " BGColor: " + bgColor + " actionMeta: " + hints + " isEnable: '" + isEnabled + "'");

        Button actionButton = new Button(uiContext);
        actionButton.setText(label);
        actionButton.setEnabled(isEnabled);
        // if (bgColor != null) {
        // Unfortunately button loses its pressed behavior when background is
        // set.
        // actionButton.setBackgroundColor(bgColor);
        // }

        // register a click listener
        OnClickListener actionButtonListener = new OnClickListener() {

            @Override
            public void onClick(View v) {

                if ( !action.isEnabled() ) {

                    Log.i(TAG, "ActionWidget is disabled, objPath: '" + action.getObjectPath() + "', not reacting onClick");
                    return;
                }

                // check if action requires an extra confirmation
                AlertDialogWidget alertDialogWidget = action.getAlertDialog();
                if (alertDialogWidget != null) {
                    Log.d(TAG, "Showing confirmation: " + alertDialogWidget.getMessage() + "?");
                    // create a confirmation dialog
                    AlertDialog confirmationDialog = createAlertDialog(alertDialogWidget);
                    confirmationDialog.show();
                } // Confirmation needed
                else { // No confirmation needed

                    // execute the action in a background task
                    ExecuteActionAsyncTask asyncTask = new ExecuteActionAsyncTask() {

                        @Override
                        protected void onFailure(ControlPanelException e) {
                            if (exceptionHandler != null) {
                                exceptionHandler.handleControlPanelException(e);
                            }
                        }

                        @Override
                        protected void onSuccess() {
                        }

                    };
                    asyncTask.execute(action);
                } // No confirmation needed

            }
        };// new OnClickListener()
        actionButton.setOnClickListener(actionButtonListener);

        uiElementToView.put(action.getObjectPath(), actionButton);
        return actionButton;
    }// createActionView

    // =====================================================================================================================

    /**
     * Creates a TextView for a LabelWidget
     * @param labelWidget the UIElement to be represented by this View
     * @return a TextView displaying the contents of the LabelWidget
     */
    public View createLabelView(final LabelWidget labelWidget) {
        String label = labelWidget.getLabel();
        Integer bgColor = labelWidget.getBgColor();
        Log.d(TAG, "Creating Label: \"" + label + "\" BGColor: " + bgColor);

        TextView labelView = new TextView(uiContext);
        labelView.setText(label);
        // if (bgColor != null)
        // labelView.setBackgroundColor(bgColor);

        uiElementToView.put(labelWidget.getObjectPath(), labelView);
        return labelView;
    }// createLabelView

    // =====================================================================================================================

    /**
     * Creates a TextView for a ErrorWidget
     * @param errorWidget the UIElement to be represented by this View
     * @return a TextView displaying the label of the ErrorWidget
     */
    public View createErrorView(final ErrorWidget errorWidget) {
        String label = errorWidget.getLabel();
        String errorMessage = errorWidget.getError();

        Log.w(TAG, "Creating Error Label: \"" + label + "\" Error: '" + errorMessage + "', Original Element Type: " + errorWidget.getOriginalUIElement());

        TextView errorView = new TextView(uiContext);
        errorView.setText(label);

        uiElementToView.put(errorWidget.getObjectPath(), errorView);
        return errorView;
    }// createErrorView

    // =====================================================================================================================
    // ============================================= PropertyWidget
    // ========================================================
    // =====================================================================================================================

    /**
     * A factory method for creating a View for a given PropertyWidget.
     * @param propertyWidget the UIElement to be represented by this View
     * @return a View that represents the property. The type of View corresponds
     *  with the property's hint.
     */
    public View createPropertyView(PropertyWidget propertyWidget) {

        return createPropertyViewImpl(propertyWidget, new HashMap<String, Object>());
    }

    /**
     * A factory method for creating a View for a given PropertyWidget.
     * @param propertyWidget the UIElement to be represented by this View
     * @param initialData The initialData to initialize the {@link PropertyWidget}
     * @return a View that represents the property. The type of View corresponds
     *  with the property's hint.
     */
    private View createPropertyViewImpl(PropertyWidget propertyWidget, Map<String, Object> initialData) {

        ValueType valueType                 = propertyWidget.getValueType();
        List<PropertyWidgetHintsType> hints = propertyWidget.getHints();
        PropertyWidgetHintsType hint        = (hints == null || hints.size() == 0) ? null : hints.get(0);

        Object initialValue                 = getPropertyInitialValue(propertyWidget, initialData);

        Log.d(TAG, "Creating a View for property '" + propertyWidget.getLabel() + "', using UI hint: ;" + hint +
                    "', value type: '" + valueType + "' initial value: '" + initialValue + "', objPath: '" +
                     propertyWidget.getObjectPath() + "'");

        // default view. just in case...
        View returnView = new TextView(uiContext);

        switch (valueType) {
        case BOOLEAN:
            // Boolean Property
            returnView = createCheckBoxView(propertyWidget, initialValue);
            break;
        case DATE:
            // Date Property
            returnView = createDatePickerView(propertyWidget, initialValue);
            break;
        case TIME:
            // Time Property
            returnView = createTimePickerView(propertyWidget, initialValue);
            break;
        case BYTE:
        case INT:
        case SHORT:
        case LONG:
        case DOUBLE:
            // Scalar Property
            if (hint == null) {
                Log.d(TAG, "No hint provided for property '" + propertyWidget.getLabel() + "', creating default: NumericView");
                returnView = createNumericView(propertyWidget, initialValue);
            } else {
                switch (hint) {
                case SPINNER:
                    returnView = createSpinnerView(propertyWidget, initialValue);
                    break;
                case RADIO_BUTTON:
                    returnView = createRadioButtonView(propertyWidget, initialValue);
                    break;
                case NUMERIC_VIEW:
                    returnView = createNumericView(propertyWidget, initialValue);
                    break;
                case SLIDER:
                    returnView = createSliderView(propertyWidget, initialValue);
                    break;
                case NUMERIC_KEYPAD:
                    returnView = createNumericKeypadView(propertyWidget, initialValue);
                    break;
                default:
                    Log.d(TAG, "Unsupported hint provided for property '" + propertyWidget.getLabel() + "', creating default: NumericView");
                    returnView = createNumericView(propertyWidget, initialValue);
                    break;
                }
            }
            break;
        case STRING:
            // String Property
            if (hint == null) {
                Log.d(TAG, "No hint provided for property '" + propertyWidget.getLabel() + "', creating default: TextView");
                returnView = createTextView(propertyWidget, initialValue);
            } else {
                switch (hint) {
                case SPINNER:
                    returnView = createSpinnerView(propertyWidget, initialValue);
                    break;
                case RADIO_BUTTON:
                    returnView = createRadioButtonView(propertyWidget, initialValue);
                    break;
                case EDIT_TEXT:
                    returnView = createEditTextView(propertyWidget, initialValue);
                    break;
                case TEXT_VIEW:
                    returnView = createTextView(propertyWidget, initialValue);
                    break;
                default:
                    Log.d(TAG, "Unsupported hint provided for property '" + propertyWidget.getLabel() + "', creating default: TextView");
                    returnView = createTextView(propertyWidget, initialValue);
                    break;
                }
            }
            break;
        default:
            Log.d(TAG, "Received an unsupported ValueType: '" + valueType + "' , returning an empty view");
            return returnView;
        }

        uiElementToView.put(propertyWidget.getObjectPath(), returnView);

        return returnView;
    }

    /**
     * Retrieves the initial value from the initialData Map.
     * If the objectPath isn't stored as the key in the initialData Map, the method
     * {@link ControlPanelAdapter#submitGetPropertyCurrentValueTask(ServiceTasksExecutor, PropertyWidget)}
     * is called. Otherwise the value retrieved from the Map is returned.
     * @param propertyWidget The property to look for in the {@link Map}.
     * @param initialData The {@link Map} with the initial values
     * @return Object to initialize the Property Widget {@link View} or NULL.
     */
    private Object getPropertyInitialValue(PropertyWidget propertyWidget, Map<String, Object> initialData) {

        String objPath = propertyWidget.getObjectPath();

        Log.d(TAG, "Searching for the initial value of the property: '" + objPath + "'");
        if ( !initialData.containsKey(objPath) ) {

            //This code may be called when there is a use of the deprecated createContainerView method
            Log.w(TAG, "The object path: '" + objPath + "' is unknown, submitting PropertyWidget.getCurrentValue()");
            ServiceTasksExecutor exec = ServiceTasksExecutor.createExecutor(1);
            Future<Object> fo         = submitGetPropertyCurrentValueTask(exec, propertyWidget);

            try {

                if ( fo == null ) {
                    return null;
                }

                return fo.get(timeoutValue, timeoutUnit);
            } catch (Exception e) {

                Log.e(TAG, "Failed to retrieve property's initial value objPath: '" + objPath + "'. Exception: ", e);
                fo.cancel(true);
                return null;
            }
            finally {
                exec.shutdown();
            }
        }

        return initialData.get(objPath);
    }

    // =====================================================================================================================

    private View createSpinnerView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a spinner for propertyWidget " + propertyWidget.getLabel());

        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.VERTICAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setPadding(10, 0, 0, 0);
        nameTextView.setText(propertyWidget.getLabel());
        final Spinner spinner = new Spinner(uiContext);

        LinearLayout.LayoutParams vLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        layout.addView(nameTextView, vLinearLayoutParams);
        layout.addView(spinner, vLinearLayoutParams);
        spinner.setTag(PROPERTY_VALUE);

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        // spinner.setEnabled(propertyWidget.isEnabled() &&
        // propertyWidget.isWritable());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        // set the data model
        final ArrayAdapter<LabelValuePair> adapter = new ArrayAdapter<LabelValuePair>(uiContext, android.R.layout.simple_spinner_item);
        int selection = 0;
        if (propertyWidget.getListOfConstraint() != null) {
            // create a spinner model made of the given ConstrainToValues
            // entries
            int position = 0;
            for (ConstrainToValues<?> valueCons : propertyWidget.getListOfConstraint()) {
                boolean isDefault = valueCons.getValue().equals(initialValue);
                Log.d(TAG, "Adding spinner item, Label: " + valueCons.getLabel() + " Value: " + valueCons.getValue() + (isDefault ? " (default)" : ""));
                adapter.add(new LabelValuePair(valueCons.getLabel(), valueCons.getValue()));
                if (isDefault) {
                    selection = position;
                }
                position++;
            }
        } else if (propertyWidget.getPropertyRangeConstraint() != null) {
            // dynamically create a spinner model made of integers from min to
            // max
            RangeConstraint<?> propertyRangeConstraint = propertyWidget.getPropertyRangeConstraint();

            ValueType valueType = propertyWidget.getValueType();
            Object minT = propertyRangeConstraint.getMin();
            int min = ValueType.SHORT.equals(valueType) ? ((Short) minT) : ValueType.INT.equals(valueType) ? ((Integer) minT) : 0;
            Object maxT = propertyRangeConstraint.getMax();
            int max = ValueType.SHORT.equals(valueType) ? ((Short) maxT) : ValueType.INT.equals(valueType) ? ((Integer) maxT) : 0;
            Object incrementT = propertyRangeConstraint.getIncrement();
            int increment = ValueType.SHORT.equals(valueType) ? ((Short) incrementT) : ValueType.INT.equals(valueType) ? ((Integer) incrementT) : 0;
            int position = 0;
            for (int i = min; i <= max; i += increment) {
                boolean isDefault = false;
                switch (valueType) {
                    case SHORT:
                        short shortI = (short) i;
                        adapter.add(new LabelValuePair(String.valueOf(i), shortI));
                        isDefault = Short.valueOf(shortI).equals(initialValue);
                        break;
                    case INT:
                    default: {
                        adapter.add(new LabelValuePair(String.valueOf(i), i));
                        isDefault = Integer.valueOf(i).equals(initialValue);
                    }
                }
                Log.d(TAG, "Added spinner item, Value: " + i + (isDefault ? " (default)" : ""));
                if (isDefault) {
                    selection = position;
                }
                position++;
            }
        }

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);
        spinner.setSelection(selection);
        final int initialSelection = selection;

        // Unfortunately spinner loses some of its UI components when background
        // is set.
        // spinner.setBackgroundColor(property.getBgColor());

        // register a selection listener
        OnItemSelectedListener listener = new OnItemSelectedListener() {
            int currentSelection = initialSelection;

            @Override
            public void onItemSelected(AdapterView<?> parent, View view, final int pos, long id) {
                if (pos == currentSelection) {
                    Log.d(TAG, String.format("Selected position %d already selected. No action required", pos));
                } else {
                    LabelValuePair item = adapter.getItem(pos);

                    // set the property in a background task
                    SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                        @Override
                        protected void onFailure(ControlPanelException e) {
                            spinner.setSelection(currentSelection);
                            if (exceptionHandler != null) {
                                // An exception was thrown. Restore old value.
                                exceptionHandler.handleControlPanelException(e);
                            }
                        }

                        @Override
                        protected void onSuccess() {
                            // All went well. Store the new value.
                            currentSelection = pos;
                        }

                    };
                    asyncTask.execute(propertyWidget, item.value);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // Another interface callback
            }
        };
        spinner.setOnItemSelectedListener(listener);

        return layout;
    }

    // =====================================================================================================================

    private View createRadioButtonView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a radio button group for property '" + propertyWidget.getLabel() + "'");

        // Create external widget layout
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.VERTICAL);
        LinearLayout.LayoutParams vLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);

        // Create internal label/unit of measure layout
        final LinearLayout innerLayout = new LinearLayout(uiContext);
        innerLayout.setOrientation(LinearLayout.HORIZONTAL);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(innerLayout, vLinearLayoutParams);

        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setPadding(10, 0, 0, 0);
        nameTextView.setText(propertyWidget.getLabel());
        innerLayout.addView(nameTextView, hLinearLayoutParams);

        // Add unit of measure
        String unitOfMeasure = propertyWidget.getUnitOfMeasure();
        if (unitOfMeasure != null && unitOfMeasure.length() > 0) {

            final TextView unitsOfMeasureTextView = new TextView(uiContext);
            unitsOfMeasureTextView.setText(unitOfMeasure);
            innerLayout.addView(unitsOfMeasureTextView, hLinearLayoutParams);
        }

        RadioGroup radioGroup = new RadioGroup(uiContext);
        layout.addView(radioGroup, vLinearLayoutParams);
        radioGroup.setTag(PROPERTY_VALUE);

        final List<ConstrainToValues<?>> listOfConstraint = propertyWidget.getListOfConstraint();
        if (listOfConstraint != null) {
            for (ConstrainToValues<?> valueCons : listOfConstraint) {

                boolean isDefault = valueCons.getValue().equals(initialValue);
                Log.d(TAG, "Adding radio button, Label: " + valueCons.getLabel() + " Value: " + valueCons.getValue() + (isDefault ? " (default)" : ""));

                RadioButton radioButton = new RadioButton(uiContext);
                radioButton.setText(valueCons.getLabel());
                LinearLayout.LayoutParams layoutParams = new RadioGroup.LayoutParams(RadioGroup.LayoutParams.WRAP_CONTENT, RadioGroup.LayoutParams.WRAP_CONTENT);
                radioGroup.addView(radioButton, layoutParams);

                // check the default value
                if (isDefault && !radioButton.isChecked()) {
                    radioButton.toggle();
                }
            }
        }// LOV constraints

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        // radioGroup.setBackgroundColor(propertyWidget.getBgColor());

        final int initialCheckedId = radioGroup.getCheckedRadioButtonId();

        // register selection listener
        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            int currentCheckedId = initialCheckedId;

            @Override
            public void onCheckedChanged(final RadioGroup group, final int checkedId) {

                int index = group.indexOfChild(group.findViewById(checkedId));
                if (index > -1) {
                    // Avoid fast consecutive selections. The problem is with
                    // the valueChange events that a controllable device sends
                    // following each selection of the RadioButton.
                    // If a user selects different RadioButton(s) from the group
                    // very fast, the valueChange events start toggling the
                    // RadioGroup automatically
                    for (int i = 0; i < group.getChildCount(); i++) {
                        group.getChildAt(i).setEnabled(false);
                    }
                    group.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            for (int i = 0; i < group.getChildCount(); i++) {
                                group.getChildAt(i).setEnabled(true);
                            }
                        }
                    }, 1000);

                    ConstrainToValues<?> item = listOfConstraint.get(index);
                    Log.d(TAG, "Selected radio button, Label: '" + item.getLabel() + "' Value: " + item.getValue());

                    // set the property in a background task
                    SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                        @Override
                        protected void onFailure(ControlPanelException e) {
                            group.check(currentCheckedId);
                            if (exceptionHandler != null) {
                                // An exception was thrown. Restore old value.
                                exceptionHandler.handleControlPanelException(e);
                            }
                        }

                        @Override
                        protected void onSuccess() {
                            // All went well. Store the new value.
                            currentCheckedId = checkedId;
                        }

                    };
                    asyncTask.execute(propertyWidget, item.getValue());
                }
            }
        });

        return layout;
    }

    // =====================================================================================================================

    private CheckBox createCheckBoxView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a checkbox for property " + propertyWidget.getLabel());

        CheckBox checkbox = new CheckBox(uiContext);
        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled() + " InitialValue: '" + initialValue + "'");

        // initialize meta data
        checkbox.setEnabled(propertyWidget.isEnabled() && propertyWidget.isWritable());
        // if (propertyWidget.getBgColor() != null)
        // checkbox.setBackgroundColor(propertyWidget.getBgColor());

        // initialize data
        if (initialValue instanceof Boolean) {
            checkbox.setChecked((Boolean) initialValue);
        }
        checkbox.setText(propertyWidget.getLabel());

        // register selection listener
        OnCheckedChangeListener listener = new OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(final CompoundButton buttonView, final boolean isChecked) {

                // Avoid fast consecutive selections. The problem is with the
                // valueChange events that a controllable device sends
                // following each change of the Checkbox state.
                // If a user selects/unselects checkbox fast, these valueChange
                // events start toggle the Checkbox state automatically
                buttonView.setEnabled(false);
                buttonView.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        buttonView.setEnabled(true);
                    }
                }, 1000);

                // set the property in a background task
                SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                    @Override
                    protected void onFailure(ControlPanelException e) {
                        buttonView.setChecked(!isChecked);
                        if (exceptionHandler != null) {
                            // An exception was thrown. Restore old value.
                            exceptionHandler.handleControlPanelException(e);
                        }
                    }

                    @Override
                    protected void onSuccess() {
                        // All went well.
                    }

                };
                asyncTask.execute(propertyWidget, isChecked);
            }
        };
        checkbox.setOnCheckedChangeListener(listener);

        return checkbox;
    }

    // =====================================================================================================================

    private View createTextView(final PropertyWidget propertyWidget, Object initialValue) {

        String label    = propertyWidget.getLabel();
        Integer bgColor = propertyWidget.getBgColor();
        Log.d(TAG, "Creating TextView: \"" + label + "\" BGColor: " + bgColor);

        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);

        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(label);
        layout.addView(nameTextView);

        final TextView valueTextView = new TextView(uiContext);
        valueTextView.setTag(PROPERTY_VALUE);

        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueTextView, hLinearLayoutParams);

        // if (bgColor != null)
        // labelView.setBackgroundColor(bgColor);

        if (initialValue != null) {
            Log.d(TAG, "Setting property value to: " + initialValue.toString());
            valueTextView.setText(initialValue.toString());
        }

        return layout;
    }// createTextView

    // =====================================================================================================================

    private View createEditTextView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a text editor for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final EditText valueEditText = new EditText(uiContext);
        valueEditText.setImeOptions(EditorInfo.IME_ACTION_DONE);
        valueEditText.setTag(PROPERTY_EDITOR);

        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueEditText, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueEditText.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());
        // valueEditText.setEnabled(propertyWidget.isEnabled() &&
        // propertyWidget.isWritable());
        // nameTextView.setEnabled(true);

        final String initialValueStr = initialValue == null ? "" : initialValue.toString();
        valueEditText.setText(initialValueStr);

        // register edit listener
        valueEditText.setInputType(InputType.TYPE_CLASS_TEXT);
        // limit the number of characters
        // editText.setFilters( new InputFilter[] {new
        // InputFilter.LengthFilter(10)});
        valueEditText.setOnEditorActionListener(new OnEditorActionListener() {
            String currentText = initialValueStr;

            @Override
            public boolean onEditorAction(final TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_DONE) {

                    // set the property in a background task
                    SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                        @Override
                        protected void onFailure(ControlPanelException e) {
                            v.setText(currentText);
                            if (exceptionHandler != null) {
                                // An exception was thrown. Restore old value.
                                exceptionHandler.handleControlPanelException(e);
                            }
                        }

                        @Override
                        protected void onSuccess() {
                            // All went well. Store the new value.
                            currentText = v.getText().toString();
                        }

                    };
                    asyncTask.execute(propertyWidget, v.getText().toString());
                }
                // otherwise keyboard remains up
                return false;
            }
        });

        return layout;
    }// createEditTextView

    // =====================================================================================================================

    private View createNumericKeypadView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a number text editor for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final EditText valueEditText = new EditText(uiContext);
        valueEditText.setImeOptions(EditorInfo.IME_ACTION_DONE);
        valueEditText.setTag(PROPERTY_EDITOR);

        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueEditText, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueEditText.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        final ValueType valueType    = propertyWidget.getValueType();
        final Number initialValueNum = (initialValue == null ? 0 : ValueType.SHORT.equals(valueType) ?
                                       ((Short) initialValue) : ValueType.INT.equals(valueType) ? ((Integer) initialValue) : 0);

        valueEditText.setText(String.valueOf(initialValue));

        // register edit listener
        valueEditText.setInputType(InputType.TYPE_CLASS_NUMBER);
        // limit to Short/Integer MAX_VALUE
        InputFilter filter = new InputFilter() {

            @Override
            public CharSequence filter(CharSequence source, int start, int end, Spanned dest, int dstart, int dend) {
                String insert = source.toString();
                String insertInto = dest.toString();
                Log.d(TAG, "Trying to insert ' " + insert + "' into '" + insertInto + "' between characters " + dstart + " and " + dend);
                try {
                    String prefix = insertInto.substring(0, dstart);
                    String suffix = insertInto.substring(dend);
                    String result = prefix + insert + suffix;
                    if (ValueType.SHORT.equals(valueType)) {
                        short input = Short.parseShort(result);
                        Log.d(TAG, "Valid short entered: " + input);
                        // if we got here we're fine. Accept the editing by
                        // returning null
                        return null;
                    }
                    if (ValueType.INT.equals(valueType)) {
                        int input = Integer.parseInt(result);
                        Log.d(TAG, "Valid int entered: " + input);
                        // if we got here we're fine. Accept the editing by
                        // returning null
                        return null;
                    }
                } catch (NumberFormatException nfe) {
                    Log.e(TAG, "Rejecting insert because'" + nfe.getMessage() + "'");
                }
                // returning "" will reject the editing action
                return "";
            }
        };

        valueEditText.setFilters(new InputFilter[] { filter });
        valueEditText.setOnEditorActionListener(new OnEditorActionListener() {
            Number currentValue = initialValueNum;

            @Override
            public boolean onEditorAction(final TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_DONE) {

                    Number readEnteredValue = null;
                    try {
                        if (ValueType.SHORT == valueType) {
                            readEnteredValue = Short.valueOf(v.getText().toString());
                        } else if (ValueType.INT == valueType) {
                            readEnteredValue = Integer.valueOf(v.getText().toString());
                        }
                    } catch (NumberFormatException nfe) {
                        if (ValueType.SHORT == valueType) {
                            Log.e(TAG, "Failed parsing Short: '" + nfe.getMessage() + "' returing to previous value");
                        } else if (ValueType.INT == valueType) {
                            Log.e(TAG, "Failed parsing Integer: '" + nfe.getMessage() + "' returing to previous value");
                        }
                        v.setText(String.valueOf(currentValue));
                        return true;
                    }

                    final Number enteredValue = readEnteredValue;

                    // set the property in a background task
                    SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                        @Override
                        protected void onFailure(ControlPanelException e) {
                            v.setText(String.valueOf(currentValue));
                            if (exceptionHandler != null) {
                                // An exception was thrown. Restore old value.
                                exceptionHandler.handleControlPanelException(e);
                            }
                        }

                        @Override
                        protected void onSuccess() {
                            // All went well. Store the new value.
                            currentValue = enteredValue;
                        }

                    };
                    asyncTask.execute(propertyWidget, enteredValue);
                }
                // otherwise keyboard remains up
                return false;
            }
        });

        return layout;
    }// createEditTextView

    // =====================================================================================================================

    private View createTimePickerView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a time picker for property " + propertyWidget.getLabel());

        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final Button valueButton = new Button(uiContext);
        valueButton.setTag(PROPERTY_VALUE);

        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueButton, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueButton.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        // set the current value
         if (initialValue != null && (ValueType.TIME.equals(propertyWidget.getValueType()))) {
            PropertyWidget.Time time = (PropertyWidget.Time) initialValue;
            valueButton.setText(formatTime(time.getHour(), time.getMinute()));
        } else {
            Log.e(TAG, "TimePicker property.getValueType() is not TIME, initializing property without current value");
        }

        // register time picker dialog listener
        final TimePickerDialog.OnTimeSetListener onTimeSetListener = new TimePickerDialog.OnTimeSetListener() {

            @Override
            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                PropertyWidget.Time time = new PropertyWidget.Time();
                time.setHour((short) hourOfDay);
                time.setMinute((short) minute);
                time.setSecond((short) 0);

                // set the property in a background task
                SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                    @Override
                    protected void onFailure(ControlPanelException e) {
                        if (exceptionHandler != null) {
                            // no need to restore value, it hasn't changed.
                            exceptionHandler.handleControlPanelException(e);
                        }
                    }

                    @Override
                    protected void onSuccess() {
                        // don't worry about the result, it will be broadcasted
                        // with ValueChanged
                        // All went well.
                    }

                };
                asyncTask.execute(propertyWidget, time);
            }
        };

        // register click listener
        OnClickListener onClickListener = new OnClickListener() {

            @Override
            public void onClick(View v) {

                // set the current value
                ServiceTasksExecutor exec   = ServiceTasksExecutor.createExecutor(1);
                Future<Object> futCurrValue = submitGetPropertyCurrentValueTask(exec, propertyWidget);

                Object currentValue = null;
                PropertyWidget.Time time;
                try {

                    if ( futCurrValue != null ) {

                        currentValue = futCurrValue.get(timeoutValue, timeoutUnit);
                    }
                } catch (Exception e) {
                    Log.e(TAG, "TimePickerView property.getCurrentValue() failed, initializing picker without current value", e);
                    futCurrValue.cancel(true);
                    time = new PropertyWidget.Time();
                }

                if (currentValue != null && (ValueType.TIME.equals(propertyWidget.getValueType()))) {
                    time = (PropertyWidget.Time) currentValue;
                } else {
                    Log.e(TAG, "TimePickerView property.getValueType() is not TIME, initializing picker without current value");
                    time = new PropertyWidget.Time();
                }

                // Use the current time as the default values for the picker
                final Calendar c = Calendar.getInstance();
                int hour = currentValue == null ? c.get(Calendar.HOUR_OF_DAY) : time.getHour();
                int minute = currentValue == null ? c.get(Calendar.MINUTE) : time.getMinute();

                // Pop a TimePickerDialog
                new TimePickerDialog(uiContext, onTimeSetListener, hour, minute, DateFormat.is24HourFormat(uiContext)).show();
            }
        };
        valueButton.setOnClickListener(onClickListener);

        return layout;
    }// createTimePickerView

    // =====================================================================================================================

    private View createDatePickerView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a date picker for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final Button valueButton = new Button(uiContext);
        valueButton.setTag(PROPERTY_VALUE);

        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueButton, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueButton.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        if (initialValue != null && (ValueType.DATE.equals(propertyWidget.getValueType()))) {
            PropertyWidget.Date date = (PropertyWidget.Date) initialValue;
            valueButton.setText(formatDate(date.getDay(), date.getMonth(), date.getYear()));
        } else {
            Log.e(TAG, "DatePicker property.getValueType() is not DATE, initializing property without current value");
        }

        // register time picker dialog listener
        final DatePickerDialog.OnDateSetListener onDateSetListener = new DatePickerDialog.OnDateSetListener() {

            @Override
            public void onDateSet(DatePicker view, int year, int month, int day) {
                PropertyWidget.Date date = new PropertyWidget.Date();

                // DatePicker enums months from 0..11 :(
                month++;

                date.setDay((short) day);
                date.setMonth((short) month);
                date.setYear((short) year);

                // set the property in a background task
                SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                    @Override
                    protected void onFailure(ControlPanelException e) {
                        if (exceptionHandler != null) {
                            // no need to restore value, it hasn't changed.
                            exceptionHandler.handleControlPanelException(e);
                        }
                    }

                    @Override
                    protected void onSuccess() {
                        // don't worry about the result, it will be broadcasted
                        // with ValueChanged
                        // All went well.
                    }

                };
                asyncTask.execute(propertyWidget, date);
            }
        };

        // register click listener
        OnClickListener onClickListener = new OnClickListener() {

            @Override
            public void onClick(View v) {

                // set the current value
                ServiceTasksExecutor exec   = ServiceTasksExecutor.createExecutor(1);
                Future<Object> futCurrValue = submitGetPropertyCurrentValueTask(exec, propertyWidget);

                Object currentValue = null;
                PropertyWidget.Date date;
                try {

                    if ( futCurrValue != null) {

                        currentValue = futCurrValue.get(timeoutValue, timeoutUnit);
                    }
                } catch (Exception e) {

                    Log.e(TAG, "DatePickerView property.getCurrentValue() failed, initializing picker without current value", e);
                    futCurrValue.cancel(true);
                    date = new PropertyWidget.Date();
                }
                finally {
                    exec.shutdown();
                }

                if (currentValue != null && (ValueType.DATE.equals(propertyWidget.getValueType()))) {
                    date = (PropertyWidget.Date) currentValue;
                } else {
                    Log.e(TAG, "property.getValueType() is not DATE, initializing picker without current value");
                    date = new PropertyWidget.Date();
                }

                // Use the current date as the default values for the picker
                final Calendar c = Calendar.getInstance();
                int day = currentValue == null ? c.get(Calendar.DAY_OF_MONTH) : date.getDay();
                int month = currentValue == null ? c.get(Calendar.MONTH) : date.getMonth();
                int year = currentValue == null ? c.get(Calendar.YEAR) : date.getYear();

                // DatePicker enums months from 0..11 :(
                month--;
                // Pop a DatePickerDialog
                new DatePickerDialog(uiContext, onDateSetListener, year, month, day).show();
            }
        };
        valueButton.setOnClickListener(onClickListener);

        return layout;
    }// createTimePickerView

    // =====================================================================================================================

    private View createNumberPickerView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a number picker for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final TextView valueTextView = new TextView(uiContext);
        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueTextView, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueTextView.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        RangeConstraint<?> propertyRangeConstraint = propertyWidget.getPropertyRangeConstraint();
        if (propertyRangeConstraint == null) {
            Log.e(TAG, "Found null property-range. Disabling Number Picker. Returning a plain label.");
            valueTextView.setEnabled(false);
            return layout;
        }

        if (initialValue != null) {
            valueTextView.setText(initialValue.toString());
        }

        // register touch listener
        /*
         * Only in honeycomb 3.0 final int min = (Integer)
         * propertyRangeConstraint.getMin(); final int max = (Integer)
         * propertyRangeConstraint.getMax(); //final int increment = (Integer)
         * propertyRangeConstraint.getIncrement();
         * numericTextView.setOnTouchListener(new OnTouchListener() {
         *
         * @Override public boolean onTouch (View v, MotionEvent event) { if
         * (event.getAction() == MotionEvent.ACTION_DOWN) {
         *
         * final NumberPicker picker = new NumberPicker(uiContext);
         * picker.setMaxValue(max); picker.setMinValue(min);
         * picker.setValue(Integer
         * .valueOf(numericTextView.getText().toString())); AlertDialog
         * alertDialog = new AlertDialog.Builder(uiContext) .setView(picker)
         * .setTitle(propertyWidget.getLabel())
         * .setPositiveButton(R.string.dialog_set, new
         * DialogInterface.OnClickListener() { public void
         * onClick(DialogInterface dialog, int whichButton) {
         * setPropertyValue(propertyWidget, picker.getValue());
         * numericTextView.setText(String.valueOf(picker.getValue())); }})
         * .setNegativeButton(R.string.dialog_cancel, null).create();
         * alertDialog.show(); return true; } return false; } });
         */
        return layout;
    }

    // =====================================================================================================================

    private View createNumericView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a numberic view for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final TextView valueTextView = new TextView(uiContext);
        valueTextView.setTag(PROPERTY_VALUE);

        layout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        layout.addView(valueTextView, hLinearLayoutParams);

        // initialize meta data
        // if (propertyWidget.getBgColor() != null)
        // valueTextView.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        if (initialValue != null) {
            Log.d(TAG, "Setting property value to: " + initialValue.toString());
            valueTextView.setText(initialValue.toString());
        }

        return layout;
    }

    // =====================================================================================================================

    private View createSliderView(final PropertyWidget propertyWidget, Object initialValue) {
        Log.d(TAG, "Creating a slider for property " + propertyWidget.getLabel());

        // create the label
        final LinearLayout layout = new LinearLayout(uiContext);
        layout.setOrientation(LinearLayout.VERTICAL);
        final LinearLayout innerLayout = new LinearLayout(uiContext);
        innerLayout.setOrientation(LinearLayout.HORIZONTAL);
        final TextView nameTextView = new TextView(uiContext);
        nameTextView.setText(propertyWidget.getLabel());
        final TextView valueTextView = new TextView(uiContext);
        valueTextView.setTag(PROPERTY_VALUE);
        final TextView unitsOfMeasureTextView = new TextView(uiContext);
        final SeekBar slider = new SeekBar(uiContext);
        slider.setTag(PROPERTY_EDITOR);

        innerLayout.addView(nameTextView);
        LinearLayout.LayoutParams hLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        hLinearLayoutParams.setMargins(10, 0, 0, 0);
        innerLayout.addView(valueTextView, hLinearLayoutParams);
        innerLayout.addView(unitsOfMeasureTextView, hLinearLayoutParams);

        LinearLayout.LayoutParams vLinearLayoutParams = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        layout.addView(innerLayout, vLinearLayoutParams);
        layout.addView(slider, vLinearLayoutParams);

        // initialize meta data
        // valueTextView.setBackgroundColor(propertyWidget.getBgColor());

        Log.d(TAG, "Property isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());
        enableViewGroup(layout, propertyWidget.isEnabled() && propertyWidget.isWritable());

        String unitOfMeasure = propertyWidget.getUnitOfMeasure();
        Log.d(TAG, "Setting property units of measure to: " + unitOfMeasure);
        unitsOfMeasureTextView.setText(unitOfMeasure);

        RangeConstraint<?> propertyRangeConstraint = propertyWidget.getPropertyRangeConstraint();
        if (propertyRangeConstraint == null) {
            Log.e(TAG, "Found null property-range. Disabling Slider. Returning a plain label.");
            return new TextView(uiContext);
        }

        // set the current value
        ValueType valueType       = propertyWidget.getValueType();
        final int initialValueNum = (initialValue == null ? 0 : ValueType.SHORT.equals(valueType) ?
                                    ((Short) initialValue) : ValueType.INT.equals(valueType) ? ((Integer) initialValue) : 0);

        // !!! Android sliders always start from 0 !!!
        Object minT = propertyRangeConstraint.getMin();
        final int min = ValueType.SHORT.equals(valueType) ? ((Short) minT) : ValueType.INT.equals(valueType) ? ((Integer) minT) : 0;

        Object maxT = propertyRangeConstraint.getMax();
        int max = ValueType.SHORT.equals(valueType) ? ((Short) maxT) : ValueType.INT.equals(valueType) ? ((Integer) maxT) : 0;

        Object incrementT = propertyRangeConstraint.getIncrement();
        final int increment = ValueType.SHORT.equals(valueType) ? ((Short) incrementT) : ValueType.INT.equals(valueType) ? ((Integer) incrementT) : 0;

        // because the minimum value in android always starts from 0, we move
        // the max value to persist the min,max range
        max -= min;

        slider.setMax(max);
        slider.setKeyProgressIncrement(increment);

        final int initialValueTrans = initialValueNum - min;
        Log.d(TAG, "Setting property value to: " + String.valueOf(initialValueNum) + " Slider value: '" + initialValueTrans + "'");
        slider.setProgress(initialValueTrans);
        valueTextView.setText(String.valueOf(initialValueNum));

        slider.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int currentProgress = initialValueTrans;
            final int listenMin = min;

            @Override
            public void onStopTrackingTouch(final SeekBar seekBar) {

                // set the property in a background task
                SetPropertyAsyncTask asyncTask = new SetPropertyAsyncTask() {

                    @Override
                    protected void onFailure(ControlPanelException e) {
                        seekBar.setProgress(currentProgress);

                        valueTextView.setText(String.valueOf(currentProgress + listenMin));

                        if (exceptionHandler != null) {
                            // An exception was thrown. Restore old value.
                            exceptionHandler.handleControlPanelException(e);
                        }
                    }

                    @Override
                    protected void onSuccess() {
                        // All went well. Store the new value.
                        currentProgress = seekBar.getProgress();
                    }
                };

                int progress = seekBar.getProgress();
                int valueToUpdate = progress + listenMin;
                Log.d(TAG, "The slider progress: '" + progress + "' valueToUpdate: '" + valueToUpdate + "'");
                asyncTask.execute(propertyWidget, valueToUpdate);
            }// onStopTrackingTouch

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                valueTextView.setText(String.valueOf(progress + listenMin));
            }
        });

        return layout;
    }

    // =====================================================================================================================

    /**
     * A callback that is invoked when the meta data of a UIElement has changed.
     * Refreshes the UIElement and the corresponding View.
     *
     * @param element
     *            the abstract UIElement
     */
    public void onMetaDataChange(UIElement element) {
        UIElementType elementType = element.getElementType();
        Log.d(TAG, "Refreshing the Android View for element : '" + element.getObjectPath() + "'");

        switch (elementType) {
        case ACTION_WIDGET: {
            onActionMetaDataChange((ActionWidget) element);
            break;
        }// ACTION_WIDGET
        case CONTAINER: {
            onContainerMetaDataChange((ContainerWidget) element);
            break;
        }// CONTAINER
         // case LIST_PROPERTY_WIDGET: {
         // break;
         // }//LIST_PROPERTY_WIDGET
        case PROPERTY_WIDGET: {
            onPropertyMetaDataChange((PropertyWidget) element);
            break;
        }// PROPERTY_WIDGET
        case ALERT_DIALOG: {
            onAlertDialogMetaDataChange((AlertDialogWidget) element);
            break;
        }// PROPERTY_WIDGET
        default:
            break;
        }// switch :: elementType
    }

    // =====================================================================================================================

    private void onContainerMetaDataChange(ContainerWidget container) {

        if (container == null) {
            Log.e(TAG, "onContainerMetaDataChange");
            return;
        }

        // get a handle to the corresponding view
        ViewGroup containerLayout = (ViewGroup) uiElementToView.get(container.getObjectPath());
        if (containerLayout == null) {
            Log.d(TAG, "ViewGroup not found for widget " + container.getObjectPath());
            return;
        }

        boolean enabled = container.isEnabled();
        Log.d(TAG, "Refreshing ContainerWidget bgColor:  " + container.getBgColor() + " enabled: " + enabled);
        // if (container.getBgColor() != null)
        // containerLayout.setBackgroundColor(container.getBgColor());

        if (enabled && !containerLayout.isEnabled()) {
            enableViewGroup(containerLayout, true);
        } else if (!enabled && containerLayout.isEnabled()) {
            enableViewGroup(containerLayout, false);
        }

    }

    // =====================================================================================================================

    private void onActionMetaDataChange(final ActionWidget actionWidget) {

        // get a handle to the corresponding view
        Button actionButton = (Button) uiElementToView.get(actionWidget.getObjectPath());
        if (actionButton == null) {
            Log.d(TAG, "Button not found for widget " + actionWidget.getObjectPath());
            return;
        }

        String label = actionWidget.getLabel();
        Integer bgColor = actionWidget.getBgColor();
        boolean enabled = actionWidget.isEnabled();

        Log.d(TAG, "Refreshing ActionWidget: Label: " + label + " BGColor: " + bgColor + " Enabled: " + enabled);

        actionButton.setText(label);
        actionButton.setEnabled(enabled);
        // if (bgColor != null) {
        // Unfortunately button loses its pressed behavior when background is
        // set.
        // actionButton.setBackgroundColor(bgColor);
        // }

    }// onActionMetaDataChange

    // =====================================================================================================================

    private void onAlertDialogMetaDataChange(final AlertDialogWidget alertDialogWidget) {

        // get a handle to the corresponding AlertDialog
        AlertDialog alertDialog = alertWidgetToDialog.get(alertDialogWidget.getObjectPath());
        if (alertDialog == null) {
            Log.d(TAG, "AlertDialog not found for widget " + alertDialogWidget.getObjectPath());
            return;
        }

        if (!alertDialogWidget.isEnabled() && alertDialog.isShowing()) {
            Log.d(TAG, "Dismissing AlertDialog");
            alertDialog.dismiss();
        }
    }// onAlertDialogMetaDataChange

    // =====================================================================================================================

    private void onPropertyMetaDataChange(PropertyWidget propertyWidget) {

        // get a handle to the corresponding view
        View propertyView = uiElementToView.get(propertyWidget.getObjectPath());
        if (propertyView == null) {
            Log.d(TAG, "Property View not found for widget " + propertyWidget.getObjectPath());
            return;
        }

        Log.d(TAG, "Refreshing the view of property '" + propertyWidget.getLabel() + "' isWritable? " + propertyWidget.isWritable() + ", isEnabled? " + propertyWidget.isEnabled());

        // Set enable/disable property
        if (propertyView instanceof ViewGroup) {
            enableViewGroup(propertyView, propertyWidget.isEnabled() && propertyWidget.isWritable());
        }// if :: ViewGroup
        else {
            propertyView.setEnabled(propertyWidget.isEnabled() && propertyWidget.isWritable());
        }

        // if (!(propertyView instanceof Spinner)) {
        // if (propertyWidget.getBgColor() != null)
        // propertyView.setBackgroundColor(propertyWidget.getBgColor());
        // }
    }

    /**
     * Iteration over the given {@link ViewGroup} and set it enable/disable state
     * @param propertyView {@link ViewGroup} to set its enable/disable state
     * @param enable
     */
    private void enableViewGroup(View propertyView, boolean enable) {
        if (!(propertyView instanceof ViewGroup)) {
            Log.w(TAG, "The given propertyView is no intanceof ViewGroup");
            return;
        }

        ViewGroup viewGroup = (ViewGroup) propertyView;
        for (int i = 0; i < viewGroup.getChildCount(); ++i) {

            View element = viewGroup.getChildAt(i);
            if (element instanceof ViewGroup) {
                enableViewGroup(element, enable);
            }
            if (PROPERTY_EDITOR.equals(element.getTag())) {
                element.setEnabled(enable);
            }
        }// for :: viewGroup

        viewGroup.setEnabled(enable);
    }// enableViewGroup

    // =====================================================================================================================

    /**
     * A callback that is invoked when a value of a UIElement has changed.
     * Refreshes the UIElement and the corresponding View.
     * @param element the abstract UIElement
     * @param newValue the new value
     */
    public void onValueChange(UIElement element, Object newValue) {
        UIElementType elementType = element.getElementType();
        Log.d(TAG, "Value changed for the Android View for element : '" + element.getObjectPath() + "', newValue: '" + newValue);

        switch (elementType) {
        case PROPERTY_WIDGET: {
            onPropertyValueChange((PropertyWidget) element, newValue);
            break;
        }// PROPERTY_WIDGET
        case ACTION_WIDGET: {
            Log.d(TAG, "Ignoring change of value for action : '" + element.getObjectPath() + "'");
            break;
        }// ACTION_WIDGET
        case CONTAINER: {
            Log.d(TAG, "Ignoring change of value for container : '" + element.getObjectPath() + "'");
            break;
        }// CONTAINER
         // case LIST_PROPERTY_WIDGET: {
         // break;
         // }//LIST_PROPERTY_WIDGET
        default:
            break;
        }// switch :: elementType
    }

    // =====================================================================================================================

    /**
     * A callback that is invoked when a value of a property has changed.
     * Refreshes the property and the corresponding View.
     * @param propertyWidget the property whose value has changed.
     * @param newValue new value for the property
     */
    private void onPropertyValueChange(PropertyWidget propertyWidget, Object newValue) {

        // get a handle to the corresponding view
        View propertyView = uiElementToView.get(propertyWidget.getObjectPath());
        if (propertyView == null) {
            Log.d(TAG, "Property View not found for widget " + propertyWidget.getObjectPath());
            return;
        }
        if (newValue == null) {
            Log.e(TAG, "onPropertyValueChange() failed, new value is null");
            // ====
            return;
        }

        ValueType valueType = propertyWidget.getValueType();

        List<PropertyWidgetHintsType> hints = propertyWidget.getHints();
        PropertyWidgetHintsType hint = (hints == null || hints.size() == 0) ? null : hints.get(0);

        Log.d(TAG, "Refreshing the View for property '" + propertyWidget.getLabel() + "' , using UI hint: " + hint + ", value type: '" + valueType + "', objPath: '" + propertyWidget.getObjectPath()
                + "'" + "', newValue: '" + newValue);

        switch (valueType) {
        case BOOLEAN:
            // Boolean Property
            onCheckBoxValueChange(propertyView, propertyWidget, newValue);
            break;
        case DATE:
            // Date Property
            onDateValueChange(propertyView, propertyWidget, newValue);
            break;
        case TIME:
            // Time Property
            onTimeValueChange(propertyView, propertyWidget, newValue);
            break;
        case BYTE:
        case INT:
        case SHORT:
        case LONG:
        case DOUBLE:
            // Scalar Property
            if (hint == null) {
                onNumericViewValueChange(propertyView, propertyWidget, newValue);
            } else {
                switch (hint) {
                case SPINNER:
                    onSpinnerValueChange(propertyView, propertyWidget, newValue);
                    break;
                case RADIO_BUTTON:
                    onRadioButtonValueChange(propertyView, propertyWidget, newValue);
                    break;
                case NUMERIC_VIEW:
                    onNumericViewValueChange(propertyView, propertyWidget, newValue);
                    break;
                case SLIDER:
                    onSliderValueChange(propertyView, propertyWidget, newValue);
                    break;
                case NUMERIC_KEYPAD:
                    onNumericKeypadValueChange(propertyView, propertyWidget, newValue);
                    break;
                default:
                    onNumericViewValueChange(propertyView, propertyWidget, newValue);
                    break;
                }
            }
            break;
        case STRING:
            // String Property
            if (hint == null) {
                Log.d(TAG, "No hint provided for property '" + propertyWidget.getLabel() + "', creating default: TextView");
                onTextViewValueChange(propertyView, propertyWidget, newValue);
            } else {
                switch (hint) {
                case SPINNER:
                    onSpinnerValueChange(propertyView, propertyWidget, newValue);
                    break;
                case RADIO_BUTTON:
                    onRadioButtonValueChange(propertyView, propertyWidget, newValue);
                    break;
                case EDIT_TEXT:
                    onEditTextValueChange(propertyView, propertyWidget, newValue);
                    break;
                case TEXT_VIEW:
                    onTextViewValueChange(propertyView, propertyWidget, newValue);
                    break;
                default:
                    onTextViewValueChange(propertyView, propertyWidget, newValue);
                    break;
                }
            }
            break;
        default:
            Log.d(TAG, "Received an unsupported ValueType: '" + valueType + "' , not refreshing any view");
            break;
        }
    }

    // =====================================================================================================================

    private void onSliderValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue2) {
        ViewGroup layout = (ViewGroup) propertyView;
        final TextView valueTextView = (TextView) layout.findViewWithTag(PROPERTY_VALUE);
        final SeekBar slider = (SeekBar) layout.findViewWithTag(PROPERTY_EDITOR);
        Log.d(TAG, "Refreshing the value of property " + propertyWidget.getLabel());

        // set the current value
        ValueType valueType = propertyWidget.getValueType();
        int newValue = -1;
        switch (valueType) {
        case SHORT:
            newValue = (Short) newValue2;
            break;
        case INT:
            newValue = (Integer) newValue2;
            break;
        default:
            Log.e(TAG, "property.getValueType() has unexpected value type: " + valueType);
            // ====
            return;
        }

        // set value
        RangeConstraint<?> rangeCons = propertyWidget.getPropertyRangeConstraint();
        if (rangeCons == null) {
            Log.e(TAG, "Found null property-range nothing to do with it...");
            return;
        }

        Object minT = rangeCons.getMin();
        final int min = ValueType.SHORT.equals(valueType) ? ((Short) minT) : ValueType.INT.equals(valueType) ? ((Integer) minT) : 0;

        valueTextView.setText(String.valueOf(newValue));
        slider.setProgress(newValue - min);
    }

    // =====================================================================================================================

    private void onCheckBoxValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the CheckBox of property " + propertyWidget.getLabel());

        CheckBox checkbox = (CheckBox) propertyView;
        ValueType valueType = propertyWidget.getValueType();

        // set checked
        if (ValueType.BOOLEAN.equals(valueType)) {
            checkbox.setChecked((Boolean) newValue);
        } else {
            Log.e(TAG, "property.getCurrentValue() failed, cannot update property with value: " + newValue);
        }
    }

    // =====================================================================================================================

    private void onTextViewValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        String label = propertyWidget.getLabel();
        Log.d(TAG, "Refreshing the TextView of property '" + label + "'");

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final TextView valueTextView = (TextView) layout.findViewWithTag(PROPERTY_VALUE);

        // set the current value
        String newValueStr = newValue.toString();
        Log.d(TAG, "Setting property value to: '" + newValueStr + "'");
        valueTextView.setText(newValueStr);
    }

    // =====================================================================================================================

    private void onNumericViewValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the NumericView of property " + propertyWidget.getLabel());

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final TextView valueTextView = (TextView) layout.findViewWithTag(PROPERTY_VALUE);

        // set the current value
        Log.d(TAG, "Setting property value to: " + newValue.toString());
        valueTextView.setText(newValue.toString());

    }

    // =====================================================================================================================

    private void onNumericKeypadValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the NumericKeypad View of property " + propertyWidget.getLabel());

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final EditText valueEditText = (EditText) layout.findViewWithTag(PROPERTY_EDITOR);

        // set the current value
        Log.d(TAG, "Setting property value to: " + newValue.toString());
        valueEditText.setText(newValue.toString());
    }

    // =====================================================================================================================

    private void onTimeValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the Time View of property " + propertyWidget.getLabel());

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final Button valueButton = (Button) layout.findViewWithTag(PROPERTY_VALUE);

        // set the current value
        if (ValueType.TIME.equals(propertyWidget.getValueType())) {
            PropertyWidget.Time time = (PropertyWidget.Time) newValue;
            String formattedTime = formatTime(time.getHour(), time.getMinute());
            Log.d(TAG, "Setting property value to: " + formattedTime);
            valueButton.setText(formattedTime);
        } else {
            Log.e(TAG, "property.getValueType() is not TIME, cannot update property with new value: " + newValue);
        }
    }

    // =====================================================================================================================

    private void onDateValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the Date View of property " + propertyWidget.getLabel());

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final Button valueButton = (Button) layout.findViewWithTag(PROPERTY_VALUE);

        // set the current value
        if (ValueType.DATE.equals(propertyWidget.getValueType())) {
            PropertyWidget.Date date = (PropertyWidget.Date) newValue;
            String formattedDate = formatDate(date.getDay(), date.getMonth(), date.getYear());
            Log.d(TAG, "Setting property value to: " + formattedDate);
            valueButton.setText(formattedDate);
        } else {
            Log.e(TAG, "property.getValueType() is not DATE,  cannot update property with current value: " + newValue);
        }
    }

    // =====================================================================================================================

    private void onEditTextValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the EditText View of property " + propertyWidget.getLabel());

        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final EditText valueEditText = (EditText) layout.findViewWithTag(PROPERTY_EDITOR);

        // set the current value
        Log.d(TAG, "Setting property value to: " + newValue.toString());
        valueEditText.setText(newValue.toString());
    }

    // =====================================================================================================================

    private void onSpinnerValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        // extract the text view
        final ViewGroup layout = (ViewGroup) propertyView;
        final Spinner spinner = (Spinner) layout.findViewWithTag(PROPERTY_VALUE);
        Log.d(TAG, "Refreshing the spinner of property " + propertyWidget.getLabel());

        // set the selected item
        int selection = 0;
        @SuppressWarnings("unchecked")
        final ArrayAdapter<LabelValuePair> adapter = (ArrayAdapter<LabelValuePair>) spinner.getAdapter();
        for (int i = 0; i < adapter.getCount(); i++) {
            LabelValuePair item = adapter.getItem(i);
            if (item != null && item.value.equals(newValue)) {
                selection = i;
                break;
            }
        }
        spinner.setSelection(selection);
        adapter.notifyDataSetChanged();
    }

    // =====================================================================================================================

    private void onRadioButtonValueChange(View propertyView, PropertyWidget propertyWidget, Object newValue) {
        Log.d(TAG, "Refreshing the RadioButton of property '" + propertyWidget.getLabel() + "', new value: " + newValue);

        final ViewGroup layout = (ViewGroup) propertyView;
        final RadioGroup radioGroup = (RadioGroup) layout.findViewWithTag(PROPERTY_VALUE);

        // set the selected item
        int selection = 0;
        final List<ConstrainToValues<?>> listOfConstraint = propertyWidget.getListOfConstraint();
        if (listOfConstraint != null) {
            for (ConstrainToValues<?> valueCons : listOfConstraint) {
                boolean selectThis = valueCons.getValue().equals(newValue);

                // check the default value
                if (selectThis) {
                    Log.d(TAG, "Selecting radio button, Label: " + valueCons.getLabel() + " Value: " + valueCons.getValue());
                    RadioButton radioButton = (RadioButton) radioGroup.getChildAt(selection);
                    if (!radioButton.isChecked()) {
                        radioButton.setChecked(true);
                    }
                } else {
                    selection++;
                }

            }
        }// LOV constraints
    }

    // =====================================================================================================================

    /**
     * A wrapper class for hosting a {label,value} pair inside an ArrayAdapter.
     * So that the label is displayed, while practically the real value is used.
     */
    class LabelValuePair {

        final String label;
        final Object value;

        public LabelValuePair(String label, Object value) {
            super();
            this.value = value;
            this.label = label;
        }

        @Override
        // This does the trick of displaying the label and not the value in the
        // Adapter
        public String toString() {
            return label;
        }
    }

    /**
     * A utility method that formats time for display, following the user's
     * locale as given by context
     * @param hour
     * @param minute
     * @return
     */
    private String formatTime(short hour, short minute) {
        Calendar calendar = new GregorianCalendar();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.HOUR_OF_DAY, hour);
        calendar.set(Calendar.MINUTE, minute);
        return DateFormat.getTimeFormat(uiContext.getApplicationContext()).format(calendar.getTime());
        // return String.format("%02d", hour) + ":" + String.format("%02d",
        // minute);
    }

    /**
     * A utility method that formats dates for display, following the user's
     * locale as given by context
     * @param day
     * @param month
     * @param year
     * @return
     */
    private String formatDate(short day, short month, short year) {
        // GregorianCalendar enums months from 0..11
        month--;
        Calendar calendar = new GregorianCalendar(year, month, day);
        return DateFormat.getDateFormat(uiContext.getApplicationContext()).format(calendar.getTime());
    }

}
