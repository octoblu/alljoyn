#ifndef LD_SESSIONPORTLISTENERIMPL_H
#define LD_SESSIONPORTLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <SessionPortListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include <queue>
#include <string>

class SessionPortListenerImpl : public ajn::SessionPortListener {
  private:
    uv_loop_t *loop;
    uv_async_t accept_async, joined_async;

    struct AcceptCallbackHolder {
      Nan::Callback *callback;
      std::string data;
      ajn::SessionPort port;
      bool rval;
      bool complete;
      uv_mutex_t datalock;
      uv_cond_t datacond;
      AcceptCallbackHolder() {
        uv_mutex_init(&datalock);
        uv_cond_init(&datacond);
      }
      ~AcceptCallbackHolder() {
        uv_mutex_destroy(&datalock);
        uv_cond_destroy(&datacond);
      }
    } acceptCallback;

    struct JoinedCallbackData {
      std::string data;
      ajn::SessionId id;
      ajn::SessionPort port;
    };

    struct JoinedCallbackHolder {
      Nan::Callback* callback;
      std::queue<JoinedCallbackData> dataqueue;
      uv_mutex_t datalock;
      JoinedCallbackHolder() {
        uv_mutex_init(&datalock);
      }
      ~JoinedCallbackHolder() {
        uv_mutex_destroy(&datalock);
      }
    } joinedCallback;

    template<typename... Args>
      static void accept_callback(uv_async_t *handle, Args... );
    template<typename... Args>
      static void joined_callback(uv_async_t *handle, Args... );

  public:
    SessionPortListenerImpl(Nan::Callback* accept, Nan::Callback* joined);

    virtual bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
    virtual void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);
};

#endif
