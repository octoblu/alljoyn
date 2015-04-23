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

package org.alljoyn.ioe.controlpanelservice.ui;

import java.lang.reflect.Method;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.VariantTypeReference;
import org.alljoyn.bus.VariantUtil;
import org.alljoyn.bus.ifaces.Properties;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControlSuper;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.PropertyWidgetConstrainToValuesAJ;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.PropertyWidgetRangeConstraintAJ;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.PropertyWidgetThreeShortAJ;

import android.util.Log;

/**
 * Property widget
 */
public class PropertyWidget extends UIElement {
	private static final String TAG = "cpan" + PropertyWidget.class.getSimpleName();
	
	/**
	 * The possible types that {@link PropertyWidget} can be casted. <br>
	 * The {@link PropertyWidget} type is defined according to the received DBus signature.
	 */
	public static enum ValueType {
		BOOLEAN,
		DOUBLE,
		INT,
		SHORT,
		STRING,
		LONG,
		BYTE,
		DATE,
		TIME
		;
	}
	
	//=================================================//
	
	/**
	 * The class defines a range constraint of {@link PropertyWidget}  <br>
	 * The generic type designates the actual type of the property value
	 */
	public static class RangeConstraint<T> {
		/**
		 * The min value with the same type as that of the property
		 */
		private T min;
		
		/**
		 * The max value with the same type as that of the property
		 */
		private T max;
		
		/**
		 * The increment/decrement value with the same type as that of the property
		 */
		private T increment;

		/**
		 * Conctructor 
		 * @param min The min value with the same type as that of the property
		 * @param max The max value with the same type as that of the property
		 * @param increment The increment/decrement value with the same type as that of the property <br>
	     * The generic type designates the actual type of the property value
		 */
		public RangeConstraint(T min, T max, T increment) {
			this.min 		= min;
			this.max 		= max;
			this.increment  = increment;
		}

		/**
		 * The min value with the same type as that of the property
		 */
		public T getMin() {
			return min;
		}

		/**
		 * The max value with the same type as that of the property
		 */
		public T getMax() {
			return max;
		}

		/**
		 * The increment/decrement value with the same type as that of the property
		 */
		public T getIncrement() {
			return increment;
		}
		
	}
	
	//=====================================================//
	
	/**
	 * The class define a single entry of the List-Of-Values constraint <br>
	 * {@link PropertyWidget#listOfValuesConstraint}   <br>
	 * The generic type designates the actual type of the property value
	 */
	public static class ConstrainToValues<T> {
		
		/**
		 * Constraint value
		 */
		private T value;
		
		/**
		 * The value to be displayed
		 */
		private String label;

		/**
		 * Constructor
		 * @param value Constraint value
		 * @param label The value to be displayed
		 */
		public ConstrainToValues(T value, String label) {
			super();
			this.value = value;
			this.label = label;
		}

		/**
		 * Constraint value
		 * @return The constraint value
		 */
		public T getValue() {
			return value;
		}

		/**
		 * The value to be displayed
		 * @return The label content
		 */
		public String getLabel() {
			return label;
		}
		
	}
	
	//=====================================================//
	
	/**
	 * The class defines the {@link ValueType#DATE} object of the {@link PropertyWidget}
	 */
	public static class Date {
		private short day;
		private short month;
		private short year;
		
		public static final short TYPE_CODE = 0; 
		
		public short getDay() {
			return day;
		}
		public void setDay(short day) {
			this.day = day;
		}
		public short getMonth() {
			return month;
		}
		public void setMonth(short month) {
			this.month = month;
		}
		public short getYear() {
			return year;
		}
		public void setYear(short year) {
			this.year = year;
		}
		
		@Override
		public String toString() {
			return day + "/" + month + "/" + year;
		}
	}
	
	//=====================================================//
	
	/**
	 * The class defines the {@link ValueType#TIME} object of the {@link PropertyWidget}
	 */
	public static class Time {
		private short hour;
		private short minute;
		private short second;
		
		public static final short TYPE_CODE = 1; 
		
		public short getHour() {
			return hour;
		}
		public void setHour(short hour) {
			this.hour = hour;
		}
		public short getMinute() {
			return minute;
		}
		public void setMinute(short minute) {
			this.minute = minute;
		}
		public short getSecond() {
			return second;
		}
		public void setSecond(short second) {
			this.second = second;
		}
		
		@Override
		public String toString() {
			return hour + ":" + minute + ":" + second;
		}
	}
	
	//=====================================================//
	
	//                 END OF NESTED CLASSES               // 
	
	//=====================================================//
	
	
	private static final int ENABLED_MASK 	= 0x01;
	private static final int WRITABLE_MASK	= 0x02;
	
    /**
     * The remote property object
     */
    private PropertyControlSuper remoteControl;
	
	/**
	 * The property value dbus signature
	 */
	private String signature;
	
	/**
	 * Use this variable for decision to what type the property currentValue should be casted
	 */
	private ValueType valueType;
	
	/**
	 * The propertyWidget type used to unmarshal the property value from a Variant type
	 */
	private Type conversionType;
	
	/**
	 * Property label  <br>
     * Not required to be set
	 */
	private String label;
	
	/**
	 * The property unit of measure <br>
     * Not required to be set
	 */
	private String unitOfMeasure;
	
	/**
	 * The background color
	 * <br>
     * Not required to be set
	 */
	private Integer bgColor;
	
	/**
	 * Whether the property is enabled <br>
     * Not required to be set
	 */
	private boolean enabled;
	
	/**
	 * Does the property has a permits a read & write 
	 * <br>
     * Not required to be set
	 */
	private boolean writable;
	
	/**
	 * The list of value constraints
	 * <br>
     * Not required to be set
	 */
	private List<ConstrainToValues<?>> listOfValuesConstraint;

	/**
	 * Stores PropertyRange constraint
	 * <br>
     * Not required to be set
	 */
	private RangeConstraint<?> propertyRangeConstraint;
	
	/**
	 * The widget rendering hints
	 */
	private List<PropertyWidgetHintsType> hints;
	
	/**
	 * Optional parameters
	 */
	private Map<Short, Variant> optParams;
	
	/**
	 * Constructor
	 * @param ifName
	 * @param objectPath
	 * @param controlPanel
	 * @param children
	 * @throws ControlPanelException
	 */
	public PropertyWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
		super(UIElementType.PROPERTY_WIDGET, ifName, objectPath, controlPanel, children);
	}

	/**
	 * @return Returns the current property value of the remote controllable device 
	 * @throws ControlPanelException If failed to read the remote current property value
	 */
	public Object getCurrentValue() throws ControlPanelException {
		Object retValue;
		Log.d(TAG, "getCurrentValue called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
		try{
			Variant currValVar = remoteControl.getValue();
		    retValue           = unmarshalCurrentValue(currValVar);
		}
		catch(BusException be) {
			String msg = "Failed to	getCurrentValue,  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
			Log.e(TAG, msg); 
			throw new ControlPanelException(msg);
		}
		
		return retValue;
	}//getValue
	
	/**
	 * @param newValue Set the new property value to the remote controllable device
	 * @throws ControlPanelException If failed to set the new property value 
	 */
	public void setCurrentValue(Object newValue) throws ControlPanelException {
		Log.i(TAG, "setCurrentValue called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "', value: '" + newValue + "'");
		
		if ( newValue == null ) {
			throw new ControlPanelException("PropertyWidget: '" + objectPath + "', the new value can't be NULL");
		}
		
		try {
			if ( valueType == ValueType.DATE ) {
				Date dt = (Date)newValue;
				PropertyWidgetThreeShortAJ struct = new PropertyWidgetThreeShortAJ();
				struct.dataType   = Date.TYPE_CODE;
				struct.data       = new PropertyWidgetThreeShortAJ.ThreeShortAJ();
				struct.data.var1  = dt.day;
				struct.data.var2  = dt.month;
				struct.data.var3  = dt.year;
				remoteControl.setValue( new Variant(struct, signature) );
			}//if DATE_TIME
			else if ( valueType == ValueType.TIME ) {
				PropertyWidgetThreeShortAJ struct = new PropertyWidgetThreeShortAJ();
				Time time = (Time)newValue;
				struct.dataType   = Time.TYPE_CODE;
				struct.data       = new PropertyWidgetThreeShortAJ.ThreeShortAJ();
				struct.data.var1  = time.hour;
				struct.data.var2  = time.minute;
				struct.data.var3  = time.second;
				remoteControl.setValue( new Variant(struct, signature) );
			}
			else {
				remoteControl.setValue( new Variant(newValue, signature) );
			}
		}//try
		catch(BusException be) {
			String msg = "Failed to setCurrentValue objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
	}//setCurrentValue
		
	public String getSignature() {
		return signature;
	}
	
	/**
	 * @return Returns the list of value constraints. <br>
	 * Each constraint value stored in {@link ConstrainToValues}  
	 */
	public List<ConstrainToValues<?>> getListOfConstraint() {
		return listOfValuesConstraint;
	}
	
	/**
	 * Use the type for casting the property value returned from getCurrentValue
	 * @return PropertyValueType
	 */
	public ValueType getValueType() {
		return valueType;
	}

	public String getLabel() {
		return label;
	}

	public String getUnitOfMeasure() {
		return unitOfMeasure;
	}

	public Integer getBgColor() {
		return bgColor;
	}

	public boolean isEnabled() {
		return enabled;
	}

	public boolean isWritable() {
		return writable;
	}

	public RangeConstraint<?> getPropertyRangeConstraint() {
		return propertyRangeConstraint;
	}

	public List<PropertyWidgetHintsType> getHints() {
		return hints;
	}
	
    //=============================================================//
	
	/**
	 * Unmarshal the current value from Variant object 
	 * @param curValVar The variant object
	 * @return The unmarshalled value
	 * @throws ControlPanelException If failed to unmarshal the current value from variant 
	 */
	Object unmarshalCurrentValue(Variant curValVar) throws ControlPanelException {
	    Object retVal;
		
	    try {
			if ( valueType == ValueType.DATE ) {
				Date retDate = new Date();
				PropertyWidgetThreeShortAJ struct = curValVar.getObject(PropertyWidgetThreeShortAJ.class);
				retDate.day    = struct.data.var1;
				retDate.month  = struct.data.var2;
				retDate.year   = struct.data.var3;
				retVal         = retDate;
			}
			else if ( valueType == ValueType.TIME ) {
				Time retTime = new Time();
				PropertyWidgetThreeShortAJ struct = curValVar.getObject(PropertyWidgetThreeShortAJ.class);
				retTime.hour    = struct.data.var1;
				retTime.minute  = struct.data.var2;
				retTime.second  = struct.data.var3;
				retVal          = retTime;
			}
			else {
				retVal = curValVar.getObject(conversionType);
			}
		}//try
		catch(BusException be) {
		    String msg = "Failed to unmarshal current property value, Error: '" + be.getMessage() + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
	     
	    return retVal;
	}//unmarshalCurrentValue

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#setRemoteController()
	 */
	@Override
	protected void setRemoteController() throws ControlPanelException {
        WidgetFactory widgetFactory = WidgetFactory.getWidgetFactory(ifName);
        
        if ( widgetFactory == null ) {
            String msg = "Received an unrecognized interface name: '" + ifName + "'";
            Log.e(TAG, msg);
            throw new ControlPanelException(msg);
        }
        
        Class<?> ifClass = widgetFactory.getIfaceClass();
		
	    ProxyBusObject proxyObj = ConnectionManager.getInstance().getProxyObject(
                device.getSender(),
                objectPath,
                sessionId,
                new Class[]{ ifClass, Properties.class }
        );
    
        Log.d(TAG, "Setting remote control PropertyWidget, objPath: '" + objectPath + "'");
        properties    = proxyObj.getInterface(Properties.class);
        remoteControl = (PropertyControlSuper) proxyObj.getInterface(ifClass);
        
        try {
        	this.version = remoteControl.getVersion();
		} catch (BusException e) {
			String msg = "Failed to call getVersion for element: '" + elementType + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
	}//setRemoteController
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#registerSignalHandler()
	 */
	protected void registerSignalHandler() {
		try {
			
			Method propertyMetaDataChanged = CommunicationUtil.getPropertyMetadataChanged("MetadataChanged");
			if ( propertyMetaDataChanged == null ) {
	    		String msg = "PropertyWidget, MetadataChanged method isn't defined";
	    		Log.e(TAG, msg);
	    		throw new ControlPanelException(msg);
			}
			
			Method propertyValueChanged = CommunicationUtil.getPropertyValueChanged("ValueChanged");
			if ( propertyValueChanged  == null ) {
	    		String msg = "PropertyWidget, ValueChanged method isn't defined";
	    		Log.e(TAG, msg);
	    		throw new ControlPanelException(msg);
			}
			
			Log.d(TAG, "PropertyWidget objPath: '" + objectPath + "', registering signal handler 'MetadataChanged'");
            registerSignalHander(new PropertyWidgetSignalHandler(this), propertyMetaDataChanged);
            
            Log.d(TAG, "PropertyWidget objPath: '" + objectPath + "', registering signal handler 'ValueChanged'");
            registerSignalHander(new PropertyWidgetSignalHandler(this), propertyValueChanged);
		}//try
		catch (ControlPanelException cpe) {
	          String msg = "Device: '" + device.getDeviceId() + 
                      "', PropertyWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
	          Log.e(TAG, msg);
	          controlPanel.getEventsListener().errorOccurred(controlPanel, msg);
		}
	}//registerSignalHandler
	
	//=============================================================//
	
	/**
	 * One note fillOptionalParameter method needs the valueType for execution. Therefore there is a code that ensures that the valueType
	 * will be defined before the fillOptionalParameter method is called
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#setProperty(java.lang.String, org.alljoyn.bus.Variant)
	 */
	@Override
	protected void setProperty(String propName, Variant propValue) throws ControlPanelException {
		try {
			if( "States".equals(propName) ) {
			    int states = propValue.getObject(int.class);
	            enabled    =  (states & ENABLED_MASK)  == ENABLED_MASK;
	            writable   =  (states & WRITABLE_MASK) == WRITABLE_MASK;
			}
			else if ( "OptParams".equals(propName) ) {
				optParams = propValue.getObject(new VariantTypeReference<HashMap<Short, Variant>>() {});
				if ( valueType != null ) {  // The property type has been already realized, it's possible to call fillOptionalParams
					fillOptionalParams(optParams);
					optParams = null;
				}
			}
			else if ( "Value".equals(propName) ) {
			    Variant value = propValue.getObject(Variant.class);
			    definePropertyType(value);
				if ( optParams != null ) {  // The OptionalParameters have been already set, call fillOptionalParams
					fillOptionalParams(optParams);
					optParams = null;
				}
			}
		}//try
		catch(BusException be) {
			throw new ControlPanelException("Failed to unmarshal the property: '" + propName + "', Error: '" + be.getMessage() + "'");
		}
	}//setProperty

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#createChildWidgets()
	 */

	@Override
	protected void createChildWidgets() throws ControlPanelException {
		int size = children.size();
		Log.d(TAG, "Test PropertyWidget validity - PropertyWidget can't has child nodes. #ChildNodes: '" + size + "'");
		if ( size > 0 ) {
			throw new ControlPanelException("The PropertyWidget objPath: '" + objectPath + "' is not valid, found '" + size + "' child nodes");
		}
	}//createChildWidgets

	/**
	 * Define the property type from the given value
	 * @param value 
	 * @throws ControlPanelException Is throws when failed to realize the property type
	 */
	private void definePropertyType(Variant value) throws ControlPanelException {
		if ( valueType != null ) {
			return;
		}
		
		try {
			signature = VariantUtil.getSignature(value);
			fromDbusSignatureToType(value);
			
			Log.d(TAG, "The PropertyWidget objPath: '" + objectPath + "', type is: '" + valueType + "', DbusSign: '" + signature + "'");
		} catch (BusException be) {
		    throw new ControlPanelException("Failed to read the property objPath: '" + objectPath + "' value, Error: '" + be.getMessage() + "'");
		}
		
	}//realize
	
	/**
	 * Converts from DBUS signature to the PropertyValueType 
	 * @param propValue the value 
	 * @throws ControlPanelException if the signature is of unsupported type
	 */
	private void fromDbusSignatureToType(Variant propValue) throws ControlPanelException {
		if ( "b".equals(signature) ) {
			valueType 		= ValueType.BOOLEAN;
			conversionType  = boolean.class;
		}
		else if ( "d".equals(signature) ) {
			valueType       = ValueType.DOUBLE;
			conversionType  = double.class;
		}
		else if ( "s".equals(signature) ) {
			valueType       = ValueType.STRING;
			conversionType  = String.class;
		}
		else if ( "y".equals(signature) ) {
			valueType       = ValueType.BYTE;
			conversionType  = byte.class;
		}
		else if ( "n".equals(signature) || "q".equals(signature) ) {
			valueType       = ValueType.SHORT;
			conversionType  = short.class;
		}
		else if ( "i".equals(signature) || "u".equals(signature) ) {
			valueType       = ValueType.INT;
			conversionType  = int.class;
		}
		else if ( "t".equals(signature) || "x".equals(signature) ) {
			valueType       = ValueType.LONG;
			conversionType  = long.class;
		}
		//Handler 3q structure
		else if ( "(q(qqq))".equals(signature) ) {
			PropertyWidgetThreeShortAJ struct;
			
			try {
				struct = propValue.getObject(PropertyWidgetThreeShortAJ.class);
			}
			catch (BusException be) {
				throw new ControlPanelException("PropertyWidget objPath: '" + objectPath + "', failed to unmarshal value of sugnature: '" + signature + "'");
			}
			
			switch ( struct.dataType ) {
			  	case Date.TYPE_CODE: {
			  		valueType 	    = ValueType.DATE;
			  		conversionType  = PropertyWidgetThreeShortAJ.class;
			  		break;
			  	}
			  	case Time.TYPE_CODE: {
			  		valueType 	    = ValueType.TIME;
			  		conversionType  = PropertyWidgetThreeShortAJ.class;
			  		break;
			  	}
			  	default: {
					throw new ControlPanelException("PropertyWidget objPath: '" + objectPath + "' belongs to an unsupported composite type: '" + signature + "'");
			  	}
			}//switch
			
		}//if :: 3q struct
		else {
			throw new ControlPanelException("PropertyWidget objPath: '" + objectPath + "' belongs to an unsupported type: '" + signature + "'");
		}
	}//fromDbusSignatureToType
		
	/**
	 * Fill the PropertyWidget optional parameters
	 * @param optParams The optinal parameters to fill
	 * @throws ControlPanelException
	 */
    private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
        // Add optional parameters
        Log.d(TAG, "PropertyWidget - scanning optional parameters");

        for (PropertyWidgetEnum optKeyEnum : PropertyWidgetEnum.values()) {

            Variant optParam = optParams.get(optKeyEnum.ID);

            if ( optParam == null ) {
                Log.v(TAG, "OptionalParameter: '" + optKeyEnum + "', is not found");
                continue;
            }

            Log.v(TAG, "Found OptionalParameter: '" + optKeyEnum + "'");

            try {
                switch (optKeyEnum) {
                    case LABEL: {
                        label = optParam.getObject(String.class);
                        break;
                    }
                    case UNIT_OF_MEASURE: {
                        unitOfMeasure = optParam.getObject(String.class);
                        break;
                    }
                    case BG_COLOR: {
                        bgColor = optParam.getObject(int.class);
                        break;
                    }
                    case CONSTRAINT_TO_VALUES: {
                       PropertyWidgetConstrainToValuesAJ[] lovConsAJ = optParam.getObject( PropertyWidgetConstrainToValuesAJ[].class );
                       setListOfValuesConstraints(lovConsAJ);
                       break;
                    }
                    case RANGE: {
                        PropertyWidgetRangeConstraintAJ propRangeConsAJ = optParam.getObject( PropertyWidgetRangeConstraintAJ.class );
                        RangeConstraint<?> propRangeCons                = propRangeConsAJ.getPropertyWidgetRangeConstraint(valueType);
                        
                        if ( propRangeCons == null ) {
                            throw new ControlPanelException("Fail to unmarshal a range constraint for PropertyWidget objPath: '" + objectPath + "'");
                        }
                        
                        this.propertyRangeConstraint = propRangeCons;
                        break;
                    }
                    case HINTS: {
                        short[] propertyWidgetHints = optParam.getObject(short[].class);
                        setListOfPropertyWidgetHints(propertyWidgetHints);
                        break;
                    }
                }//switch
            }//try
            catch(BusException be) {
                throw new ControlPanelException("Failed to unmarshal optional parameters for PropertyWidget objPath: '" + objectPath + "'");
            }
        }//for

    }//fillOptionalParams
    
    /**
     * Iterates over PropertyWidgetListOfValuesConstraintAJ[]. For each value call {@link PropertyWidgetConstrainToValuesAJ#getPropertyWidgetConstrainToValues(ValueType)} 
     * to receive {@link ConstrainToValues}
     * @param <T>
     * @param lovConsAJ Array of PropertyWidgetListOfValuesConstraintAJ
     * @param valueType The type of property value
     * @return List of values constraints or null on fail
     * @throws ControlPanelException If failed to unmarshal ConstaintToValues property
     */
    private void setListOfValuesConstraints(PropertyWidgetConstrainToValuesAJ[] lovConsAJ) throws ControlPanelException {
    	listOfValuesConstraint = new ArrayList<ConstrainToValues<?>>( lovConsAJ.length );

        for (PropertyWidgetConstrainToValuesAJ consValueAJ : lovConsAJ) {
            ConstrainToValues<?> consValue = consValueAJ.getPropertyWidgetConstrainToValues(valueType);
            if ( consValue == null ) {
                throw new ControlPanelException("PropertyWidget objPath: '" + objectPath + "', failed to unmarshal constraint value object");
            }
            listOfValuesConstraint.add(consValue);
        }//for :: consAJ[]
    }//getListOfValuesConstraints

    /**
     * Iterates over the hintIds array and fills list of {@link PropertyWidgetHintsType}
     * @param hintIds Ids of the widget hints
     * @return
     */
    private void setListOfPropertyWidgetHints(short[] hintIds) {
        Log.v(TAG, "PropertyWidget objPath: '" + objectPath + "', filling PropertyWidget hints");
        this.hints = new ArrayList<PropertyWidgetHintsType>( hintIds.length );
        
        for (short hId : hintIds) {
            PropertyWidgetHintsType hintType = PropertyWidgetHintsType.getEnumById(hId);
            if (hintType == null) {
                Log.w(TAG, "Received unrecognized hintId: '" + hId + "', ignoring");
                continue;
            }
            
            hints.add(hintType);
        }//for :: hintId
    }//setListOfPropertyWidgetHints

}
