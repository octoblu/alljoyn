#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/AllJoynStd.h>

class BusConnection {
	ajn::BusAttachment* bus;
  BusBinding binding;
	InterfaceDescription* interface;
	QStatus status;
	int connected;
  public:
  	BusAttachment(const char* shortName);
  	void createInterface(const char* interfaceName, const char* signalName, const char* signalParams, const char* signalArgs));
	void connectInterface();
    void disconnect();
    int connected(){ return connected};
    QStatus getStatus(){return status};
};