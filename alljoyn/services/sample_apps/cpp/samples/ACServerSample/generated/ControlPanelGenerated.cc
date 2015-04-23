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
Container* ControlPanelGenerated::myDeviceTempAndHumidityContainer = 0;
MyDeviceCurrentTempStringProperty* ControlPanelGenerated::myDeviceCurrentTempStringProperty = 0;
MyDeviceCurrentHumidityStringProperty* ControlPanelGenerated::myDeviceCurrentHumidityStringProperty = 0;
Container* ControlPanelGenerated::myDeviceControlsContainer = 0;
MyDeviceAc_mode* ControlPanelGenerated::myDeviceAc_mode = 0;
MyDeviceStatusStringProperty* ControlPanelGenerated::myDeviceStatusStringProperty = 0;
MyDeviceSet_temperature* ControlPanelGenerated::myDeviceSet_temperature = 0;
MyDeviceFan_speed* ControlPanelGenerated::myDeviceFan_speed = 0;
NotificationAction* ControlPanelGenerated::myDeviceTurnFanOnNotificationAction = 0;
MyDeviceTurnFanOn* ControlPanelGenerated::myDeviceTurnFanOn = 0;
NotificationAction* ControlPanelGenerated::myDeviceTurnFanOffNotificationAction = 0;
MyDeviceTurnFanOff* ControlPanelGenerated::myDeviceTurnFanOff = 0;


#define CHECK(x) if ((status = x) != ER_OK) { return status; }

void ControlPanelGenerated::PrepareLanguageSets()
{
    if (languageSetsDone) {
        return;
    }

    LanguageSet myDeviceMyLanguages("myDeviceMyLanguages");
    myDeviceMyLanguages.addLanguage("en");
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
    myDeviceRootContainer->setBgColor(0x1e90ff);

    std::vector<uint16_t> myDeviceRootContainerHintsVec;
    myDeviceRootContainerHintsVec.push_back(VERTICAL_LINEAR);
    myDeviceRootContainerHintsVec.push_back(HORIZONTAL_LINEAR);
    myDeviceRootContainer->setHints(myDeviceRootContainerHintsVec);

    myDeviceTempAndHumidityContainer = new Container("tempAndHumidityContainer", myDeviceRootContainer);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceTempAndHumidityContainer));

    myDeviceTempAndHumidityContainer->setEnabled(true);
    myDeviceTempAndHumidityContainer->setIsSecured(false);
    myDeviceTempAndHumidityContainer->setBgColor(0x200);

    std::vector<uint16_t> myDeviceTempAndHumidityContainerHintsVec;
    myDeviceTempAndHumidityContainerHintsVec.push_back(VERTICAL_LINEAR);
    myDeviceTempAndHumidityContainer->setHints(myDeviceTempAndHumidityContainerHintsVec);

    myDeviceCurrentTempStringProperty = new MyDeviceCurrentTempStringProperty("CurrentTempStringProperty", myDeviceTempAndHumidityContainer, STRING_PROPERTY);
    CHECK(myDeviceTempAndHumidityContainer->addChildWidget(myDeviceCurrentTempStringProperty));

    myDeviceCurrentTempStringProperty->setEnabled(true);
    myDeviceCurrentTempStringProperty->setIsSecured(false);
    myDeviceCurrentTempStringProperty->setWritable(false);
    CHECK(myDeviceCurrentTempStringProperty->setGetValue(getCurrentTemperatureString));
    myDeviceCurrentTempStringProperty->setBgColor(0x500);

    std::vector<qcc::String> myDeviceCurrentTempStringPropertylabelVec;
    myDeviceCurrentTempStringPropertylabelVec.push_back("Current Temperature:");
    myDeviceCurrentTempStringProperty->setLabels(myDeviceCurrentTempStringPropertylabelVec);

    std::vector<uint16_t> myDeviceCurrentTempStringPropertyHintsVec;
    myDeviceCurrentTempStringPropertyHintsVec.push_back(TEXTVIEW);
    myDeviceCurrentTempStringProperty->setHints(myDeviceCurrentTempStringPropertyHintsVec);

    myDeviceCurrentHumidityStringProperty = new MyDeviceCurrentHumidityStringProperty("CurrentHumidityStringProperty", myDeviceTempAndHumidityContainer, STRING_PROPERTY);
    CHECK(myDeviceTempAndHumidityContainer->addChildWidget(myDeviceCurrentHumidityStringProperty));

    myDeviceCurrentHumidityStringProperty->setEnabled(true);
    myDeviceCurrentHumidityStringProperty->setIsSecured(false);
    myDeviceCurrentHumidityStringProperty->setWritable(false);
    CHECK(myDeviceCurrentHumidityStringProperty->setGetValue(getCurrentHumidityString));
    myDeviceCurrentHumidityStringProperty->setBgColor(0x500);

    std::vector<qcc::String> myDeviceCurrentHumidityStringPropertylabelVec;
    myDeviceCurrentHumidityStringPropertylabelVec.push_back("Current Humidity:");
    myDeviceCurrentHumidityStringProperty->setLabels(myDeviceCurrentHumidityStringPropertylabelVec);

    std::vector<uint16_t> myDeviceCurrentHumidityStringPropertyHintsVec;
    myDeviceCurrentHumidityStringPropertyHintsVec.push_back(TEXTVIEW);
    myDeviceCurrentHumidityStringProperty->setHints(myDeviceCurrentHumidityStringPropertyHintsVec);

    myDeviceControlsContainer = new Container("controlsContainer", myDeviceRootContainer);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceControlsContainer));

    myDeviceControlsContainer->setEnabled(true);
    myDeviceControlsContainer->setIsSecured(false);
    myDeviceControlsContainer->setBgColor(0x200);

    std::vector<uint16_t> myDeviceControlsContainerHintsVec;
    myDeviceControlsContainerHintsVec.push_back(HORIZONTAL_LINEAR);
    myDeviceControlsContainer->setHints(myDeviceControlsContainerHintsVec);

    myDeviceAc_mode = new MyDeviceAc_mode("ac_mode", myDeviceControlsContainer, UINT16_PROPERTY);
    CHECK(myDeviceControlsContainer->addChildWidget(myDeviceAc_mode));

    myDeviceAc_mode->setEnabled(true);
    myDeviceAc_mode->setIsSecured(false);
    myDeviceAc_mode->setWritable(true);
    CHECK(myDeviceAc_mode->setGetValue(getCurrentMode));
    myDeviceAc_mode->setBgColor(0xffd700);

    std::vector<qcc::String> myDeviceAc_modelabelVec;
    myDeviceAc_modelabelVec.push_back("Mode");
    myDeviceAc_mode->setLabels(myDeviceAc_modelabelVec);

    std::vector<uint16_t> myDeviceAc_modeHintsVec;
    myDeviceAc_modeHintsVec.push_back(SPINNER);
    myDeviceAc_mode->setHints(myDeviceAc_modeHintsVec);

    std::vector<ConstraintList> myDeviceAc_modeConstraintListVec(5);

    std::vector<qcc::String> myDeviceAc_modeDisplay1Vec;
    myDeviceAc_modeDisplay1Vec.push_back("Auto");
    myDeviceAc_modeConstraintListVec[0].setDisplays(myDeviceAc_modeDisplay1Vec);
    myDeviceAc_modeConstraintListVec[0].setConstraintValue((uint16_t)0);

    std::vector<qcc::String> myDeviceAc_modeDisplay2Vec;
    myDeviceAc_modeDisplay2Vec.push_back("Cool");
    myDeviceAc_modeConstraintListVec[1].setDisplays(myDeviceAc_modeDisplay2Vec);
    myDeviceAc_modeConstraintListVec[1].setConstraintValue((uint16_t)1);

    std::vector<qcc::String> myDeviceAc_modeDisplay3Vec;
    myDeviceAc_modeDisplay3Vec.push_back("Heat");
    myDeviceAc_modeConstraintListVec[2].setDisplays(myDeviceAc_modeDisplay3Vec);
    myDeviceAc_modeConstraintListVec[2].setConstraintValue((uint16_t)2);

    std::vector<qcc::String> myDeviceAc_modeDisplay4Vec;
    myDeviceAc_modeDisplay4Vec.push_back("Fan");
    myDeviceAc_modeConstraintListVec[3].setDisplays(myDeviceAc_modeDisplay4Vec);
    myDeviceAc_modeConstraintListVec[3].setConstraintValue((uint16_t)3);

    std::vector<qcc::String> myDeviceAc_modeDisplay5Vec;
    myDeviceAc_modeDisplay5Vec.push_back("Off");
    myDeviceAc_modeConstraintListVec[4].setDisplays(myDeviceAc_modeDisplay5Vec);
    myDeviceAc_modeConstraintListVec[4].setConstraintValue((uint16_t)4);

    myDeviceAc_mode->setConstraintList(myDeviceAc_modeConstraintListVec);

    myDeviceStatusStringProperty = new MyDeviceStatusStringProperty("statusStringProperty", myDeviceControlsContainer, STRING_PROPERTY);
    CHECK(myDeviceControlsContainer->addChildWidget(myDeviceStatusStringProperty));

    myDeviceStatusStringProperty->setEnabled(true);
    myDeviceStatusStringProperty->setIsSecured(false);
    myDeviceStatusStringProperty->setWritable(false);
    CHECK(myDeviceStatusStringProperty->setGetValue(getStatusString));
    myDeviceStatusStringProperty->setBgColor(0x500);

    std::vector<qcc::String> myDeviceStatusStringPropertylabelVec;
    myDeviceStatusStringPropertylabelVec.push_back("Status:");
    myDeviceStatusStringProperty->setLabels(myDeviceStatusStringPropertylabelVec);

    std::vector<uint16_t> myDeviceStatusStringPropertyHintsVec;
    myDeviceStatusStringPropertyHintsVec.push_back(TEXTVIEW);
    myDeviceStatusStringProperty->setHints(myDeviceStatusStringPropertyHintsVec);

    myDeviceSet_temperature = new MyDeviceSet_temperature("set_temperature", myDeviceRootContainer, UINT16_PROPERTY);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceSet_temperature));

    myDeviceSet_temperature->setEnabled(false);
    myDeviceSet_temperature->setIsSecured(false);
    myDeviceSet_temperature->setWritable(true);
    CHECK(myDeviceSet_temperature->setGetValue(getTargetTemperature));
    myDeviceSet_temperature->setBgColor(0x008000);

    std::vector<qcc::String> myDeviceSet_temperaturelabelVec;
    myDeviceSet_temperaturelabelVec.push_back("Temperature");
    myDeviceSet_temperature->setLabels(myDeviceSet_temperaturelabelVec);

    std::vector<uint16_t> myDeviceSet_temperatureHintsVec;
    myDeviceSet_temperatureHintsVec.push_back(SLIDER);
    myDeviceSet_temperature->setHints(myDeviceSet_temperatureHintsVec);

    std::vector<qcc::String> myDeviceSet_temperatureunitMeasureVec;
    myDeviceSet_temperatureunitMeasureVec.push_back("Degrees");
    myDeviceSet_temperature->setUnitOfMeasures(myDeviceSet_temperatureunitMeasureVec);

    ConstraintRange* myDeviceSet_temperatureConstraintRange = new ConstraintRange();
    myDeviceSet_temperature->setConstraintRange(myDeviceSet_temperatureConstraintRange);
    CHECK(myDeviceSet_temperatureConstraintRange->setConstraintMin((uint16_t)50));
    CHECK(myDeviceSet_temperatureConstraintRange->setConstraintMax((uint16_t)90));
    CHECK(myDeviceSet_temperatureConstraintRange->setConstraintIncrement((uint16_t)1));

    myDeviceFan_speed = new MyDeviceFan_speed("fan_speed", myDeviceRootContainer, UINT16_PROPERTY);
    CHECK(myDeviceRootContainer->addChildWidget(myDeviceFan_speed));

    myDeviceFan_speed->setEnabled(false);
    myDeviceFan_speed->setIsSecured(false);
    myDeviceFan_speed->setWritable(true);
    CHECK(myDeviceFan_speed->setGetValue(getFanSpeed));
    myDeviceFan_speed->setBgColor(0xff69b4);

    std::vector<qcc::String> myDeviceFan_speedlabelVec;
    myDeviceFan_speedlabelVec.push_back("Fan Speed");
    myDeviceFan_speed->setLabels(myDeviceFan_speedlabelVec);

    std::vector<uint16_t> myDeviceFan_speedHintsVec;
    myDeviceFan_speedHintsVec.push_back(SPINNER);
    myDeviceFan_speed->setHints(myDeviceFan_speedHintsVec);

    std::vector<ConstraintList> myDeviceFan_speedConstraintListVec(3);

    std::vector<qcc::String> myDeviceFan_speedDisplay1Vec;
    myDeviceFan_speedDisplay1Vec.push_back("Low");
    myDeviceFan_speedConstraintListVec[0].setDisplays(myDeviceFan_speedDisplay1Vec);
    myDeviceFan_speedConstraintListVec[0].setConstraintValue((uint16_t)0);

    std::vector<qcc::String> myDeviceFan_speedDisplay2Vec;
    myDeviceFan_speedDisplay2Vec.push_back("Medium");
    myDeviceFan_speedConstraintListVec[1].setDisplays(myDeviceFan_speedDisplay2Vec);
    myDeviceFan_speedConstraintListVec[1].setConstraintValue((uint16_t)1);

    std::vector<qcc::String> myDeviceFan_speedDisplay3Vec;
    myDeviceFan_speedDisplay3Vec.push_back("High");
    myDeviceFan_speedConstraintListVec[2].setDisplays(myDeviceFan_speedDisplay3Vec);
    myDeviceFan_speedConstraintListVec[2].setConstraintValue((uint16_t)2);

    myDeviceFan_speed->setConstraintList(myDeviceFan_speedConstraintListVec);

    myDeviceTurnFanOnNotificationAction = NotificationAction::createNotificationAction(LanguageSets::get("myDeviceMyLanguages"));
    if (!myDeviceTurnFanOnNotificationAction) {
        return ER_FAIL;
    }
    CHECK(myDeviceUnit->addNotificationAction(myDeviceTurnFanOnNotificationAction));

    myDeviceTurnFanOn = new MyDeviceTurnFanOn("TurnFanOn", NULL);
    CHECK(myDeviceTurnFanOnNotificationAction->setRootWidget(myDeviceTurnFanOn));

    myDeviceTurnFanOn->setEnabled(true);
    myDeviceTurnFanOn->setIsSecured(false);

    std::vector<qcc::String> myDeviceTurnFanOnmessageVec;
    myDeviceTurnFanOnmessageVec.push_back("Turn fan on ?");
    myDeviceTurnFanOn->setMessages(myDeviceTurnFanOnmessageVec);
    myDeviceTurnFanOn->setNumActions(2);
    myDeviceTurnFanOn->setBgColor(0x789);

    std::vector<qcc::String> myDeviceTurnFanOnLabelAction1Vec;
    myDeviceTurnFanOnLabelAction1Vec.push_back("Yes");
    myDeviceTurnFanOn->setLabelsAction1(myDeviceTurnFanOnLabelAction1Vec);

    std::vector<qcc::String> myDeviceTurnFanOnLabelAction2Vec;
    myDeviceTurnFanOnLabelAction2Vec.push_back("No");
    myDeviceTurnFanOn->setLabelsAction2(myDeviceTurnFanOnLabelAction2Vec);

    myDeviceTurnFanOffNotificationAction = NotificationAction::createNotificationAction(LanguageSets::get("myDeviceMyLanguages"));
    if (!myDeviceTurnFanOffNotificationAction) {
        return ER_FAIL;
    }
    CHECK(myDeviceUnit->addNotificationAction(myDeviceTurnFanOffNotificationAction));

    myDeviceTurnFanOff = new MyDeviceTurnFanOff("TurnFanOff", NULL);
    CHECK(myDeviceTurnFanOffNotificationAction->setRootWidget(myDeviceTurnFanOff));

    myDeviceTurnFanOff->setEnabled(true);
    myDeviceTurnFanOff->setIsSecured(false);

    std::vector<qcc::String> myDeviceTurnFanOffmessageVec;
    myDeviceTurnFanOffmessageVec.push_back("Turn fan off ?");
    myDeviceTurnFanOff->setMessages(myDeviceTurnFanOffmessageVec);
    myDeviceTurnFanOff->setNumActions(2);
    myDeviceTurnFanOff->setBgColor(0x789);

    std::vector<qcc::String> myDeviceTurnFanOffLabelAction1Vec;
    myDeviceTurnFanOffLabelAction1Vec.push_back("Yes");
    myDeviceTurnFanOff->setLabelsAction1(myDeviceTurnFanOffLabelAction1Vec);

    std::vector<qcc::String> myDeviceTurnFanOffLabelAction2Vec;
    myDeviceTurnFanOffLabelAction2Vec.push_back("No");
    myDeviceTurnFanOff->setLabelsAction2(myDeviceTurnFanOffLabelAction2Vec);

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
    if (myDeviceTempAndHumidityContainer) {
        delete (myDeviceTempAndHumidityContainer);
        myDeviceTempAndHumidityContainer = 0;
    }
    if (myDeviceCurrentTempStringProperty) {
        delete (myDeviceCurrentTempStringProperty);
        myDeviceCurrentTempStringProperty = 0;
    }
    if (myDeviceCurrentHumidityStringProperty) {
        delete (myDeviceCurrentHumidityStringProperty);
        myDeviceCurrentHumidityStringProperty = 0;
    }
    if (myDeviceControlsContainer) {
        delete (myDeviceControlsContainer);
        myDeviceControlsContainer = 0;
    }
    if (myDeviceAc_mode) {
        delete (myDeviceAc_mode);
        myDeviceAc_mode = 0;
    }
    if (myDeviceStatusStringProperty) {
        delete (myDeviceStatusStringProperty);
        myDeviceStatusStringProperty = 0;
    }
    if (myDeviceSet_temperature) {
        delete (myDeviceSet_temperature);
        myDeviceSet_temperature = 0;
    }
    if (myDeviceFan_speed) {
        delete (myDeviceFan_speed);
        myDeviceFan_speed = 0;
    }
    if (myDeviceTurnFanOnNotificationAction) {
        delete (myDeviceTurnFanOnNotificationAction);
        myDeviceTurnFanOnNotificationAction = 0;
    }
    if (myDeviceTurnFanOn) {
        delete (myDeviceTurnFanOn);
        myDeviceTurnFanOn = 0;
    }
    if (myDeviceTurnFanOffNotificationAction) {
        delete (myDeviceTurnFanOffNotificationAction);
        myDeviceTurnFanOffNotificationAction = 0;
    }
    if (myDeviceTurnFanOff) {
        delete (myDeviceTurnFanOff);
        myDeviceTurnFanOff = 0;
    }

}
