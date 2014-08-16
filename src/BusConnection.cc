#include <nan.h>

#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"
#include "SessionPortListenerWrapper.h"
#include "BusObjectWrapper.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> bus_constructor;

v8::Handle<v8::Value> BusConnection::NewInstance(v8::Local<v8::String> &appName) {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(bus_constructor);
    v8::Handle<v8::Value> argv[] = { appName };
    obj = con->GetFunction()->NewInstance(1, argv);
    return obj;
}

NAN_METHOD(BusAttachmentWrapper) {
    NanScope();
    v8::Local<v8::String> appName = args[0].As<v8::String>();
    NanReturnValue(BusConnection::NewInstance(appName));
}

BusConnection::BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent){
    bus = new ajn::BusAttachment(shortName, allowRemoteMessages, maxConcurrent);
}

void BusConnection::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(BusConnection::New);
  NanAssignPersistent(bus_constructor, tpl);
  tpl->SetClassName(NanSymbol("BusAttachment"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "start", BusConnection::Start);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", BusConnection::Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "join", BusConnection::Join);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connect", BusConnection::Connect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "createInterface", BusConnection::CreateInterface);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getInterface", BusConnection::GetInterface);
  NODE_SET_PROTOTYPE_METHOD(tpl, "registerBusListener", BusConnection::RegisterBusListener);
  NODE_SET_PROTOTYPE_METHOD(tpl, "registerBusObject", BusConnection::RegisterBusObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findAdvertisedName", BusConnection::FindAdvertisedName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "joinSession", BusConnection::JoinSession);
}

NAN_METHOD(BusConnection::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("constructor requires an applicationName string argument");

  BusConnection* obj = new BusConnection(*NanUtf8String(args[0]), true, 4);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(BusConnection::Start) {
  NanScope();
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->Start();
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Stop) {
  NanScope();
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->Stop();
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Join) {
  NanScope();
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->Join();
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Connect) {
  NanScope();
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->Connect();
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::CreateInterface) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("CreateInterface requires a name string argument");
  if (args.Length() == 1)
    return NanThrowError("CreateInterface requires a new InterfaceDescription argument");
  
  char* name = *NanUtf8String(args[0]);

  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->CreateInterface(name, interface);
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[1].As<v8::Object>());
  wrapper->interface = interface;
  if (ER_OK == status) {
    //callback
  } else {
      printf("Failed to create interface \"%s\" (%s)\n", name, QCC_StatusText(status));
  }

  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::GetInterface) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("CreateInterface requires a name string argument");
  if (args.Length() == 1)
    return NanThrowError("CreateInterface requires a new InterfaceDescription argument");
  
  char* name = *NanUtf8String(args[0]);
  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  interface = const_cast<ajn::InterfaceDescription*>(connection->bus->GetInterface(name));
  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[1].As<v8::Object>());
  wrapper->interface = interface;

  NanReturnUndefined();
}

NAN_METHOD(BusConnection::RegisterBusListener) {
  NanScope();
  printf("RegisterBusListener\n");
  if (args.Length() == 0)
    return NanThrowError("RegisterBusListener requires a BusListener argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  BusListenerWrapper* wrapper = node::ObjectWrap::Unwrap<BusListenerWrapper>(args[0].As<v8::Object>());
  connection->bus->RegisterBusListener(*(wrapper->listener));

  NanReturnUndefined();
}

NAN_METHOD(BusConnection::RegisterBusObject) {
  NanScope();
  printf("RegisterBusObject\n");
  if (args.Length() == 0)
    return NanThrowError("RegisterBusObject requires a BusObject argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  BusObjectWrapper* wrapper = node::ObjectWrap::Unwrap<BusObjectWrapper>(args[0].As<v8::Object>());

  QStatus status = connection->bus->RegisterBusObject(*(wrapper->object));

  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::FindAdvertisedName) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("FindAdvertisedName requires a namePrefix string argument");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  QStatus status = connection->bus->FindAdvertisedName(*NanUtf8String(args[0]));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::JoinSession) {
  NanScope();
  printf("JoinSession\n");
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsNumber())
    return NanThrowError("JoinSession requires a sessionHost name, sessionPort number, and (optional) SessionListener callback");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  ajn::SessionId sessionId = static_cast<ajn::SessionPort>(args[1]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  // if(args.Length() == 3 && args[2]->IsObject() && !args[2]->IsNull()){
  //   SessionPortListenerWrapper* wrapper = node::ObjectWrap::Unwrap<SessionPortListenerWrapper>(args[2].As<v8::Object>());
  //   QStatus status = connection->bus->JoinSession(*NanUtf8String(args[0]), args[1]->IntegerValue(), *(wrapper->listener), args[1]->IntegerValue(), opts);
  // }else{
    QStatus status = connection->bus->JoinSession(*NanUtf8String(args[0]), static_cast<ajn::SessionPort>(args[1]->Int32Value()), NULL, sessionId, opts);
  // }

  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(sessionId)));
}

NAN_METHOD(BusConnection::BindSessionPort) {
  NanScope();
  printf("BindSessionPort\n");
  if (args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsObject())
    return NanThrowError("BindSessionPort requires a sessionPort number and SessionPortListener callback");

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  SessionPortListenerWrapper* wrapper = node::ObjectWrap::Unwrap<SessionPortListenerWrapper>(args[1].As<v8::Object>());
  ajn::SessionPort port = static_cast<ajn::SessionPort>(args[0]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  QStatus status = connection->bus->BindSessionPort(port, opts, *(wrapper->listener));

  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}
