#include <nan.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

class BusBinding : public BusListener, public SessionPortListener, public SessionListener {
    int sessionId;
    ajn::BusAttachment* bus;
};