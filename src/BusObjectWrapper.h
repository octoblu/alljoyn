#ifndef LD_BUSOBJECTWRAPPER_H
#define LD_BUSOBJECTWRAPPER_H

#include <nan.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>
#include <unordered_map>

class UVThreadSwitcher;

using namespace Nan;  // NOLINT(build/namespaces)

class BusObjectImpl : public ajn::BusObject{
public:
    BusObjectImpl(const char* path);
    ~BusObjectImpl();
    QStatus AddInterface(ajn::InterfaceDescription* interface);
    QStatus AddMethodHandlers(ajn::InterfaceDescription* interface, v8::Local<v8::Object> v8CallbackObject);
    void MethodHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& message);
    void v8MethodHandler(void *userData);
  private:
    UVThreadSwitcher* switcher;
    std::unordered_map<const ajn::InterfaceDescription*, Nan::Callback*> v8CallbackMap;
};

class BusObjectWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(AddInterfaceInternal);
    static NAN_METHOD(Signal);

    static Nan::Persistent<v8::Function> constructor;    

  public:
  	BusObjectWrapper(const char* path);
    static void Init (v8::Handle<v8::Object> target);
    BusObjectImpl* object;
};

#endif
