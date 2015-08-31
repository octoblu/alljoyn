var alljoyn = require('./');

console.log("Test loading alljoyn bus...", alljoyn);
var SegfaultHandler = require('segfault-handler');

console.log(SegfaultHandler);
SegfaultHandler.registerHandler();


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

bus.start();
bus.connect();

var sessionId;

var listener = new alljoyn.BusListener(
  function(name){
    console.log("FoundAdvertisedName", name);
    sessionId = bus.joinSession(name, SERVICE_PORT, 0);
    console.log("JoinSession "+sessionId);

    var proxyObj = new alljoyn.ProxyBusObject(bus, SERVICE_NAME, SERVICE_PATH, sessionId);
    console.log("proxyObj", proxyObj);
    console.log("getInterface", bus.getInterface(INTERFACE_NAME, intf));
    console.log("addInterface", proxyObj.addInterface(intf));

    var params = {
      interfaceName : INTERFACE_NAME,
      methodName : "cat",
      args : [
        {
          signature : "s",
          value : "hello"
        },
        {
          signature : "s",
          value : "world"
        },
      ],
      callback : function(err, sender, returnVal){
        console.log("methodCall callback", arguments);
      }
    }

    console.log(params);

    console.log("methodCall",proxyObj.methodCall(params));

  },
  function(name){
    console.log("LostAdvertisedName", name);
  },
  function(name){
    console.log("NameOwnerChanged", name);
  }
);

bus.registerBusListener(listener);
console.log("FindAdvertisedName "+bus.findAdvertisedName(SERVICE_NAME));


// proxyObj.addInterface(intf);



