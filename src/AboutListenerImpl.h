#ifndef LD_ABOUTLISTENERIMPL_H
#define LD_ABOUTLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/AllJoynStd.h>
#include <functional>

#include "AboutListener.h"
#include "UVThreadSwitcher.h"

using namespace ajn;
using namespace Nan;

struct AnnouncedData{
  const char* busName;
  uint16_t version;
  SessionPort port;
  ajn::MsgArg* objectDescriptionArg;
  ajn::MsgArg* aboutDataArg;
};

class AboutListenerImpl : public AboutListener {
  Callback *jsCallback;
  UVThreadSwitcher *worker;

  public:
    AboutListenerImpl(Callback *callback);
    virtual ~AboutListenerImpl();

  private:
    void uvCallback(void *userData);
    void Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg);
};


#endif