#include "nan.h"

#include "BusListenerWrapper.h"
#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

Persistent<v8::Function> BusListenerWrapper::constructor;

BusListenerWrapper::BusListenerWrapper(Nan::Callback* foundName, Nan::Callback* lostName, Nan::Callback* nameChanged)
  :listener(new BusListenerImpl(foundName, lostName, nameChanged)){
}

BusListenerWrapper::~BusListenerWrapper(){
}

void BusListenerWrapper::Init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("BusListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("BusListener").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(BusListenerWrapper::New) {
  if(info.Length() < 3){
    return Nan::ThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }

  if (info.IsConstructCall()) {
    v8::Local<v8::Function> foundName = info[0].As<v8::Function>();
    Nan::Callback *foundNameCall = new Nan::Callback(foundName);
    v8::Local<v8::Function> lostName = info[1].As<v8::Function>();
    Nan::Callback *lostNameCall = new Nan::Callback(lostName);
    v8::Local<v8::Function> changeName = info[2].As<v8::Function>();
    Nan::Callback *nameChangeCall = new Nan::Callback(changeName);

    BusListenerWrapper* obj = new BusListenerWrapper(foundNameCall, lostNameCall, nameChangeCall);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {info[0], info[1], info[2]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

