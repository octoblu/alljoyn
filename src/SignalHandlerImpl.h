#ifndef LD_SIGNALHANDLERIMPL_H
#define LD_SIGNALHANDLERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <InterfaceDescription.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/MessageReceiver.h>
#include <Message.h>

#include <queue>
#include <memory>

class SignalHandlerImpl : public ajn::MessageReceiver {
  private:
    uv_loop_t *loop;
    uv_async_t signal_async;

    struct CallbackHolder{
      Nan::Callback* callback;
      std::queue<std::unique_ptr<ajn::Message>> messages;
      uv_mutex_t lock;
      CallbackHolder() {
        uv_mutex_init(&lock);
      }
      ~CallbackHolder() {
        uv_mutex_destroy(&lock);
      }
    } signalCallback;

    template<typename... Args>
      static void signal_callback(uv_async_t *handle, Args... );

  public:
    SignalHandlerImpl(Nan::Callback* sig);
    ~SignalHandlerImpl();

    void Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message);
};

#endif
