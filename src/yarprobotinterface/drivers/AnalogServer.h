/*
 * Copyright (C) 2012 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino, Daniele E. Domenichelli
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef ANALOG_SERVER_H
#define ANALOG_SERVER_H

#include <yarp/os/BufferedPort.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <string>
#include <vector>

namespace yarp {
    namespace dev {
        class IAnalogSensor;
    }
}


/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
 **/
class AnalogServerHandler : public yarp::os::PortReader
{
    yarp::dev::IAnalogSensor* is;   // analog sensor to calibrate, when required
    yarp::os::Port rpcPort;         // rpc port related to the analog sensor

public:
    AnalogServerHandler(const char* n);
    virtual ~AnalogServerHandler();

    virtual bool read(yarp::os::ConnectionReader& connection);

    void setInterface(yarp::dev::IAnalogSensor *is);
    bool _handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply);

private:
    // Copy disabled
    AnalogServerHandler(const AnalogServerHandler&);
    AnalogServerHandler &operator=(const AnalogServerHandler&);
};


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */
struct AnalogPortEntry
{
    AnalogPortEntry();
    AnalogPortEntry(const AnalogPortEntry &alt);
    AnalogPortEntry &operator=(const AnalogPortEntry &alt);

    yarp::os::BufferedPort<yarp::sig::Vector> port;
    std::string port_name;      // the complete name of the port
    int offset;                 // an offset, the port is mapped starting from this taxel
    int length;                 // length of the output vector of the port (-1 for max length)
};


/**
  * It reads the data from an analog sensor and sends them on one or more ports.
  * It creates one rpc port and its related handler for every output port.
  */
class AnalogServer : public yarp::os::RateThread
{
public:
    /**
     * Constructor used when there is only one output port
     */
    AnalogServer(const char* name, int rate = 20);

    /**
     * Contructor used when one or more output ports are specified
     */
    AnalogServer(const std::vector<AnalogPortEntry> &_analogPorts, int rate = 20);

    virtual ~AnalogServer();

    /**
      * Specify which analog sensor this thread has to read from.
      */
    void attach(yarp::dev::IAnalogSensor *s);

    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

private:
    void setHandlers();

    yarp::dev::IAnalogSensor *is;               // the analog sensor to read from
    std::vector<AnalogPortEntry> analogPorts;   // the list of output ports
    std::vector<AnalogServerHandler*> handlers; // the list of rpc port handlers
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
};

#endif // ANALOG_SERVER_H