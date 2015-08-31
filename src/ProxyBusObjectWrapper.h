#ifndef LD_PROXYBUSOBJECTWRAPPER_H
#define LD_PROXYBUSOBJECTWRAPPER_H

#include <nan.h>
#include <alljoyn/ProxyBusObject.h>

using namespace Nan;  // NOLINT(build/namespaces)
using namespace v8;
using namespace ajn;

class UVThreadSwitcher;

class ProxyBusObjectWrapper : public node::ObjectWrap, public ajn::MessageReceiver {
  public:
    static void Init(Handle<Object> target);
    ajn::ProxyBusObject* proxyObj;

  private:
    explicit ProxyBusObjectWrapper(BusAttachment& bus, const char* service, const char* path, SessionId sessionId);
    ~ProxyBusObjectWrapper();

    void asyncMethodCallback(ajn::Message& message, void* context);    
    void uvMethodCallback(void* userData);

    static NAN_METHOD(New);
    static NAN_METHOD(AddInterface);
    static NAN_METHOD(MethodCall);
    
    static Nan::Persistent<v8::Function> constructor;
};

#endif
