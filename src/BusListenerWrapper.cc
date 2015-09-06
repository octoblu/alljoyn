#include "nan.h"

#include "BusListenerWrapper.h"
#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> listener_constructor;

NAN_METHOD(BusListenerConstructor) {
  if(info.Length() < 3){
    return Nan::ThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(listener_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0],
    info[1],
    info[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  info.GetReturnValue().Set(obj);
}

BusListenerWrapper::BusListenerWrapper(Nan::Callback* foundName, Nan::Callback* lostName, Nan::Callback* nameChanged)
  :listener(new BusListenerImpl(foundName, lostName, nameChanged)){
}

BusListenerWrapper::~BusListenerWrapper(){
}

void BusListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusListenerWrapper::New);
  listener_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("BusListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(BusListenerWrapper::New) {
  if(info.Length() < 3){
    return Nan::ThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Function> foundName = info[0].As<v8::Function>();
  Nan::Callback *foundNameCall = new Nan::Callback(foundName);
  v8::Local<v8::Function> lostName = info[1].As<v8::Function>();
  Nan::Callback *lostNameCall = new Nan::Callback(lostName);
  v8::Local<v8::Function> changeName = info[2].As<v8::Function>();
  Nan::Callback *nameChangeCall = new Nan::Callback(changeName);

  BusListenerWrapper* obj = new BusListenerWrapper(foundNameCall, lostNameCall, nameChangeCall);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

