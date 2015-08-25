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

#include "ControllerUtil.h"
#include <alljoyn/controlpanel/ControlPanel.h>
#include <alljoyn/controlpanel/ActionWithDialog.h>
#include <iostream>

using namespace ajn;
using namespace services;

qcc::String ControllerUtil::PROPERTY_HINTS_STRINGS[PROPERTY_HINTS_SIZE] = { "", "SWITCH", "CHECKBOX", "SPINNER", "RADIOBUTTON", "SLIDER",
                                                                            "TIMEPICKER", "DATEPICKER", "NUMBERPICKER", "KEYPAD", "ROTARYKNOB",
                                                                            "TEXTVIEW", "NUMERICVIEW", "EDITTEXT" };

ControllerUtil::ControllerUtil()
{

}

ControllerUtil::~ControllerUtil()
{

}

void ControllerUtil::printRootWidget(RootWidget* rootWidget)
{
    if (!rootWidget) {
        return;
    }

    std::vector<Action*> actionsToExecute;
    std::vector<Property*> propertiesToChange;
    std::vector<Dialog*> dialogsToExecute;

    if (rootWidget->getWidgetType() == WIDGET_TYPE_CONTAINER) {

        printContainer((Container*)rootWidget, actionsToExecute, dialogsToExecute, propertiesToChange);

        std::cout << "Going to execute all Actions now" << std::endl;
        executeActions(actionsToExecute);
        std::cout << std::endl;

        std::cout << "Going to execute all Dialog Actions now" << std::endl;
        executeDialogActions(dialogsToExecute);
        std::cout << std::endl;

        std::cout << "Going to call setValue for all Properties now" << std::endl;
        setPropertyValues(propertiesToChange);
        std::cout << std::endl;

    } else if (rootWidget->getWidgetType() == WIDGET_TYPE_DIALOG) {

        printDialog((Dialog*)rootWidget, "");
        dialogsToExecute.push_back((Dialog*)rootWidget);

        std::cout << "Going to execute all Dialog Actions now" << std::endl;
        executeDialogActions(dialogsToExecute);
        std::cout << std::endl;

    } else {
        std::cout << "RootWidget is of unknown type." << std::endl;
    }
}

void ControllerUtil::printContainer(Container* container, std::vector<Action*>& actionsToExecute, std::vector<Dialog*>& dialogsToExecute,
                                    std::vector<Property*>& propertiesToChange, qcc::String const& indent)
{
    printBasicWidget(container, "Container", indent);

    std::vector<Widget*> childWidgets = container->getChildWidgets();
    std::cout << indent.c_str() << "Print ChildWidgets: " << std::endl;
    for (size_t i = 0; i < childWidgets.size(); i++) {
        WidgetType widgetType = childWidgets[i]->getWidgetType();
        switch (widgetType) {
        case WIDGET_TYPE_ACTION:
            printBasicWidget(childWidgets[i], "Action", indent + "  ");
            actionsToExecute.push_back((Action*)childWidgets[i]);
            break;

        case WIDGET_TYPE_ACTION_WITH_DIALOG:
            printBasicWidget(childWidgets[i], "Action", indent + "  ");
            std::cout << indent.c_str() << "  Printing ChildDialog: " << std::endl;
            printDialog(((ActionWithDialog*)childWidgets[i])->getChildDialog(), indent + "    ");
            dialogsToExecute.push_back(((ActionWithDialog*)childWidgets[i])->getChildDialog());
            break;

        case WIDGET_TYPE_LABEL:
            printBasicWidget(childWidgets[i], "Label", indent + "  ");
            break;

        case WIDGET_TYPE_PROPERTY:
            printProperty(((Property*)childWidgets[i]), indent + "  ");
            propertiesToChange.push_back((Property*)childWidgets[i]);
            break;

        case WIDGET_TYPE_CONTAINER:
            printContainer(((Container*)childWidgets[i]), actionsToExecute, dialogsToExecute, propertiesToChange, indent + "  ");
            break;

        case WIDGET_TYPE_DIALOG:
            printDialog(((Dialog*)childWidgets[i]), indent + "  ");
            break;

        case WIDGET_TYPE_ERROR:
            printErrorWidget(childWidgets[i], indent + "  ");
            break;
        }

        std::cout << std::endl;
    }
}

void ControllerUtil::printProperty(Property* property, qcc::String const& indent)
{
    printBasicWidget(property, "Property", indent);
    printPropertyValue(property->getPropertyValue(), property->getPropertyType(), indent);
    std::cout << indent.c_str() << "Property is " << (property->getIsWritable() ? "writable" : "not writable") << std::endl;

    if (property->getUnitOfMeasure().size()) {
        std::cout << indent.c_str() << "Property unitOfMeasure: " << property->getUnitOfMeasure().c_str() << std::endl;
    }
    if (property->getConstraintRange()) {
        std::cout << indent.c_str() << "Property has ConstraintRange: " << std::endl;
        printConstraintRange(property->getConstraintRange(), property->getPropertyType(), indent + "  ");
    }
    if (property->getConstraintList().size()) {
        std::cout << indent.c_str() << "Property has ConstraintList: " << std::endl;
        printConstraintList(property->getConstraintList(), property->getPropertyType(), indent + "  ");
    }
}

void ControllerUtil::printDialog(Dialog* dialog, qcc::String const& indent)
{
    printBasicWidget(dialog, "Dialog", indent);
    std::cout << indent.c_str() << "Dialog message: " << dialog->getMessage().c_str() << std::endl;
    std::cout << indent.c_str() << "Dialog numActions: " << dialog->getNumActions() << std::endl;
    if (dialog->getLabelAction1().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action1: " << dialog->getLabelAction1().c_str() << std::endl;
    }
    if (dialog->getLabelAction2().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action2: " << dialog->getLabelAction2().c_str() << std::endl;
    }
    if (dialog->getLabelAction3().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action3: " << dialog->getLabelAction3().c_str() << std::endl;
    }
}

void ControllerUtil::printBasicWidget(Widget* widget, qcc::String const& widgetType, qcc::String const& indent)
{
    std::cout << indent.c_str() << widgetType.c_str() << " name: " << widget->getWidgetName().c_str() << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " version: " << widget->getInterfaceVersion() << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " is " << (widget->getIsSecured() ? "secured" : "not secured") << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " is " << (widget->getIsEnabled() ? "enabled" : "not enabled") << std::endl;
    if (widget->getLabel().size()) {
        std::cout << indent.c_str() << widgetType.c_str() << " label: " << widget->getLabel().c_str() << std::endl;
    }
    if (widget->getBgColor() != UINT32_MAX) {
        std::cout << indent.c_str() << widgetType.c_str() << " bgColor: " << widget->getBgColor() << std::endl;
    }
    printHints(widget, widgetType, indent);
}

void ControllerUtil::printErrorWidget(Widget* widget, qcc::String const& indent)
{
    std::cout << indent.c_str() << "Received error widget with name: " << widget->getWidgetName().c_str() << std::endl;
    if (widget->getLabel().size()) {
        std::cout << indent.c_str() << "ErrorWidget label: " << widget->getLabel().c_str() << std::endl;
    }
}

void ControllerUtil::printConstraintList(const std::vector<ConstraintList>& constraintList, PropertyType propertyType, qcc::String const& indent)
{
    for (size_t i = 0; i < constraintList.size(); i++) {
        std::cout << indent.c_str() << "ConstraintList " << i << " Display: " << constraintList[i].getDisplay().c_str() << std::endl;
        switch (propertyType) {
        case UINT16_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint16Value << std::endl;
            break;

        case INT16_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int16Value << std::endl;
            break;

        case UINT32_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint32Value << std::endl;
            break;

        case INT32_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int32Value << std::endl;
            break;

        case UINT64_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint64Value << std::endl;
            break;

        case INT64_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int64Value << std::endl;
            break;

        case DOUBLE_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().doubleValue << std::endl;
            break;

        case STRING_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().charValue << std::endl;
            break;

        default:
            std::cout << indent.c_str() << "ConstraintList is unknown property type" << std::endl;
            break;
        }
    }
}

void ControllerUtil::printConstraintRange(ConstraintRange* constraintRange, PropertyType propertyType, qcc::String const& indent)
{
    switch (propertyType) {
    case UINT16_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint16Value << std::endl;
        break;

    case INT16_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int16Value << std::endl;
        break;

    case UINT32_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint32Value << std::endl;
        break;

    case INT32_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int32Value << std::endl;
        break;

    case UINT64_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint64Value << std::endl;
        break;

    case INT64_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int64Value << std::endl;
        break;

    case DOUBLE_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().doubleValue << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().doubleValue << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().doubleValue << std::endl;
        break;

    default:
        std::cout << indent.c_str() << "ConstraintRange is unknown property type" << std::endl;
        break;
    }
}

void ControllerUtil::printPropertyValue(PropertyValue propertyValue, PropertyType propertyType, qcc::String const& indent)
{
    switch (propertyType) {
    case UINT16_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT16 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint16Value << std::endl;
        break;

    case INT16_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT16 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int16Value << std::endl;
        break;

    case UINT32_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT32 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint32Value << std::endl;
        break;

    case INT32_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT32 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int32Value << std::endl;
        break;

    case UINT64_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT64 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint64Value << std::endl;
        break;

    case INT64_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT64 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int64Value << std::endl;
        break;

    case DOUBLE_PROPERTY:
        std::cout << indent.c_str() << "Property is a DOUBLE Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.doubleValue << std::endl;
        break;

    case STRING_PROPERTY:
        std::cout << indent.c_str() << "Property is a STRING Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.charValue << std::endl;
        break;

    case BOOL_PROPERTY:
        std::cout << indent.c_str() << "Property is a BOOL Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << (propertyValue.boolValue ? "true" : "false") << std::endl;
        break;

    case DATE_PROPERTY:
        std::cout << indent.c_str() << "Property is a Date Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.dateValue->getDay() << "/" <<
        propertyValue.dateValue->getMonth() << "/" <<
        propertyValue.dateValue->getYear() << std::endl;
        break;

    case TIME_PROPERTY:
        std::cout << indent.c_str() << "Property is a Time Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.timeValue->getHour() << ":" <<
        propertyValue.timeValue->getMinute() << ":" <<
        propertyValue.timeValue->getSecond() << std::endl;
        break;

    default:
        std::cout << indent.c_str() << "Property is unknown property type" << std::endl;
        break;
    }
}

void ControllerUtil::executeActions(std::vector<Action*>& actionsToExecute)
{
    for (size_t i = 0; i < actionsToExecute.size(); i++) {
        std::cout << "Execute Action for " << actionsToExecute[i]->getWidgetName().c_str() << std::endl;
        QStatus status = actionsToExecute[i]->executeAction();
        std::cout << "    Action execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::executeDialogActions(std::vector<Dialog*>& dialogsToExecute)
{
    for (size_t i = 0; i < dialogsToExecute.size(); i++) {
        std::cout << "Execute Actions for " << dialogsToExecute[i]->getWidgetName().c_str() << " which has " <<
        dialogsToExecute[i]->getNumActions() << " actions: " << std::endl;
        QStatus status = dialogsToExecute[i]->executeAction1();
        std::cout << "    Action 1 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
        status = dialogsToExecute[i]->executeAction2();
        std::cout << "    Action 2 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
        status = dialogsToExecute[i]->executeAction3();
        std::cout << "    Action 3 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::setPropertyValues(std::vector<Property*>& propertiesToChange)
{
    for (size_t i = 0; i < propertiesToChange.size(); i++) {
        std::cout << "Setting Value for Property " << propertiesToChange[i]->getWidgetName().c_str() << std::endl;
        QStatus status = ER_OK;
        switch (propertiesToChange[i]->getPropertyType()) {
        case UINT16_PROPERTY:
            std::cout << "    Setting Value to " << 130 << std::endl;
            status = propertiesToChange[i]->setValue((uint16_t)130);
            break;

        case INT16_PROPERTY:
            std::cout << "    Setting Value to " << 140 << std::endl;
            status = propertiesToChange[i]->setValue((int16_t)140);
            break;

        case UINT32_PROPERTY:
            std::cout << "    Setting Value to " << 150 << std::endl;
            status = propertiesToChange[i]->setValue((uint32_t)150);
            break;

        case INT32_PROPERTY:
            std::cout << "    Setting Value to " << 160 << std::endl;
            status = propertiesToChange[i]->setValue((int32_t)160);
            break;

        case UINT64_PROPERTY:
            std::cout << "    Setting Value to " << 170 << std::endl;
            status = propertiesToChange[i]->setValue((uint64_t)170);
            break;

        case INT64_PROPERTY:
            std::cout << "    Setting Value to " << 180 << std::endl;
            status = propertiesToChange[i]->setValue((int64_t)180);
            break;

        case DOUBLE_PROPERTY:
            std::cout << "    Setting Value to " << 190 << std::endl;
            status = propertiesToChange[i]->setValue((double)190);
            break;

        case STRING_PROPERTY:
            std::cout << "    Setting Value to 'TestingValue'" << std::endl;
            status = propertiesToChange[i]->setValue("TestingValue");
            break;

        case BOOL_PROPERTY:
            std::cout << "    Setting Value to true" << std::endl;
            status = propertiesToChange[i]->setValue(true);
            break;

        case DATE_PROPERTY: {
                std::cout << "    Setting Value to 22/6/1892" << std::endl;
                CPSDate date(22, 6, 1892);
                status = propertiesToChange[i]->setValue(date);
                break;
            }

        case TIME_PROPERTY: {
                std::cout << "    Setting Value to 12:10:30" << std::endl;
                CPSTime time(12, 10, 30);
                status = propertiesToChange[i]->setValue(time);
                break;
            }

        case UNDEFINED: {
                status = ER_NOT_IMPLEMENTED;
                break;
            }
        }
        std::cout << "    Set Value " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::printHints(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent)
{
    std::vector<uint16_t> hints = widget->getHints();
    if (hints.size()) {
        std::cout << indent.c_str() << widgetType.c_str() << " hints: ";
        for (size_t i = 0; i < hints.size(); i++) {
            switch (widget->getWidgetType()) {
            case WIDGET_TYPE_ACTION:
            case WIDGET_TYPE_ACTION_WITH_DIALOG:
                if (hints[i] == ACTIONBUTTON) {
                    std::cout << "ACTIONBUTTON";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_LABEL:
                if (hints[i] == TEXTLABEL) {
                    std::cout << "TEXTLABEL";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_PROPERTY:
                if (hints[i] > 0 && hints[i] < PROPERTY_HINTS_SIZE) {
                    std::cout << PROPERTY_HINTS_STRINGS[hints[i]].c_str();
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_CONTAINER:
                if (hints[i] == VERTICAL_LINEAR) {
                    std::cout << "VERTICAL_LINEAR";
                } else if (hints[i] == HORIZONTAL_LINEAR) {
                    std::cout << "HORIZONTAL_LINEAR";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_DIALOG:
                if (hints[i] == ALERTDIALOG) {
                    std::cout << "ALERTDIALOG";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_ERROR:
                std::cout << "UNKNOWN";
                break;
            }
            std::cout << (i == hints.size() - 1 ? "\n" : ", ");
        }
    }
}
