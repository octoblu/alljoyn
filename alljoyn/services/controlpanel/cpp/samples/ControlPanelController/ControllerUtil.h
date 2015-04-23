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

#ifndef CONTROLLERUTIL_H_
#define CONTROLLERUTIL_H_

#include <alljoyn/controlpanel/Container.h>
#include <alljoyn/controlpanel/Property.h>
#include <alljoyn/controlpanel/Dialog.h>
#include <alljoyn/controlpanel/Action.h>

#define PROPERTY_HINTS_SIZE 14

/**
 * Class used to simulate accessing of widgets, executing actions setting properties etc
 */
class ControllerUtil {

  public:

    ControllerUtil();

    virtual ~ControllerUtil();

    static void printRootWidget(ajn::services::RootWidget* rootWidget);

    static void printPropertyValue(ajn::services::PropertyValue propertyValue, ajn::services::PropertyType propertyType, qcc::String const& indent = "  ");

  private:

    static void printErrorWidget(ajn::services::Widget* widget, qcc::String const& indent);

    static void printBasicWidget(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent);

    static void printContainer(ajn::services::Container* rootContainer, std::vector<ajn::services::Action*>& actionsToExecute,
                               std::vector<ajn::services::Dialog*>& dialogsToExecute, std::vector<ajn::services::Property*>& propertiesToChange,
                               qcc::String const& indent = "");

    static void printProperty(ajn::services::Property* property, qcc::String const& indent);

    static void printDialog(ajn::services::Dialog* dialog, qcc::String const& indent);

    static void printConstraintRange(ajn::services::ConstraintRange* constraintRange, ajn::services::PropertyType propertyType,
                                     qcc::String const& indent);

    static void printConstraintList(const std::vector<ajn::services::ConstraintList>& constraintList, ajn::services::PropertyType propertyType,
                                    qcc::String const& indent);

    static void executeActions(std::vector<ajn::services::Action*>& actionsToExecute);

    static void executeDialogActions(std::vector<ajn::services::Dialog*>& dialogsToExecute);

    static void setPropertyValues(std::vector<ajn::services::Property*>& propertiesToChange);

    static void printHints(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent);

    static qcc::String PROPERTY_HINTS_STRINGS[PROPERTY_HINTS_SIZE];

};

#endif /* CONTROLLERUTIL_H_ */
