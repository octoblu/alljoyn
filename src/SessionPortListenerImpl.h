#ifndef LD_SESSIONPORTLISTENERIMPL_H
#define LD_SESSIONPORTLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <SessionPortListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

class SessionPortListenerImpl : public ajn::SessionPortListener {
  private:
  	uv_loop_t *loop;
  	uv_async_t accept_async, joined_async;
    uv_rwlock_t calllock;

  struct CallbackHolder{
    NanCallback* callback;
    const char* data;
    int id, port;
    bool rval;
    bool complete;
  } acceptCallback, joinedCallback;

  public:
  	SessionPortListenerImpl(NanCallback* accept, NanCallback* joined);
  	~SessionPortListenerImpl();
  	static void accept_callback(uv_async_t *handle, int status);
  	static void joined_callback(uv_async_t *handle, int status);

    virtual bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
    virtual void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);
};

#endif