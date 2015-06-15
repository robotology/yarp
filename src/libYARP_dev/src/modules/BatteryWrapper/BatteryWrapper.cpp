// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <iostream>
#include "BatteryWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createBatteryWrapper() {
    return new DriverCreatorOf<yarp::dev::BatteryWrapper>("batteryWrapper",
        "batteryWrapper",
        "yarp::dev::BatteryWrapper");
}


/**
  * It reads the data from an battery sensor and sends them on one port.
  * It creates one rpc port.
  */

// Constructor used when there is only one output port
BatteryWrapper::BatteryWrapper(const char* name, int rate) : RateThread(rate)
{
    // init ROS struct
    useROS          = ROS_disabled;
    frame_id        = "";
    rosNodeName     = "";
    rosTopicName    = "";
    rosNode         = NULL;
    rosMsgCounter   = 0;

    sensorId = "batteryWrapper";
}

BatteryWrapper::BatteryWrapper() : RateThread(DEFAULT_THREAD_PERIOD)
{
    _rate = DEFAULT_THREAD_PERIOD;
    battery_p = NULL;
}

BatteryWrapper::~BatteryWrapper()
{
    threadRelease();
    _rate = DEFAULT_THREAD_PERIOD;
    battery_p = NULL;
}

/**
  * Specify which battery sensor this thread has to read from.
  */

bool BatteryWrapper::attachAll(const PolyDriverList &battery2attach)
{
    if (battery2attach.size() != 1)
    {
        yError("BatteryWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = battery2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(battery_p);
    }

    if(NULL == battery_p)
    {
        yError("BatteryWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(battery_p);
    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool BatteryWrapper::detachAll()
{
    battery_p = NULL;
    return true;
}

void BatteryWrapper::attach(yarp::dev::IBattery *s)
{
    battery_p=s;
}

void BatteryWrapper::detach()
{
    // Set interface to NULL
    battery_p = NULL;
}

bool BatteryWrapper::threadInit()
{
    // open data port
    if (!streamingPort.open(streamingPortName.c_str()))
        {
            yError("BatteryWrapper: failed to open port %s", streamingPortName.c_str());
            return false;
        }
    return true;
}

void BatteryWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string BatteryWrapper::getId()
{
    return sensorId;
}

#if ROS_PART_STILL_TO_BE_DONE
bool BatteryWrapper::checkROSParams(Searchable &config)
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

    // check for frame_id parameter
    if(!rosGroup.check("frame_id"))
    {
        yError() << sensorId << " cannot find frame_id parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    frame_id = rosGroup.find("frame_id").asString();
    yInfo() << sensorId << "frame_id is " << frame_id;

    // check for ROS_msgType parameter
    if(!rosGroup.check("ROS_msgType"))
    {
        yError() << sensorId << " cannot find ROS_msgType parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    std::string rosMsgType = rosGroup.find("ROS_msgType").asString();
    if(rosMsgType == "geometry_msgs/WrenchedStamped")
    {
        yInfo() << sensorId << "ROS_msgType is " << rosTopicName;
    }
    else
    {
        yError() << sensorId << "ROS_msgType '" << rosMsgType << "' not supported ";
        return false;
    }
    return true;
}
#endif

#if ROS_PART_STILL_TO_BE_DONE
bool BatteryWrapper::initialize_ROS()
{
    bool success = false;
    switch(useROS)
    {
        case ROS_enabled:
        case ROS_only:
        {
            rosNode = new yarp::os::Node(rosNodeName);   // add a ROS node

            if(rosNode == NULL)
            {
                yError() << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }

            if (!rosPublisherPort.topic(rosTopicName) )
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
#endif

bool BatteryWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());

    if (!config.check("period"))
    {
        yError() << "BatteryWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
        _rate = config.find("period").asInt();

    if (!config.check("name"))
    {
        yError() << "BatteryWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString().c_str();
        rpcPortName = streamingPortName + "/rpc:i";
        setId("batteryWrapper");
    }

#if ROS_PART_STILL_TO_BE_DONE
    if(!checkROSParams(config) )
    {
        yError() << sensorId << "ROS parameter are not correct, check your configuration file";
        return false;
    }

    if(!initialize_ROS() )
    {
        yError() << sensorId << "Error initializing ROS topics";
        return false;
    }
#endif

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }
    return true;
}

bool BatteryWrapper::initialize_YARP(yarp::os::Searchable &params)
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
                streamingPort.open(streamingPortName.c_str());
                rpcPort.open(rpcPortName.c_str() );
                //_rate;
                
                //if(! Network::exists(streamingPortName + "/rpc:i")) return false;
            }
        } break;
    }
    return true;
}

void BatteryWrapper::threadRelease()
{
    streamingPort.interrupt();
    streamingPort.close();
}

void BatteryWrapper::run()
{
    bool ret = true;

    if (battery_p!=0)
    {
        double charge  = 0;
        double voltage = 0;
        double current = 0;
        double temperature = 0;
        bool ret_g = battery_p->getBatteryCharge(charge);
        bool ret_v = battery_p->getBatteryVoltage(voltage);
        bool ret_c = battery_p->getBatteryCurrent(current);
        bool ret_t = battery_p->getBatteryTemperature(temperature);

        if (ret)
        {
            if(useROS != ROS_only)
            {
                lastStateStamp.update();
                yarp::os::Bottle& b = streamingPort.prepare();
                b.clear();
                b.addDouble(voltage);
                b.addDouble(current);
                b.addDouble(charge);
                b.addDouble(temperature);
                streamingPort.setEnvelope(lastStateStamp);
                streamingPort.write();
            }

#if ROS_PART_STILL_TO_BE_DONE
            if(useROS != ROS_disabled)
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

                rosPublisherPort.write(rosData);
            }
#endif
        }
        else
        {
            yError("BatteryWrapper: %s: Sensor returned error", sensorId.c_str(), ret);
        }
    }
}

bool BatteryWrapper::close()
{
    yTrace("BatteryWrapper::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    RateThread::stop();
    detachAll();
    return true;
}

// eof



