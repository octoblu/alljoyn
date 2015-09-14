#ifndef LD_INTERFACEWRAPPER_H
#define LD_INTERFACEWRAPPER_H

#include <nan.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(InterfaceDescriptionWrapper);

class InterfaceWrapper : public Nan::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(AddSignal);
    static NAN_METHOD(Activate);
  public:
    InterfaceWrapper();
    ajn::InterfaceDescription* interface;
    static void Init ();
    static v8::Local<v8::Value> NewInstance();
};

#endif
