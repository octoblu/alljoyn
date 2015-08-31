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

#include <alljoyn/controlpanel/ConstraintRange.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ConstraintRange::ConstraintRange() : m_PropertyType(UNDEFINED)
{
    m_MinValue.uint16Value = 0;
    m_MaxValue.uint16Value = 0;
    m_IncrementValue.uint16Value = 0;
}

ConstraintRange::~ConstraintRange()
{
}

const ConstraintRangeVal& ConstraintRange::getIncrementValue() const
{
    return m_IncrementValue;
}

const ConstraintRangeVal& ConstraintRange::getMaxValue() const
{
    return m_MaxValue;
}

const ConstraintRangeVal& ConstraintRange::getMinValue() const
{
    return m_MinValue;
}

bool ConstraintRange::validateConstraintValue(PropertyType propertyType)
{
    if (m_PropertyType != propertyType && m_PropertyType != UNDEFINED) {
        QCC_DbgHLPrintf(("Could not set Constraint Value. Value Type is wrong"));
        return false;
    }
    m_PropertyType = propertyType;
    return true;
}

QStatus ConstraintRange::setConstraintMin(uint16_t minValue)
{
    if (!validateConstraintValue(UINT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.uint16Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(int16_t minValue)
{
    if (!validateConstraintValue(INT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.int16Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(uint32_t minValue)
{
    if (!validateConstraintValue(UINT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.uint32Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(int32_t minValue)
{
    if (!validateConstraintValue(INT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.int32Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(uint64_t minValue)
{
    if (!validateConstraintValue(UINT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.uint64Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(int64_t minValue)
{
    if (!validateConstraintValue(INT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.int64Value = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMin(double minValue)
{
    if (!validateConstraintValue(DOUBLE_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MinValue.doubleValue = minValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(uint16_t maxValue)
{
    if (!validateConstraintValue(UINT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.uint16Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(int16_t maxValue)
{
    if (!validateConstraintValue(INT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.int16Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(uint32_t maxValue)
{
    if (!validateConstraintValue(UINT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.uint32Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(int32_t maxValue)
{
    if (!validateConstraintValue(INT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.int32Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(uint64_t maxValue)
{
    if (!validateConstraintValue(UINT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.uint64Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(int64_t maxValue)
{
    if (!validateConstraintValue(INT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.int64Value = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintMax(double maxValue)
{
    if (!validateConstraintValue(DOUBLE_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_MaxValue.doubleValue = maxValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(uint16_t incrementValue)
{
    if (!validateConstraintValue(UINT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.uint16Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(int16_t incrementValue)
{
    if (!validateConstraintValue(INT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.int16Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(uint32_t incrementValue)
{
    if (!validateConstraintValue(UINT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.uint32Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(int32_t incrementValue)
{
    if (!validateConstraintValue(INT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.int32Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(uint64_t incrementValue)
{
    if (!validateConstraintValue(UINT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.uint64Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(int64_t incrementValue)
{
    if (!validateConstraintValue(INT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.int64Value = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::setConstraintIncrement(double incrementValue)
{
    if (!validateConstraintValue(DOUBLE_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_IncrementValue.doubleValue = incrementValue;
    return ER_OK;
}

QStatus ConstraintRange::fillConstraintArg(MsgArg& val, uint16_t languageIndx, PropertyType propertyType)
{
    if (m_PropertyType != propertyType) {
        return ER_BUS_SIGNATURE_MISMATCH;
    }

    QStatus status;

    MsgArg* minValue = new MsgArg();
    MsgArg* maxValue = new MsgArg();
    MsgArg* incrementValue = new MsgArg();

    switch (m_PropertyType) {
    case UINT16_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_UINT16.c_str(), m_MinValue.uint16Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_UINT16.c_str(), m_MaxValue.uint16Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_UINT16.c_str(), m_IncrementValue.uint16Value));
        break;

    case INT16_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_INT16.c_str(), m_MinValue.int16Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_INT16.c_str(), m_MaxValue.int16Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_INT16.c_str(), m_IncrementValue.int16Value));
        break;

    case UINT32_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_UINT32.c_str(), m_MinValue.uint32Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_UINT32.c_str(), m_MaxValue.uint32Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_UINT32.c_str(), m_IncrementValue.uint32Value));
        break;

    case INT32_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_INT32.c_str(), m_MinValue.int32Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_INT32.c_str(), m_MaxValue.int32Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_INT32.c_str(), m_IncrementValue.int32Value));
        break;

    case UINT64_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_UINT64.c_str(), m_MinValue.uint64Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_UINT64.c_str(), m_MaxValue.uint64Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_UINT64.c_str(), m_IncrementValue.uint64Value));
        break;

    case INT64_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_INT64.c_str(), m_MinValue.int64Value));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_INT64.c_str(), m_MaxValue.int64Value));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_INT64.c_str(), m_IncrementValue.int64Value));
        break;

    case DOUBLE_PROPERTY:
        CHECK_AND_BREAK(minValue->Set(AJPARAM_DOUBLE.c_str(), m_MinValue.doubleValue));
        CHECK_AND_BREAK(maxValue->Set(AJPARAM_DOUBLE.c_str(), m_MaxValue.doubleValue));
        CHECK_AND_BREAK(incrementValue->Set(AJPARAM_DOUBLE.c_str(), m_IncrementValue.doubleValue));
        break;

    default:
        status = ER_BUS_BAD_SIGNATURE;
        break;
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Constraint Range"));
        delete minValue;
        delete maxValue;
        delete incrementValue;
        return status;
    }

    status = val.Set(AJPARAM_STRUCT_VAR_VAR_VAR.c_str(), minValue, maxValue, incrementValue);

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Constraint Range"));
        delete minValue;
        delete maxValue;
        delete incrementValue;
        return status;
    }

    val.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return status;
}

QStatus ConstraintRange::readConstraintArg(MsgArg* val)
{
    QStatus status = ER_OK;
    MsgArg* minValue;
    MsgArg* maxValue;
    MsgArg* incrementValue;
    CHECK_AND_RETURN(val->Get(AJPARAM_STRUCT_VAR_VAR_VAR.c_str(), &minValue, &maxValue, &incrementValue))

    if (minValue->typeId != maxValue->typeId || minValue->typeId != incrementValue->typeId) {
        return ER_BUS_SIGNATURE_MISMATCH;
    }

    switch (minValue->typeId) {

    case ALLJOYN_UINT16:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_UINT16.c_str(), &m_MinValue.uint16Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_UINT16.c_str(), &m_MaxValue.uint16Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_UINT16.c_str(), &m_IncrementValue.uint16Value))
            m_PropertyType = UINT16_PROPERTY;
            break;
        }

    case ALLJOYN_INT16:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_INT16.c_str(), &m_MinValue.int16Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_INT16.c_str(), &m_MaxValue.int16Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_INT16.c_str(), &m_IncrementValue.int16Value))
            m_PropertyType = INT16_PROPERTY;
            break;
        }

    case ALLJOYN_UINT32:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_UINT32.c_str(), &m_MinValue.uint32Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_UINT32.c_str(), &m_MaxValue.uint32Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_UINT32.c_str(), &m_IncrementValue.uint32Value))
            m_PropertyType = UINT32_PROPERTY;
            break;
        }

    case ALLJOYN_INT32:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_INT32.c_str(), &m_MinValue.int32Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_INT32.c_str(), &m_MaxValue.int32Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_INT32.c_str(), &m_IncrementValue.int32Value))
            m_PropertyType = INT32_PROPERTY;
            break;
        }

    case ALLJOYN_UINT64:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_UINT64.c_str(), &m_MinValue.uint64Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_UINT64.c_str(), &m_MaxValue.uint64Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_UINT64.c_str(), &m_IncrementValue.uint64Value))
            m_PropertyType = UINT64_PROPERTY;
            break;
        }

    case ALLJOYN_INT64:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_INT64.c_str(), &m_MinValue.int64Value))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_INT64.c_str(), &m_MaxValue.int64Value))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_INT64.c_str(), &m_IncrementValue.int64Value))
            m_PropertyType = INT64_PROPERTY;
            break;
        }

    case ALLJOYN_DOUBLE:
        {
            CHECK_AND_RETURN(minValue->Get(AJPARAM_DOUBLE.c_str(), &m_MinValue.doubleValue))
            CHECK_AND_RETURN(maxValue->Get(AJPARAM_DOUBLE.c_str(), &m_MaxValue.doubleValue))
            CHECK_AND_RETURN(incrementValue->Get(AJPARAM_DOUBLE.c_str(), &m_IncrementValue.doubleValue))
            m_PropertyType = DOUBLE_PROPERTY;
            break;
        }

    default:
        status = ER_BUS_SIGNATURE_MISMATCH;
        break;
    }

    return status;
}

} /* namespace services */
} /* namespace ajn */
