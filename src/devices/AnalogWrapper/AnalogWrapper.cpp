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
using namespace std;


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
    if (is==nullptr)
      return false;

    const size_t msgsize=cmd.size();
    int ret=IAnalogSensor::AS_ERROR;

    int code=cmd.get(1).asVocab32();
    switch (code)
    {
    case VOCAB_CALIBRATE:
      if (msgsize==2)
        ret=is->calibrateSensor();
      else if (msgsize>2)
      {
        size_t offset=2;
        Vector v(msgsize-offset);
        for (unsigned int i=0; i<v.size(); i++)
        {
          v[i]=cmd.get(i+offset).asFloat64();
        }
        ret=is->calibrateSensor(v);
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
    if (!ok) return false;

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
    // init ROS struct
    frame_idVec.resize(1);
    frame_idVec.at(0)      = "";
    rosTopicNamesVec.resize(1);
    rosTopicNamesVec.at(0) = "";
    rosMsgCounterVec.resize(1);
    rosMsgCounterVec.at(0) = 0;
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
    if( (subDeviceOwned != nullptr) || (ownDevices == true) )
        yCError(ANALOGWRAPPER) << "AnalogWrapper: something wrong with the initialization.";
    return true;
}


/**
  * Specify which analog sensor this thread has to read from.
  */

bool AnalogWrapper::attachAll(const PolyDriverList &analog2attach)
{
    //check if we already instantiated a subdevice previously
    if (ownDevices)
        return false;

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
    if (ownDevices)
        return false;

    analogSensor_p = nullptr;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        if(handlers[i] != nullptr)
            handlers[i]->setInterface(analogSensor_p);
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

void AnalogWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string AnalogWrapper::getId()
{
    return sensorId;
}

bool AnalogWrapper::checkROSParams(Searchable &config)
{
    // check for ROS parameter group
    if(!config.check("ROS") )
    {
        useROS = ROS_disabled;
        yCInfo(ANALOGWRAPPER)  << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }

    yCInfo(ANALOGWRAPPER)  << "ROS group was FOUND in config file.";

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        yCError(ANALOGWRAPPER) << sensorId << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if(!rosGroup.check("useROS"))
    {
        yCError(ANALOGWRAPPER) << sensorId << " cannot find useROS parameter, mandatory when using ROS message. \n \
                    Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    std::string ros_use_type = rosGroup.find("useROS").asString();
    if(ros_use_type == "false")
    {
        yCInfo(ANALOGWRAPPER) << sensorId << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }
    else if(ros_use_type == "true")
    {
        yCInfo(ANALOGWRAPPER) << sensorId << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    }
    else if(ros_use_type == "only")
    {
        yCInfo(ANALOGWRAPPER) << sensorId << "useROS topic if set to 'only";
        useROS = ROS_only;
    }
    else
    {
        yCInfo(ANALOGWRAPPER) << sensorId << "useROS parameter is set to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if(!rosGroup.check("ROS_nodeName"))
    {
        yCError(ANALOGWRAPPER) << sensorId << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
    yCInfo(ANALOGWRAPPER) << sensorId << "rosNodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if(!rosGroup.check("ROS_topicName"))
    {
        yCError(ANALOGWRAPPER) << sensorId << " cannot find ROS_topicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }

    if(rosGroup.find("ROS_topicName").isString())
    {
        rosTopicNamesVec.at(0) = rosGroup.find("ROS_topicName").asString();
        yCInfo(ANALOGWRAPPER) << sensorId << "ROS_topicName is " << rosTopicNamesVec.at(0);

    }
    else if(rosGroup.find("ROS_topicName").isList())
    {
        yarp::os::Bottle *rosTopicNamesBottle = rosGroup.find("ROS_topicName").asList();
        yCInfo(ANALOGWRAPPER) << sensorId << "ROS_topicName list is " << rosTopicNamesBottle->toString();

        rosTopicNamesVec.resize(rosTopicNamesBottle->size());
        for(size_t i = 0; i < rosTopicNamesBottle->size(); i++)
        {
            rosTopicNamesVec.at(i) = rosTopicNamesBottle->get(i).asString();
        }

        // resize the ros msg counter vector
        rosMsgCounterVec.resize(rosTopicNamesVec.size());
    }

    // check for ROS_msgType parameter
    if (!rosGroup.check("ROS_msgType"))
    {
        yCError(ANALOGWRAPPER) << sensorId << " cannot find ROS_msgType parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosMsgType = rosGroup.find("ROS_msgType").asString();

    // check for frame_id parameter
    if (rosMsgType == "geometry_msgs/WrenchStamped")
    {
        yCInfo(ANALOGWRAPPER) << sensorId << "ROS_msgType is " << rosMsgType;
        if (!rosGroup.check("frame_id"))
        {
            yCError(ANALOGWRAPPER) << sensorId << " cannot find frame_id parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }

        if(rosGroup.find("frame_id").isString())
        {
            frame_idVec.at(0) = rosGroup.find("frame_id").asString();
            yCInfo(ANALOGWRAPPER) << sensorId << "frame_id is " << frame_idVec.at(0);

        }
        else if(rosGroup.find("frame_id").isList())
        {
            yarp::os::Bottle *frame_idBottle = rosGroup.find("frame_id").asList();

            if(frame_idBottle->size() != rosTopicNamesVec.size())
            {
                yCError(ANALOGWRAPPER, "AnalogWrapper: mismatch between the number of ros topics and frame_ids");
                return false;
            }

            yCInfo(ANALOGWRAPPER) << sensorId << "frame_id list is " << frame_idBottle->toString();

            frame_idVec.resize(frame_idBottle->size());
            for(size_t i = 0; i < frame_idBottle->size(); i++)
            {
                frame_idVec.at(i) = frame_idBottle->get(i).asString();
            }
        }

        if(!rosGroup.check("rosOffset"))
        {
            yCWarning(ANALOGWRAPPER) << sensorId << "cannot find rosOffset parameter, using the default offset 0 while reading analog sensor data";
        }

        if(rosGroup.find("rosOffset").isInt32())
        {
            rosOffset = rosGroup.find("rosOffset").asInt32();
            yCInfo(ANALOGWRAPPER) << sensorId << " rosOffset is " << rosOffset;
        }

        if(!rosGroup.check("rosPadding"))
        {
            yCWarning(ANALOGWRAPPER) << sensorId << "cannot find rosPadding parameter, using the default padding 0 while reading analog sensor data";
        }

        if(rosGroup.find("rosPadding").isInt32())
        {
            rosOffset = rosGroup.find("rosPadding").asInt32();
            yCInfo(ANALOGWRAPPER) << sensorId << " rosPadding is " << rosOffset;
        }
    }
    else if (rosMsgType == "sensor_msgs/JointState")
    {
        std::string jointName;
        yCInfo(ANALOGWRAPPER) << sensorId << "ROS_msgType is " << rosMsgType;
        bool oldParam = false;
        bool newParam = false;

        if(rosGroup.check("joint_names"))
        {
            oldParam = true;
            jointName = "joint_names";
            yCWarning(ANALOGWRAPPER) << sensorId << " using DEPRECATED 'joint_names' parameter. Please use 'jointNames' instead.";
        }

        if(rosGroup.check("jointNames"))
        {
            newParam = true;
            jointName = "jointNames";
        }

        if(!oldParam && !newParam)
        {
            yCError(ANALOGWRAPPER) << sensorId << " missing 'jointNames' parameter needed when broadcasting 'sensor_msgs/JointState' message type";
            useROS = ROS_config_error;
            return false;
        }
        // Throw an error if both new and old are present
        if(oldParam && newParam)
        {
            yCError(ANALOGWRAPPER) << sensorId << " found both DEPRECATED 'joint_names' and new 'jointNames' parameters. Please remove the old 'joint_names' from your config file.";
            useROS = ROS_config_error;
            return false;
        }

        yarp::os::Bottle& jnam = rosGroup.findGroup(jointName);
        if(jnam.isNull())
        {
            yCError(ANALOGWRAPPER) << sensorId << "Cannot find 'jointNames' parameters.";
            return false;
        }

        // Cannot check number of channels here because need to wait for the attach function
        int joint_names_size = jnam.size()-1;
        for (int i = 0; i < joint_names_size; i++)
        {
            ros_joint_names.push_back(jnam.get(i+1).asString());
        }
    }
    else
    {
        yCError(ANALOGWRAPPER) << sensorId << "ROS_msgType '" << rosMsgType << "' not supported ";
        return false;
    }

    return true;
}

bool AnalogWrapper::initialize_ROS()
{
    bool success = false;
    switch(useROS)
    {
        case ROS_enabled:
        case ROS_only:
        {
            rosNode = new yarp::os::Node(rosNodeName);   // add a ROS node

            if(rosNode == nullptr)
            {
                yCError(ANALOGWRAPPER) << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            if (rosMsgType == "geometry_msgs/WrenchStamped")
            {
                rosPublisherWrenchPortVec.resize(rosTopicNamesVec.size());

                for(size_t i = 0; i < rosTopicNamesVec.size(); i++)
                {
                    if(!rosPublisherWrenchPortVec.at(i).topic(rosTopicNamesVec.at(i)))
                    {
                        yCError(ANALOGWRAPPER) << " opening " << rosTopicNamesVec.at(i) << " Topic, check your yarp-ROS network configuration\n";
                        success = false;
                        break;
                    }
                }
            }
            else if (rosMsgType == "sensor_msgs/JointState")
            {
                if (!rosPublisherJointPort.topic(rosTopicNamesVec.at(0)))
                {
                    yCError(ANALOGWRAPPER) << " opening " << rosTopicNamesVec.at(0) << " Topic, check your yarp-ROS network configuration\n";
                    success = false;
                    break;
                }
            }
            else
            {
                yCError(ANALOGWRAPPER) << sensorId << "Invalid rosMsgType: " << rosMsgType;
            }

            yCInfo(ANALOGWRAPPER) << sensorId << "ROS initialized successfully, node:" << rosNodeName << " and opened the following topics: ";
            for(size_t i = 0; i < rosTopicNamesVec.size(); i++)
            {
                yCInfo(ANALOGWRAPPER) << rosTopicNamesVec.at(0);
            }

            success = true;
        } break;

        case ROS_disabled:
        {
            yCInfo(ANALOGWRAPPER) << sensorId << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yCError(ANALOGWRAPPER) << sensorId << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yCError(ANALOGWRAPPER) << sensorId << " something went wrong with ROS configuration, we should never be here!!!";
            success = false;
        } break;
    }
    return success;
}

bool AnalogWrapper::open(yarp::os::Searchable &config)
{
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
        setId("AnalogServer");
    }

    if(!checkROSParams(config) )
    {
        yCError(ANALOGWRAPPER) << sensorId << "ROS parameter are not correct, check your configuration file";
        return false;
    }

    if(!initialize_YARP(config) )
    {
        yCError(ANALOGWRAPPER) << sensorId << "Error initializing YARP ports";
        return false;
    }

    if(!initialize_ROS() )
    {
        yCError(ANALOGWRAPPER) << sensorId << "Error initializing ROS topics";
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
        if(!openDeferredAttach(config))
            return false;
    }

    return true;
}

bool AnalogWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    switch(useROS)
    {
        case ROS_only:
        {
            yCInfo(ANALOGWRAPPER) << sensorId << " No YARP initialization required";
            return true;
        } break;

        default:
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
                if(! Network::exists(streamingPortName + "/rpc:i"))
                    return false;
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

                    if (Network::exists(streamingPortName + "/" + string(ports->get(k).asString()) + "/rpc:i")
                        || Network::exists(streamingPortName + "/" + string(ports->get(k).asString())))
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
                    tmpPorts[k].port_name = streamingPortName+ "/" + string(ports->get(k).asString());

                    sumOfChannels+=portChannels;
                }
                createPorts(tmpPorts, _rate);

                if (sumOfChannels!=deviceChannels.asInt32())
                {
                    yCError(ANALOGWRAPPER) << "AnalogWrapper: Total number of mapped taxels does not correspond to total taxels";
                    return false;
                }
            }
        } break;
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
                if(useROS != ROS_only)
                {
                    lastStateStamp.update();
                    // send the data on the port(s), splitting them as specified in the config file
                    for(auto& analogPort : analogPorts)
                    {
                        yarp::sig::Vector &pv = analogPort.port.prepare();
                        first = analogPort.offset;
                        if(analogPort.length == -1)   // read the max length available
                            last = lastDataRead.size()-1;
                        else
                            last = analogPort.offset + analogPort.length - 1;

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

                if (useROS != ROS_disabled && rosMsgType == "geometry_msgs/WrenchStamped")
                {
                    std::vector<yarp::rosmsg::geometry_msgs::WrenchStamped> rosDataVec;
                    rosDataVec.resize(rosPublisherWrenchPortVec.size());

                    for(size_t i = 0; i < rosDataVec.size(); i++)
                    {
                        rosDataVec.at(i).header.seq = rosMsgCounterVec.at(i)++;
                        rosDataVec.at(i).header.stamp = yarp::os::Time::now();
                        rosDataVec.at(i).header.frame_id = frame_idVec.at(i);

                        rosDataVec.at(i).wrench.force.x = lastDataRead[(rosOffset + 6 + rosPadding) * i + 0];
                        rosDataVec.at(i).wrench.force.y = lastDataRead[(rosOffset + 6 + rosPadding) * i + 1];
                        rosDataVec.at(i).wrench.force.z = lastDataRead[(rosOffset + 6 + rosPadding) * i + 2];

                        rosDataVec.at(i).wrench.torque.x = lastDataRead[(rosOffset + 6 + rosPadding) * i + 3];
                        rosDataVec.at(i).wrench.torque.y = lastDataRead[(rosOffset + 6 + rosPadding) * i + 4];
                        rosDataVec.at(i).wrench.torque.z = lastDataRead[(rosOffset + 6 + rosPadding) * i + 5];

                        rosPublisherWrenchPortVec.at(i).write(rosDataVec.at(i));
                    }
                }
                else if (useROS != ROS_disabled && rosMsgType == "sensor_msgs/JointState")
                {
                    yarp::rosmsg::sensor_msgs::JointState rosData;
                    size_t data_size = lastDataRead.size();
                    rosData.name.resize(data_size);
                    rosData.position.resize(data_size);
                    rosData.velocity.resize(data_size);
                    rosData.effort.resize(data_size);

                    if (data_size != ros_joint_names.size())
                    {
                        yCDebug(ANALOGWRAPPER) << "Invalid jointNames size:" << data_size << "!=" << ros_joint_names.size();
                    }
                    else
                    {
                        for (size_t i = 0; i< data_size; i++)
                        {
                            //JointTypeEnum jType;
                            // if (jType == VOCAB_JOINTTYPE_REVOLUTE)
                            {
                                rosData.name[i] = ros_joint_names[i];
                                rosData.position[i] = convertDegreesToRadians(lastDataRead[i]);
                                rosData.velocity[i] = 0;
                                rosData.effort[i] = 0;
                            }
                        }
                    }
                    rosData.header.seq = rosMsgCounterVec.at(0)++;
                    rosData.header.stamp = yarp::os::Time::now();
                    rosPublisherJointPort.write(rosData);
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

    if(rosNode!=nullptr) {
        rosNode->interrupt();
        delete rosNode;

        rosNode = nullptr;
    }

    return true;
}
