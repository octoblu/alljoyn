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

package org.alljoyn.ioe.controlpanelservice.ui.ajstruct;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.annotation.Position;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ConstrainToValues;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ValueType;

import android.util.Log;

public class PropertyWidgetConstrainToValuesAJ {
	
	@Position(0)
	public Variant value;
	
	@Position(1)
	public String label;
	
	/**
	 * Returns single object of PropertyWidget list of values
	 * @param valueType The property type of value
	 * @return
	 */
	public ConstrainToValues<?> getPropertyWidgetConstrainToValues(ValueType valueType) {
		Log.v("cpan" + PropertyWidgetConstrainToValuesAJ.class.getSimpleName(), "Unmarshalling PropertyWidget LOV constraint, label: '" + label + "'");
		try {
			switch (valueType) {
				case BOOLEAN: {
					Boolean valueCast = value.getObject(boolean.class);
					return new ConstrainToValues<Boolean>(valueCast, label);
				}
				case BYTE: {
					Byte valueCast    = value.getObject(byte.class);
					return new ConstrainToValues<Byte>(valueCast, label);
				}
				case DOUBLE: {
					Double valueCast  = value.getObject(double.class);
					return new ConstrainToValues<Double>(valueCast, label);					
				}
				case INT: {
					Integer valueCast = value.getObject(int.class);
					return new ConstrainToValues<Integer>(valueCast, label);					
				}
				case LONG: {
					Long valueCast    = value.getObject(long.class);
					return new ConstrainToValues<Long>(valueCast, label);					
				}
				case SHORT: {
					Short valueCast   = value.getObject(short.class);
					return new ConstrainToValues<Short>(valueCast, label);					
				}
				case STRING: {
					String valueCast  = value.getObject(String.class);
					return new ConstrainToValues<String>(valueCast, label);					
				}
				default: {
					break;
				}
			}//SWITCH
		}//TRY
		catch (BusException be) {
			 Log.e("cpan" + PropertyWidgetConstrainToValuesAJ.class.getSimpleName(), "Failed to unmarshal PropertyWidget LOV - Error: '" + be.getMessage() + "'" );
			return null;
		}
		Log.e("cpan" + PropertyWidgetConstrainToValuesAJ.class.getSimpleName(), "Failed to unmarshal PropertyWidget LOV" );
		return null;
	}//getPropertyWidgetRangeConstraint
	
}
