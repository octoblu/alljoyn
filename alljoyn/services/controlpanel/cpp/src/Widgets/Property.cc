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

#include <alljoyn/controlpanel/Property.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../ControlPanelConstants.h"
#include "../BusObjects/PropertyBusObject.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

Property::Property(qcc::String name, Widget* rootWidget, PropertyType propertyType) :
    Widget(name, rootWidget, WIDGET_TYPE_PROPERTY), m_PropertyType(propertyType), m_ValueString(""),
    m_UnitOfMeasure(""), m_GetUnitOfMeasures(0), m_ConstraintRange(0)
{
    m_GetValue.getBoolValue = 0; // initialize to null
    m_Value.dateValue = 0; // initialize to null
}

Property::Property(qcc::String name, Widget* rootWidget, ControlPanelDevice* device) :
    Widget(name, rootWidget, device, WIDGET_TYPE_PROPERTY), m_PropertyType(UNDEFINED), m_ValueString(""),
    m_UnitOfMeasure(""), m_GetUnitOfMeasures(0), m_ConstraintRange(0)
{
    m_GetValue.getBoolValue = 0; // initialize to null
    m_Value.dateValue = 0; // initialize to null
}

Property::~Property()
{
    if (m_ConstraintRange) {
        delete m_ConstraintRange;
    }

    if (m_ControlPanelMode == CONTROLLER_MODE && m_PropertyType == DATE_PROPERTY && m_Value.dateValue != 0) {
        delete m_Value.dateValue;
    } else if (m_ControlPanelMode == CONTROLLER_MODE && m_PropertyType == TIME_PROPERTY && m_Value.timeValue != 0) {
        delete m_Value.timeValue;
    }
}

WidgetBusObject* Property::createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                                 uint16_t langIndx, QStatus& status)
{
    return new PropertyBusObject(bus, objectPath, langIndx, status, this);
}

PropertyType Property::getPropertyType() const
{
    return m_PropertyType;
}

PropertyValue Property::getPropertyValue() const
{
    return m_Value;
}

const qcc::String& Property::getUnitOfMeasure() const
{
    return m_UnitOfMeasure;
}

const std::vector<qcc::String>& Property::getUnitOfMeasures() const
{
    return m_UnitOfMeasures;
}

void Property::setUnitOfMeasures(const std::vector<qcc::String>& unitOfMeasures)
{
    m_UnitOfMeasures = unitOfMeasures;
}

GetStringFptr Property::getGetUnitOfMeasures() const
{
    return m_GetUnitOfMeasures;
}

void Property::setGetUnitOfMeasures(GetStringFptr getUnitOfMeasures)
{
    m_GetUnitOfMeasures = getUnitOfMeasures;
}

const std::vector<ConstraintList>& Property::getConstraintList() const
{
    return m_ConstraintList;
}

void Property::setConstraintList(const std::vector<ConstraintList>& constraintList)
{
    m_ConstraintList = constraintList;
}

ConstraintRange* Property::getConstraintRange() const
{
    return m_ConstraintRange;
}

void Property::setConstraintRange(ConstraintRange* constraintRange)
{
    m_ConstraintRange = constraintRange;
}

bool Property::validateGetValue(PropertyType propertyType)
{
    if (m_PropertyType != propertyType) {
        QCC_DbgHLPrintf(("Could not set GetValue. Value Type is wrong."));
        return false;
    }
    return true;
}

QStatus Property::validateValue(PropertyType propertyType)
{
    if (m_PropertyType != UNDEFINED && m_PropertyType != propertyType) {
        QCC_DbgHLPrintf(("Could not set Value. Value Type is wrong."));
        return ER_BUS_SIGNATURE_MISMATCH;
    }
    m_PropertyType = propertyType;
    return ER_OK;
}

QStatus Property::setGetValue(uint16_t (*getUint16Value)())
{
    if (!validateGetValue(UINT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getUint16Value = getUint16Value;
    return ER_OK;
}

QStatus Property::setGetValue(int16_t (*getInt16Value)())
{
    if (!validateGetValue(INT16_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getInt16Value = getInt16Value;
    return ER_OK;
}

QStatus Property::setGetValue(uint32_t (*getUint32Value)())
{
    if (!validateGetValue(UINT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getUint32Value = getUint32Value;
    return ER_OK;
}

QStatus Property::setGetValue(int32_t (*getInt32Value)())
{
    if (!validateGetValue(INT32_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getInt32Value = getInt32Value;
    return ER_OK;
}

QStatus Property::setGetValue(uint64_t (*getUint64Value)())
{
    if (!validateGetValue(UINT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getUint64Value = getUint64Value;
    return ER_OK;
}

QStatus Property::setGetValue(int64_t (*getInt64Value)())
{
    if (!validateGetValue(INT64_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getInt64Value = getInt64Value;
    return ER_OK;
}

QStatus Property::setGetValue(double (*getDoubleValue)())
{
    if (!validateGetValue(DOUBLE_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getDoubleValue = getDoubleValue;
    return ER_OK;
}

QStatus Property::setGetValue(const char* (*getCharValue)())
{
    if (!validateGetValue(STRING_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getCharValue = getCharValue;
    return ER_OK;
}

QStatus Property::setGetValue(bool (*getBoolValue)())
{
    if (!validateGetValue(BOOL_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getBoolValue = getBoolValue;
    return ER_OK;
}

QStatus Property::setGetValue(const CPSDate& (*getDateValue)())
{
    if (!validateGetValue(DATE_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getDateValue = getDateValue;
    return ER_OK;
}

QStatus Property::setGetValue(const CPSTime& (*getTimeValue)())
{
    if (!validateGetValue(TIME_PROPERTY)) {
        return ER_BUS_SET_WRONG_SIGNATURE;
    }

    m_GetValue.getTimeValue = getTimeValue;
    return ER_OK;
}

QStatus Property::fillPropertyValueArg(MsgArg& val, uint16_t languageIndx)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;

    if (!m_GetValue.getBoolValue) {
        QCC_DbgHLPrintf(("GetValue is not defined"));
        return status;
    }

    MsgArg* variantArg = new MsgArg();
    switch (m_PropertyType) {
    case UINT16_PROPERTY:
        status = variantArg->Set(AJPARAM_UINT16.c_str(), m_GetValue.getUint16Value());
        break;

    case INT16_PROPERTY:
        status = variantArg->Set(AJPARAM_INT16.c_str(), m_GetValue.getInt16Value());
        break;

    case UINT32_PROPERTY:
        status = variantArg->Set(AJPARAM_UINT32.c_str(), m_GetValue.getUint32Value());
        break;

    case INT32_PROPERTY:
        status = variantArg->Set(AJPARAM_INT32.c_str(), m_GetValue.getInt32Value());
        break;

    case UINT64_PROPERTY:
        status = variantArg->Set(AJPARAM_UINT64.c_str(), m_GetValue.getUint64Value());
        break;

    case INT64_PROPERTY:
        status = variantArg->Set(AJPARAM_INT64.c_str(), m_GetValue.getInt64Value());
        break;

    case DOUBLE_PROPERTY:
        status = variantArg->Set(AJPARAM_DOUBLE.c_str(), m_GetValue.getDoubleValue());
        break;

    case STRING_PROPERTY:
        status = variantArg->Set(AJPARAM_STR.c_str(), m_GetValue.getCharValue());
        break;

    case BOOL_PROPERTY:
        status = variantArg->Set(AJPARAM_BOOL.c_str(), m_GetValue.getBoolValue());
        break;

    case DATE_PROPERTY: {
            CPSDate date = m_GetValue.getDateValue();
            status = variantArg->Set(AJPARAM_DATE_OR_TIME.c_str(), DATE_PROPERTY_TYPE,
                                     date.getDay(), date.getMonth(), date.getYear());
            break;
        }

    case TIME_PROPERTY: {
            CPSTime time = m_GetValue.getTimeValue();
            status = variantArg->Set(AJPARAM_DATE_OR_TIME.c_str(), TIME_PROPERTY_TYPE,
                                     time.getHour(), time.getMinute(), time.getSecond());
            break;
        }

    case UNDEFINED:
        QCC_DbgHLPrintf(("Property type is not defined"));
        break;
    }
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not marshal VariantArg"));
        delete variantArg;
        return status;
    }

    status = val.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not marshal ValueArg"));
        delete variantArg;
        return status;
    }
    val.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return status;
}

QStatus Property::fillOptParamsArg(MsgArg& val, uint16_t languageIndx)
{
    QStatus status;
    MsgArg* optParams = new MsgArg[NUM_OPT_PARAMS];

    size_t optParamIndx = 0;

    status = Widget::fillOptParamsArg(optParams, languageIndx, optParamIndx);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal optParams"));
        delete[] optParams;
        return status;
    }

    if (m_UnitOfMeasures.size() > languageIndx || m_GetUnitOfMeasures) {
        MsgArg* unitMeasureArg = new MsgArg(AJPARAM_STR.c_str(), m_GetUnitOfMeasures ?
                                            m_GetUnitOfMeasures(languageIndx) : m_UnitOfMeasures[languageIndx].c_str());

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  UNIT_MEASURE, unitMeasureArg)) != ER_OK) {
            delete unitMeasureArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    if (m_ConstraintList.size()) {

        MsgArg* constraintListArrayArg = new MsgArg[m_ConstraintList.size()];
        for (uint16_t indx = 0; indx < m_ConstraintList.size(); indx++) {
            status = m_ConstraintList[indx].fillConstraintArg(constraintListArrayArg[indx], languageIndx, m_PropertyType);
            if (status != ER_OK) {
                break;
            }
        }

        if (status != ER_OK) {
            delete[] constraintListArrayArg;
            delete[] optParams;
            return status;
        }

        MsgArg* constraintListArg = new MsgArg(AJPARAM_ARRAY_STRUCT_VAR_STR.c_str(),
                                               m_ConstraintList.size(), constraintListArrayArg);

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  CONSTRAINT_LIST, constraintListArg)) != ER_OK) {
            delete constraintListArg;
            delete[] constraintListArrayArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    if (m_ConstraintRange) {

        MsgArg* constraintRangeArg = new MsgArg();
        status = m_ConstraintRange->fillConstraintArg(*constraintRangeArg, languageIndx, m_PropertyType);

        if (status != ER_OK) {
            delete constraintRangeArg;
            delete[] optParams;
            return status;
        }

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  CONSTRAINT_RANGE, constraintRangeArg)) != ER_OK) {
            delete constraintRangeArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    return val.Set(AJPARAM_ARRAY_DICT_UINT16_VAR.c_str(), optParamIndx, optParams);
}

QStatus Property::setPropertyValue(MsgArg& val, uint16_t languageIndx)
{
    QStatus status;

    switch (m_PropertyType) {
    case UINT16_PROPERTY:
        {
            uint16_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_UINT16.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case INT16_PROPERTY:
        {
            int16_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_INT16.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case UINT32_PROPERTY:
        {
            uint32_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_UINT32.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case INT32_PROPERTY:
        {
            int32_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_INT32.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case UINT64_PROPERTY:
        {
            uint64_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_UINT64.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case INT64_PROPERTY:
        {
            int64_t value;
            CHECK_AND_RETURN(val.Get(AJPARAM_INT64.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case DOUBLE_PROPERTY:
        {
            double value;
            CHECK_AND_RETURN(val.Get(AJPARAM_DOUBLE.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case STRING_PROPERTY:
        {
            char* value;
            CHECK_AND_RETURN(val.Get(AJPARAM_STR.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case BOOL_PROPERTY:
        {
            bool value;
            CHECK_AND_RETURN(val.Get(AJPARAM_BOOL.c_str(), &value));
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case DATE_PROPERTY:
        {
            uint16_t type;
            uint16_t day;
            uint16_t month;
            uint16_t year;
            CHECK_AND_RETURN(val.Get(AJPARAM_DATE_OR_TIME.c_str(), &type, &day, &month, &year));

            if (type != DATE_PROPERTY_TYPE) {
                QCC_DbgHLPrintf(("Did not receive the DateProperty type as expected"));
                return ER_BUS_SIGNATURE_MISMATCH;
            }

            CPSDate value(day, month, year);
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    case TIME_PROPERTY:
        {
            uint16_t type;
            uint16_t hour;
            uint16_t minute;
            uint16_t second;
            CHECK_AND_RETURN(val.Get(AJPARAM_DATE_OR_TIME.c_str(), &type, &hour, &minute, &second));

            if (type != TIME_PROPERTY_TYPE) {
                QCC_DbgHLPrintf(("Did not receive the TimeProperty type as expected"));
                return ER_BUS_SIGNATURE_MISMATCH;
            }

            CPSTime value(hour, minute, second);
            CHECK_AND_RETURN(setValue(value));
            break;
        }

    default:
        status = ER_BUS_NO_SUCH_PROPERTY;
        break;
    }

    if (status == ER_OK) {
        QCC_DbgPrintf(("Set property succeeded - sending ValueChanged signal."));
        status = SendValueChangedSignal();
    }

    return status;
}

QStatus Property::readOptParamsArg(uint16_t key, MsgArg* val)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;
    switch (key) {
    case UNIT_MEASURE:
        {
            char* unitOfMeasure;
            CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &unitOfMeasure))
            m_UnitOfMeasure = unitOfMeasure;
            break;
        }

    case CONSTRAINT_LIST:
        {
            m_ConstraintList.clear();
            MsgArg* constraintEntries;
            size_t constraintNum;
            CHECK_AND_RETURN(val->Get(AJPARAM_ARRAY_STRUCT_VAR_STR.c_str(), &constraintNum, &constraintEntries));
            for (size_t i = 0; i < constraintNum; i++) {
                ConstraintList constraint;
                CHECK_AND_RETURN(constraint.readConstraintArg(constraintEntries[i]))
                m_ConstraintList.push_back(constraint);
            }
            break;
        }

    case CONSTRAINT_RANGE:
        {
            if (!m_ConstraintRange) {
                m_ConstraintRange = new ConstraintRange();
            }

            CHECK_AND_RETURN(m_ConstraintRange->readConstraintArg(val))
            break;
        }

    default:
        status = Widget::readOptParamsArg(key, val);
    }

    return status;
}

QStatus Property::readValueArg(const MsgArg* val)
{
    QStatus status = ER_OK;
    switch (val->typeId) {

    case ALLJOYN_UINT16:
        {
            CHECK_AND_RETURN(validateValue(UINT16_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_UINT16.c_str(), &m_Value.uint16Value))
            break;
        }

    case ALLJOYN_INT16:
        {
            CHECK_AND_RETURN(validateValue(INT16_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_INT16.c_str(), &m_Value.int16Value))
            break;
        }

    case ALLJOYN_UINT32:
        {
            CHECK_AND_RETURN(validateValue(UINT32_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_UINT32.c_str(), &m_Value.uint32Value))
            break;
        }

    case ALLJOYN_INT32:
        {
            CHECK_AND_RETURN(validateValue(INT32_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_INT32.c_str(), &m_Value.int32Value))
            break;
        }

    case ALLJOYN_UINT64:
        {
            CHECK_AND_RETURN(validateValue(UINT64_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_UINT64.c_str(), &m_Value.uint64Value))
            break;
        }

    case ALLJOYN_INT64:
        {
            CHECK_AND_RETURN(validateValue(INT64_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_INT64.c_str(), &m_Value.int64Value))
            break;
        }

    case ALLJOYN_DOUBLE:
        {
            CHECK_AND_RETURN(validateValue(DOUBLE_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_DOUBLE.c_str(), &m_Value.doubleValue))
            break;
        }

    case ALLJOYN_STRING:
        {
            char* value;
            CHECK_AND_RETURN(validateValue(STRING_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &value))
            m_ValueString = value;
            m_Value.charValue = m_ValueString.c_str();
            break;
        }

    case ALLJOYN_BOOLEAN:
        {
            CHECK_AND_RETURN(validateValue(BOOL_PROPERTY))
            CHECK_AND_RETURN(val->Get(AJPARAM_BOOL.c_str(), &m_Value.boolValue))
            break;
        }

    case ALLJOYN_STRUCT:
        {
            uint16_t dateOrTime;
            uint16_t dateOrTimeVal1;
            uint16_t dateOrTimeVal2;
            uint16_t dateOrTimeVal3;

            CHECK_AND_RETURN(val->Get(AJPARAM_DATE_OR_TIME.c_str(), &dateOrTime, &dateOrTimeVal1, &dateOrTimeVal2, &dateOrTimeVal3))
            if (dateOrTime == DATE_PROPERTY_TYPE) {
                CHECK_AND_RETURN(validateValue(DATE_PROPERTY))
                if (m_Value.dateValue == 0) {
                    m_Value.dateValue = new CPSDate(dateOrTimeVal1, dateOrTimeVal2, dateOrTimeVal3);
                } else {
                    m_Value.dateValue->setDay(dateOrTimeVal1);
                    m_Value.dateValue->setMonth(dateOrTimeVal2);
                    m_Value.dateValue->setYear(dateOrTimeVal3);
                }
            } else if (dateOrTime == TIME_PROPERTY_TYPE) {
                CHECK_AND_RETURN(validateValue(TIME_PROPERTY))
                if (m_Value.timeValue == 0) {
                    m_Value.timeValue = new CPSTime(dateOrTimeVal1, dateOrTimeVal2, dateOrTimeVal3);
                } else {
                    m_Value.timeValue->setHour(dateOrTimeVal1);
                    m_Value.timeValue->setMinute(dateOrTimeVal2);
                    m_Value.timeValue->setSecond(dateOrTimeVal3);
                }
            } else {
                status = ER_BUS_SIGNATURE_MISMATCH;
            }
            break;
        }

    default:
        status = ER_BUS_SIGNATURE_MISMATCH;
        break;
    }

    return status;
}

QStatus Property::SendValueChangedSignal()
{
    QStatus status = ER_OK;

    for (size_t indx = 0; indx < m_BusObjects.size(); indx++) {
        status = ((PropertyBusObject*)m_BusObjects[indx])->SendValueChangedSignal();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not send Value Changed Signal"));
            return status;
        }
    }
    return status;
}

void Property::ValueChanged(Message& msg)
{
    ControlPanelListener* listener = m_Device->getListener();
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();
    if (busAttachment) {
        busAttachment->EnableConcurrentCallbacks();
    }

    const ajn::MsgArg* returnArgs;
    size_t numArgs;
    msg->GetArgs(numArgs, returnArgs);
    if (numArgs != 1) {
        QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
        if (listener) {
            listener->errorOccured(m_Device, ER_BUS_UNEXPECTED_SIGNATURE, REFRESH_VALUE, "Received unexpected amount of returnArgs");
        }
        return;
    }

    QStatus status = ER_OK;
    if (returnArgs[0].typeId == ALLJOYN_VARIANT) {
        status = readValueArg(returnArgs[0].v_variant.val);
    } else {
        status = readValueArg(&returnArgs[0]);
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Something went wrong reading the Value Argument"));
        if (listener) {
            listener->errorOccured(m_Device, status, REFRESH_VALUE, "Could not read Value Argument");
        }
        return;
    }

    if (listener) {
        listener->signalPropertyValueChanged(m_Device, this);
    }
}

QStatus Property::defaultErrorSetValue()
{
    QCC_DbgHLPrintf(("setValue function not defined"));
    return ER_NOT_IMPLEMENTED;
}

QStatus Property::setValue(bool value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_BOOL.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(uint16_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_UINT16.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(int16_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_INT16.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(uint32_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_UINT32.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(int32_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_INT32.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(uint64_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_UINT64.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(int64_t value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_INT64.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(double value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_DOUBLE.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(const char* value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_STR.c_str(), value);
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(const CPSDate& value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_DATE_OR_TIME.c_str(), DATE_PROPERTY_TYPE, value.getDay(), value.getMonth(), value.getYear());
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

QStatus Property::setValue(const CPSTime& value)
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        return defaultErrorSetValue();
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot SetValue. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    MsgArg valueArg;
    MsgArg* variantArg = new MsgArg(AJPARAM_DATE_OR_TIME.c_str(), TIME_PROPERTY_TYPE, value.getHour(), value.getMinute(), value.getSecond());
    QStatus status = valueArg.Set(AJPARAM_VAR.c_str(), variantArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not SetValue of property"));
        delete variantArg;
        return status;
    }
    valueArg.SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return ((PropertyBusObject*)m_BusObjects[0])->SetValue(valueArg);
}

} /* namespace services */
} /* namespace ajn */
