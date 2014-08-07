#ifndef LD_INTERFACEWRAPPER_H
#define LD_INTERFACEWRAPPER_H

#include <nan.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

class InterfaceWrapper : public node::ObjectWrap {
  private:
  	ajn::InterfaceDescription* interface;

    static NAN_METHOD(New);
    static NAN_METHOD(AddSignal);
    static NAN_METHOD(Activate);
  public:
  	InterfaceWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();
};

#endif