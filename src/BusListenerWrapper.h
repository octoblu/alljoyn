#ifndef LD_BUSLISTENERWRAPPER_H
#define LD_BUSLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <BusListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(BusListenerConstructor);

class BusListenerWrapper : public ajn::BusListener, public node::ObjectWrap {
  private:
  	uv_loop_t *loop;
  	uv_async_t found_async, lost_async, name_change_async;
  	NanCallback *foundNameCallback;
  	NanCallback *lostNameCallback;
  	NanCallback *nameChangedCallback;

    static NAN_METHOD(New);
  public:
  	BusListenerWrapper(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged);
  	~BusListenerWrapper();
  	static void found_callback(uv_async_t *handle, int status);
  	static void lost_callback(uv_async_t *handle, int status);
  	static void name_change_callback(uv_async_t *handle, int status);
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    virtual void FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    //virtual void PropertyChanged(const char* propName, const ajn::MsgArg* propValue) { }
};

#endif