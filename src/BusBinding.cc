#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

void BusBinding::FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
    bus->EnableConcurrentCallbacks();
    status = bus->SetLinkTimeout(sessionId, timeout);
    //TODO async found callback
}
void BusBinding::LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
    printf("LostAdvertisedName for %s from transport 0x%x\n", name, transport);
}
void BusBinding::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
    printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s\n", busName, previousOwner ? previousOwner : "<none>",
           newOwner ? newOwner : "<none>");
}
bool BusBinding::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
    //TODO accept/reject binding callback
    bool accept = true;
    if (accept) {
        printf("Rejecting join attempt on session port %d\n", sessionPort);
    }else{
        printf("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x)\n",
           joiner, opts.proximity, opts.traffic, opts.transports);
    }

    return accept;
}

void BusBinding::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
    sessionId = id;
    printf("SessionJoined with %s (id=%d)\n", joiner, id);
    bus->EnableConcurrentCallbacks();
    //TODO set (optional) timeout via config
    uint32_t timeout = 20;
    QStatus status = bus->SetLinkTimeout(sessionId, timeout);
    if (ER_OK == status) {
        printf("Set link timeout to %d\n", timeout);
    } else {
        printf("Set link timeout failed\n");
    }
}