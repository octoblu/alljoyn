#include "nan.h"

#include "InterfaceWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

using namespace ajn;

Persistent<v8::Function> InterfaceWrapper::constructor;

InterfaceWrapper::InterfaceWrapper():interface(NULL){}

void InterfaceWrapper::Init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(InterfaceWrapper::New);
  tpl->SetClassName(Nan::New<v8::String>("InterfaceDescription").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "addProperty", InterfaceWrapper::AddProperty);
  SetPrototypeMethod(tpl, "addMethod", InterfaceWrapper::AddMethod);
  SetPrototypeMethod(tpl, "addSignal", InterfaceWrapper::AddSignal);
  SetPrototypeMethod(tpl, "activate", InterfaceWrapper::Activate);
  SetPrototypeMethod(tpl, "introspect", InterfaceWrapper::Introspect);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("InterfaceDescription").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(InterfaceWrapper::New) {
  InterfaceWrapper* obj = new InterfaceWrapper();
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(InterfaceWrapper::AddProperty){
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AddSignal requires a name of property");
  if (info.Length() == 1 || !info[1]->IsString())
    return Nan::ThrowError("AddSignal requires a signature");
  if (info.Length() == 2 || !info[2]->IsNumber())
    return Nan::ThrowError("AddSignal requires a access");

  uint8_t access = info[2]->Int32Value();

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  QStatus status = wrapper->interface->AddProperty(*Utf8String(info[0]), *Utf8String(info[1]), access);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(InterfaceWrapper::AddMethod){
  int annotation = 0;
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AddSignal requires a name string argument");
  if (info.Length() == 1 || !info[1]->IsString())
    return Nan::ThrowError("AddSignal requires Signature of input parameters or NULL for none");
  if (info.Length() == 2 || !info[2]->IsString())
    return Nan::ThrowError("AddSignal requires Signature of output parameters or NULL for none");
  if (info.Length() == 3 || !info[2]->IsString())
    return Nan::ThrowError("AddSignal requires Comma separated list of input and then output arg names used in annotation XML.");
  if(info.Length() >= 5){
    annotation = info[4]->Int32Value();
  }

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  QStatus status = wrapper->interface->AddMethod(*Utf8String(info[0]), *Utf8String(info[1]), *Utf8String(info[2]), *Utf8String(info[3]), annotation);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}


NAN_METHOD(InterfaceWrapper::AddSignal) {
  
  int annotation = 0;
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AddSignal requires a name string argument");
  if (info.Length() == 1 || !info[1]->IsString())
    return Nan::ThrowError("AddSignal requires an param signature string argument");
  if (info.Length() == 2 || !info[2]->IsString())
    return Nan::ThrowError("AddSignal requires an argument list string argument");
  if(info.Length() >= 4){
    annotation = info[3]->Int32Value();
  }

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  QStatus status = wrapper->interface->AddSignal(*Utf8String(info[0]), *Utf8String(info[1]), *Utf8String(info[2]), annotation);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(InterfaceWrapper::Activate) {
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  wrapper->interface->Activate();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(InterfaceWrapper::Introspect) {
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  info.GetReturnValue().Set(Nan::New<v8::String>(wrapper->interface->Introspect().c_str()).ToLocalChecked());
}

