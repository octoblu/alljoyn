
#include "util.h"
#include "SignalHandlerImpl.h"
#include <Message.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

SignalHandlerImpl::SignalHandlerImpl(Nan::Callback* sig){
  loop = uv_default_loop();
  signalCallback.callback = sig;
  uv_async_init(loop, &signal_async, (uv_async_cb) signal_callback);
}

SignalHandlerImpl::~SignalHandlerImpl(){
}

void SignalHandlerImpl::signal_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Local<v8::Object> msg = Nan::New<v8::Object>();
    size_t msgIndex = 0;
    const ajn::MsgArg* arg = (*holder->message)->GetArg(msgIndex);
    while(arg != NULL){
      msgArgToObject(arg, msgIndex, msg);
      msgIndex++;
      arg = (*holder->message)->GetArg(msgIndex);
    }

    v8::Local<v8::Object> sender = Nan::New<v8::Object>();

    Nan::Set(sender, 
      Nan::New<v8::String>("sender").ToLocalChecked(), 
      Nan::New<v8::String>((*holder->message)->GetSender()).ToLocalChecked()
    );
    Nan::Set(sender, 
      Nan::New<v8::String>("session_id").ToLocalChecked(), 
      Nan::New<v8::Integer>((*holder->message)->GetSessionId())
    );
    Nan::Set(sender, 
      Nan::New<v8::String>("timestamp").ToLocalChecked(), 
      Nan::New<v8::Integer>((*holder->message)->GetTimeStamp())
    );
    Nan::Set(sender, 
      Nan::New<v8::String>("member_name").ToLocalChecked(), 
      Nan::New<v8::String>((*holder->message)->GetMemberName()).ToLocalChecked()
    );
    Nan::Set(sender, 
      Nan::New<v8::String>("object_path").ToLocalChecked(), 
      Nan::New<v8::String>((*holder->message)->GetObjectPath()).ToLocalChecked()
    );
    Nan::Set(sender, 
      Nan::New<v8::String>("signature").ToLocalChecked(), 
      Nan::New<v8::String>((*holder->message)->GetSignature()).ToLocalChecked()
    );    
    // sender->Set(Nan::New<v8::String>("sender"), Nan::New<v8::String>((*holder->message)->GetSender()));
    // sender->Set(Nan::New<v8::String>("session_id"), Nan::New<v8::Integer>((*holder->message)->GetSessionId()));
    // sender->Set(Nan::New<v8::String>("timestamp"), Nan::New<v8::Integer>((*holder->message)->GetTimeStamp()));
    // sender->Set(Nan::New<v8::String>("member_name"), Nan::New<v8::String>((*holder->message)->GetMemberName()));
    // sender->Set(Nan::New<v8::String>("object_path"), Nan::New<v8::String>((*holder->message)->GetObjectPath()));
    // sender->Set(Nan::New<v8::String>("signature"), Nan::New<v8::String>((*holder->message)->GetSignature()));

    v8::Local<v8::Value> argv[] = {
      msg,
      sender
    };
    holder->callback->Call(2, argv);

    if(holder->message){
      delete holder->message;
      holder->message = NULL;
    }
}

void SignalHandlerImpl::Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message){
    signal_async.data = (void*) &signalCallback;
    signalCallback.message = new ajn::Message(message);
    uv_async_send(&signal_async);
}
