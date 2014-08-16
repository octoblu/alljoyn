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
}

SessionPortListenerImpl::~SessionPortListenerImpl(){
  printf("SessionPortListenerImpl-destructor\n");
  if(acceptCallback.callback){
    delete acceptCallback.callback;
  }
  if(joinedCallback.callback){
    delete joinedCallback.callback;
  }
}

void SessionPortListenerImpl::accept_callback(uv_async_t *handle, int status) {
    printf("Calling accept callback\n");
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    v8::Handle<v8::Value> accept = holder->callback->Call(1, argv);
    holder->rval = accept->BooleanValue();
    holder->callback_finished = true;
}

void SessionPortListenerImpl::joined_callback(uv_async_t *handle, int status) {
    printf("Calling joined callback\n");
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void SessionPortListenerImpl::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner){
    printf("Got SessionJoined for 0x%x from id 0x%x\n", sessionPort, id);
    joined_async.data = (void*) &joinedCallback;
    joinedCallback.data = joiner;
    uv_async_send(&joined_async);
}

bool SessionPortListenerImpl::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts){
    printf("Got AcceptSessionJoiner for %s on port 0x%x\n", joiner, sessionPort);
    accept_async.data = (void*) &acceptCallback;
    acceptCallback.data = joiner;
    acceptCallback.callback_finished = false;
    uv_async_send(&accept_async);
    while(!acceptCallback.callback_finished){}
    return acceptCallback.rval;
}

