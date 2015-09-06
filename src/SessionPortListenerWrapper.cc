#include "nan.h"

#include "SessionPortListenerWrapper.h"
#include "SessionPortListenerImpl.h"
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> portlistener_constructor;

NAN_METHOD(SessionPortListenerConstructor) {
  if(info.Length() < 2){
    return Nan::ThrowError("SessionPortListener requires callbacks for AcceptSessionJoiner and SessionJoined.");
  }
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(portlistener_constructor);

  v8::Handle<v8::Value> argv[] = {
    info[0],
    info[1],
    info[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  info.GetReturnValue().Set(obj);
}

SessionPortListenerWrapper::SessionPortListenerWrapper(Nan::Callback* accept, Nan::Callback* joined)
  :listener(new SessionPortListenerImpl(accept, joined)){
}

SessionPortListenerWrapper::~SessionPortListenerWrapper(){
}

void SessionPortListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SessionPortListenerWrapper::New);
  portlistener_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("SessionPortListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
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

