var alljoyn = require('bindings')('node-alljoyn');

console.log("Test loading alljoyn bus...", alljoyn);
var bus = alljoyn.BusAttachment("test");
var inter = alljoyn.InterfaceDescription();
console.log("Result: "+bus+" - "+inter);

console.log("CreateInterface "+bus.createInterface("Test", inter));
console.log("AddSignal "+inter.addSignal("testname", "s",  "str"));