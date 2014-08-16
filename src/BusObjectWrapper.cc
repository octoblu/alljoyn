#include "nan.h"

#include "BusObjectWrapper.h"
#include "InterfaceWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> busobject_constructor;

v8::Handle<v8::Value> BusObjectWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(busobject_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(BusObjectConstructor) {
  NanScope();
  if(args.Length() == 0 || !args[0]->IsString()){
    return NanThrowError("BusObject requires a path string.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(busobject_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  NanReturnValue(obj);
}

BusObjectWrapper::BusObjectWrapper(const char* path):object(new BusObjectImpl(path)){}

void BusObjectWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(BusObjectWrapper::New);
  NanAssignPersistent(busobject_constructor, tpl);
  tpl->SetClassName(NanSymbol("BusObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addInterface", BusObjectWrapper::AddInterfaceInternal);
}

NAN_METHOD(BusObjectWrapper::New) {
  NanScope();
  if(args.Length() < 1 || !args[0]->IsString()){
    return NanThrowError("BusObject requires a path string.");
  }
  char* path = *NanUtf8String(args[0]);
  BusObjectWrapper* obj = new BusObjectWrapper(path);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(BusObjectWrapper::AddInterfaceInternal) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("BusObject.AddInterface requires an Interface.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(args.This());
  InterfaceWrapper* interWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[0].As<v8::Object>());
  QStatus status = obj->object->AddInter(interWrapper->interface);
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

BusObjectImpl::BusObjectImpl(const char* path):ajn::BusObject(path){}
QStatus BusObjectImpl::AddInter(ajn::InterfaceDescription* interface){
    return AddInterface(*interface);
}

