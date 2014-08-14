#include "nan.h"

#include "BusListenerWrapper.h"
#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> listener_constructor;

v8::Handle<v8::Value> BusListenerWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(BusListenerConstructor) {
  NanScope();
  if(args.Length() < 3){
    return NanThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  NanReturnValue(obj);
}

BusListenerWrapper::BusListenerWrapper(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged)
  :listener(new BusListenerImpl(foundName, lostName, nameChanged)){
}

BusListenerWrapper::~BusListenerWrapper(){
  printf("BusListenerWrapper-destructor\n");
  if(listener){
    delete listener;
  }
}

void BusListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(BusListenerWrapper::New);
  NanAssignPersistent(listener_constructor, tpl);
  tpl->SetClassName(NanSymbol("BusListener"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(BusListenerWrapper::New) {
  NanScope();
  if(args.Length() < 3){
    return NanThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Function> foundName = args[0].As<v8::Function>();
  NanCallback *foundNameCall = new NanCallback(foundName);
  v8::Local<v8::Function> lostName = args[1].As<v8::Function>();
  NanCallback *lostNameCall = new NanCallback(lostName);
  v8::Local<v8::Function> changeName = args[2].As<v8::Function>();
  NanCallback *nameChangeCall = new NanCallback(changeName);

  BusListenerWrapper* obj = new BusListenerWrapper(foundNameCall, lostNameCall, nameChangeCall);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

