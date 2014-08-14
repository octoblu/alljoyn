#ifndef LD_BUSLISTENERWRAPPER_H
#define LD_BUSLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <BusListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include "BusListenerImpl.h"

NAN_METHOD(BusListenerConstructor);

class BusListenerWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
  	BusListenerWrapper(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged);
  	~BusListenerWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    BusListenerImpl *listener;
};

#endif