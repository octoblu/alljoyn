var alljoyn = require('./');

var SegfaultHandler = require('segfault-handler');

console.log(SegfaultHandler);
SegfaultHandler.registerHandler();

console.log("Test loading alljoyn bus...", alljoyn);
var bus = alljoyn.BusAttachment("about");
console.log(bus);

console.log("Start "+bus.start());
console.log("Connect "+bus.connect());

bus.whoImplements(["org.alljoyn.About", "org.alljoyn.Icon"]);

bus.registerAboutListener(function(busName, version, port, descriptions, aboutData){
	console.log("busName : " + busName);
	console.log("version : " + version);
	console.log("port : " + port);
	console.log("descriptions");
	for(var i in descriptions){
		var description = descriptions[i];
		console.log(description);
	}
	console.log("aboutData");
	for(var key in aboutData){
		var value = aboutData[key];
		console.log(key + " : " + value);
	}
});


setTimeout(function(){
	console.log("Finished, shutting down.");
	bus.disconnect();
	bus.stop();
	bus.join();
}, 10000);