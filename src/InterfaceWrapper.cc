#include "nan.h"

#include "InterfaceWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> interface_constructor;

v8::Local<v8::Value> InterfaceWrapper::NewInstance() {
    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(interface_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(InterfaceDescriptionWrapper) {
    info.GetReturnValue().Set(InterfaceWrapper::NewInstance());
}

InterfaceWrapper::InterfaceWrapper():interface(NULL){}

void InterfaceWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(InterfaceWrapper::New);
  interface_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("InterfaceDescription").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "addSignal", InterfaceWrapper::AddSignal);
  Nan::SetPrototypeMethod(tpl, "activate", InterfaceWrapper::Activate);
}

NAN_METHOD(InterfaceWrapper::New) {
  InterfaceWrapper* obj = new InterfaceWrapper();
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
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

  InterfaceWrapper* wrapper = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  QStatus status = wrapper->interface->AddSignal(strdup(*Nan::Utf8String(info[0])), strdup(*Nan::Utf8String(info[1])), strdup(*Nan::Utf8String(info[2])), annotation);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(InterfaceWrapper::Activate) {
  InterfaceWrapper* wrapper = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  wrapper->interface->Activate();
  info.GetReturnValue().SetUndefined();
}


