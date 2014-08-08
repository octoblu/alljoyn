#ifndef LD_BUSCONNECTION_H
#define LD_BUSCONNECTION_H

#include <nan.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(BusAttachmentWrapper);

class BusConnection : public node::ObjectWrap {
  private:
  	ajn::BusAttachment* bus;

    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_METHOD(Join);
    static NAN_METHOD(CreateInterface);
    static NAN_METHOD(RegisterBusListener);
  public:
  	BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent);
    static void Init ();
    static v8::Handle<v8::Value> NewInstance(v8::Local<v8::String> &appName);
};

#endif