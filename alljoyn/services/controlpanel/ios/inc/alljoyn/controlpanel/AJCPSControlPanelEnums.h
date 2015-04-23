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

#import <Foundation/Foundation.h>

/**
 * AJCPSControlPanelEnums
 */
@interface AJCPSControlPanelEnums : NSObject

@end

/**
 * Mode ControlPanel is in
 */
typedef NS_ENUM (NSInteger, AJCPSControlPanelMode) {
    /***
     * CONTROLLEE_MODE
     */
	AJCPS_CONTROLLEE_MODE,
    /***
     * CONTROLLER_MODE
     */
	AJCPS_CONTROLLER_MODE
};

/**
 * WidgetType
 */
typedef NS_ENUM (NSInteger, AJCPSWidgetType) {
    /**
     * AJCPS_CONTAINER
     */
	AJCPS_CONTAINER = 0,
    /**
     * AJCPS_ACTION
     */
	AJCPS_ACTION = 1,
    /**
     * AJCPS_ACTION_WITH_DIALOG
     */
	AJCPS_ACTION_WITH_DIALOG = 2,
    /**
     * AJCPS_LABEL
     */
	AJCPS_LABEL = 3,
    /**
     * AJCPS_PROPERTY
     */
	AJCPS_PROPERTY = 4,
    /**
     * AJCPS_DIALOG
     */
	AJCPS_DIALOG = 5,
    /**
     * AJCPS_ERROR
     */
    AJCPS_ERROR = 6
};

/**
 * Enum to define the type of Property
 */
typedef NS_ENUM (NSInteger, AJCPSPropertyType) {
    /**
     * bool property
     */
	AJCPS_BOOL_PROPERTY = 0,
    /**
     * uint16 property
     */
    AJCPS_UINT16_PROPERTY = 1,
    /**
     * int16 property
     */
    AJCPS_INT16_PROPERTY = 2,
    /**
     * uint32 property
     */
    AJCPS_UINT32_PROPERTY = 3,
    /**
     * int32 property
     */
    AJCPS_INT32_PROPERTY = 4,
    /**
     * uint64 property
     */
    AJCPS_UINT64_PROPERTY = 5,
    /**
     * int64 property
     */
    AJCPS_INT64_PROPERTY = 6,
    /**
     * double property
     */
    AJCPS_DOUBLE_PROPERTY = 7,
    /**
     * String property
     */
    AJCPS_STRING_PROPERTY = 8,
    /**
     * Date property
     */
    AJCPS_DATE_PROPERTY = 9,
    /**
     * Time property
     */
    AJCPS_TIME_PROPERTY = 10,
    /**
     * Undefined
     */
    AJCPS_UNDEFINED = 11
};

/**
 * Transactions that could go wrong resulting in an Error Occurred event being fired
 */
typedef NS_ENUM (NSInteger, AJCPSControlPanelTransaction) {
    /**
     * Session join
     */
	AJCPS_SESSION_JOIN = 0,
    /**
     * Register objects
     */
    AJCPS_REGISTER_OBJECTS = 1,
    /**
     * refresh value
     */
    AJCPS_REFRESH_VALUE = 2,
    /**
     * Refresh properties
     */
    AJCPS_REFRESH_PROPERTIES = 3
};

/**
 * Hints for Containers Widgets
 * determining the layout
 */
typedef NS_ENUM (NSInteger, AJCPS_LAYOUT_HINTS) {
    /**
     * Vertical linear
     */
	AJCPS_VERTICAL_LINEAR = 1,
    /**
     * Horizontal linear
     */
	AJCPS_HORIZONTAL_LINEAR = 2
};

/**
 * Hints for Dialog Widgets
 */
typedef NS_ENUM (NSInteger, AJCPS_DIALOG_HINTS) {
    /**
     * Alert dialog
     */
	AJCPS_ALERTDIALOG = 1
};

/**
 * Hints for Property Widgets
 */
typedef NS_ENUM (NSInteger, AJCPS_PROPERTY_HINTS) {
    /**
     * Switch
     */
	AJCPS_SWITCH = 1,
    /**
     * Checkbox
     */
    AJCPS_CHECKBOX = 2,
    /**
     * Spinner
     */
    AJCPS_SPINNER = 3,
    /**
     * Radio button
     */
    AJCPS_RADIOBUTTON = 4,
    /**
     * slider
     */
    AJCPS_SLIDER = 5,
    /**
     * Time picker
     */
    AJCPS_TIMEPICKER = 6,
    /**
     * Date picker
     */
    AJCPS_DATEPICKER = 7,
    /**
     * Number picker
     */
    AJCPS_NUMBERPICKER = 8,
    /**
     * Keypad
     */
    AJCPS_KEYPAD = 9,
    /**
     * Rotaryk nob
     */
    AJCPS_ROTARYKNOB = 10,
    /**
     * text view
     */
    AJCPS_TEXTVIEW = 11,
    /**
     * Numeric view
     */
    AJCPS_NUMERICVIEW = 12,
    /**
     * Edit text
     */
    AJCPS_EDITTEXT = 13
};

/**
 * Hints for Label Widgets
 */
typedef NS_ENUM (NSInteger, AJCPS_LABEL_HINTS) {
    /**
     * Text label
     */
	AJCPS_TEXTLABEL = 1
};

/**
 * Hints for ListProperty Widgets
 */
typedef NS_ENUM (NSInteger, LIST_PROPERTY_HINTS) {
    /**
     * Dynamic spinner
     */
	AJCPS_DYNAMICSPINNER = 1
};

/**
 * Hints for Action Widgets
 */
typedef NS_ENUM (NSInteger, ACTION_HINTS) {
    /**
     * Action button
     */
	AJCPS_ACTIONBUTTON = 1
};
