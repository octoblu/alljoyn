#ifndef LD_BUSCONNECTION_H
#define LD_BUSCONNECTION_H

#include <nan.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(AlljoynBus);

class BusConnection : public node::ObjectWrap {
  private:
  	ajn::BusAttachment* bus;
  	QStatus status;

    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_METHOD(Join);
  public:
  	BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent);
    static void Init ();
    static v8::Handle<v8::Value> NewInstance(v8::Local<v8::String> &appName);
};

#endif