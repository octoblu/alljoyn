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
    sessionId = bus.joinSession(name, portNumber, 0);
    console.log("JoinSession "+sessionId);
    setTimeout(function(){
      chatObject.signal(null, sessionId, inter, "Chat", "Hello, I am the client!");
    }, 1000);
  },
  function(name){
    console.log("LostAdvertisedName", name);
  },
  function(name){
    console.log("NameOwnerChanged", name);
  }
);
var portListener = alljoyn.SessionPortListener(
  function(port, joiner){
      console.log("AcceptSessionJoiner", port, joiner);
      return true;
  },
  function(port, sessionId, joiner){
    console.log("SessionJoined", port, sessionId, joiner);
  }
);

console.log("CreateInterface "+bus.createInterface("org.alljoyn.bus.samples.chat", inter));
console.log("AddSignal "+inter.addSignal("Chat", "s",  "msg"));
bus.registerBusListener(listener);
console.log("Start "+bus.start());
var chatObject = alljoyn.BusObject("/chatService");
console.log("chat.AddInterface "+chatObject.addInterface(inter));
console.log("RegisterSignalHandler "+bus.registerSignalHandler(chatObject, function(msg, info){
  // console.log("Signal received: ", msg, info);
  console.log(msg["0"]);
}, inter, "Chat"));
console.log("RegisterBusObject "+bus.registerBusObject(chatObject));
console.log("Connect"+bus.connect());
console.log("FindAdvertisedName "+bus.findAdvertisedName('org.alljoyn.bus.samples.chat'));

// Added Chat to example
var stdin = process.stdin;

// without this, we would only get streams once enter is pressed
stdin.setRawMode( true );

// resume stdin in the parent process (node app won't quit all by itself
// unless an error or process.exit() happens)
stdin.resume();

// i don't want binary, do you?
stdin.setEncoding( 'utf8' );

// on any data into stdin
stdin.on( 'data', function( key ){
  // ctrl-c ( end of text )
  if ( key === '\u0003' ) {
    process.exit();
  }
  // write the key to stdout all normal like
  process.stdout.write( key + '\n' );
  // chatObject.signal(null, sessionId, inter, 'hello' );
  chatObject.signal(null, sessionId, inter, "Chat", key);
});
