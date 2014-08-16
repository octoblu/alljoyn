#include "nan.h"

#include "SessionPortListenerWrapper.h"
#include "SessionPortListenerImpl.h"
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> portlistener_constructor;

v8::Handle<v8::Value> SessionPortListenerWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(portlistener_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(SessionPortListenerConstructor) {
  NanScope();
  if(args.Length() < 2){
    return NanThrowError("SessionPortListener requires callbacks for AcceptSessionJoiner and SessionJoined.");
  }
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(portlistener_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  NanReturnValue(obj);
}

SessionPortListenerWrapper::SessionPortListenerWrapper(NanCallback* accept, NanCallback* joined)
  :listener(new SessionPortListenerImpl(accept, joined)){
}

SessionPortListenerWrapper::~SessionPortListenerWrapper(){
  printf("SessionPortListenerWrapper-destructor\n");
  if(listener){
    delete listener;
  }
}

void SessionPortListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(SessionPortListenerWrapper::New);
  NanAssignPersistent(portlistener_constructor, tpl);
  tpl->SetClassName(NanSymbol("SessionPortListener"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(SessionPortListenerWrapper::New) {
  NanScope();
  if(args.Length() < 2){
    return NanThrowError("SessionPortListener requires callbacks for AcceptSessionJoiner and SessionJoined.");
  }
  v8::Local<v8::Function> accept = args[0].As<v8::Function>();
  NanCallback *acceptCall = new NanCallback(accept);
  v8::Local<v8::Function> joined = args[1].As<v8::Function>();
  NanCallback *joinedCall = new NanCallback(joined);

  SessionPortListenerWrapper* obj = new SessionPortListenerWrapper(acceptCall, joinedCall);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

