var alljoyn = require('./');

console.log("Test loading alljoyn bus...", alljoyn);
var bus = alljoyn.BusAttachment("chat");

console.log("Start "+bus.start());
console.log("Connect "+bus.connect());

var PORT_NUMBER = 0;
var notificationService = alljoyn.NotificationService("MyNotificationTestName", bus, PORT_NUMBER);
console.log("Created Notification Service: "+notificationService);
var TTL_SECONDS = 300;
console.log("Send notification: "+notificationService.notify("Hello from Octoblu!", TTL_SECONDS));
setTimeout(function(){
	console.log("Finished, shutting down.");
	bus.disconnect();
	bus.stop();
	bus.join();
}, 1000);