#ifndef LD_BUSCONNECTION_H
#define LD_BUSCONNECTION_H

#include <nan.h>
#include "BusObjectWrapper.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

using namespace Nan;  // NOLINT(build/namespaces)

class BusConnection : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> target);

    ajn::BusAttachment* bus;

  private:
    explicit BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent);
    ~BusConnection();

    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_METHOD(Join);
    static NAN_METHOD(Connect);
    static NAN_METHOD(Disconnect);
    static NAN_METHOD(CreateInterface);
    static NAN_METHOD(GetInterface);
    static NAN_METHOD(RegisterBusListener);
    static NAN_METHOD(RegisterBusObject);
    static NAN_METHOD(FindAdvertisedName);
    static NAN_METHOD(JoinSession);
    static NAN_METHOD(BindSessionPort);
    static NAN_METHOD(RequestName);
    static NAN_METHOD(AdvertiseName);
    static NAN_METHOD(RegisterSignalHandler);
    static NAN_METHOD(AddMatch);
    static NAN_METHOD(CreateInterfacesFromXml);
    static NAN_METHOD(RegisterAboutListener);
    static NAN_METHOD(WhoImplements);
    
    static Persistent<v8::Function> constructor;
};

#endif