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

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <alljoyn/controlpanel/Widget.h>
#include <alljoyn/controlpanel/ConstraintList.h>
#include <alljoyn/controlpanel/ConstraintRange.h>
#include <alljoyn/controlpanel/CPSDate.h>
#include <alljoyn/controlpanel/CPSTime.h>
#include <map>

namespace ajn {
namespace services {

/**
 * Union that allows the definition of a function pointer
 * to get the Properties value
 */
typedef union {
    /**
     * Get Value for type uint16t
     * @return value of Property
     */
    uint16_t (*getUint16Value)();
    /**
     * Get Value for type int16_t
     * @return value of Property
     */
    int16_t (*getInt16Value)();
    /**
     * Get Value for type uint32_t
     * @return value of Property
     */
    uint32_t (*getUint32Value)();
    /**
     * Get Value for type int32_t
     * @return value of Property
     */
    int32_t (*getInt32Value)();
    /**
     * Get Value for type uint64_t
     * @return value of Property
     */
    uint64_t (*getUint64Value)();
    /**
     * Get Value for type int64_t
     * @return value of Property
     */
    int64_t (*getInt64Value)();
    /**
     * Get Value for type double
     * @return value of Property
     */
    double (*getDoubleValue)();
    /**
     * Get Value for type const char*
     * @return value of Property
     */
    const char* (*getCharValue)();
    /**
     * Get Value for type bool
     * @return value of Property
     */
    bool (*getBoolValue)();
    /**
     * Get Value for type CPSDate
     * @return value of Property
     */
    const CPSDate& (*getDateValue)();
    /**
     * Get Value for type CPSTime
     * @return value of Property
     */
    const CPSTime& (*getTimeValue)();
} PropertyValueFptr;

/**
 * Union that allows the definition of a function pointer
 * to get the Properties value
 */
typedef union {
    /**
     * Get Value for type uint16t
     * @return value of Property
     */
    uint16_t uint16Value;
    /**
     * Get Value for type int16_t
     * @return value of Property
     */
    int16_t int16Value;
    /**
     * Get Value for type uint32_t
     * @return value of Property
     */
    uint32_t uint32Value;
    /**
     * Get Value for type int32_t
     * @return value of Property
     */
    int32_t int32Value;
    /**
     * Get Value for type uint64_t
     * @return value of Property
     */
    uint64_t uint64Value;
    /**
     * Get Value for type int64_t
     * @return value of Property
     */
    int64_t int64Value;
    /**
     * Get Value for type double
     * @return value of Property
     */
    double doubleValue;
    /**
     * Get Value for type const char*
     * @return value of Property
     */
    const char* charValue;

    /**
     * Get Value for type bool
     * @return value of Property
     */
    bool boolValue;
    /**
     * Get Value for type CPSDate
     * @return value of Property
     */
    CPSDate* dateValue;
    /**
     * Get Value for type CPSTime
     * @return value of Property
     */
    CPSTime* timeValue;
} PropertyValue;

/**
 * Property Class. Used to display a property Widget.
 * This is an abstract class. The setValue function should be overwritten
 * in the implementation to allow setting of the property.
 */
class Property : public Widget {
  public:

    /**
     * Constructor for Property Class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param propertyType - type of Property Value
     */
    Property(qcc::String name, Widget* rootWidget, PropertyType propertyType);

    /**
     * Constructor for Property Class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - the device that contains this Property
     */
    Property(qcc::String name, Widget* rootWidget, ControlPanelDevice* device);


    /**
     * Destructor of Property class
     */
    virtual ~Property();

    /**
     * creates and returns the appropriate BusObject for this Widget
     * @param bus - the bus used to create the widget
     * @param objectPath - the objectPath of the widget
     * @param langIndx - the language Indx
     * @param status - the status indicating success or failure
     * @return a newly created WidgetBusObject
     */
    WidgetBusObject* createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                           uint16_t langIndx, QStatus& status);

    /**
     * Get the PropertyType
     * @return PropertyType
     */
    PropertyType getPropertyType() const;

    /**
     * Get the PropertyValue of the property
     * @return the value of the property
     */
    PropertyValue getPropertyValue() const;

    /**
     * Get the Unit of Measure
     * @return Unit of Measure Values
     */
    const qcc::String& getUnitOfMeasure() const;

    /**
     * Get the Unit of Measure vector
     * @return vector of the Unit of Measure Values
     */
    const std::vector<qcc::String>& getUnitOfMeasures() const;

    /**
     * Set the Unit of Measure vector
     * @param unitOfMeasures - vector of Unit of Measure values
     */
    void setUnitOfMeasures(const std::vector<qcc::String>& unitOfMeasures);

    /**
     * Get the GetUnitOfMeasure Function pointer
     * @return unitOfMeasure Function pointer
     */
    GetStringFptr getGetUnitOfMeasures() const;

    /**
     * Set the GetUnitOfMeasure Function pointer
     * @param getUnitOfMeasures - the GetUnitOfMeasure function Pointer
     */
    void setGetUnitOfMeasures(GetStringFptr getUnitOfMeasures);

    /**
     * Get the Constraint List vector
     * @return the Constraint List vector
     */
    const std::vector<ConstraintList>& getConstraintList() const;

    /**
     * Set the Constraint List vector
     * @param constraintList - constraintList vector
     */
    void setConstraintList(const std::vector<ConstraintList>& constraintList);

    /**
     * Get the Constraint Range
     * @return the Constraint Range
     */
    ConstraintRange* getConstraintRange() const;

    /**
     * Set the Constraint Range
     * @param constraintRange
     */
    void setConstraintRange(ConstraintRange* constraintRange);

    /**
     * Set the GetValue Function Pointer
     * @param getUint16Value - functionPointer to get a uint16 Value
     * @return status - success/failure
     */
    QStatus setGetValue(uint16_t (*getUint16Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getInt16Value - functionPointer to get a int16 Value
     * @return status - success/failure
     */
    QStatus setGetValue(int16_t (*getInt16Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getUint32Value - functionPointer to get a uint32 Value
     * @return status - success/failure
     */
    QStatus setGetValue(uint32_t (*getUint32Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getInt32Value - functionPointer to get a int32 Value
     * @return status - success/failure
     */
    QStatus setGetValue(int32_t (*getInt32Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getUint64Value - functionPointer to get a uint64 Value
     * @return status - success/failure
     */
    QStatus setGetValue(uint64_t (*getUint64Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getInt64Value - functionPointer to get a int64 Value
     * @return status - success/failure
     */
    QStatus setGetValue(int64_t (*getInt64Value)());

    /**
     * Set the GetValue Function Pointer
     * @param getDoubleValue - functionPointer to get a double Value
     * @return status - success/failure
     */
    QStatus setGetValue(double (*getDoubleValue)());

    /**
     * Set the GetValue Function Pointer
     * @param getCharValue - functionPointer to get a Char* Value
     * @return status - success/failure
     */
    QStatus setGetValue(const char* (*getCharValue)());

    /**
     * Set the GetValue Function Pointer
     * @param getBoolValue - functionPointer to get a bool Value
     * @return status - success/failure
     */
    QStatus setGetValue(bool (*getBoolValue)());

    /**
     * Set the GetValue Function Pointer
     * @param getDateValue - functionPointer to get a date Value
     * @return status - success/failure
     */
    QStatus setGetValue(const CPSDate& (*getDateValue)());

    /**
     * Set the GetValue Function Pointer
     * @param getTimeValue - functionPointer to get a time Value
     * @return status - success/failure
     */
    QStatus setGetValue(const CPSTime& (*getTimeValue)());

    /**
     * Fill the MsgArg passed in with the Property Value
     * @param val - msgArg to fill
     * @param languageIndx - language requested
     * @return status - success/failure
     */
    QStatus fillPropertyValueArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Fill the MsgArg passed in with the OptParams
     * @param val - msgArg to fill
     * @param languageIndx - language requested
     * @return status - success/failure
     */
    QStatus fillOptParamsArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Read the MsgArg passed in to fill the Value property
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    QStatus readValueArg(const MsgArg* val);

    /**
     * Set the Property Value
     * @param val - msgArg that includes the new Value
     * @param languageIndx - languageIndx setValue was called for
     * @return status - success/failure
     */
    QStatus setPropertyValue(MsgArg& val, uint16_t languageIndx);

    /**
     * Send a signal that the Property's Value has changed
     * @return status - success/failure
     */
    QStatus SendValueChangedSignal();

    /**
     * Callback when value is changed
     * @param msg
     */
    void ValueChanged(Message& msg);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(bool value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(uint16_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(int16_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(uint32_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(int32_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(uint64_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(int64_t value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(double value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(const char* value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(const CPSDate& value);

    /**
     * Set the new Value
     * @param value - new Value to be set to
     * @return status - success/failure
     */
    virtual QStatus setValue(const CPSTime& value);

  private:

    /**
     * The Property Type of the Property
     */
    PropertyType m_PropertyType;

    /**
     * The Value of the Property
     */
    PropertyValue m_Value;

    /**
     * The string placeholder for the value if it is a string propertyType
     */
    qcc::String m_ValueString;

    /**
     * The GetValue functionPointers of the Property
     */
    PropertyValueFptr m_GetValue;

    /**
     * The Unit of Measure of the Property
     */
    qcc::String m_UnitOfMeasure;

    /**
     * The Unit of Measure of the Property
     */
    std::vector<qcc::String> m_UnitOfMeasures;

    /**
     * The GetUnitOfMeasure function Pointer of the Property
     */
    GetStringFptr m_GetUnitOfMeasures;

    /**
     * The ConstraintList of the Property
     */
    std::vector<ConstraintList> m_ConstraintList;

    /**
     * The ConstraintRange of the Property
     */
    ConstraintRange* m_ConstraintRange;

    /**
     * Read the MsgArg passed in and fill the OptParam
     * @param key - the optParam to fill
     * @param val - the MsgArg passed in
     * @return status - success/failure
     */
    QStatus readOptParamsArg(uint16_t key, MsgArg* val);

    /**
     * Validate that the GetValue passed in fits the Property's property Type
     * @param propertyType - the property Type passed in
     * @return true/false
     */
    bool validateGetValue(PropertyType propertyType);

    /**
     * Validate that the GetValue passed in fits the Property's property Type
     * @param propertyType - the property Type passed in
     * @return status - success/failure
     */
    QStatus validateValue(PropertyType propertyType);

    /**
     * Error Message if the correct SetValue function
     * is not defined in the implementation
     * @return status - success/failure
     */
    QStatus defaultErrorSetValue();

};
} //namespace services
} //namespace ajn

#endif /* PROPERTY_H_ */
