#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"
#include "BusObjectWrapper.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
  InterfaceWrapper::Init();
  BusListenerWrapper::Init();
  BusObjectWrapper::Init();

  Local<Function> busWrap = FunctionTemplate::New(BusAttachmentWrapper)->GetFunction();
  target->Set(NanSymbol("BusAttachment"), busWrap);
  Local<Function> interfaceWrap = FunctionTemplate::New(InterfaceDescriptionWrapper)->GetFunction();
  target->Set(NanSymbol("InterfaceDescription"), interfaceWrap);
  Local<Function> listenerConstructor = FunctionTemplate::New(BusListenerConstructor)->GetFunction();
  target->Set(NanSymbol("BusListener"), listenerConstructor);
  Local<Function> objectConstructor = FunctionTemplate::New(BusObjectConstructor)->GetFunction();
  target->Set(NanSymbol("BusObject"), objectConstructor);
}

NODE_MODULE(node_alljoyn, init)