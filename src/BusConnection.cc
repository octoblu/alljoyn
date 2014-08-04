#include "BusConnection.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

BusAttachment::BusAttachment(const char* shortName):connected(0){
    status = ER_OK;
    bus = new BusAttachment(shortName, true);
    if (!s_bus) {
        status = ER_OUT_OF_MEMORY;
    }
}

void BusAttachment::createInterface(const char* interfaceName, const char* signalName, const char* signalParams, const char* signalArgs)){
	InterfaceDescription* interface = NULL;
    status = bus->CreateInterface(interfaceName, interface);

    if (ER_OK == status) {
        interface->AddSignal(signalName, signalParams,  signalArgs, 0);
        interface->Activate();
    } else {
        printf("Failed to create interface \"%s\" (%s)\n", interfaceName, QCC_StatusText(status));
    }

    return status;
}

void BusAttachment::connectInterface(){
	bus->RegisterBusListener(binding);
}