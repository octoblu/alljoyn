#include "nan.h"

#include "SignalHandlerImpl.h"
#include <Message.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

SignalHandlerImpl::SignalHandlerImpl(NanCallback* sig){
  loop = uv_default_loop();
  signalCallback.callback = sig;
  uv_async_init(loop, &signal_async, signal_callback);
}

SignalHandlerImpl::~SignalHandlerImpl(){
}

void SignalHandlerImpl::signal_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void SignalHandlerImpl::Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message){
    printf("Got Signal for %s from id %s\n", srcPath, message->GetSender());
    signal_async.data = (void*) &signalCallback;
    //TODO message data
    signalCallback.data = message->GetSender();
    uv_async_send(&signal_async);
}


