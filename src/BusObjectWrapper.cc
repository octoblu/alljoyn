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

BusObjectImpl::BusObjectImpl(const char* path):ajn::BusObject(path){
  loop = uv_default_loop();
  signalCallback.callback = NULL;
  uv_async_init(loop, &signal_async, signal_callback);
}

QStatus BusObjectImpl::AddInter(ajn::InterfaceDescription* interface){
    return AddInterface(*interface);
}

BusObjectImpl::~BusObjectImpl(){
  if(signalCallback.callback){
    delete signalCallback.callback;
  }
}

void BusObjectImpl::signal_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>((*holder->message)->GetSender())
    };
    holder->callback->Call(1, argv);
    if(holder->message){
      delete holder->message;
      holder->message = NULL;
    }
}

void BusObjectImpl::Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message){
  if(signalCallback.callback){
    printf("Got Signal for %s from id %s\n", srcPath, message->GetSender());
    signal_async.data = (void*) &signalCallback;
    //TODO message data
    signalCallback.message = new ajn::Message(message);
    uv_async_send(&signal_async);
  }
}

void BusObjectImpl::SetSignalCallback(NanCallback* callback){
  signalCallback.callback = callback;
}

