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

#include <alljoyn/controlpanel/ConstraintList.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ConstraintList::ConstraintList() : m_PropertyType(UNDEFINED), m_Display(""), m_GetDisplays(0)
{
}

ConstraintList::~ConstraintList()
{
}

void ConstraintList::setConstraintValue(uint16_t value)
{
    m_ConstraintValue.uint16Value = value;
    m_PropertyType = UINT16_PROPERTY;
}

void ConstraintList::setConstraintValue(int16_t value)
{
    m_ConstraintValue.int16Value = value;
    m_PropertyType = INT16_PROPERTY;
}

void ConstraintList::setConstraintValue(uint32_t value)
{
    m_ConstraintValue.uint32Value = value;
    m_PropertyType = UINT32_PROPERTY;
}

void ConstraintList::setConstraintValue(int32_t value)
{
    m_ConstraintValue.int32Value = value;
    m_PropertyType = INT32_PROPERTY;
}

void ConstraintList::setConstraintValue(uint64_t value)
{
    m_ConstraintValue.uint64Value = value;
    m_PropertyType = UINT64_PROPERTY;
}

void ConstraintList::setConstraintValue(int64_t value)
{
    m_ConstraintValue.int64Value = value;
    m_PropertyType = INT64_PROPERTY;
}

void ConstraintList::setConstraintValue(double value)
{
    m_ConstraintValue.doubleValue = value;
    m_PropertyType = DOUBLE_PROPERTY;
}

void ConstraintList::setConstraintValue(qcc::String const& value)
{
    m_ConstraintValueString = value;
    m_ConstraintValue.charValue = m_ConstraintValueString.c_str();
    m_PropertyType = STRING_PROPERTY;
}

ConstraintValue ConstraintList::getConstraintValue() const
{
    return m_ConstraintValue;
}

PropertyType ConstraintList::getPropertyType() const
{
    return m_PropertyType;
}

const qcc::String& ConstraintList::getDisplay() const
{
    return m_Display;
}

const std::vector<qcc::String>& ConstraintList::getDisplays() const
{
    return m_Displays;
}

void ConstraintList::setDisplays(const std::vector<qcc::String>& displays)
{
    m_Displays = displays;
}

GetStringFptr ConstraintList::getGetDisplays() const
{
    return m_GetDisplays;
}

void ConstraintList::setGetDisplays(GetStringFptr getDisplays)
{
    m_GetDisplays = getDisplays;
}

QStatus ConstraintList::fillConstraintArg(MsgArg& val, uint16_t languageIndx, PropertyType propertyType)
{
    if (m_PropertyType != propertyType) {
        QCC_DbgHLPrintf(("Could not fill the Constraint Arg. PropertyTypes do not match"));
        return ER_FAIL;
    }

    if (!(m_Displays.size() > languageIndx) && !m_GetDisplays) {
        QCC_DbgHLPrintf(("Could not fill the Constraint Arg. Display is not set"));
        return ER_FAIL;
    }

    QStatus status;
    MsgArg* valueArg = new MsgArg();

    switch (propertyType) {
    case UINT16_PROPERTY:
        status = valueArg->Set(AJPARAM_UINT16.c_str(), m_ConstraintValue.uint16Value);
        break;

    case INT16_PROPERTY:
        status = valueArg->Set(AJPARAM_INT16.c_str(), m_ConstraintValue.int16Value);
        break;

    case UINT32_PROPERTY:
        status = valueArg->Set(AJPARAM_UINT32.c_str(), m_ConstraintValue.uint32Value);
        break;

    case INT32_PROPERTY:
        status = valueArg->Set(AJPARAM_INT32.c_str(), m_ConstraintValue.int32Value);
        break;

    case UINT64_PROPERTY:
        status = valueArg->Set(AJPARAM_UINT64.c_str(), m_ConstraintValue.uint64Value);
        break;

    case INT64_PROPERTY:
        status = valueArg->Set(AJPARAM_INT64.c_str(), m_ConstraintValue.int64Value);
        break;

    case DOUBLE_PROPERTY:
        status = valueArg->Set(AJPARAM_DOUBLE.c_str(), m_ConstraintValue.doubleValue);
        break;

    case STRING_PROPERTY:
        status = valueArg->Set(AJPARAM_STR.c_str(), m_ConstraintValue.charValue);
        break;

    default:
        status = ER_BUS_BAD_SIGNATURE;
        break;
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Constraint Value"));
        delete valueArg;
        return status;
    }

    const char* display = m_GetDisplays ? m_GetDisplays(languageIndx) : m_Displays[languageIndx].c_str();
    status = val.Set(AJPARAM_STRUCT_VAR_STR.c_str(), valueArg, display);

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Constraint Value"));
        delete valueArg;
        return status;
    }
    val.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return status;
}

QStatus ConstraintList::readConstraintArg(MsgArg& val)
{
    QStatus status = ER_OK;
    MsgArg* valueArg;
    char* display;
    CHECK_AND_RETURN(val.Get(AJPARAM_STRUCT_VAR_STR.c_str(), &valueArg, &display))
    m_Display = display;

    switch (valueArg->typeId) {

    case ALLJOYN_UINT16:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_UINT16.c_str(), &m_ConstraintValue.uint16Value))
            m_PropertyType = UINT16_PROPERTY;
            break;
        }

    case ALLJOYN_INT16:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_INT16.c_str(), &m_ConstraintValue.int16Value))
            m_PropertyType = INT16_PROPERTY;
            break;
        }

    case ALLJOYN_UINT32:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_UINT32.c_str(), &m_ConstraintValue.uint32Value))
            m_PropertyType = UINT32_PROPERTY;
            break;
        }

    case ALLJOYN_INT32:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_INT32.c_str(), &m_ConstraintValue.int32Value))
            m_PropertyType = INT32_PROPERTY;
            break;
        }

    case ALLJOYN_UINT64:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_UINT64.c_str(), &m_ConstraintValue.uint64Value))
            m_PropertyType = UINT64_PROPERTY;
            break;
        }

    case ALLJOYN_INT64:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_INT64.c_str(), &m_ConstraintValue.int64Value))
            m_PropertyType = INT64_PROPERTY;
            break;
        }

    case ALLJOYN_DOUBLE:
        {
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_DOUBLE.c_str(), &m_ConstraintValue.doubleValue))
            m_PropertyType = DOUBLE_PROPERTY;
            break;
        }

    case ALLJOYN_STRING:
        {
            char* constraintValue;
            CHECK_AND_RETURN(valueArg->Get(AJPARAM_STR.c_str(), &constraintValue))
            m_ConstraintValueString = constraintValue;
            m_ConstraintValue.charValue = m_ConstraintValueString.c_str();
            m_PropertyType = STRING_PROPERTY;
            break;
        }

    default:
        status = ER_BUS_SIGNATURE_MISMATCH;
        break;
    }

    return ER_OK;
}

} /* namespace services */
} /* namespace ajn */
