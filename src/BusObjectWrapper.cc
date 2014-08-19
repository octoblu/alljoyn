#include "nan.h"

#include "BusObjectWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include <string.h>
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
  tpl->SetClassName(NanNew<v8::String>("BusObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addInterface", BusObjectWrapper::AddInterfaceInternal);
  NODE_SET_PROTOTYPE_METHOD(tpl, "signal", BusObjectWrapper::Signal);
}

NAN_METHOD(BusObjectWrapper::New) {
  NanScope();
  if(args.Length() < 1 || !args[0]->IsString()){
    return NanThrowError("BusObject requires a path string.");
  }
  char* path = strdup(*NanUtf8String(args[0]));
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

NAN_METHOD(BusObjectWrapper::Signal) {
  NanScope();
  if(args.Length() < 3){
    return NanThrowError("BusObject.Signal requires a (nullable) destination, SessionId, Interface, member name, and message args.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(args.This());
  InterfaceWrapper* interface = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*NanUtf8String(args[3]));
  const char* destination = NULL;
  if(!args[0]->IsNull() && args[0]->IsString()){
    destination = strdup(*NanUtf8String(args[0]));
  }
  ajn::MsgArg* msgArgs = objToMsgArg(args[4]);
  QStatus status = ER_OK;
  if(args.Length() == 4){
    status = obj->object->Signal(destination, args[1]->Int32Value(), *signalMember, NULL, 0, 0, 0);
  }else if(args.Length() == 5){
    status = obj->object->Signal(destination, args[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else if(args.Length() > 5){
    //TODO handle multi-arg messages
    status = obj->object->Signal(destination, args[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else{
    return NanThrowError("BusObject.Signal requires a SessionId, Interface, member name, and (optionally) destination, message args.");
  }
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

    v8::Local<v8::Object> msg = v8::Object::New();
    size_t msgIndex = 0;
    const ajn::MsgArg* arg = (*holder->message)->GetArg(msgIndex);
    while(arg != NULL){
      msgArgToObject(arg, msgIndex, msg);
      msgIndex++;
      arg = (*holder->message)->GetArg(msgIndex);
    }

    v8::Local<v8::Object> sender = v8::Object::New();
    sender->Set(NanNew<v8::String>("sender"), NanNew<v8::String>((*holder->message)->GetSender()));
    sender->Set(NanNew<v8::String>("session_id"), NanNew<v8::Integer>((*holder->message)->GetSessionId()));
    sender->Set(NanNew<v8::String>("timestamp"), NanNew<v8::Integer>((*holder->message)->GetTimeStamp()));
    sender->Set(NanNew<v8::String>("member_name"), NanNew<v8::String>((*holder->message)->GetMemberName()));
    sender->Set(NanNew<v8::String>("object_path"), NanNew<v8::String>((*holder->message)->GetObjectPath()));
    sender->Set(NanNew<v8::String>("signature"), NanNew<v8::String>((*holder->message)->GetSignature()));

    v8::Handle<v8::Value> argv[] = {
      msg,
      sender
    };
    holder->callback->Call(2, argv);

    if(holder->message){
      delete holder->message;
      holder->message = NULL;
    }
}

void BusObjectImpl::ReceiveSignal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message){
  if(signalCallback.callback){
    signal_async.data = (void*) &signalCallback;
    signalCallback.message = new ajn::Message(message);
    uv_async_send(&signal_async);
  }
}

void BusObjectImpl::SetSignalCallback(NanCallback* callback){
  signalCallback.callback = callback;
}

