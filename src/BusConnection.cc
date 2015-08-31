#include <nan.h>

#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"
#include "SessionPortListenerWrapper.h"
#include "BusObjectWrapper.h"
#include "SignalHandlerImpl.h"
#include "AboutListenerImpl.h"
#include "util.h"

#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <MessageReceiver.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>


Persistent<v8::Function> BusConnection::constructor;


BusConnection::BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent){
  bus = new ajn::BusAttachment(shortName, allowRemoteMessages, maxConcurrent);
}

BusConnection::~BusConnection(){
}

void BusConnection::Init (v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusConnection::New);
  tpl->SetClassName(Nan::New("BusAttachment").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "start", BusConnection::Start);
  SetPrototypeMethod(tpl, "stop", BusConnection::Stop);
  SetPrototypeMethod(tpl, "join", BusConnection::Join);
  SetPrototypeMethod(tpl, "connect", BusConnection::Connect);
  SetPrototypeMethod(tpl, "disconnect", BusConnection::Disconnect);
  SetPrototypeMethod(tpl, "createInterface", BusConnection::CreateInterface);
  SetPrototypeMethod(tpl, "getInterface", BusConnection::GetInterface);
  SetPrototypeMethod(tpl, "registerBusListener", BusConnection::RegisterBusListener);
  SetPrototypeMethod(tpl, "registerBusObject", BusConnection::RegisterBusObject);
  SetPrototypeMethod(tpl, "findAdvertisedName", BusConnection::FindAdvertisedName);
  SetPrototypeMethod(tpl, "joinSession", BusConnection::JoinSession);
  SetPrototypeMethod(tpl, "bindSessionPort", BusConnection::BindSessionPort);
  SetPrototypeMethod(tpl, "requestName", BusConnection::RequestName);
  SetPrototypeMethod(tpl, "advertiseName", BusConnection::AdvertiseName);
  SetPrototypeMethod(tpl, "registerSignalHandler", BusConnection::RegisterSignalHandler);
  SetPrototypeMethod(tpl, "addMatch", BusConnection::AddMatch);
  SetPrototypeMethod(tpl, "createInterfacesFromXml", BusConnection::CreateInterfacesFromXml);
  SetPrototypeMethod(tpl, "registerAboutListener", BusConnection::RegisterAboutListener);
  SetPrototypeMethod(tpl, "whoImplements", BusConnection::WhoImplements);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("BusAttachment").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(BusConnection::New) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("constructor requires an applicationName string argument");

  if (info.IsConstructCall()) {
    BusConnection* obj = new BusConnection(strdup(*Utf8String(info[0])), true, 4);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

NAN_METHOD(BusConnection::Start) {
  
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Start();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Stop) {
  
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Stop();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Join) {
  
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Join();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Connect) {
  
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Connect();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Disconnect) {
  
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Disconnect();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::CreateInterface) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("CreateInterface requires a name string argument");
  if (info.Length() == 1)
    return Nan::ThrowError("CreateInterface requires a new InterfaceDescription argument");
  
  char* name = strdup(*Utf8String(info[0]));

  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->CreateInterface(name, interface);
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[1].As<v8::Object>());
  wrapper->interface = interface;
  if (ER_OK == status) {
    //callback
  } else {
      printf("Failed to create interface \"%s\" (%s)\n", name, QCC_StatusText(status));
  }

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::GetInterface) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("GetInterface requires a name string argument");
  if (info.Length() == 1)
    return Nan::ThrowError("GetInterface requires a new InterfaceDescription argument");
  
  char* name = *Utf8String(info[0]);
  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  interface = const_cast<ajn::InterfaceDescription*>(connection->bus->GetInterface(name));
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[1].As<v8::Object>());
  wrapper->interface = interface;

  if(interface!=NULL) printf("interface found \n");
  else printf("interface notfound \n");

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::RegisterBusListener) {
  
  if (info.Length() == 0)
    return Nan::ThrowError("RegisterBusListener requires a BusListener argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  BusListenerWrapper* wrapper = node::ObjectWrap::Unwrap<BusListenerWrapper>(info[0].As<v8::Object>());
  connection->bus->RegisterBusListener(*(wrapper->listener));

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::RegisterBusObject) {
  
  if (info.Length() == 0)
    return Nan::ThrowError("RegisterBusObject requires a BusObject argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  BusObjectWrapper* wrapper = node::ObjectWrap::Unwrap<BusObjectWrapper>(info[0].As<v8::Object>());

  QStatus status = connection->bus->RegisterBusObject(*(wrapper->object));

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::FindAdvertisedName) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("FindAdvertisedName requires a namePrefix string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->FindAdvertisedName(*Utf8String(info[0]));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::JoinSession) {
  
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsNumber())
    return Nan::ThrowError("JoinSession requires a sessionHost name, sessionPort number, and (optional) SessionListener callback");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  ajn::SessionId sessionId = static_cast<ajn::SessionPort>(info[1]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  // if(info.Length() == 3 && info[2]->IsObject() && !info[2]->IsNull()){
  //   SessionPortListenerWrapper* wrapper = node::ObjectWrap::Unwrap<SessionPortListenerWrapper>(info[2].As<v8::Object>());
  //   QStatus status = connection->bus->JoinSession(*Utf8String(info[0]), info[1]->IntegerValue(), *(wrapper->listener), info[1]->IntegerValue(), opts);
  // }else{
  connection->bus->JoinSession(strdup(*Utf8String(info[0])), static_cast<ajn::SessionPort>(info[1]->Int32Value()), NULL, sessionId, opts);
  // }

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(sessionId)));
}

NAN_METHOD(BusConnection::BindSessionPort) {
  
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsObject())
    return Nan::ThrowError("BindSessionPort requires a sessionPort number and SessionPortListener callback");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  SessionPortListenerWrapper* wrapper = node::ObjectWrap::Unwrap<SessionPortListenerWrapper>(info[1].As<v8::Object>());
  ajn::SessionPort port = static_cast<ajn::SessionPort>(info[0]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  QStatus status = connection->bus->BindSessionPort(port, opts, *(wrapper->listener));

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RequestName) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("RequestName requires a requestedName string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->RequestName(strdup(*Utf8String(info[0])), DBUS_NAME_FLAG_DO_NOT_QUEUE);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::AdvertiseName) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AdvertiseName requires a name string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->AdvertiseName(strdup(*Utf8String(info[0])), ajn::TRANSPORT_ANY);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RegisterSignalHandler) {
  
  if (info.Length() < 4 || !info[0]->IsObject() || !info[1]->IsFunction() || !info[2]->IsObject() || !info[3]->IsString())
    return Nan::ThrowError("RegisterSignalHandler requires a receiver BusObject, signalHandler callback, interface, interface member name, and (optional) srcPath.");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  InterfaceWrapper* interface = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*Utf8String(info[3]));

  v8::Local<v8::Function> fn = info[1].As<v8::Function>();
  Nan::Callback *callback = new Nan::Callback(fn);
  SignalHandlerImpl* signalHandler = new SignalHandlerImpl(callback);
  QStatus status = ER_OK;
  if(info.Length() == 5){
    status = connection->bus->RegisterSignalHandler(signalHandler, static_cast<ajn::MessageReceiver::SignalHandler>(&SignalHandlerImpl::Signal), signalMember, strdup(*Utf8String(info[4])));
  }else{
    status = connection->bus->RegisterSignalHandler(signalHandler, static_cast<ajn::MessageReceiver::SignalHandler>(&SignalHandlerImpl::Signal), signalMember, NULL);
  }

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::AddMatch) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AddMatch requires a name string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->AddMatch(strdup(*Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::CreateInterfacesFromXml) {
  
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("CreateInterfacesFromXml requires a name string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->CreateInterfacesFromXml(strdup(*Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RegisterAboutListener) {
  
  if (info.Length() == 0 || !info[0]->IsFunction())
    return Nan::ThrowError("RegisterAboutListener requires a callback argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());

  v8::Local<v8::Function> fn = info[0].As<v8::Function>();
  Nan::Callback *callback = new Nan::Callback(fn);

  AboutListenerImpl *listener = new AboutListenerImpl(callback);
  connection->bus->RegisterAboutListener(*listener);
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::WhoImplements) {
  if (info.Length() == 0 || !info[0]->IsArray())
    return Nan::ThrowError("WhoImplements requires a array of interfaces argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info.This());

  v8::Local<v8::Array> infs = info[0].As<v8::Array>();
  uint32_t infsSize = infs->Length();

  const char** interfaces = (const char**) malloc(sizeof(void *)*2);

  for(uint32_t i=0;i<infsSize;i++){
    v8::Local<v8::String> inf = Nan::Get(infs, i).ToLocalChecked().As<v8::String>();
    interfaces[i] = *Utf8String(inf);
  }

  QStatus status = connection->bus->WhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));

  free(interfaces);

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}
