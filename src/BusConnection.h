#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

class BusConnection {
	ajn::BusAttachment* bus;
	InterfaceDescription* interface;
	QStatus status;
  public:
    void disconnect();
    QStatus getStatus(){return status};
};