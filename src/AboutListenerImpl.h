#ifndef LD_ABOUTLISTENERIMPL_H
#define LD_ABOUTLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/AllJoynStd.h>

#include "AboutListener.h"

using namespace ajn;
using namespace Nan;

class AboutListenerImpl : public AboutListener {
    uv_loop_t *loop;
    uv_async_t about_async;
    uv_rwlock_t calllock;

    struct CallbackHolder{
      Nan::Callback* callback;
      const char* busName;
      uint16_t version;
      SessionPort port;
      ajn::MsgArg* objectDescriptionArg;
      ajn::MsgArg* aboutDataArg;
    } aboutCallback;


  public:
    AboutListenerImpl(Callback *callback);


  private:
    static void about_callback(uv_async_t *handle, int status);

    void Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg);
};


#endif