# AllJoyn

## Installation

`npm install alljoyn`

## Usage

```js
var alljoyn = require('alljoyn');

// create a bus
var bus = alljoyn.BusAttachment('test');

// create interface description
var inter = alljoyn.InterfaceDescription();

// create listener
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

// create the interface
bus.createInterface('org.alljoyn', inter);

// register the listener on the bus
bus.registerBusListener(listener)

// initialize the bus
bus.start();

// start listening
bus.connect();

// discover devices with prefix 'org.alljoyn'
bus.findAdvertisedName('org.alljoyn')
```

## Currently Implemented

* discovery

## Currently Supported Operating Systems

* Mac OSX

## License

MIT
