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
  BusConnection::Init(target);
  InterfaceWrapper::Init(target);
  BusListenerWrapper::Init(target);
  BusObjectWrapper::Init(target);
  SessionPortListenerWrapper::Init(target);
  NotificationWrapper::Init(target);
}

NODE_MODULE(node_alljoyn, init)