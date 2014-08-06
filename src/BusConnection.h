#ifndef LD_BUSCONNECTION_H
#define LD_BUSCONNECTION_H

#include <nan.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(AlljoynBus);

class BusConnection : public node::ObjectWrap {
  private:
  	ajn::BusAttachment* bus;
    //BusBinding binding;
  	//InterfaceDescription* interface;
  	//QStatus status;
  	//int connected;

    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_METHOD(Join);
  public:
  	BusConnection(const char* shortName);
    static void Init ();
  	//void createInterface(const char* interfaceName, const char* signalName, const char* signalParams, const char* signalArgs));
    //void connectInterface();
    //void disconnect();
    //int connected(){ return connected};
    //QStatus getStatus(){return status};
};

#endif