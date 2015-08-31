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

#ifndef CONSTRAINTLIST_H_
#define CONSTRAINTLIST_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/controlpanel/Widget.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * ConstraintValue - a union combining all possible values
 * that can make up a constraint
 */
typedef union {
    /**
     * Value for uint16_t
     */
    uint16_t uint16Value;
    /**
     * Value for int16_t
     */
    int16_t int16Value;
    /**
     * Value for uint32_t
     */
    uint32_t uint32Value;
    /**
     * Value for int32_t
     */
    int32_t int32Value;
    /**
     * Value for uint64_t
     */
    uint64_t uint64Value;
    /**
     * Value for int64_t
     */
    int64_t int64Value;
    /**
     * Value for double
     */
    double doubleValue;
    /**
     * Value for const char*
     */
    const char* charValue;
} ConstraintValue;

/**
 * Constraint List class to define a list of
 * Values and constrain a property to those values
 * The Constraint is applied on the controller side.
 * No validations are done in the Controllee
 */
class ConstraintList {
  public:

    /**
     * Constructor for ConstraintList
     */
    ConstraintList();

    /**
     * Destructor for ConstraintList
     */
    virtual ~ConstraintList();

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(uint16_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(int16_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(uint32_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(int32_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(uint64_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(int64_t value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(double value);

    /**
     * Set the value for the Constraint
     * @param value - value to be Constrained to
     */
    void setConstraintValue(qcc::String const& value);

    /**
     * Get the Constraint Value
     * @return the Constraint Value
     */
    ConstraintValue getConstraintValue() const;

    /**
     * Get the Property Type of the Constraint
     * @return propertyType of the Constraint
     */
    PropertyType getPropertyType() const;

    /**
     * Get the Display
     * @return Display Value
     */
    const qcc::String& getDisplay() const;

    /**
     * Get the Display vector
     * @return vector of the Display Values
     */
    const std::vector<qcc::String>& getDisplays() const;

    /**
     * Set the Display vector
     * @param displays - vector of the display values
     */
    void setDisplays(const std::vector<qcc::String>& displays);

    /**
     * Get the function pointer to get the display values
     * @return the GetDisplay function pointer
     */
    GetStringFptr getGetDisplays() const;

    /**
     * Set the GetDisplay function pointer
     * @param getDisplays - the function pointer to get the display values
     */
    void setGetDisplays(GetStringFptr getDisplays);

    /**
     * fill the MsgArg passed in with the Constraint
     * @param val - msgArg to fill
     * @param languageIndx - the languageIndx for the Display value
     * @param propertyType - the property Type expected
     * @return status - success/failure
     */
    QStatus fillConstraintArg(MsgArg& val, uint16_t languageIndx, PropertyType propertyType);

    /**
     * Read the MsgArg passed in to fill the Constraint
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    QStatus readConstraintArg(MsgArg& val);

  private:

    /**
     * Property Type of Constraint
     */
    PropertyType m_PropertyType;

    /**
     * The Constraint Value of the Constraint
     */
    ConstraintValue m_ConstraintValue;

    /**
     * The Display value of the Constraint
     */
    qcc::String m_Display;

    /**
     * The Display value of the Constraint
     */
    std::vector<qcc::String> m_Displays;

    /**
     * The GetDisplay function pointer of the Constraint
     */
    GetStringFptr m_GetDisplays;

    /**
     * A local string to store the Constraint if the property Type is a string
     */
    qcc::String m_ConstraintValueString;
};

} /* namespace services */
} /* namespace ajn */

#endif /* CONSTRAINTLIST_H_ */
