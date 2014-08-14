var alljoyn = require('bindings')('node-alljoyn');

console.log("Test loading alljoyn bus...", alljoyn);
var bus = alljoyn.BusAttachment("test");
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
console.log("Result: "+bus+" - "+inter+" - "+listener);

console.log("CreateInterface "+bus.createInterface("", inter));
console.log("AddSignal "+inter.addSignal("Chat", "s",  "str"));
console.log("RegisterBusListener "+bus.registerBusListener(listener));
console.log("Start "+bus.start());
var chatObject = alljoyn.BusObject("/chatService");
//console.log("chat.AddInterface "+chatObject.addInterface(inter));
console.log("RegisterBusObject "+bus.registerBusObject(chatObject));
console.log("Connect"+bus.connect());
console.log("FindAdvertisedName "+bus.findAdvertisedName(""));