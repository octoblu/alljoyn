#include "nan.h"

#include "InterfaceWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> interface_constructor;

v8::Handle<v8::Value> InterfaceWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(interface_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(InterfaceDescriptionWrapper) {
    NanScope();
    NanReturnValue(InterfaceWrapper::NewInstance());
}

InterfaceWrapper::InterfaceWrapper():interface(NULL){}

void InterfaceWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(InterfaceWrapper::New);
  NanAssignPersistent(interface_constructor, tpl);
  tpl->SetClassName(NanSymbol("InterfaceDescription"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addSignal", InterfaceWrapper::AddSignal);
  NODE_SET_PROTOTYPE_METHOD(tpl, "activate", InterfaceWrapper::Activate);
}

NAN_METHOD(InterfaceWrapper::New) {
  NanScope();

  InterfaceWrapper* obj = new InterfaceWrapper();
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(InterfaceWrapper::AddSignal) {
  NanScope();
  int annotation = 0;
  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("AddSignal requires a name string argument");
  if (args.Length() == 1 || !args[1]->IsString())
    return NanThrowError("AddSignal requires an param signature string argument");
  if (args.Length() == 2 || !args[2]->IsString())
    return NanThrowError("AddSignal requires an argument list string argument");
  if(args.Length() >= 4){
    annotation = args[3]->Int32Value();
  }

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args.This());
  QStatus status = wrapper->interface->AddSignal(*NanUtf8String(args[0]), *NanUtf8String(args[1]), *NanUtf8String(args[2]), annotation);
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(InterfaceWrapper::Activate) {
  NanScope();
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args.This());
  wrapper->interface->Activate();
  NanReturnUndefined();
}


