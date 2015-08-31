#include <nan.h>

#include <alljoyn/MsgArg.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <MessageReceiver.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

#include "ProxyBusObjectWrapper.h"
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include "UVThreadSwitcher.h"


using namespace Nan;  // NOLINT(build/namespaces)

Nan::Persistent<v8::Function> ProxyBusObjectWrapper::constructor;

using namespace v8;
using namespace ajn;

ProxyBusObjectWrapper::ProxyBusObjectWrapper(BusAttachment& bus, const char* service, const char* path, SessionId sessionId){
  proxyObj = new ProxyBusObject(bus, service, path, sessionId);
}

ProxyBusObjectWrapper::~ProxyBusObjectWrapper(){


}

void 
ProxyBusObjectWrapper::Init (v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(ProxyBusObjectWrapper::New);
  tpl->SetClassName(Nan::New("ProxyBusObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "addInterface", ProxyBusObjectWrapper::AddInterface);
  SetPrototypeMethod(tpl, "methodCall", ProxyBusObjectWrapper::MethodCall);
  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("ProxyBusObject").ToLocalChecked(), tpl->GetFunction());
}

struct AsyncMethodCallData{
  ajn::Message *message;
  UVThreadSwitcher *switcher;
  Callback *v8Callback;
};

void
ProxyBusObjectWrapper::asyncMethodCallback(ajn::Message& message, void* context){
  printf("ProxyBusObjectWrapper::asyncMethodCallback(ajn::Message& message, void* context)\n");
  Callback *v8Callback = (Callback *) context;

  AsyncMethodCallData *data = new AsyncMethodCallData();
  data->switcher = new UVThreadSwitcher(std::bind(&ProxyBusObjectWrapper::uvMethodCallback, static_cast<ProxyBusObjectWrapper *>(this), std::placeholders::_1));
  data->v8Callback = v8Callback;
  data->message = new ajn::Message(message);

  data->switcher->execute((void *) data);
}

void
ProxyBusObjectWrapper::uvMethodCallback(void *userData){
  printf("ProxyBusObjectWrapper::uvMethodCallback\n");

  AsyncMethodCallData *data = (AsyncMethodCallData *) userData;
  UVThreadSwitcher *switcher = data->switcher;
  Callback *v8Callback = data->v8Callback;
  ajn::Message *ajnMsg = data->message;


  v8::Local<v8::Object> msg = Nan::New<v8::Object>();
  size_t msgIndex = 0;
  const ajn::MsgArg* arg = (*ajnMsg)->GetArg(msgIndex);
  while(arg != NULL){
    msgArgToObject(arg, msgIndex, msg);
    msgIndex++;
    arg = (*ajnMsg)->GetArg(msgIndex);
  }


  v8::Local<v8::Object> sender = Nan::New<v8::Object>();

  Nan::Set(sender, 
    Nan::New<v8::String>("sender").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetSender()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("session_id").ToLocalChecked(), 
    Nan::New<v8::Integer>((*ajnMsg)->GetSessionId())
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("timestamp").ToLocalChecked(), 
    Nan::New<v8::Integer>((*ajnMsg)->GetTimeStamp())
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("member_name").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetMemberName()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("object_path").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetObjectPath()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("signature").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetSignature()).ToLocalChecked()
  );    

  v8::Local<v8::Value> argv[] = {
    sender,
    msg    
  };

  v8Callback->Call(2, argv);


  delete switcher;
  delete v8Callback;
  delete ajnMsg;

  delete data;
}

NAN_METHOD(ProxyBusObjectWrapper::New) {
  if (info.Length() < 4)
    return Nan::ThrowError("constructor requires an BusAttachment, service name, path, sessionId");

  if (!info[0]->IsObject())
    return Nan::ThrowError("ProxyBusObject constructor requires first parameter as a busAttachment instance");
  if (!info[1]->IsString())
    return Nan::ThrowError("ProxyBusObject constructor requires second parameter as a service name ");
  if (!info[2]->IsString())
    return Nan::ThrowError("ProxyBusObject constructor requires third parameter as a path name");
  if (!info[3]->IsNumber())
    return Nan::ThrowError("ProxyBusObject constructor requires forth parameter as a sessionId");

  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(info[0].As<v8::Object>());
  BusAttachment *bus = busWrapper->bus;

  ajn::SessionId sessionId = static_cast<ajn::SessionId>(info[3]->Int32Value());

  ProxyBusObjectWrapper* obj = new ProxyBusObjectWrapper(*bus, *Utf8String(info[1]), *Utf8String(info[2]), sessionId);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(ProxyBusObjectWrapper::AddInterface) {
  if (info.Length()==0)
    return ThrowError("AddInterface need a parameter as InterfaceDescription Instance");

  ProxyBusObjectWrapper* proxyWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  InterfaceWrapper* interfaceWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[0].As<v8::Object>());

  InterfaceDescription *interfaceDescription = interfaceWrapper->interface;

  QStatus status = proxyWrapper->proxyObj->AddInterface(*interfaceDescription);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(ProxyBusObjectWrapper::MethodCall) {
  if(info.Length()!=1) return ThrowError("methodCall requires one parameters object");

  ProxyBusObjectWrapper* proxyWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  ProxyBusObject *proxyObj = proxyWrapper->proxyObj;

  Local<Object> params = info[0].As<Object>();

  Local<String> v8ifaceName = Nan::Get(params, Nan::New("interfaceName").ToLocalChecked()).ToLocalChecked().As<String>();
  Local<String> v8methodName = Nan::Get(params, Nan::New("methodName").ToLocalChecked()).ToLocalChecked().As<String>();
  Local<Array> v8args = Nan::Get(params, Nan::New("args").ToLocalChecked()).ToLocalChecked().As<Array>();
  Local<Function> v8CallFunc = Nan::Get(params, Nan::New("callback").ToLocalChecked()).ToLocalChecked().As<Function>();

  size_t numArgs = v8args->Length();

  MsgArg inputs[numArgs];

  for(size_t i=0;i<numArgs;i++){
    Local<Object> v8argObj = Nan::Get(v8args, i).ToLocalChecked().As<Object>();

    Local<String> v8signature = Nan::Get(v8argObj, Nan::New("signature").ToLocalChecked()).ToLocalChecked().As<String>();

    Local<Value> v8value = Nan::Get(v8argObj, Nan::New("value").ToLocalChecked()).ToLocalChecked();
    char *signature = strdup(*Utf8String(v8signature));
    objToMsgArg(signature, v8value, &inputs[i]);
    free(signature);
  }

  Callback *v8Callback = new Callback(v8CallFunc);

  QStatus status = proxyObj->MethodCallAsync(
    *Utf8String(v8ifaceName), 
    *Utf8String(v8methodName),
    const_cast<MessageReceiver*>(static_cast<const MessageReceiver* const>(proxyWrapper)),
    static_cast<MessageReceiver::ReplyHandler>(&ProxyBusObjectWrapper::asyncMethodCallback),
    &inputs[0],
    numArgs,
    (void *)v8Callback
  );

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}
