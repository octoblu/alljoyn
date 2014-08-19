#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"
#include "BusObjectWrapper.h"
#include "SessionPortListenerWrapper.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
  InterfaceWrapper::Init();
  BusListenerWrapper::Init();
  BusObjectWrapper::Init();
  SessionPortListenerWrapper::Init();

  Local<Function> busWrap = FunctionTemplate::New(BusAttachmentWrapper)->GetFunction();
  target->Set(NanNew<String>("BusAttachment"), busWrap);
  Local<Function> interfaceWrap = FunctionTemplate::New(InterfaceDescriptionWrapper)->GetFunction();
  target->Set(NanNew<String>("InterfaceDescription"), interfaceWrap);
  Local<Function> listenerConstructor = FunctionTemplate::New(BusListenerConstructor)->GetFunction();
  target->Set(NanNew<String>("BusListener"), listenerConstructor);
  Local<Function> objectConstructor = FunctionTemplate::New(BusObjectConstructor)->GetFunction();
  target->Set(NanNew<String>("BusObject"), objectConstructor);
  Local<Function> sessionPortListenerConstructor = FunctionTemplate::New(SessionPortListenerConstructor)->GetFunction();
  target->Set(NanNew<String>("SessionPortListener"), sessionPortListenerConstructor);
}

NODE_MODULE(node_alljoyn, init)