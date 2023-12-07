/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "AnalogWrapper.h"
#include <sstream>
#include <iostream>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;


YARP_LOG_COMPONENT(ANALOGWRAPPER, "yarp.devices.AnalogWrapper")

/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
 **/
class AnalogServerHandler :
        public yarp::os::PortReader
{
    yarp::dev::IAnalogSensor* is;   // analog sensor to calibrate, when required
    yarp::os::Port rpcPort;         // rpc port related to the analog sensor

public:
    AnalogServerHandler(const char* n);
    ~AnalogServerHandler();

    void setInterface(yarp::dev::IAnalogSensor *is);

    bool _handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply);

    bool read(yarp::os::ConnectionReader& connection) override;
};


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */
class AnalogPortEntry
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


/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
  **/

AnalogServerHandler::AnalogServerHandler(const char* n) : is(nullptr)
{
    rpcPort.open(n);
    rpcPort.setReader(*this);
}

AnalogServerHandler::~AnalogServerHandler()
{
    rpcPort.close();
    is = nullptr;
}

void AnalogServerHandler::setInterface(yarp::dev::IAnalogSensor *is)
{
    this->is = is;
}

bool AnalogServerHandler::_handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply)
{
    if (is == nullptr) {
        return false;
    }

    const size_t msgsize=cmd.size();
    int ret=IAnalogSensor::AS_ERROR;

    int code=cmd.get(1).asVocab32();
    switch (code)
    {
    case VOCAB_CALIBRATE:
        if (msgsize == 2) {
            ret = is->calibrateSensor();
        } else if (msgsize > 2) {
            size_t offset = 2;
            Vector v(msgsize - offset);
            for (unsigned int i = 0; i < v.size(); i++) {
                v[i] = cmd.get(i + offset).asFloat64();
            }
            ret = is->calibrateSensor(v);
        }
      break;
    case VOCAB_CALIBRATE_CHANNEL:
      if (msgsize==3)
      {
        int ch=cmd.get(2).asInt32();
        ret=is->calibrateChannel(ch);
      }
      else if (msgsize==4)
      {
        int ch=cmd.get(2).asInt32();
        double v=cmd.get(3).asFloat64();
        ret=is->calibrateChannel(ch, v);
      }
      break;
    default:
      return false;
    }

    reply.addInt32(ret);
    return true;
}

bool AnalogServerHandler::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok=in.read(connection);
    if (!ok) {
        return false;
    }

    // parse in, prepare out
    int code = in.get(0).asVocab32();
    bool ret=false;
    if (code==VOCAB_IANALOG)
    {
        ret=_handleIAnalog(in, out);
    }

    if (!ret)
    {
        out.clear();
        out.addVocab32(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender!=nullptr) {
        out.write(*returnToSender);
    }
    return true;
}


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */

AnalogPortEntry::AnalogPortEntry() :
    offset(0),
    length(0)
{}

AnalogPortEntry::AnalogPortEntry(const AnalogPortEntry &alt)
{
    this->length = alt.length;
    this->offset = alt.offset;
    this->port_name = alt.port_name;
}

AnalogPortEntry &AnalogPortEntry::operator =(const AnalogPortEntry &alt)
{
    this->length = alt.length;
    this->offset = alt.offset;
    this->port_name = alt.port_name;
    return *this;
}

 // closing anonimous namespace


/**
  * It reads the data from an analog sensor and sends them on one or more ports.
  * It creates one rpc port and its related handler for every output port.
  */

bool AnalogWrapper::createPort(const char* name, int rate)
{
    analogSensor_p=nullptr;
    analogPorts.resize(1);
    analogPorts[0].offset = 0;
    analogPorts[0].length = -1; // max length
    analogPorts[0].port_name = std::string(name);
    setHandlers();
    setPeriod(rate / 1000.0);
    return true;
}

bool AnalogWrapper::createPorts(const std::vector<AnalogPortEntry>& _analogPorts, int rate)
{
    analogSensor_p=nullptr;
    this->analogPorts=_analogPorts;
    setHandlers();
    setPeriod(rate / 1000.0);
    return true;
}

AnalogWrapper::AnalogWrapper() :
        PeriodicThread(DEFAULT_THREAD_PERIOD / 1000.0)
{
}

AnalogWrapper::~AnalogWrapper()
{
    threadRelease();
    close();
    _rate = DEFAULT_THREAD_PERIOD;
    analogSensor_p = nullptr;
}

void AnalogWrapper::setHandlers()
{
    for(auto& analogPort : analogPorts)
    {
        std::string rpcPortName = analogPort.port_name;
        rpcPortName += "/rpc:i";
        auto* ash = new AnalogServerHandler(rpcPortName.c_str());
        handlers.push_back(ash);
    }
}

void AnalogWrapper::removeHandlers()
{
    for(auto& handler : handlers)
    {
        if (handler != nullptr)
        {
            delete handler;
            handler = nullptr;
        }
    }
    handlers.clear();
}

bool AnalogWrapper::openAndAttachSubDevice(Searchable &prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

//     p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device", prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(ANALOGWRAPPER, "opening AnalogWrapper subdevice...");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yCError(ANALOGWRAPPER, "opening AnalogWrapper subdevice... FAILED\n");
        return false;
    }

    subDeviceOwned->view(analogSensor_p);

    if (analogSensor_p == nullptr)
    {
        yCError(ANALOGWRAPPER, "Opening IAnalogSensor interface of AnalogWrapper subdevice... FAILED\n");
        return false;
    }

    int chNum = analogSensor_p->getChannels();

    if (chNum <= 0)
    {
        yCError(ANALOGWRAPPER, "Calling analog sensor has invalid channels number %d.\n", chNum);
        return false;
    }

    attach(analogSensor_p);
    PeriodicThread::setPeriod(_rate / 1000.0);
    return PeriodicThread::start();
}


bool AnalogWrapper::openDeferredAttach(yarp::os::Searchable &prop)
{
    // nothing to do here?
    if ((subDeviceOwned != nullptr) || (ownDevices == true)) {
        yCError(ANALOGWRAPPER) << "AnalogWrapper: something wrong with the initialization.";
    }
    return true;
}


/**
  * Specify which analog sensor this thread has to read from.
  */

bool AnalogWrapper::attachAll(const PolyDriverList &analog2attach)
{
    //check if we already instantiated a subdevice previously
    if (ownDevices) {
        return false;
    }

    if (analog2attach.size() != 1)
    {
        yCError(ANALOGWRAPPER, "AnalogWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach=analog2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(analogSensor_p);
    }

    if(nullptr == analogSensor_p)
    {
        yCError(ANALOGWRAPPER, "AnalogWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(analogSensor_p);
    PeriodicThread::setPeriod(_rate / 1000.0);
    return PeriodicThread::start();
}

bool AnalogWrapper::detachAll()
{
    //check if we already instantiated a subdevice previously
    if (ownDevices) {
        return false;
    }

    analogSensor_p = nullptr;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        if (handlers[i] != nullptr) {
            handlers[i]->setInterface(analogSensor_p);
        }
    }
    return true;
}

void AnalogWrapper::attach(yarp::dev::IAnalogSensor *s)
{
    analogSensor_p=s;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
    //Resize vector of read data to avoid further allocation of memory
    //as long as the number of channels does not change
    lastDataRead.resize((size_t)analogSensor_p->getChannels(),0.0);
}

void AnalogWrapper::detach()
{
    // Set interface to NULL
    analogSensor_p = nullptr;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
}

bool AnalogWrapper::threadInit()
{
    for(auto& analogPort : analogPorts)
    {
        // open data port
        if (!analogPort.port.open(analogPort.port_name))
           {
               yCError(ANALOGWRAPPER, "AnalogWrapper: failed to open port %s", analogPort.port_name.c_str());
               return false;
           }
    }
    return true;
}

bool AnalogWrapper::open(yarp::os::Searchable &config)
{
    yCWarning(ANALOGWRAPPER) << "The 'AnalogWrapper' device is deprecated.";
    yCWarning(ANALOGWRAPPER) << "Possible alternatives, depending on the specific type sensor data, are:";
    yCWarning(ANALOGWRAPPER) << "'MultipleAnalogSensorsRemapper`+`MultipleAnalogSensorsServer`, `PoseStampedRosPublisher`, `WrenchStampedRosPublisher`,`IMURosPublisher`,etc.";
    yCWarning(ANALOGWRAPPER) << "The old device is no longer supported, and it will be deprecated in YARP 3.7 and removed in YARP 4.";
    yCWarning(ANALOGWRAPPER) << "Please update your scripts.";

    Property params;
    params.fromString(config.toString());
    yCTrace(ANALOGWRAPPER) << "AnalogWrapper params are: " << config.toString();

    if (!config.check("period"))
    {
        yCError(ANALOGWRAPPER) << "AnalogWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
    {
        _rate = config.find("period").asInt32();
    }

    if (config.check("deviceId"))
    {
        yCError(ANALOGWRAPPER) << "AnalogWrapper: the parameter 'deviceId' has been removed, please use parameter 'name' instead. \n"
            << "e.g. In the FT wrapper configuration files of your robot, replace '<param name=""deviceId""> left_arm </param>' \n"
            << "with '/icub/left_arm/analog:o' ";
        return false;
    }

    if (!config.check("name"))
    {
        yCError(ANALOGWRAPPER) << "AnalogWrapper: missing 'name' parameter. Check you configuration file; it must be like:\n"
                    "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString();
    }

    if(!initialize_YARP(config) )
    {
        yCError(ANALOGWRAPPER) << sensorId << "Error initializing YARP ports";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(config.check("subdevice"))
    {
        ownDevices=true;
        if(! openAndAttachSubDevice(config))
        {
            yCError(ANALOGWRAPPER, "AnalogWrapper: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        ownDevices=false;
        if (!openDeferredAttach(config)) {
            return false;
        }
    }

    return true;
}

bool AnalogWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    // Create the list of ports
    // port names are optional, do not check for correctness.
    if(!params.check("ports"))
    {
        // if there is no "ports" section open only 1 port and use name as is.
        if (Network::exists(streamingPortName + "/rpc:i") || Network::exists(streamingPortName))
        {
            yCError(ANALOGWRAPPER) << "AnalogWrapper: unable to open the analog server, address conflict";
            return false;
        }
        createPort((streamingPortName ).c_str(), _rate );
        // since createPort always return true, check the port is really been opened is done here
        if (!Network::exists(streamingPortName + "/rpc:i")) {
            return false;
        }
    }
    else
    {
        Bottle *ports=params.find("ports").asList();

        Value &deviceChannels =  params.find("channels");
        if (deviceChannels.isNull())
        {
            yCError(ANALOGWRAPPER, "AnalogWrapper: 'channels' parameters was not found in config file.");
            return false;
        }

        int nports=ports->size();
        int sumOfChannels = 0;
        std::vector<AnalogPortEntry> tmpPorts;
        tmpPorts.resize(nports);

        for(size_t k=0; k<ports->size(); k++)
        {
            Bottle parameters=params.findGroup(ports->get(k).asString());

            if (parameters.size()!=5)
            {
                yCError(ANALOGWRAPPER) << "AnalogWrapper: check skin port parameters in part description, I was expecting "
                            << ports->get(k).asString().c_str() << " followed by four integers";
                yCError(ANALOGWRAPPER) << " your param is " << parameters.toString();
                return false;
            }

            if (Network::exists(streamingPortName + "/" + std::string(ports->get(k).asString()) + "/rpc:i")
                || Network::exists(streamingPortName + "/" + std::string(ports->get(k).asString())))
            {
                yCError(ANALOGWRAPPER) << "AnalogWrapper: unable to open the analog server, address conflict";
                return false;
            }
            int wBase=parameters.get(1).asInt32();
            int wTop=parameters.get(2).asInt32();
            int base=parameters.get(3).asInt32();
            int top=parameters.get(4).asInt32();

            yCDebug(ANALOGWRAPPER) << "--> " << wBase << " " << wTop << " " << base << " " << top;

            //check consistenty
            if(wTop-wBase != top-base){
                yCError(ANALOGWRAPPER) << "AnalogWrapper: numbers of mapped taxels do not match, check "
                            << ports->get(k).asString().c_str() << " port parameters in part description";
                return false;
            }
            int portChannels = top-base+1;

            tmpPorts[k].length = portChannels;
            tmpPorts[k].offset = wBase;
            yCDebug(ANALOGWRAPPER) << "opening port " << ports->get(k).asString().c_str();
            tmpPorts[k].port_name = streamingPortName+ "/" + std::string(ports->get(k).asString());

            sumOfChannels+=portChannels;
        }
        createPorts(tmpPorts, _rate);

        if (sumOfChannels!=deviceChannels.asInt32())
        {
            yCError(ANALOGWRAPPER) << "AnalogWrapper: Total number of mapped taxels does not correspond to total taxels";
            return false;
        }
    }
    return true;
}

void AnalogWrapper::threadRelease()
{
    for(auto& analogPort : analogPorts)
    {
        analogPort.port.interrupt();
        analogPort.port.close();
    }
}

void AnalogWrapper::run()
{
    int first, last, ret;

    if (analogSensor_p!=nullptr)
    {
        ret=analogSensor_p->read(lastDataRead);

        if (ret==yarp::dev::IAnalogSensor::AS_OK)
        {
            if (lastDataRead.size()>0)
            {
                if(1)
                {
                    lastStateStamp.update();
                    // send the data on the port(s), splitting them as specified in the config file
                    for(auto& analogPort : analogPorts)
                    {
                        yarp::sig::Vector &pv = analogPort.port.prepare();
                        first = analogPort.offset;
                        if (analogPort.length == -1) { // read the max length available
                            last = lastDataRead.size()-1;
                        } else {
                            last = analogPort.offset + analogPort.length - 1;
                        }

                        // check vector limit
                        if(last>=(int)lastDataRead.size()){
                            yCError(ANALOGWRAPPER, )<<"AnalogWrapper: error while sending analog sensor output on port "<< analogPort.port_name
                                    <<" Vector size expected to be at least "<<last<<" whereas it is "<< lastDataRead.size();
                            continue;
                        }
                        pv = lastDataRead.subVector(first, last);

                        analogPort.port.setEnvelope(lastStateStamp);
                        analogPort.port.write();
                    }
                }
            }
            else
            {
                yCError(ANALOGWRAPPER, "AnalogWrapper: %s: vector size non valid: %lu", sensorId.c_str(), static_cast<unsigned long> (lastDataRead.size()));
            }
        }
        else
        {
            switch(ret)
            {
                case IAnalogSensor::AS_OVF:
                    yCError(ANALOGWRAPPER, "AnalogWrapper: %s: Sensor returned overflow error (code %d).", sensorId.c_str(), ret);
                    break;
                case IAnalogSensor::AS_TIMEOUT:
                    yCError(ANALOGWRAPPER, "AnalogWrapper: %s: Sensor returned timeout error (code %d).", sensorId.c_str(), ret);
                    break;
                case IAnalogSensor::AS_ERROR:
                default:
                    yCError(ANALOGWRAPPER, "AnalogWrapper: %s: Sensor returned error with code %d.", sensorId.c_str(), ret);
                    break;
            }
        }
    }
}

bool AnalogWrapper::close()
{
    yCTrace(ANALOGWRAPPER, "AnalogWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    removeHandlers();

    if(subDeviceOwned)
    {
        subDeviceOwned->close();
        delete subDeviceOwned;
        subDeviceOwned = nullptr;
    }

    return true;
}
