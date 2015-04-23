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
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.RangeConstraint;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ValueType;

import android.util.Log;

/**
 * Property widget range constraint 
 */
public class PropertyWidgetRangeConstraintAJ {

	/**
	 * Minimum range value
	 */
	@Position(0)
	public Variant min;
	
	/**
	 * Maximum range value
	 */
	@Position(1)
	public Variant max;
	
	/**
	 * The value to increment/decerement
	 */
	@Position(2)
	public Variant increment;
	
	/**
	 * @param valueType The value type of this property 
	 * @param <T>
	 * @return Property widget range constraint or NULL if failed to unmarshal
	 */
	public RangeConstraint<?> getPropertyWidgetRangeConstraint(ValueType valueType) {
		Log.v("cpan" + PropertyWidgetRangeConstraintAJ.class.getSimpleName(), "Unmarshalling PropertyWidget range constraint");
		try {
			switch (valueType) {
				case BYTE: {
					 Byte minCast 		    = min.getObject(byte.class);
					 Byte maxCast 			= max.getObject(byte.class);
					 Byte incrementCast		= increment.getObject(byte.class);
					 return new RangeConstraint<Byte>(minCast, maxCast, incrementCast);
				}
				case DOUBLE: {
					 Double minCast 		= min.getObject(double.class);
					 Double maxCast			= max.getObject(double.class);
					 Double incrementCast	= increment.getObject(double.class);
					 return new RangeConstraint<Double>(minCast, maxCast, incrementCast);
				}
				case INT: {
					 Integer minCast 		= min.getObject(int.class);
					 Integer maxCast 		= max.getObject(int.class);
					 Integer incrementCast	= increment.getObject(int.class);
					 return new RangeConstraint<Integer>(minCast, maxCast, incrementCast);
				}
				case LONG: {
					 Long minCast 	 	    = min.getObject(long.class);
					 Long maxCast 		    = max.getObject(long.class);
					 Long incrementCast     = increment.getObject(long.class);
					 return new RangeConstraint<Long>(minCast, maxCast, incrementCast);
				}
				case SHORT: {
					 Short minCast 	 	    = min.getObject(short.class);
					 Short maxCast 		    = max.getObject(short.class);
					 Short incrementVal 	= increment.getObject(short.class);
					 return new RangeConstraint<Short>(minCast, maxCast, incrementVal);
				}
				default : {
					break;
				}
			}//SWITCH
		}//TRY
		catch (BusException be) {
			 Log.e("cpan" + PropertyWidgetRangeConstraintAJ.class.getSimpleName(), "Failed to unmarshal Range constraint: Error: '" + be.getMessage() + "'" );
			return null;
		}
		Log.e("cpan" + PropertyWidgetRangeConstraintAJ.class.getSimpleName(), "Failed to unmarshal Range constraint" );
		return null;
	}//getPropertyWidgetRangeConstraint
}
