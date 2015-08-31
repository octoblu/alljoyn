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

#include "ControlPanelGenerated.h"
#include <alljoyn/controlpanel/LanguageSets.h>
#include <alljoyn/controlpanel/Label.h>
#include "ControlPanelProvided.h"



using namespace ajn;
using namespace services;

bool ControlPanelGenerated::languageSetsDone = false;
ControlPanelControlleeUnit* ControlPanelGenerated::myDeviceUnit = 0;
ControlPanel* ControlPanelGenerated::myDeviceRootContainerControlPanel = 0;
Container* ControlPanelGenerated::myDeviceRootContainer = 0;
Label* ControlPanelGenerated::myDeviceCurrentTemp = 0;
MyDeviceHeatProperty* ControlPanelGenerated::myDeviceHeatProperty = 0;
MyDeviceOvenAction* ControlPanelGenerated::myDeviceOvenAction = 0;
ActionWithDialog* ControlPanelGenerated::myDeviceLightAction = 0;
MyDeviceLightConfirm* ControlPanelGenerated::myDeviceLightConfirm = 0;
NotificationAction* ControlPanelGenerated::myDeviceAreYouSureNotificationAction = 0;
MyDeviceAreYouSure* ControlPanelGenerated::myDeviceAreYouSure = 0;


#define CHECK(x) if ((status = x) != ER_OK) { return status; }

void ControlPanelGenerated::PrepareLanguageSets()
{
    if (languageSetsDone) {
        return;
    }

    LanguageSet myDeviceMyLanguages("myDeviceMyLanguages");
    myDeviceMyLanguages.addLanguage("en");
    myDeviceMyLanguages.addLanguage("de_AT");
    myDeviceMyLanguages.addLanguage("zh_Hans_CN");
    LanguageSets::add(myDeviceMyLanguages.getLanguageSetName(), myDeviceMyLanguages);

    languageSetsDone = true;
}

QStatus ControlPanelGenerated::PrepareWidgets(ControlPanelControllee*& controlPanelControllee)
{
    if (controlPanelControllee) {
        return ER_BAD_ARG_1;
    }

    PrepareLanguageSets();

    QStatus status = ER_OK;
    controlPanelControllee = new ControlPanelControllee();

    myDeviceUnit = new ControlPanelControlleeUnit("MyDevice");
    CHECK(controlPanelControllee->addControlPanelUnit(myDeviceUnit));

    myDeviceRootContainerControlPanel = ControlPanel::createControlPanel(LanguageSets::get("myDeviceMyLanguages"));
    if (!myDeviceRootContainerControlPanel) {
        return ER_FAIL;
    }
    CHECK(myDeviceUnit->addControlPanel(myDeviceRootContainerControlPanel));

    myDeviceRootContainer = new Container("rootContainer", NULL);
    CHECK(myDeviceRootContainerControlPanel->setRootWidget(myDeviceRootContainer));

    myDeviceRootContainer->setEnabled(true);
    myDeviceRootContainer->setIsSecured(false);
    myDeviceRootContainer->setBgColor(0x200);

    std::vector<qcc::String> myDeviceRootContainerlabelVec;
    myDeviceRootContainerlabelVec.push_back("My Label of my container");
    myDeviceRootContainerlabelVec.push_back("Container Etikett");
    myDeviceRootContainerlabelVec.push_back(UNICODE_MY_LABEL_CONTAINER);
    myDeviceRootContainer->setLabels(myDeviceRootContainerlabelVec);

    std::vector<uint16_t> myDeviceRootContainerHintsVec;
    myDeviceRootContainerHintsVec.push_back(VERTICAL_LINEAR);
    myDeviceRootContainerHintsVec.push_back(HORIZONTAL_LINEAR);
    myDeviceRootContainer->setHints(myDeviceRootContainerHintsVec);

    myDeviceCurrentTemp = new Label("CurrentTemp", myDeviceRootContainer);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceCurrentTemp));

    myDeviceCurrentTemp->setEnabled(true);
    myDeviceCurrentTemp->setBgColor(0x98765);

    std::vector<qcc::String> myDeviceCurrentTemplabelVec;
    myDeviceCurrentTemplabelVec.push_back("Current Temperature:");
    myDeviceCurrentTemplabelVec.push_back("Aktuelle Temperatur:");
    myDeviceCurrentTemplabelVec.push_back(UNICODE_CURENT_TEMPERATURE);
    myDeviceCurrentTemp->setLabels(myDeviceCurrentTemplabelVec);

    std::vector<uint16_t> myDeviceCurrentTempHintsVec;
    myDeviceCurrentTempHintsVec.push_back(TEXTLABEL);
    myDeviceCurrentTemp->setHints(myDeviceCurrentTempHintsVec);

    myDeviceHeatProperty = new MyDeviceHeatProperty("heatProperty", myDeviceRootContainer, UINT16_PROPERTY);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceHeatProperty));

    myDeviceHeatProperty->setEnabled(true);
    myDeviceHeatProperty->setIsSecured(false);
    myDeviceHeatProperty->setWritable(true);
    CHECK(myDeviceHeatProperty->setGetValue(getuint16Var));
    myDeviceHeatProperty->setBgColor(0x500);

    std::vector<qcc::String> myDeviceHeatPropertylabelVec;
    myDeviceHeatPropertylabelVec.push_back("Oven Temperature");
    myDeviceHeatPropertylabelVec.push_back("Ofentemperatur");
    myDeviceHeatPropertylabelVec.push_back(UNICODE_OVEN_TEMPERATURE);
    myDeviceHeatProperty->setLabels(myDeviceHeatPropertylabelVec);

    std::vector<uint16_t> myDeviceHeatPropertyHintsVec;
    myDeviceHeatPropertyHintsVec.push_back(SPINNER);
    myDeviceHeatProperty->setHints(myDeviceHeatPropertyHintsVec);

    std::vector<qcc::String> myDeviceHeatPropertyunitMeasureVec;
    myDeviceHeatPropertyunitMeasureVec.push_back("Degrees");
    myDeviceHeatPropertyunitMeasureVec.push_back("Grad");
    myDeviceHeatPropertyunitMeasureVec.push_back(UNICODE_DEGREES);
    myDeviceHeatProperty->setUnitOfMeasures(myDeviceHeatPropertyunitMeasureVec);

    std::vector<ConstraintList> myDeviceHeatPropertyConstraintListVec(3);

    std::vector<qcc::String> myDeviceHeatPropertyDisplay1Vec;
    myDeviceHeatPropertyDisplay1Vec.push_back("Regular");
    myDeviceHeatPropertyDisplay1Vec.push_back("Normal");
    myDeviceHeatPropertyDisplay1Vec.push_back(UNICODE_REGULAR);
    myDeviceHeatPropertyConstraintListVec[0].setDisplays(myDeviceHeatPropertyDisplay1Vec);
    myDeviceHeatPropertyConstraintListVec[0].setConstraintValue((uint16_t)175);

    std::vector<qcc::String> myDeviceHeatPropertyDisplay2Vec;
    myDeviceHeatPropertyDisplay2Vec.push_back("Hot");
    myDeviceHeatPropertyDisplay2Vec.push_back("Heiss");
    myDeviceHeatPropertyDisplay2Vec.push_back(UNICODE_HOT);
    myDeviceHeatPropertyConstraintListVec[1].setDisplays(myDeviceHeatPropertyDisplay2Vec);
    myDeviceHeatPropertyConstraintListVec[1].setConstraintValue((uint16_t)200);

    std::vector<qcc::String> myDeviceHeatPropertyDisplay3Vec;
    myDeviceHeatPropertyDisplay3Vec.push_back("Very Hot");
    myDeviceHeatPropertyDisplay3Vec.push_back("Sehr Heiss");
    myDeviceHeatPropertyDisplay3Vec.push_back(UNICODE_VERY_HOT);
    myDeviceHeatPropertyConstraintListVec[2].setDisplays(myDeviceHeatPropertyDisplay3Vec);
    myDeviceHeatPropertyConstraintListVec[2].setConstraintValue((uint16_t)225);

    myDeviceHeatProperty->setConstraintList(myDeviceHeatPropertyConstraintListVec);

    myDeviceOvenAction = new MyDeviceOvenAction("ovenAction", myDeviceRootContainer);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceOvenAction));

    myDeviceOvenAction->setEnabled(true);
    myDeviceOvenAction->setIsSecured(false);
    myDeviceOvenAction->setBgColor(0x400);

    std::vector<qcc::String> myDeviceOvenActionlabelVec;
    myDeviceOvenActionlabelVec.push_back("Start Oven");
    myDeviceOvenActionlabelVec.push_back("Ofen started");
    myDeviceOvenActionlabelVec.push_back(UNICODE_START_OVEN);
    myDeviceOvenAction->setLabels(myDeviceOvenActionlabelVec);

    std::vector<uint16_t> myDeviceOvenActionHintsVec;
    myDeviceOvenActionHintsVec.push_back(ACTIONBUTTON);
    myDeviceOvenAction->setHints(myDeviceOvenActionHintsVec);

    myDeviceLightAction = new ActionWithDialog("lightAction", myDeviceRootContainer);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceLightAction));

    myDeviceLightAction->setEnabled(true);
    myDeviceLightAction->setIsSecured(false);
    myDeviceLightAction->setBgColor(0x400);

    std::vector<qcc::String> myDeviceLightActionlabelVec;
    myDeviceLightActionlabelVec.push_back("Turn on oven light");
    myDeviceLightActionlabelVec.push_back("Ofenlicht anschalten");
    myDeviceLightActionlabelVec.push_back(UNICODE_TURN_ON_OVEN_LIGHT);
    myDeviceLightAction->setLabels(myDeviceLightActionlabelVec);

    std::vector<uint16_t> myDeviceLightActionHintsVec;
    myDeviceLightActionHintsVec.push_back(ACTIONBUTTON);
    myDeviceLightAction->setHints(myDeviceLightActionHintsVec);

    myDeviceLightConfirm = new MyDeviceLightConfirm("LightConfirm", NULL);
    CHECK(myDeviceLightAction->addChildDialog(myDeviceLightConfirm));

    myDeviceLightConfirm->setEnabled(true);
    myDeviceLightConfirm->setIsSecured(false);

    std::vector<qcc::String> myDeviceLightConfirmmessageVec;
    myDeviceLightConfirmmessageVec.push_back("Are you sure you want to turn on the light");
    myDeviceLightConfirmmessageVec.push_back("Are you sure you want to turn on the light");
    myDeviceLightConfirmmessageVec.push_back(UNICODE_ARE_YOU_SURE_YOU_WANT_TO_TURN_OFF_THE_LIGHT);
    myDeviceLightConfirm->setMessages(myDeviceLightConfirmmessageVec);
    myDeviceLightConfirm->setNumActions(3);
    myDeviceLightConfirm->setBgColor(0x789);

    std::vector<qcc::String> myDeviceLightConfirmlabelVec;
    myDeviceLightConfirmlabelVec.push_back("Are you sure?");
    myDeviceLightConfirmlabelVec.push_back("Sind sie sicher?");
    myDeviceLightConfirmlabelVec.push_back(UNICODE_ARE_YOU_SURE);
    myDeviceLightConfirm->setLabels(myDeviceLightConfirmlabelVec);

    std::vector<uint16_t> myDeviceLightConfirmHintsVec;
    myDeviceLightConfirmHintsVec.push_back(ALERTDIALOG);
    myDeviceLightConfirm->setHints(myDeviceLightConfirmHintsVec);

    std::vector<qcc::String> myDeviceLightConfirmLabelAction1Vec;
    myDeviceLightConfirmLabelAction1Vec.push_back("Yes");
    myDeviceLightConfirmLabelAction1Vec.push_back("Ja");
    myDeviceLightConfirmLabelAction1Vec.push_back(UNICODE_YES);
    myDeviceLightConfirm->setLabelsAction1(myDeviceLightConfirmLabelAction1Vec);

    std::vector<qcc::String> myDeviceLightConfirmLabelAction2Vec;
    myDeviceLightConfirmLabelAction2Vec.push_back("No");
    myDeviceLightConfirmLabelAction2Vec.push_back("Nein");
    myDeviceLightConfirmLabelAction2Vec.push_back(UNICODE_NO);
    myDeviceLightConfirm->setLabelsAction2(myDeviceLightConfirmLabelAction2Vec);

    std::vector<qcc::String> myDeviceLightConfirmLabelAction3Vec;
    myDeviceLightConfirmLabelAction3Vec.push_back("Cancel");
    myDeviceLightConfirmLabelAction3Vec.push_back("Abrechen");
    myDeviceLightConfirmLabelAction3Vec.push_back(UNICODE_CANCEL);
    myDeviceLightConfirm->setLabelsAction3(myDeviceLightConfirmLabelAction3Vec);

    myDeviceAreYouSureNotificationAction = NotificationAction::createNotificationAction(LanguageSets::get("myDeviceMyLanguages"));
    if (!myDeviceAreYouSureNotificationAction) {
        return ER_FAIL;
    }
    CHECK(myDeviceUnit->addNotificationAction(myDeviceAreYouSureNotificationAction));

    myDeviceAreYouSure = new MyDeviceAreYouSure("areYouSure", NULL);
    CHECK(myDeviceAreYouSureNotificationAction->setRootWidget(myDeviceAreYouSure));

    myDeviceAreYouSure->setEnabled(true);
    myDeviceAreYouSure->setIsSecured(false);

    std::vector<qcc::String> myDeviceAreYouSuremessageVec;
    myDeviceAreYouSuremessageVec.push_back("Are you sure?");
    myDeviceAreYouSuremessageVec.push_back("Sind sie sicher?");
    myDeviceAreYouSuremessageVec.push_back(UNICODE_ARE_YOU_SURE);
    myDeviceAreYouSure->setMessages(myDeviceAreYouSuremessageVec);
    myDeviceAreYouSure->setNumActions(1);
    myDeviceAreYouSure->setBgColor(0x789);

    std::vector<qcc::String> myDeviceAreYouSurelabelVec;
    myDeviceAreYouSurelabelVec.push_back("Are you sure?");
    myDeviceAreYouSurelabelVec.push_back("Sind sie sicher?");
    myDeviceAreYouSurelabelVec.push_back(UNICODE_ARE_YOU_SURE);
    myDeviceAreYouSure->setLabels(myDeviceAreYouSurelabelVec);

    std::vector<qcc::String> myDeviceAreYouSureLabelAction1Vec;
    myDeviceAreYouSureLabelAction1Vec.push_back("Yes");
    myDeviceAreYouSureLabelAction1Vec.push_back("Ja");
    myDeviceAreYouSureLabelAction1Vec.push_back(UNICODE_YES);
    myDeviceAreYouSure->setLabelsAction1(myDeviceAreYouSureLabelAction1Vec);

    return status;
}

void ControlPanelGenerated::Shutdown()
{
    if (myDeviceUnit) {
        delete (myDeviceUnit);
        myDeviceUnit = 0;
    }
    if (myDeviceRootContainerControlPanel) {
        delete (myDeviceRootContainerControlPanel);
        myDeviceRootContainerControlPanel = 0;
    }
    if (myDeviceRootContainer) {
        delete (myDeviceRootContainer);
        myDeviceRootContainer = 0;
    }
    if (myDeviceCurrentTemp) {
        delete (myDeviceCurrentTemp);
        myDeviceCurrentTemp = 0;
    }
    if (myDeviceHeatProperty) {
        delete (myDeviceHeatProperty);
        myDeviceHeatProperty = 0;
    }
    if (myDeviceOvenAction) {
        delete (myDeviceOvenAction);
        myDeviceOvenAction = 0;
    }
    if (myDeviceLightAction) {
        delete (myDeviceLightAction);
        myDeviceLightAction = 0;
    }
    if (myDeviceLightConfirm) {
        delete (myDeviceLightConfirm);
        myDeviceLightConfirm = 0;
    }
    if (myDeviceAreYouSureNotificationAction) {
        delete (myDeviceAreYouSureNotificationAction);
        myDeviceAreYouSureNotificationAction = 0;
    }
    if (myDeviceAreYouSure) {
        delete (myDeviceAreYouSure);
        myDeviceAreYouSure = 0;
    }

}
