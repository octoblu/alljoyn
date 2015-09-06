#include "nan.h"

#include "util.h"
#include "SignalHandlerImpl.h"
#include <Message.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

#include <algorithm>

SignalHandlerImpl::SignalHandlerImpl(Nan::Callback* sig){
  loop = uv_default_loop();
  signalCallback.callback = sig;
  uv_async_init(loop, &signal_async, signal_callback);
}

SignalHandlerImpl::~SignalHandlerImpl(){
}

template<typename... Args>
void SignalHandlerImpl::signal_callback(uv_async_t *handle, Args... ) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    Nan::HandleScope scope;

    std::queue<std::unique_ptr<ajn::Message>> messages;
    uv_mutex_lock(&holder->lock);
    messages = std::move(holder->messages);
    uv_mutex_unlock(&holder->lock);

    while(!messages.empty()) {
      std::unique_ptr<ajn::Message> message = std::move(messages.front());

      v8::Local<v8::Object> msg = Nan::New<v8::Object>();
      size_t msgIndex = 0;
      const ajn::MsgArg* arg = (*message)->GetArg(msgIndex);
      while(arg != NULL){
        msgArgToObject(arg, msgIndex, msg);
        msgIndex++;
        arg = (*message)->GetArg(msgIndex);
      }

      v8::Local<v8::Object> sender = Nan::New<v8::Object>();
      Nan::Set(sender, Nan::New<v8::String>("sender").ToLocalChecked(),
               Nan::New<v8::String>((*message)->GetSender()).ToLocalChecked());
      Nan::Set(sender, Nan::New<v8::String>("session_id").ToLocalChecked(),
               Nan::New<v8::Integer>((*message)->GetSessionId()));
      Nan::Set(sender, Nan::New<v8::String>("timestamp").ToLocalChecked(),
               Nan::New<v8::Integer>((*message)->GetTimeStamp()));
      Nan::Set(sender, Nan::New<v8::String>("member_name").ToLocalChecked(),
               Nan::New<v8::String>((*message)->GetMemberName()).ToLocalChecked());
      Nan::Set(sender, Nan::New<v8::String>("object_path").ToLocalChecked(),
               Nan::New<v8::String>((*message)->GetObjectPath()).ToLocalChecked());
      Nan::Set(sender, Nan::New<v8::String>("signature").ToLocalChecked(),
               Nan::New<v8::String>((*message)->GetSignature()).ToLocalChecked());

      v8::Local<v8::Value> argv[] = {
        msg,
        sender
      };
      holder->callback->Call(2, argv);

      messages.pop();
    }
}

void SignalHandlerImpl::Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message){
    signal_async.data = (void*) &signalCallback;

    uv_mutex_lock(&signalCallback.lock);
    signalCallback.messages.emplace(new ajn::Message(message));
    uv_mutex_unlock(&signalCallback.lock);

    uv_async_send(&signal_async);
}
