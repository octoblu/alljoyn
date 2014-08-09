#ifndef LD_BUSOBJECTWRAPPER_H
#define LD_BUSOBJECTWRAPPER_H

#include <nan.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(BusObjectConstructor);

class BusObjectWrapper : public ajn::BusObject, public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(AddInterfaceInternal);
  public:
  	BusObjectWrapper(const char* path);
  	~BusObjectWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();
};

#endif