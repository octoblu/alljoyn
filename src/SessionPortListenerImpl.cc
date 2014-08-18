#include "nan.h"

#include "SessionPortListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

SessionPortListenerImpl::SessionPortListenerImpl(NanCallback* accept, NanCallback* joined){
  loop = uv_default_loop();
  acceptCallback.callback = accept;
  joinedCallback.callback = joined;
  uv_async_init(loop, &accept_async, accept_callback);
  uv_async_init(loop, &joined_async, joined_callback);
  uv_rwlock_init(&calllock);
}

SessionPortListenerImpl::~SessionPortListenerImpl(){
  if(acceptCallback.callback){
    delete acceptCallback.callback;
  }
  if(joinedCallback.callback){
    delete joinedCallback.callback;
  }
}

void SessionPortListenerImpl::accept_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    v8::Handle<v8::Value> accept = holder->callback->Call(1, argv);
    holder->rval = accept->BooleanValue();

    printf("accept_callback result : %s\n", holder->rval ? "TRUE" : "FALSE");
    holder->complete = true;
}

void SessionPortListenerImpl::joined_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void SessionPortListenerImpl::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner){
    joined_async.data = (void*) &joinedCallback;
    joinedCallback.data = joiner;
    uv_async_send(&joined_async);
}

bool SessionPortListenerImpl::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts){
    accept_async.data = (void*) &acceptCallback;
    acceptCallback.data = joiner;
    acceptCallback.complete = false;
    uv_async_send(&accept_async);
    while(!acceptCallback.complete){sleep(1);}
    return acceptCallback.rval;
}

