#ifndef ANALOG_SERVER_H_
#define ANALOG_SERVER_H_

 //#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>

/* Using custom namespace to avoid name conflict while linking since helper
 * class for the AnalogWrapper could been used also for other devices in other
 * part of the code.
 * Better would be use internal linkage, through unnamed namespace therefore
 * enforcing internal linkage, but compiler correctly complains issuing a
 * warning message, because th eclass is used as a member of another class
 * outside the anonymous namespace.
 * This is not a problem actually, but seeing a lot of warning messages is annoying.
 */

namespace analogWrapper_yarp_internal_namespace
{
    class AnalogServerHandler;
    class AnalogPortEntry;
}

namespace yarp{
    namespace dev{
        class AnalogWrapper;
    }
}

namespace analogWrapper_yarp_internal_namespace {
/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
 **/
class AnalogServerHandler: public yarp::os::PortReader
{
    yarp::dev::IAnalogSensor* is;   // analog sensor to calibrate, when required
    yarp::os::Port rpcPort;         // rpc port related to the analog sensor

public:
    AnalogServerHandler(const char* n);
    ~AnalogServerHandler();

    void setInterface(yarp::dev::IAnalogSensor *is);

    bool _handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply);

    virtual bool read(yarp::os::ConnectionReader& connection);
};


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */
struct AnalogPortEntry
{
public:
    yarp::os::BufferedPort<yarp::sig::Vector> port;
    std::string port_name;      // the complete name of the port
    int offset;                 // an offset, the port is mapped starting from this taxel
    int length;                 // length of the output vector of the port (-1 for max length)
    AnalogPortEntry();
    AnalogPortEntry(const AnalogPortEntry &alt);
    AnalogPortEntry &operator =(const AnalogPortEntry &alt);
};

}  // closing namespace analogWrapper_yarp_internal_namespace

/**
  * It reads the data from an analog sensor and sends them on one or more ports.
  * It creates one rpc port and its related handler for every output port.
  */
class yarp::dev::AnalogWrapper:  public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper
{
private:
    std::string rpcPortName;
    std::string id;
    yarp::dev::IAnalogSensor *analogSensor_p;   // the analog sensor to read from
    std::vector<analogWrapper_yarp_internal_namespace::AnalogPortEntry> analogPorts;   // the list of output ports
    std::vector<analogWrapper_yarp_internal_namespace::AnalogServerHandler*> handlers; // the list of rpc port handlers
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
    int _rate;
    void setHandlers();
    void removeHandlers();

public:

    // Constructor used by yarp factory
    AnalogWrapper();

    // Constructor used when there is only one output port  -- obsolete, here for backward compatibility with skinwrapper
    AnalogWrapper(const char* name, int rate=20);

    // Contructor used when one or more output ports are specified  -- obsolete, here for backward compatibility with skinwrapper
    AnalogWrapper(const std::vector<analogWrapper_yarp_internal_namespace::AnalogPortEntry>& _analogPorts, int rate=20);

    ~AnalogWrapper();

    bool open(yarp::os::Searchable &params);
    bool close();
    yarp::os::Bottle getOptions();

    void setId(const std::string &i);
    std::string getId();

    /**
      * Specify which analog sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p);
    bool detachAll();

    void attach(yarp::dev::IAnalogSensor *s);
    void detach();

    bool threadInit();
    void threadRelease();
    void run();

private:
    // Function used when there is only one output port
    bool createPort(const char* name, int rate=20);
    // Function used when one or more output ports are specified
    bool createPorts(const std::vector<analogWrapper_yarp_internal_namespace::AnalogPortEntry>& _analogPorts, int rate=20);
};

#endif
