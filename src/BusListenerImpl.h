#ifndef LD_BUSLISTENERIMPL_H
#define LD_BUSLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <BusListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

class BusListenerImpl : public ajn::BusListener {
  private:
  	uv_loop_t *loop;
  	uv_async_t found_async, lost_async, name_change_async;

    struct CallbackHolder{
      NanCallback* callback;
      char* data;
      uv_rwlock_t datalock;
    } foundName, lostName, nameChanged;

  public:
  	BusListenerImpl(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged);
  	~BusListenerImpl();
  	static void found_callback(uv_async_t *handle, int status);
  	static void lost_callback(uv_async_t *handle, int status);
  	static void name_change_callback(uv_async_t *handle, int status);

    virtual void FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    //virtual void PropertyChanged(const char* propName, const ajn::MsgArg* propValue) { }
};

#endif
