#ifndef LD_BUSOBJECTWRAPPER_H
#define LD_BUSOBJECTWRAPPER_H

#include <nan.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

using namespace Nan;  // NOLINT(build/namespaces)

class BusObjectImpl : public ajn::BusObject{
public:
    BusObjectImpl(const char* path);
    ~BusObjectImpl();
    QStatus AddInter(ajn::InterfaceDescription* interface);
};

class BusObjectWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(AddInterfaceInternal);
    static NAN_METHOD(Signal);

    static Persistent<v8::Function> constructor;    

  public:
  	BusObjectWrapper(const char* path);
    static void Init (v8::Handle<v8::Object> target);
    BusObjectImpl* object;
};

#endif
