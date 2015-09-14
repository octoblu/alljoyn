#ifndef LD_BUSLISTENERIMPL_H
#define LD_BUSLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <BusListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include <queue>
#include <string>

class BusListenerImpl : public ajn::BusListener {
  private:
    uv_loop_t *loop;
    uv_async_t found_async, lost_async, name_change_async;

    struct CallbackHolder{
      Nan::Callback* callback;
      std::queue<std::string> dataqueue;
      uv_mutex_t datalock;

      CallbackHolder() {
        uv_mutex_init(&datalock);
      }
      ~CallbackHolder() {
        uv_mutex_destroy(&datalock);
      }
    } foundName, lostName, nameChanged;

    template<typename... Args>
      static void callback(uv_async_t *handle, Args... );

  public:
    BusListenerImpl(Nan::Callback* foundName, Nan::Callback* lostName, Nan::Callback* nameChanged);

    virtual void FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
    virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    //virtual void PropertyChanged(const char* propName, const ajn::MsgArg* propValue) { }
};

#endif
