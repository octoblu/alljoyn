#ifndef LD_SESSIONPORTLISTENERWRAPPER_H
#define LD_SESSIONPORTLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/AllJoynStd.h>
#include "SessionPortListenerImpl.h"

NAN_METHOD(SessionPortListenerConstructor);

class SessionPortListenerWrapper : public Nan::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
    SessionPortListenerWrapper(Nan::Callback* accept, Nan::Callback* joined);
    ~SessionPortListenerWrapper();
    static void Init ();

    SessionPortListenerImpl* listener;
};

#endif
