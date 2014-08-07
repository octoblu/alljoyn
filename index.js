var alljoyn = require('bindings')('node-alljoyn');

console.log("Test loading alljoyn bus...");
var bus = AlljoynBus();
console.log("Result: "+bus);