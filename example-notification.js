var alljoyn = require('./');

var SegfaultHandler = require('segfault-handler');

console.log(SegfaultHandler);
SegfaultHandler.registerHandler();

console.log("Test loading alljoyn bus...", alljoyn);
var bus = alljoyn.BusAttachment("chat");

console.log("Start "+bus.start());
console.log("Connect "+bus.connect());

var PORT_NUMBER = 0;
var notificationService = new alljoyn.NotificationService("MyNotificationTestName", bus, PORT_NUMBER);
console.log("Created Notification Service: "+notificationService);
var TTL_SECONDS = 300;
console.log("Send notification: "+notificationService.notify("Hello Nan v2", TTL_SECONDS));
setTimeout(function(){
	console.log("Finished, shutting down.");
	bus.disconnect();
	bus.stop();
	bus.join();
}, 1000);