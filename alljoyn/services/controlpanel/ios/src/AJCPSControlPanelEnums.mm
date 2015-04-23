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

#import "AJCPSControlPanelEnums.h"

@interface AJCPSControlPanelEnums ()

@end


@implementation AJCPSControlPanelEnums

@end



//#ifndef CONTROLPANELENUMS_H_
//#define CONTROLPANELENUMS_H_
//
//namespace ajn {
//namespace services {
//
//#ifndef UINT32_MAX
//#define UINT32_MAX      (4294967295U)
//#endif
//
///**
// * Typedefs for functionPointers used
// */
//typedef bool (*GetBoolFptr)();
//typedef uint32_t (*GetUint32Fptr)();
//typedef const char* (*GetStringFptr)(uint16_t);
//
///**
// * Mode ControlPanel is in
// */
//typedef enum {
//    CONTROLLEE_MODE, //!< CONTROLLEE_MODE
//    CONTROLLER_MODE //!< CONTROLLER_MODE
//} ControlPanelMode;
//
///**
// * WidgetType
// */
//typedef enum {
//    CONTAINER = 0,         //!< CONTAINER
//    ACTION = 1,            //!< ACTION
//    ACTION_WITH_DIALOG = 2, //!< ACTION_WITH_DIALOG
//    LABEL = 3,             //!< LABEL
//    PROPERTY = 4,          //!< PROPERTY
//    DIALOG = 5             //!< DIALOG
//} WidgetType;
//
///**
// * Enum to define the type of Property
// */
//typedef enum {
//    BOOL_PROPERTY = 0,  //!< BOOL_PROPERTY
//    UINT16_PROPERTY = 1, //!< UINT16_PROPERTY
//    INT16_PROPERTY = 2, //!< INT16_PROPERTY
//    UINT32_PROPERTY = 3, //!< UINT32_PROPERTY
//    INT32_PROPERTY = 4, //!< INT32_PROPERTY
//    UINT64_PROPERTY = 5, //!< UINT64_PROPERTY
//    INT64_PROPERTY = 6, //!< INT64_PROPERTY
//    DOUBLE_PROPERTY = 7, //!< DOUBLE_PROPERTY
//    STRING_PROPERTY = 8, //!< STRING_PROPERTY
//    DATE_PROPERTY = 9,  //!< DATE_PROPERTY
//    TIME_PROPERTY = 10, //!< TIME_PROPERTY
//    UNDEFINED = 11      //!< UNDEFINED
//} PropertyType;
//
///**
// * Transactions that could go wrong resulting in an Error Occurred event being fired
// */
//typedef enum {
//    SESSION_JOIN = 0,     //!< SESSION_JOIN
//    REGISTER_OBJECTS = 1, //!< REGISTER_OBJECTS
//    REFRESH_VALUE = 2,    //!< REFRESH_VALUE
//    REFRESH_PROPERTIES = 3 //!< REFRESH_PROPERTIES
//} ControlPanelTransaction;
//
///**
// * Hints for Containers Widgets
// * determining the layout
// */
//enum LAYOUT_HINTS {
//    VERTICAL_LINEAR = 1, //!< VERTICAL_LINEAR
//    HORIZONTAL_LINEAR = 2 //!< HORIZONTAL_LINEAR
//};
//
///**
// * Hints for Dialog Widgets
// */
//enum DIALOG_HINTS {
//    ALERTDIALOG = 1 //!< ALERTDIALOG
//};
//
///**
// * Hints for Property Widgets
// */
//enum PROPERTY_HINTS {
//    SWITCH = 1,      //!< SWITCH
//    CHECKBOX = 2,    //!< CHECKBOX
//    SPINNER = 3,     //!< SPINNER
//    RADIOBUTTON = 4, //!< RADIOBUTTON
//    SLIDER = 5,      //!< SLIDER
//    TIMEPICKER = 6,  //!< TIMEPICKER
//    DATEPICKER = 7,  //!< DATEPICKER
//    NUMBERPICKER = 8, //!< NUMBERPICKER
//    KEYPAD = 9,      //!< KEYPAD
//    ROTARYKNOB = 10, //!< ROTARYKNOB
//    TEXTVIEW = 11,   //!< TEXTVIEW
//    NUMERICVIEW = 12, //!< NUMERICVIEW
//    EDITTEXT = 13    //!< EDITTEXT
//};
//
///**
// * Hints for Label Widgets
// */
//enum LABEL_HINTS {
//    TEXTLABEL = 1 //!< TEXTLABEL
//};
//
///**
// * Hints for ListProperty Widgets
// */
//enum LIST_PROPERTY_HINTS {
//    DYNAMICSPINNER = 1 //!< DYNAMICSPINNER
//};
//
///**
// * Hints for Action Widgets
// */
//enum ACTION_HINTS {
//    ACTIONBUTTON = 1 //!< ACTIONBUTTON
//};
//
//} //namespace services
//} //namespace ajn
//
//#endif /* CONTROLPANELENUMS_H_ */
