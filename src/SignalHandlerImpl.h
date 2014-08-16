#ifndef LD_SignalHandlerIMPL_H
#define LD_SignalHandlerIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <InterfaceDescription.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include <Message.h>

class SignalHandlerImpl {
  private:
  	uv_loop_t *loop;
  	uv_async_t signal_async;

  struct CallbackHolder{
    NanCallback* callback;
    const char* data;
  } signalCallback;

  public:
  	SignalHandlerImpl(NanCallback* sig);
  	~SignalHandlerImpl();
  	static void signal_callback(uv_async_t *handle, int status);

    void Signal(const ajn::InterfaceDescription::Member *member, const char *srcPath, ajn::Message &message);
};

#endif