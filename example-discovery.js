var alljoyn = require('./');

console.log("Test loading alljoyn bus...", alljoyn);
var sessionId = 0;
var portNumber = 27;
var advertisedName = "org.alljoyn.bus.samples.chat.test";
var bus = alljoyn.BusAttachment("chat");
var inter = alljoyn.InterfaceDescription();
var listener = alljoyn.BusListener(
  function(name){
    console.log("FoundAdvertisedName", name);
  },
  function(name){
    console.log("LostAdvertisedName", name);
  },
  function(name){
    console.log("NameOwnerChanged", name);
  }
);
bus.registerBusListener(listener);
console.log("Start "+bus.start());
console.log("Connect"+bus.connect());
console.log("FindAdvertisedName "+bus.findAdvertisedName(''));
