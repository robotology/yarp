/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <iostream>
#include "AnalogWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;
using namespace yarp::dev::impl;

// needed for the driver factory.
yarp::dev::DriverCreator *createAnalogWrapper() {
    return new DriverCreatorOf<yarp::dev::AnalogWrapper>("analogServer",
        "analogServer",
        "yarp::dev::AnalogWrapper");
}


/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
  **/

AnalogServerHandler::AnalogServerHandler(const char* n)
{
    rpcPort.open(n);
    rpcPort.setReader(*this);
}

AnalogServerHandler::~AnalogServerHandler()
{
    rpcPort.close();
    is = 0;
}

void AnalogServerHandler::setInterface(yarp::dev::IAnalogSensor *is)
{
    this->is = is;
}

bool AnalogServerHandler::_handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply)
{
    if (is==0)
      return false;

    int msgsize=cmd.size();

    int code=cmd.get(1).asVocab();
    switch (code)
    {
    case VOCAB_CALIBRATE:
      if (msgsize==2)
        is->calibrateSensor();
      else
      {
        //read Vector of values and pass to is->calibrate();
      }
      return true;
      break;
    case VOCAB_CALIBRATE_CHANNEL:
      if (msgsize==3)
      {
        int ch=cmd.get(2).asInt();
        is->calibrateChannel(ch);
      }
      if (msgsize==4)
      {
        int ch=cmd.get(2).asInt();
        double v=cmd.get(3).asDouble();
        is->calibrateChannel(ch, v);
      }

      return true;
      break;
    default:
      return false;
    }
}

bool AnalogServerHandler::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok=in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret=false;
    if (code==VOCAB_IANALOG)
    {
        ret=_handleIAnalog(in, out);
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender!=YARP_NULLPTR) {
        out.write(*returnToSender);
    }
    return true;
}


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */

AnalogPortEntry::AnalogPortEntry(void) { }

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

// Constructor used when there is only one output port
AnalogWrapper::AnalogWrapper(const char* name, int rate): RateThread(rate)
{
    // init ROS struct
    useROS          = ROS_disabled;
    frame_id        = "";
    rosNodeName     = "";
    rosTopicName    = "";
    rosNode         = YARP_NULLPTR;
    rosMsgCounter   = 0;

    ownDevices      = false;
    subDeviceOwned  = YARP_NULLPTR;
    sensorId = "AnalogServer";
    createPort(name, rate);
}

bool AnalogWrapper::createPort(const char* name, int rate)
{
    analogSensor_p=0;
    analogPorts.resize(1);
    analogPorts[0].offset = 0;
    analogPorts[0].length = -1; // max length
    analogPorts[0].port_name = std::string(name);
    setHandlers();
    setRate(rate);
    return true;
}

// Contructor used when one or more output ports are specified
AnalogWrapper::AnalogWrapper(const std::vector<AnalogPortEntry>& _analogPorts, int rate): RateThread(rate)
{
    // init ROS struct
    useROS          = ROS_disabled;
    frame_id        = "";
    rosNodeName     = "";
    rosTopicName    = "";
    rosNode         = YARP_NULLPTR;
    rosMsgCounter   = 0;

    sensorId = "AnalogServer";
    createPorts(_analogPorts, rate);
}
bool AnalogWrapper::createPorts(const std::vector<AnalogPortEntry>& _analogPorts, int rate)
{
    analogSensor_p=0;
    this->analogPorts=_analogPorts;
    setHandlers();
    setRate(rate);
    return true;
}

AnalogWrapper::AnalogWrapper(): RateThread(DEFAULT_THREAD_PERIOD)
{
    _rate = DEFAULT_THREAD_PERIOD;
    analogSensor_p = YARP_NULLPTR;

    // init ROS struct
    useROS          = ROS_disabled;
    frame_id        = "";
    rosNodeName     = "";
    rosTopicName    = "";
    rosNode         = YARP_NULLPTR;
    rosMsgCounter   = 0;
}

AnalogWrapper::~AnalogWrapper()
{
    threadRelease();
    _rate = DEFAULT_THREAD_PERIOD;
    analogSensor_p = YARP_NULLPTR;
}

void AnalogWrapper::setHandlers()
{
    for(unsigned int i=0;i<analogPorts.size(); i++)
    {
        std::string rpcPortName = analogPorts[i].port_name;
        rpcPortName += "/rpc:i";
        AnalogServerHandler* ash = new AnalogServerHandler(rpcPortName.c_str());
        handlers.push_back(ash);
    }
}

void AnalogWrapper::removeHandlers()
{
    for(unsigned int i=0; i<handlers.size(); i++)
    {
        if (handlers[i]!=YARP_NULLPTR)
            delete handlers[i];
    }
    handlers.clear();
}

bool AnalogWrapper::openAndAttachSubDevice(Searchable &prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString().c_str());

//     p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device", prop.find("subdevice").asString());  // subdevice was already checked before

    // if error occour during open, quit here.
    yDebug("opening analogServer subdevice...");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yError("opening analogServer subdevice... FAILED\n");
        return false;
    }

    subDeviceOwned->view(analogSensor_p);

    if (analogSensor_p == 0)
    {
        yError("Opening IAnalogSensor interface of analogServer subdevice... FAILED\n");
        return false;
    }

    int chNum = analogSensor_p->getChannels();

    if (chNum <= 0)
    {
        yError("Calling analog sensor has invalid channels number %d.\n", chNum);
        return false;
    }

    attach(analogSensor_p);
    RateThread::setRate(_rate);
    RateThread::start();
    return true;
}


bool AnalogWrapper::openDeferredAttach(yarp::os::Searchable &prop)
{
    // nothing to do here?
    if( (subDeviceOwned != YARP_NULLPTR) || (ownDevices = true) )
        yError() << "AnalogServer: something wrong with the initialization.";
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
        yError("AnalogWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach=analog2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(analogSensor_p);
    }

    if(YARP_NULLPTR == analogSensor_p)
    {
        yError("AnalogWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(analogSensor_p);
    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool AnalogWrapper::detachAll()
{
    //check if we already instantiated a subdevice previously
    if (ownDevices)
        return false;

    analogSensor_p = YARP_NULLPTR;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
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
    analogSensor_p = YARP_NULLPTR;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
}

bool AnalogWrapper::threadInit()
{
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        // open data port
        if (!analogPorts[i].port.open(analogPorts[i].port_name.c_str()))
           {
               yError("AnalogWrapper: failed to open port %s", analogPorts[i].port_name.c_str());
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
        yInfo()  << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }

    yInfo()  << "ROS group was FOUND in config file.";

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        yError() << sensorId << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if(!rosGroup.check("useROS"))
    {
        yError() << sensorId << " cannot find useROS parameter, mandatory when using ROS message. \n \
                    Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    yarp::os::ConstString ros_use_type = rosGroup.find("useROS").asString();
    if(ros_use_type == "false")
    {
        yInfo() << sensorId << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }
    else if(ros_use_type == "true")
    {
        yInfo() << sensorId << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    }
    else if(ros_use_type == "only")
    {
        yInfo() << sensorId << "useROS topic if set to 'only";
        useROS = ROS_only;
    }
    else
    {
        yInfo() << sensorId << "useROS parameter is set to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if(!rosGroup.check("ROS_nodeName"))
    {
        yError() << sensorId << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
    yInfo() << sensorId << "rosNodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if(!rosGroup.check("ROS_topicName"))
    {
        yError() << sensorId << " cannot find ROS_topicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosTopicName = rosGroup.find("ROS_topicName").asString();
    yInfo() << sensorId << "ROS_topicName is " << rosTopicName;

    // check for ROS_msgType parameter
    if (!rosGroup.check("ROS_msgType"))
    {
        yError() << sensorId << " cannot find ROS_msgType parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosMsgType = rosGroup.find("ROS_msgType").asString();

    // check for frame_id parameter
    if (rosMsgType == "geometry_msgs/WrenchedStamped")
    {
        yInfo() << sensorId << "ROS_msgType is " << rosMsgType;
        if (!rosGroup.check("frame_id"))
        {
            yError() << sensorId << " cannot find frame_id parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        frame_id = rosGroup.find("frame_id").asString();
        yInfo() << sensorId << "frame_id is " << frame_id;
    }
    else if (rosMsgType == "sensor_msgs/JointState")
    {
        yInfo() << sensorId << "ROS_msgType is " << rosMsgType;
        if (!rosGroup.check("joint_names"))
        {
            yError() << sensorId << " cannot find some ros parameters";
            useROS = ROS_config_error;
            return false;
        }
        yarp::os::Bottle& jnam =rosGroup.findGroup("joint_names");
        int joint_names_size = jnam.size()-1;
        for (int i = 0; i < joint_names_size; i++)
        {
            ros_joint_names.push_back(jnam.get(i+1).asString());
        }
    }
    else
    {
        yError() << sensorId << "ROS_msgType '" << rosMsgType << "' not supported ";
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

            if(rosNode == YARP_NULLPTR)
            {
                yError() << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            if (rosMsgType == " geometry_msgs/WrenchedStamped" && !rosPublisherWrenchPort.topic(rosTopicName))
            {
                yError() << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            if (rosMsgType == "sensor_msgs/JointState" && !rosPublisherJointPort.topic(rosTopicName))
            {
                yError() << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            success = true;
        } break;

        case ROS_disabled:
        {
            yInfo() << sensorId << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yError() << sensorId << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yError() << sensorId << " something went wrong with ROS configuration, we should never be here!!!";
            success = false;
        } break;
    }
    return success;
}

bool AnalogWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yTrace() << "AnalogServer params are: " << config.toString();

    if (!config.check("period"))
    {
        yError() << "AnalogServer: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
    {
        _rate = config.find("period").asInt();
    }

    if (config.check("deviceId"))
    {
        yError() << "AnalogServer: the parameter 'deviceId' has been removed, please use parameter 'name' instead. \n"
            << "e.g. In the FT wrapper configuration files of your robot, replace '<param name=""deviceId""> left_arm </param>' \n"
            << "with '/icub/left_arm/analog:o' ";
        return false;
    }

    if (!config.check("name"))
    {
        yError() << "AnalogServer: missing 'name' parameter. Check you configuration file; it must be like:\n"
                    "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString().c_str();
        setId("AnalogServer");
    }

    if(!checkROSParams(config) )
    {
        yError() << sensorId << "ROS parameter are not correct, check your configuration file";
        return false;
    }

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }

    if(!initialize_ROS() )
    {
        yError() << sensorId << "Error initializing ROS topics";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(config.check("subdevice"))
    {
        ownDevices=true;
        if(! openAndAttachSubDevice(config))
        {
            yError("AnalogServer: error while opening subdevice\n");
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
            yInfo() << sensorId << " No YARP initialization required";
            return true;
        } break;

        default:
        {
            // Create the list of ports
            // port names are optional, do not check for correctness.
            if(!params.check("ports"))
            {
             // if there is no "ports" section open only 1 port and use name as is.
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
                    yError("AnalogWrapper: 'channels' parameters was not found in config file.");
                    return false;
                }

                int nports=ports->size();
                int sumOfChannels = 0;
                std::vector<AnalogPortEntry> tmpPorts;
                tmpPorts.resize(nports);

                for(int k=0; k<ports->size(); k++)
                {
                    Bottle parameters=params.findGroup(ports->get(k).asString().c_str());

                    if (parameters.size()!=5)
                    {
                        yError() << "AnalogWrapper: check skin port parameters in part description, I was expecting "
                                 << ports->get(k).asString().c_str() << " followed by four integers";
                           yError() << " your param is " << parameters.toString();
                        return false;
                    }

                    int wBase=parameters.get(1).asInt();
                    int wTop=parameters.get(2).asInt();
                    int base=parameters.get(3).asInt();
                    int top=parameters.get(4).asInt();

                    yDebug()<<"--> "<<wBase<<" "<<wTop<<" "<<base<<" "<<top;

                    //check consistenty
                    if(wTop-wBase != top-base){
                        yError() << "AnalogWrapper: numbers of mapped taxels do not match, check "
                                 << ports->get(k).asString().c_str() << " port parameters in part description";
                        return false;
                    }
                    int portChannels = top-base+1;

                    tmpPorts[k].length = portChannels;
                    tmpPorts[k].offset = wBase;
                    yDebug() << "opening port " << ports->get(k).asString().c_str();
                    tmpPorts[k].port_name = streamingPortName+ "/" + string(ports->get(k).asString().c_str());

                    createPorts(tmpPorts, _rate);
                    sumOfChannels+=portChannels;
                }

                if (sumOfChannels!=deviceChannels.asInt())
                {
                    yError() << "AnalogWrapper: Total number of mapped taxels does not correspond to total taxels";
                    return false;
                }
            }
        } break;
    }
    return true;
}

void AnalogWrapper::threadRelease()
{
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        analogPorts[i].port.interrupt();
        analogPorts[i].port.close();
    }
}

void AnalogWrapper::run()
{
    int first, last, ret;

    if (analogSensor_p!=0)
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
                    for(unsigned int i=0; i<analogPorts.size(); i++)
                    {
                        yarp::sig::Vector &pv = analogPorts[i].port.prepare();
                        first = analogPorts[i].offset;
                        if(analogPorts[i].length==-1)   // read the max length available
                            last = lastDataRead.size()-1;
                        else
                            last = analogPorts[i].offset + analogPorts[i].length - 1;

                        // check vector limit
                        if(last>=(int)lastDataRead.size()){
                            yError()<<"AnalogWrapper: error while sending analog sensor output on port "<< analogPorts[i].port_name
                                    <<" Vector size expected to be at least "<<last<<" whereas it is "<< lastDataRead.size();
                            continue;
                        }
                        pv = lastDataRead.subVector(first, last);

                        analogPorts[i].port.setEnvelope(lastStateStamp);
                        analogPorts[i].port.write();
                    }
                }

                if (useROS != ROS_disabled && rosMsgType == "geometry_msgs/WrenchedStamped")
                {
                    geometry_msgs_WrenchStamped rosData;
                    rosData.header.seq = rosMsgCounter++;
                    rosData.header.stamp = normalizeSecNSec(yarp::os::Time::now());
                    rosData.header.frame_id = frame_id;

                    rosData.wrench.force.x = lastDataRead[0];
                    rosData.wrench.force.y = lastDataRead[1];
                    rosData.wrench.force.z = lastDataRead[2];

                    rosData.wrench.torque.x = lastDataRead[3];
                    rosData.wrench.torque.y = lastDataRead[4];
                    rosData.wrench.torque.z = lastDataRead[5];

                    rosPublisherWrenchPort.write(rosData);
                }
                else if (useROS != ROS_disabled && rosMsgType == "sensor_msgs/JointState")
                {
                    sensor_msgs_JointState rosData;
                    size_t data_size = lastDataRead.size();
                    rosData.name.resize(data_size);
                    rosData.position.resize(data_size);
                    rosData.velocity.resize(data_size);
                    rosData.effort.resize(data_size);

                    if (data_size != ros_joint_names.size())
                    {
                        yDebug() << "Invalid ros_joint_names size:" << data_size << "!=" << ros_joint_names.size();
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
                    rosData.header.seq = rosMsgCounter++;
                    rosData.header.stamp = normalizeSecNSec(yarp::os::Time::now());
                    rosPublisherJointPort.write(rosData);
                }
            }
            else
            {
                yError("AnalogWrapper: %s: vector size non valid: %lu", sensorId.c_str(), static_cast<unsigned long> (lastDataRead.size()));
            }
        }
        else
        {
            switch(ret)
            {
                case IAnalogSensor::AS_OVF:
                    yError("AnalogWrapper: %s: Sensor returned overflow error (code %d).", sensorId.c_str(), ret);
                    break;
                case IAnalogSensor::AS_TIMEOUT:
                    yError("AnalogWrapper: %s: Sensor returned timeout error (code %d).", sensorId.c_str(), ret);
                    break;
                case IAnalogSensor::AS_ERROR:
                default:
                    yError("AnalogWrapper: %s: Sensor returned error with code %d.", sensorId.c_str(), ret);
                    break;
            }
        }
    }
}

bool AnalogWrapper::close()
{
    yTrace("AnalogWrapper::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    //RateThread::stop();

    detachAll();
    removeHandlers();

    if(rosNode!=YARP_NULLPTR) {
        rosNode->interrupt();
        delete rosNode;

        rosNode = YARP_NULLPTR;
    }

    return true;
}
