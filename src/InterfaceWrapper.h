#ifndef LD_INTERFACEWRAPPER_H
#define LD_INTERFACEWRAPPER_H

#include <nan.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

using namespace Nan;  // NOLINT(build/namespaces)

class InterfaceWrapper : public node::ObjectWrap {
  private:
    static NAN_METHOD(New);
    static NAN_METHOD(AddProperty);
    static NAN_METHOD(AddMethod);
    static NAN_METHOD(AddSignal);
    static NAN_METHOD(Activate);
    static NAN_METHOD(Introspect);

  public:
  	InterfaceWrapper();
    ajn::InterfaceDescription* interface;
    static void Init(v8::Handle<v8::Object> target);

    static Persistent<v8::Function> constructor;
};

#endif