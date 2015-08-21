#include "nan.h"

#include "BusObjectWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

Persistent<v8::Function> BusObjectWrapper::constructor;

BusObjectWrapper::BusObjectWrapper(const char* path):object(new BusObjectImpl(path)){}

void BusObjectWrapper::Init (v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusObjectWrapper::New);
  tpl->SetClassName(Nan::New<v8::String>("BusObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "addInterface", BusObjectWrapper::AddInterfaceInternal);
  SetPrototypeMethod(tpl, "signal", BusObjectWrapper::Signal);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("BusObject").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(BusObjectWrapper::New) {
  
  if(info.Length() < 1 || !info[0]->IsString()){
    return Nan::ThrowError("BusObject requires a path string.");
  }

  if (info.IsConstructCall()) {
    char* path = strdup(*Utf8String(info[0]));
    BusObjectWrapper* obj = new BusObjectWrapper(path);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());

  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }  
}

NAN_METHOD(BusObjectWrapper::AddInterfaceInternal) {
  if(info.Length() < 1){
    return Nan::ThrowError("BusObject.AddInterface requires an Interface.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[0].As<v8::Object>());
  QStatus status = obj->object->AddInter(interWrapper->interface);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusObjectWrapper::Signal) {
  
  if(info.Length() < 3){
    return Nan::ThrowError("BusObject.Signal requires a (nullable) destination, SessionId, Interface, member name, and message args.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interface = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*Utf8String(info[3]));
  const char* destination = NULL;
  if(!info[0]->IsNull() && info[0]->IsString()){
    destination = strdup(*Utf8String(info[0]));
  }
  ajn::MsgArg* msgArgs = objToMsgArg(info[4]);
  QStatus status = ER_OK;
  if(info.Length() == 4){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, NULL, 0, 0, 0);
  }else if(info.Length() == 5){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else if(info.Length() > 5){
    //TODO handle multi-arg messages
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else{
    return Nan::ThrowError("BusObject.Signal requires a SessionId, Interface, member name, and (optionally) destination, message args.");
  }
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

BusObjectImpl::BusObjectImpl(const char* path):ajn::BusObject(path){
}

QStatus BusObjectImpl::AddInter(ajn::InterfaceDescription* interface){
    return AddInterface(*interface);
}

BusObjectImpl::~BusObjectImpl(){
}
