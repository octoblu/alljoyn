var alljoyn = require('./');

console.log("Test loading alljoyn bus...", alljoyn);

var INTERFACE_NAME = "org.alljoyn.Bus.sample";
var SERVICE_NAME = "org.alljoyn.Bus.sample";
var SERVICE_PATH = "/sample";
var SERVICE_PORT = 25;

var bus = new alljoyn.BusAttachment("myApp");
var intf = new alljoyn.InterfaceDescription();

bus.createInterface(INTERFACE_NAME, intf);
intf.addMethod("cat", "ss",  "s", "inStr1,inStr2,outStr", 0);
intf.addMethod("test", "ss",  "s", "inStr1,inStr2,outStr", 0);
intf.activate();

console.log("introspect", intf.introspect());

var listener = new alljoyn.BusListener(
  function(name){
    console.log("FoundAdvertisedName", name);
    sessionId = bus.joinSession(name, portNumber, 0);
    console.log("JoinSession "+sessionId);
  },
  function(name){
    console.log("LostAdvertisedName", name);
  },
  function(name){
    console.log("NameOwnerChanged", name);
  }
);

bus.registerBusListener(listener);
bus.start();
var busObject = new alljoyn.BusObject(SERVICE_PATH);

console.log("chat.AddInterface "+ busObject.addInterface(intf, {
  cat : function(sender, args){
    console.log(arguments);
    return args[0] + args[1] + "wow";
  },
  test : function(sender, args){
    return "test" + args[0] + args[1];
  }
}));

bus.registerBusObject(busObject);
bus.connect();
bus.requestName(SERVICE_NAME);

var portListener = new alljoyn.SessionPortListener(
  function(port, joiner){
    console.log("AcceptSessionJoiner", port, joiner);
    return port == SERVICE_PORT;
  },
  function(port, sId, joiner){
    sessionId = sId;
    console.log("SessionJoined", port, sessionId, joiner);
  }
);

console.log("bindSessionPort", bus.bindSessionPort(SERVICE_PORT, portListener));
console.log("FindAdvertisedName "+bus.advertiseName(SERVICE_NAME));