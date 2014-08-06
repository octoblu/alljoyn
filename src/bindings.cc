#include <node.h>
#include <nan.h>
#include "BusConnection.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
}

NODE_MODULE(node_alljoyn, init)