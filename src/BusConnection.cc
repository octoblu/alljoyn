#include "BusConnection.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

BusAttachment::BusAttachment(std::string shortName, std::string interfaceName){
    bus = new BusAttachment(shortName, true);
    interface = NULL;
    status = s_bus->CreateInterface(interfaceName, chatIntf);
    if (ER_OK == status) {
        interface->Activate();
    } else {
        printf("Failed to create interface \"%s\" (%s)\n", interfaceName, QCC_StatusText(status));
    }
}