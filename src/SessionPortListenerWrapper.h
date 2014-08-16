#ifndef LD_SESSIONPORTLISTENERWRAPPER_H
#define LD_SESSIONPORTLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/AllJoynStd.h>
#include "SessionPortListenerImpl.h"

NAN_METHOD(SessionPortListenerConstructor);

class SessionPortListenerWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
  	SessionPortListenerWrapper(NanCallback* accept, NanCallback* joined);
  	~SessionPortListenerWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    SessionPortListenerImpl* listener;
};

#endif