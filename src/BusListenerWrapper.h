#ifndef LD_BUSLISTENERWRAPPER_H
#define LD_BUSLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <BusListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include "BusListenerImpl.h"

using namespace Nan;  // NOLINT(build/namespaces)

class BusListenerWrapper : public node::ObjectWrap {
  public:
	static void Init(v8::Handle<v8::Object> target);
    BusListenerImpl *listener;

  private:
  	BusListenerWrapper(Nan::Callback* foundName, Nan::Callback* lostName, Nan::Callback* nameChanged);
  	~BusListenerWrapper();

    static NAN_METHOD(New);

    static Persistent<v8::Function> constructor;

};

#endif