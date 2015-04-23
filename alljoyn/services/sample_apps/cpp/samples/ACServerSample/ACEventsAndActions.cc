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

#include "ACEventsAndActions.h"
#include <alljoyn/BusObject.h>
#include <stdio.h>
#include "ControlPanelProvided.h"
#include <vector>
#include <alljoyn/about/AboutServiceApi.h>
#include <alljoyn/AllJoynStd.h>

namespace ajn {
namespace services {

static const char* EVENTS_INTERFACE_NAME = "org.alljoyn.ACServerSample.Events";
static const char* EVENT_NAME_TEMPERATURE_80_REACHED = "temperature80Reached";
static const char* EVENT_NAME_TEMPERATURE_60_REACHED = "temperature60Reached";
static const char* EVENT_NAME_MODE_SET_TO_OFF = "ModeSetToOff";
static const char* EVENT_NAME_MODE_SET_TO_ON = "ModeSetToOn";
static const char* PROPERTY_NAME_VERSION = "Version";
static const uint16_t AC_EVENTS_AND_ACTIONS_VERSION = 1;

static const char* ACTIONS_INTERFACE_NAME = "org.alljoyn.ACServerSample.Actions";
static const char* ACTION_SET_MODE_TO_AUTO = "SetModeToAuto";
static const char* ACTION_SET_MODE_TO_COOL = "SetModeToCool";
static const char* ACTION_SET_MODE_TO_HEAT = "SetModeToHeat";
static const char* ACTION_SET_MODE_TO_FAN = "SetModeToFan";
static const char* ACTION_SET_MODE_TO_OFF = "SetModeToOff";

static const char* AC_EVENTS_AND_ACTIONS_SERVICE_PATH = "/MyDeviceEventsAndActions";

ACEventsAndActions::ACEventsAndActions(BusAttachment* busAttachment) :
    BusObject(AC_EVENTS_AND_ACTIONS_SERVICE_PATH),
    bus(busAttachment),
    temperature80ReachedMember(NULL),
    temperature60ReachedMember(NULL),
    modeSetToOffMember(NULL),
    modeSetToOnMember(NULL)
{
    printf("ACEventsAndActions::ACEventsAndActions()\n");
    QStatus status = ER_OK;

    EventsInterface();
    ActionsInterface();

    //bus object
    SetDescription("en", "Events and Actions");
    status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {
        printf("Could not register the ConfigService BusObject.\n");
        return;
    }
}

void ACEventsAndActions::EventsInterface()
{
    //org.alljoyn.ACServerSample.Events
    QStatus status = ER_OK;
    InterfaceDescription* intf = const_cast<InterfaceDescription*>(bus->GetInterface(EVENTS_INTERFACE_NAME));
    if (!intf) {
        status = bus->CreateInterface(EVENTS_INTERFACE_NAME, intf, AJ_IFC_SECURITY_OFF);
        if (status != ER_OK) {
            printf("Failed to create interface %s\n", EVENTS_INTERFACE_NAME);
            return;
        }

        if (!intf) {
            printf("Failed to create interface %s\n", EVENTS_INTERFACE_NAME);
            return;
        }

        status = intf->AddSignal(EVENT_NAME_TEMPERATURE_80_REACHED, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add signal %s\n", EVENT_NAME_TEMPERATURE_80_REACHED);
            return;
        }

        status = intf->AddSignal(EVENT_NAME_TEMPERATURE_60_REACHED, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add signal %s\n", EVENT_NAME_TEMPERATURE_60_REACHED);
            return;
        }

        status = intf->AddSignal(EVENT_NAME_MODE_SET_TO_OFF, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add signal %s\n", EVENT_NAME_MODE_SET_TO_OFF);
            return;
        }

        status = intf->AddSignal(EVENT_NAME_MODE_SET_TO_ON, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add signal %s\n", EVENT_NAME_MODE_SET_TO_ON);
            return;
        }

        status = intf->AddProperty(PROPERTY_NAME_VERSION, "q", PROP_ACCESS_READ);
        if (status != ER_OK) {
            printf("Failed to add property %s\n", PROPERTY_NAME_VERSION);
            return;
        }

        intf->SetDescriptionLanguage("en");
        intf->SetDescription("Events of the air conditioner");
        intf->SetMemberDescription(EVENT_NAME_TEMPERATURE_80_REACHED, "Triggerred when extreme temperature of 80F is reached while heating");
        intf->SetMemberDescription(EVENT_NAME_TEMPERATURE_60_REACHED, "Triggerred when extreme temperature of 60F is reached while cooling");
        intf->SetMemberDescription(EVENT_NAME_MODE_SET_TO_OFF, "Triggerred when the AC is turned OFF");
        intf->SetMemberDescription(EVENT_NAME_MODE_SET_TO_ON, "Triggerred when the AC is turned ON");
        intf->Activate();
    }

    status = AddInterface(*intf);
    if (status != ER_OK) {
        printf("AddInterface failed\n");
        return;
    }

    temperature80ReachedMember = intf->GetMember(EVENT_NAME_TEMPERATURE_80_REACHED);
    temperature60ReachedMember = intf->GetMember(EVENT_NAME_TEMPERATURE_60_REACHED);
    modeSetToOffMember = intf->GetMember(EVENT_NAME_MODE_SET_TO_OFF);
    modeSetToOnMember = intf->GetMember(EVENT_NAME_MODE_SET_TO_ON);

}

void ACEventsAndActions::ActionsInterface()
{
    //org.alljoyn.ACServerSample.Actions
    QStatus status = ER_OK;
    InterfaceDescription* intf = const_cast<InterfaceDescription*>(bus->GetInterface(ACTIONS_INTERFACE_NAME));
    if (!intf) {
        status = bus->CreateInterface(ACTIONS_INTERFACE_NAME, intf, AJ_IFC_SECURITY_OFF);
        if (status != ER_OK) {
            printf("Failed to create interface %s\n", ACTIONS_INTERFACE_NAME);
            return;
        }

        if (!intf) {
            printf("Failed to create interface %s\n", ACTIONS_INTERFACE_NAME);
            return;
        }

        status = intf->AddMethod(ACTION_SET_MODE_TO_AUTO, NULL, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add method %s\n", ACTION_SET_MODE_TO_AUTO);
            return;
        }

        status = intf->AddMethod(ACTION_SET_MODE_TO_COOL, NULL, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add method %s\n", ACTION_SET_MODE_TO_COOL);
            return;
        }

        status = intf->AddMethod(ACTION_SET_MODE_TO_HEAT, NULL, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add method %s\n", ACTION_SET_MODE_TO_HEAT);
            return;
        }

        status = intf->AddMethod(ACTION_SET_MODE_TO_FAN, NULL, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add method %s\n", ACTION_SET_MODE_TO_FAN);
            return;
        }

        status = intf->AddMethod(ACTION_SET_MODE_TO_OFF, NULL, NULL, NULL);
        if (status != ER_OK) {
            printf("Failed to add method %s\n", ACTION_SET_MODE_TO_OFF);
            return;
        }

        status = intf->AddProperty(PROPERTY_NAME_VERSION, "q", PROP_ACCESS_READ);
        if (status != ER_OK) {
            printf("Failed to add property %s\n", PROPERTY_NAME_VERSION);
            return;
        }
    }

    intf->SetDescriptionLanguage("en");
    intf->SetDescription("Actions of the air conditioner");
    intf->SetMemberDescription(ACTION_SET_MODE_TO_AUTO, "Action set mode to auto");
    intf->SetMemberDescription(ACTION_SET_MODE_TO_COOL, "Action set mode to cool");
    intf->SetMemberDescription(ACTION_SET_MODE_TO_HEAT, "Action set mode to heat");
    intf->SetMemberDescription(ACTION_SET_MODE_TO_FAN, "Action set mode to fan");
    intf->SetMemberDescription(ACTION_SET_MODE_TO_OFF, "Action set mode to off");
    intf->Activate();

    status = AddInterface(*intf);
    if (status != ER_OK) {
        printf("AddInterface failed\n");
        return;
    }

    //Add the handlers
    status = AddMethodHandler(intf->GetMember(ACTION_SET_MODE_TO_AUTO),
                              static_cast<MessageReceiver::MethodHandler>(&ACEventsAndActions::ActionSetModeToAuto));
    if (status != ER_OK) {
        printf("Failed to add method handler for %s\n", ACTION_SET_MODE_TO_AUTO);
        return;
    }

    status = AddMethodHandler(intf->GetMember(ACTION_SET_MODE_TO_COOL),
                              static_cast<MessageReceiver::MethodHandler>(&ACEventsAndActions::ActionSetModeToCool));
    if (status != ER_OK) {
        printf("Failed to add method handler for %s\n", ACTION_SET_MODE_TO_COOL);
        return;
    }

    status = AddMethodHandler(intf->GetMember(ACTION_SET_MODE_TO_HEAT),
                              static_cast<MessageReceiver::MethodHandler>(&ACEventsAndActions::ActionSetModeToHeat));
    if (status != ER_OK) {
        printf("Failed to add method handler for %s\n", ACTION_SET_MODE_TO_HEAT);
        return;
    }

    status = AddMethodHandler(intf->GetMember(ACTION_SET_MODE_TO_FAN),
                              static_cast<MessageReceiver::MethodHandler>(&ACEventsAndActions::ActionSetModeToFan));
    if (status != ER_OK) {
        printf("Failed to add method handler for %s\n", ACTION_SET_MODE_TO_FAN);
        return;
    }

    status = AddMethodHandler(intf->GetMember(ACTION_SET_MODE_TO_OFF),
                              static_cast<MessageReceiver::MethodHandler>(&ACEventsAndActions::ActionSetModeToOff));
    if (status != ER_OK) {
        printf("Failed to add method handler for %s\n", ACTION_SET_MODE_TO_OFF);
        return;
    }

}

void ACEventsAndActions::Check_MethodReply(const Message& msg, QStatus status)
{
    //check it the ALLJOYN_FLAG_NO_REPLY_EXPECTED exists if so send response ER_INVALID_DATA
    if (!(msg->GetFlags() & ALLJOYN_FLAG_NO_REPLY_EXPECTED)) {
        status = MethodReply(msg, status);
        if (status != ER_OK) {
            printf("Method did not execute successfully.\n");
        }
    }
}

void ACEventsAndActions::ActionSetModeToAuto(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    printf("ACEventsAndActions::ActionSetModeToAuto\n");
    setCurrentMode(0);
    Check_MethodReply(msg, ER_OK);
}
void ACEventsAndActions::ActionSetModeToCool(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    printf("ACEventsAndActions::ActionSetModeToCool\n");
    setCurrentMode(1);
    Check_MethodReply(msg, ER_OK);
}
void ACEventsAndActions::ActionSetModeToHeat(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    printf("ACEventsAndActions::ActionSetModeToHeat\n");
    setCurrentMode(2);
    Check_MethodReply(msg, ER_OK);
}
void ACEventsAndActions::ActionSetModeToFan(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    printf("ACEventsAndActions::ActionSetModeToFan\n");
    setCurrentMode(3);
    Check_MethodReply(msg, ER_OK);
}
void ACEventsAndActions::ActionSetModeToOff(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    printf("ACEventsAndActions::ActionSetModeToOff\n");
    setCurrentMode(4);
    Check_MethodReply(msg, ER_OK);
}

ACEventsAndActions::~ACEventsAndActions() {

}

QStatus ACEventsAndActions::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    printf("Get property was called.");

    if (0 != strcmp(PROPERTY_NAME_VERSION, propName)) {
        printf("Called for property different than version.\n");
        return ER_BUS_NO_SUCH_PROPERTY;
    }

    val.typeId = ALLJOYN_UINT16;
    val.v_uint16 = AC_EVENTS_AND_ACTIONS_VERSION;
    return ER_OK;
}

QStatus ACEventsAndActions::Set(const char* ifcName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

QStatus ACEventsAndActions::SendEvent(const InterfaceDescription::Member* actualEvent)
{
    printf("SendEvent called\n");

    if (!actualEvent) {
        printf("signalMethod not set. Can't send signal.\n");
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    uint8_t flags =  ALLJOYN_FLAG_SESSIONLESS;
    QStatus status = Signal(NULL, 0, *actualEvent, NULL, 0, 0, flags);
    if (status != ER_OK) {
        printf("Could not send signal.\n");
        return status;
    }

    printf("Sent signal successfully\n");
    return status;
}

void ACEventsAndActions::SendEventsForActions()
{
    uint8_t sendEvents = getEventsToSend();
    if (sendEvents > 0) {
        // 0x01 == need to send event 80F reached whilst heating
        // 0x02 == need to send event 60F reached whilst cooling
        // 0x04 == need to send event mode turned off
        // 0x08 == need to send event mode turned on

        if ((sendEvents & (1 << 0)) != 0) {
            printf("Going to SendEvent(temperature80ReachedMember)\n");
            SendEvent(temperature80ReachedMember);
        }
        if ((sendEvents & (1 << 1)) != 0) {
            printf("Going to SendEvent(temperature60ReachedMember)\n");
            SendEvent(temperature60ReachedMember);
        }
        if ((sendEvents & (1 << 2)) != 0) {
            printf("Going to SendEvent(modeSetToOffMember)\n");
            SendEvent(modeSetToOffMember);
        }
        if ((sendEvents & (1 << 3)) != 0) {
            printf("Going to SendEvent(modeSetToOnMember)\n");
            SendEvent(modeSetToOnMember);
        }
        resetEventsToSend();
    }
}

} //namespace services
} //namespace ajn
