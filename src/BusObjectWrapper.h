#ifndef LD_BUSOBJECTWRAPPER_H
#define LD_BUSOBJECTWRAPPER_H

#include <nan.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(BusObjectConstructor);

class BusObjectImpl : public ajn::BusObject{
    uv_loop_t *loop;
    uv_async_t signal_async;

    struct CallbackHolder{
      NanCallback* callback;
      ajn::Message *message;
    } signalCallback;

	public:
		BusObjectImpl(const char* path);
    ~BusObjectImpl();
    static void signal_callback(uv_async_t *handle, int status);
	  QStatus AddInter(ajn::InterfaceDescription* interface);
    void ReceiveSignal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message);
    void SetSignalCallback(NanCallback* callback);
};

class BusObjectWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(AddInterfaceInternal);
    static NAN_METHOD(Signal);
  public:
  	BusObjectWrapper(const char* path);
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();
    BusObjectImpl* object;
};


#endif