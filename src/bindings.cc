#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
  InterfaceWrapper::Init();
  BusListenerWrapper::Init();

  Local<Function> busWrap = FunctionTemplate::New(BusAttachmentWrapper)->GetFunction();
  target->Set(NanSymbol("BusAttachment"), busWrap);
  Local<Function> interfaceWrap = FunctionTemplate::New(InterfaceDescriptionWrapper)->GetFunction();
  target->Set(NanSymbol("InterfaceDescription"), interfaceWrap);
  Local<Function> listenerConstructor = FunctionTemplate::New(BusListenerConstructor)->GetFunction();
  target->Set(NanSymbol("BusListener"), listenerConstructor);
}

NODE_MODULE(node_alljoyn, init)