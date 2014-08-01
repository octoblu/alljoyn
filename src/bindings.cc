#include <node.h>
#include <node_buffer.h>


#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>
#include <qcc/String.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

std::vector<BusConnection*> connectionList;

// Interface args:
// { 	interfaceName, 
//		signalName, 
//		signalParams, 
//		signalArgs }

void createConnection(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  BusConnection* conn = new BusConnection();
  connectionList.push(conn);
  conn->createInterface(args);
  conn->connectInterface();
}

static void exitCleanup(void* arg) {
	for (std::vector<int>::iterator ix = connectionList.begin() ; ix != connectionList.end(); ++ix){
		if(ix.connected()){
			ix.disconnect();
			delete ix;
		}
	}
}


void init(Handle<Object> target) {
  NODE_SET_METHOD(target, "createConnection", createConnection);
  NODE_SET_METHOD(target, "connectBus", createConnection);
  AtExit(exitCleanup);
}

NODE_MODULE(node_alljoyn, init)