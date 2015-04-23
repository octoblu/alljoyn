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

#ifndef CONTROLPANELGENERATED_H_
#define CONTROLPANELGENERATED_H_

#include <alljoyn/controlpanel/ControlPanelControllee.h>
#include <alljoyn/controlpanel/Container.h>
#include <alljoyn/controlpanel/Label.h>
#include <alljoyn/controlpanel/ActionWithDialog.h>
#include "../generated/MyDeviceCurrentTempStringProperty.h"
#include "../generated/MyDeviceCurrentHumidityStringProperty.h"
#include "../generated/MyDeviceAc_mode.h"
#include "../generated/MyDeviceStatusStringProperty.h"
#include "../generated/MyDeviceSet_temperature.h"
#include "../generated/MyDeviceFan_speed.h"
#include "../generated/MyDeviceTurnFanOn.h"
#include "../generated/MyDeviceTurnFanOff.h"


/**
 * Generated Class - do not change code
 */
class ControlPanelGenerated {
  public:

    static QStatus PrepareWidgets(ajn::services::ControlPanelControllee*& controlPanelControllee);

    static void PrepareLanguageSets();

    static void Shutdown();

    static bool languageSetsDone;

    static ajn::services::ControlPanelControlleeUnit* myDeviceUnit;

    static ajn::services::ControlPanel* myDeviceRootContainerControlPanel;

    static ajn::services::Container* myDeviceRootContainer;

    static ajn::services::Container* myDeviceTempAndHumidityContainer;

    static ajn::services::MyDeviceCurrentTempStringProperty* myDeviceCurrentTempStringProperty;

    static ajn::services::MyDeviceCurrentHumidityStringProperty* myDeviceCurrentHumidityStringProperty;

    static ajn::services::Container* myDeviceControlsContainer;

    static ajn::services::MyDeviceAc_mode* myDeviceAc_mode;

    static ajn::services::MyDeviceStatusStringProperty* myDeviceStatusStringProperty;

    static ajn::services::MyDeviceSet_temperature* myDeviceSet_temperature;

    static ajn::services::MyDeviceFan_speed* myDeviceFan_speed;

    static ajn::services::NotificationAction* myDeviceTurnFanOnNotificationAction;

    static ajn::services::MyDeviceTurnFanOn* myDeviceTurnFanOn;

    static ajn::services::NotificationAction* myDeviceTurnFanOffNotificationAction;

    static ajn::services::MyDeviceTurnFanOff* myDeviceTurnFanOff;


};

#endif /* CONTROLPANELGENERATED_H_ */
