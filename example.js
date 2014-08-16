var alljoyn = require('./');

console.log("Test loading alljoyn bus...", alljoyn);
var sessionId = 0;
var bus = alljoyn.BusAttachment("test");
var inter = alljoyn.InterfaceDescription();
var listener = alljoyn.BusListener(
  function(name){
    console.log("FoundAdvertisedName", name);
    sessionId = bus.joinSession(name, 27, 0);
    console.log("JoinSession "+sessionId);
  },
  function(name){
    console.log("LostAdvertisedName", name);
  },
  function(name){
    console.log("NameOwnerChanged", name);
  }
);
var portListener = alljoyn.SessionPortListener(
	function(joiner){
    	console.log("AcceptSessionJoiner", joiner);
    	return true;
	},
	function(joiner){
    	console.log("SessionJoined", joiner);
	}
);
console.log("Result: "+bus+" - "+inter+" - "+listener+" - "+portListener);

console.log("CreateInterface "+bus.createInterface('org.alljoyn', inter));
console.log("AddSignal "+inter.addSignal("Chat", "s",  "str"));
console.log("RegisterBusListener "+bus.registerBusListener(listener));
console.log("Start "+bus.start());
var chatObject = alljoyn.BusObject("/chatService");
//console.log("chat.AddInterface "+chatObject.addInterface(inter));
console.log("RegisterBusObject "+bus.registerBusObject(chatObject));
console.log("Connect"+bus.connect());
console.log("FindAdvertisedName "+bus.findAdvertisedName('org.alljoyn'));

//console.log("BindSessionPort "+bus.bindSessionPort(27, portListener));