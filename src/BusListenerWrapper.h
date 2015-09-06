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

class BusListenerWrapper : public Nan::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
    BusListenerWrapper(Nan::Callback* foundName, Nan::Callback* lostName, Nan::Callback* nameChanged);
    ~BusListenerWrapper();

    static void Init ();

    BusListenerImpl *listener;
};

#endif
