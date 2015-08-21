#include "nan.h"

#include "SessionPortListenerWrapper.h"
#include "SessionPortListenerImpl.h"
#include <alljoyn/AllJoynStd.h>

Persistent<v8::Function> SessionPortListenerWrapper::constructor;


SessionPortListenerWrapper::SessionPortListenerWrapper(Nan::Callback* accept, Nan::Callback* joined)
  :listener(new SessionPortListenerImpl(accept, joined)){
}

SessionPortListenerWrapper::~SessionPortListenerWrapper(){
}

void SessionPortListenerWrapper::Init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SessionPortListenerWrapper::New);
  tpl->SetClassName(Nan::New<v8::String>("SessionPortListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("SessionPortListener").ToLocalChecked(), tpl->GetFunction());

}

NAN_METHOD(SessionPortListenerWrapper::New) {
  
  if(info.Length() < 2){
    return Nan::ThrowError("SessionPortListener requires callbacks for AcceptSessionJoiner and SessionJoined.");
  }
  v8::Local<v8::Function> accept = info[0].As<v8::Function>();
  Nan::Callback *acceptCall = new Nan::Callback(accept);
  v8::Local<v8::Function> joined = info[1].As<v8::Function>();
  Nan::Callback *joinedCall = new Nan::Callback(joined);

  SessionPortListenerWrapper* obj = new SessionPortListenerWrapper(acceptCall, joinedCall);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

