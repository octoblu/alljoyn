#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "BusListenerWrapper.h"
#include "BusObjectWrapper.h"
#include "SessionPortListenerWrapper.h"
#include "NotificationWrapper.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
  InterfaceWrapper::Init();
  BusListenerWrapper::Init();
  BusObjectWrapper::Init();
  SessionPortListenerWrapper::Init();
  NotificationWrapper::Init();

  Nan::SetMethod(target, "BusAttachment", BusAttachmentWrapper);
  Nan::SetMethod(target, "InterfaceDescription", InterfaceDescriptionWrapper);
  Nan::SetMethod(target, "BusListener", BusListenerConstructor);
  Nan::SetMethod(target, "BusObject", BusObjectConstructor);
  Nan::SetMethod(target, "SessionPortListener", SessionPortListenerConstructor);
  Nan::SetMethod(target, "NotificationService", NotificationConstructor);
}

NODE_MODULE(node_alljoyn, init)
