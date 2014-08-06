#include <nan.h>

#include "BusConnection.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> bus_constructor;

v8::Handle<v8::Value> BusConnection::NewInstance (v8::Local<v8::String> &appName) {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanPersistentToLocal(bus_constructor);
    v8::Handle<v8::Value> argv[] = { appName };
    obj = con->GetFunction()->NewInstance(1, argv);
    return obj;
}

NAN_METHOD(AlljoynBus) {
    NanScope();
    v8::Local<v8::String> appName = args[0].As<v8::String>();
    NanReturnValue(BusConnection::NewInstance(appName));
}

BusConnection::BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent):connected(0){
    status = ER_OK;
    bus = new BusAttachment(shortName, allowRemoteMessages, maxConcurrent);
    if (!s_bus) {
        status = ER_OUT_OF_MEMORY;
    }
}

void BusConnection::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(BusConnection::New);
  NanAssignPersistent(v8::FunctionTemplate, bus_constructor, tpl);
  tpl->SetClassName(NanSymbol("BusConnection"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "start", BusConnection::Start);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", BusConnection::Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "join", BusConnection::Join);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "getStatus", BusConnection::GetStatus);
}

NAN_METHOD(BusConnection::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("constructor requires an applicationName string argument");

  char* name = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  BusConnection* obj = new BusConnection(name, true, 4);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(BusConnection::Start) {
  NanScope();
  int statusCode = ER_OK;
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  statusCode = connection->bus->Start();
  NanReturnValue(NanNew<Integer>(statusCode));
}

NAN_METHOD(BusConnection::Stop) {
  NanScope();
  int statusCode = ER_OK;
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  statusCode = connection->bus->Stop();
  NanReturnValue(NanNew<Integer>(statusCode));
}

NAN_METHOD(BusConnection::Join) {
  NanScope();
  int statusCode = ER_OK;
  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args.This());
  statusCode = connection->bus->Join();
  NanReturnValue(NanNew<Integer>(statusCode));
}


