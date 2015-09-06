#include "nan.h"

#include "SessionPortListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

#include <algorithm>

SessionPortListenerImpl::SessionPortListenerImpl(NanCallback* accept, NanCallback* joined){
  loop = uv_default_loop();
  acceptCallback.callback = accept;
  joinedCallback.callback = joined;
  uv_async_init(loop, &accept_async, accept_callback);
  uv_async_init(loop, &joined_async, joined_callback);
}

void SessionPortListenerImpl::accept_callback(uv_async_t *handle, int status) {
  AcceptCallbackHolder* holder = (AcceptCallbackHolder*) handle->data;

  uv_mutex_lock(&holder->datalock);
  v8::Handle<v8::Value> argv[] = {
    NanNew<v8::Integer>(holder->port),
    NanNew<v8::String>(std::move(holder->data))
  };
  v8::Handle<v8::Value> accept = holder->callback->Call(2, argv);
  holder->rval = accept->BooleanValue();
  holder->complete = true;
  uv_cond_signal(&holder->datacond);
  uv_mutex_unlock(&holder->datalock);
}

void SessionPortListenerImpl::joined_callback(uv_async_t *handle, int status) {
  JoinedCallbackHolder* holder = (JoinedCallbackHolder*) handle->data;

  std::queue<JoinedCallbackData> dataqueue;
  uv_mutex_lock(&holder->datalock);
  dataqueue = std::move(holder->dataqueue);
  uv_mutex_unlock(&holder->datalock);

  while (!dataqueue.empty()) {
    const JoinedCallbackData& data = dataqueue.front();
    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::Integer>(data.port),
      NanNew<v8::Integer>(data.id),
      NanNew<v8::String>(std::move(data.data))
    };
    holder->callback->Call(3, argv);
    dataqueue.pop();
  }
}

void SessionPortListenerImpl::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner){
  joined_async.data = (void*) &joinedCallback;

  uv_mutex_lock(&joinedCallback.datalock);
  joinedCallback.dataqueue.emplace(JoinedCallbackData{ joiner, id, sessionPort });
  uv_mutex_unlock(&joinedCallback.datalock);

  uv_async_send(&joined_async);
}

bool SessionPortListenerImpl::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts){
  accept_async.data = (void*) &acceptCallback;

  bool rval;

  uv_mutex_lock(&acceptCallback.datalock);
  acceptCallback.data = joiner;
  acceptCallback.complete = false;
  acceptCallback.port = sessionPort;
  uv_async_send(&accept_async);
  while(!acceptCallback.complete)
    uv_cond_wait(&acceptCallback.datacond, &acceptCallback.datalock);
  rval = acceptCallback.rval;
  uv_mutex_unlock(&acceptCallback.datalock);

  return rval;
}

