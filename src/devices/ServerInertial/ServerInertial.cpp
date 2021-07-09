/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ServerInertial.h"
#include <cstdio>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>


using namespace yarp::dev;
using namespace yarp::os;

YARP_LOG_COMPONENT(SERVERINERTIAL, "yarp.devices.ServerInertial")


/**
 * Constructor.
 */
ServerInertial::ServerInertial() :
        IMU_polydriver(nullptr),
        ownDevices(false),
        subDeviceOwned(nullptr)
{
    IMU = nullptr;
    spoke = false;
    iTimed=nullptr;
    period = 0.005;
    prev_timestamp_counter=0;
    curr_timestamp_counter=0;
    trap = 0;
    strict = false;
    partName = "Server Inertial";

    // init ROS data
    frame_id = "";
    rosNodeName = "";
    rosTopicName = "";
    rosNode = nullptr;
    rosMsgCounter = 0;
    useROS = ROS_disabled;

    // init a fake covariance matrix
    covariance.resize(9);
    covariance.assign(9, 0);

//    rosData.angular_velocity.x = 0;
//    rosData.angular_velocity.y = 0;
//    rosData.angular_velocity.z = 0;
//    rosData.angular_velocity_covariance = covariance;

//    rosData.linear_acceleration.x = 0;
//    rosData.linear_acceleration.y = 0;
//    rosData.linear_acceleration.z = 0;
//    rosData.linear_acceleration_covariance = covariance;

//    rosData.orientation.x = 0;
//    rosData.orientation.y = 0;
//    rosData.orientation.z = 0;
//    rosData.orientation.w = 0;
//    rosData.orientation_covariance = covariance;

//     yCDebug(SERVERINERTIAL) << "covariance size is " << covariance.size();
}

ServerInertial::~ServerInertial()
{
    if (IMU != nullptr) close();
}


bool ServerInertial::checkROSParams(yarp::os::Searchable &config)
{
    // check for ROS parameter group
    if(!config.check("ROS") )
    {
        useROS = ROS_disabled;
        yCInfo(SERVERINERTIAL) << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }

    yCInfo(SERVERINERTIAL) << "ROS group was FOUND in config file.";

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        yCError(SERVERINERTIAL) << partName << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if(!rosGroup.check("useROS"))
    {
        yCError(SERVERINERTIAL) << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                    Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    std::string ros_use_type = rosGroup.find("useROS").asString();
    if(ros_use_type == "false")
    {
        yCInfo(SERVERINERTIAL) << partName << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }
    else if(ros_use_type == "true")
    {
        yCInfo(SERVERINERTIAL) << partName << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    }
    else if(ros_use_type == "only")
    {
        yCInfo(SERVERINERTIAL) << partName << "useROS topic if set to 'only";
        useROS = ROS_only;
    }
    else
    {
        yCInfo(SERVERINERTIAL) << partName << "useROS parameter is seet to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if(!rosGroup.check("ROS_nodeName"))
    {
        yCError(SERVERINERTIAL) << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
    yCInfo(SERVERINERTIAL) << partName << "rosNodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if(!rosGroup.check("ROS_topicName"))
    {
        yCError(SERVERINERTIAL) << partName << " cannot find ROS_topicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosTopicName = rosGroup.find("ROS_topicName").asString();
    yCInfo(SERVERINERTIAL) << partName << "ROS_topicName is " << rosTopicName;

    // check for frame_id parameter
    if(!rosGroup.check("frame_id"))
    {
        yCError(SERVERINERTIAL) << partName << " cannot find frame_id parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    frame_id = rosGroup.find("frame_id").asString();
    yCInfo(SERVERINERTIAL) << partName << "frame_id is " << frame_id;

    return true;
}

bool ServerInertial::initialize_ROS()
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
                yCError(SERVERINERTIAL) << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration";
                success = false;
                break;
            }

            if (!rosPublisherPort.topic(rosTopicName) )
            {
                yCError(SERVERINERTIAL) << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration";
                success = false;
                break;
            }
            success = true;
        } break;

        case ROS_disabled:
        {
            yCInfo(SERVERINERTIAL) << partName << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yCError(SERVERINERTIAL) << partName << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yCError(SERVERINERTIAL) << partName << " something went wrong with ROS configuration, we should never be here!!!";
            success = false;
        } break;
    }
    return success;
}

bool ServerInertial::openDeferredAttach(yarp::os::Property& prop)
{
    return true;
}

// If a subdevice parameter is given to the wrapper, it will open it as well
// and attach to it immediately.
bool ServerInertial::openAndAttachSubDevice(yarp::os::Property& prop)
{
    yarp::os::Value &subdevice = prop.find("subdevice");
    IMU_polydriver = new yarp::dev::PolyDriver;

    yCDebug(SERVERINERTIAL, "Subdevice %s", subdevice.toString().c_str());
    if (subdevice.isString())
    {
        // maybe user isn't doing nested configuration
        yarp::os::Property p;
        p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
        p.fromString(prop.toString());
        p.put("device",subdevice.toString());
        IMU_polydriver->open(p);
    }
    else
        IMU_polydriver->open(subdevice);

    if (!IMU_polydriver->isValid())
    {
        yCError(SERVERINERTIAL, "cannot create device <%s>", subdevice.toString().c_str());
        return false;
    }

    // if we are here, poly is valid
    IMU_polydriver->view(IMU);     // attach to subdevice
    if(IMU == nullptr)
    {
        yCError(SERVERINERTIAL, "Error, subdevice <%s> has no valid IMU interface", subdevice.toString().c_str());
        IMU_polydriver->close();
        return false;
    }

    // iTimed interface
    IMU_polydriver->view(iTimed);  // not mandatory
    return true;
}

/**
 * Configure with a set of options. These are:
 * <TABLE>
 * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "fakeFrameGrabber"). </TD></TR>
 * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
 * </TABLE>
 *
 * @param config The options to use
 * @return true iff the object could be configured.
 */
bool ServerInertial::open(yarp::os::Searchable& config)
{
    yCWarning(SERVERINERTIAL) << "The 'inertial' device is deprecated in favour of 'multipleanalogsensorsremapper' + 'multipleanalogsensorsserver' + 'IMURosPublisher'.";
    yCWarning(SERVERINERTIAL) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(SERVERINERTIAL) << "Please update your scripts.";

    Property prop;
    prop.fromString(config.toString());

    p.setReader(*this);

    period = config.check("period",yarp::os::Value(0.005),"maximum period").asFloat64();
    strict = config.check("strict",yarp::os::Value(false),"strict write").asBool();

    //Look for the device name (serial Port). Usually /dev/ttyUSB0
    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(prop.check("subdevice"))
    {
        ownDevices=true;
        if(! openAndAttachSubDevice(prop))
        {
            yCError(SERVERINERTIAL, "ControlBoardWrapper: error while opening subdevice");
            return false;
        }
    }
    else
    {
        ownDevices=false;
        if(!openDeferredAttach(prop))
            return false;
    }


    checkROSParams(config);



    //Look for the portname to register (--name option), defaulting to /inertial if missing
    std::string portName;
    if(useROS != ROS_only)
    {
        if (config.check("name"))
            portName = config.find("name").asString();
        else
        {
            yCInfo(SERVERINERTIAL) << "Using default values for port name, you can change it by using '--name /myPortName' parameter";
            portName = "/inertial";
        }

        if(!p.open(portName))
        {
            yCError(SERVERINERTIAL) << "ServerInertial, cannot open port " << portName;
            return false;
        }
        writer.attach(p);
    }

    // call ROS node/topic initialization, if needed
    if(!initialize_ROS() )
    {
        return false;
    }

    if( (ownDevices) && (IMU!=nullptr) )
    {
        start();
    }

    return true;
}

bool ServerInertial::close()
{
    yCInfo(SERVERINERTIAL, "Closing Server Inertial...");
    stop();

    if(rosNode!=nullptr) {
        rosNode->interrupt();
        delete rosNode;
        rosNode = nullptr;
    }

    if( (ownDevices) && (IMU_polydriver != nullptr) )
    {
        IMU_polydriver->close();
        IMU = nullptr;
    }
    return true;
}


bool ServerInertial::getInertial(yarp::os::Bottle &bot)
{
    if (IMU==nullptr)
    {
        return false;
    }
    else
    {
        int nchannels;
        IMU->getChannels (&nchannels);

        yarp::sig::Vector indata(nchannels);
        bool worked(false);

        worked=IMU->read(indata);
        if (worked)
        {
            bot.clear();

            // Euler+accel+gyro+magn orientation values
            for (int i = 0; i < nchannels; i++)
                bot.addFloat64 (indata[i]);
        }
        else
        {
            bot.clear(); //dummy info.
        }

        return(worked);
    }
}

void ServerInertial::run()
{
    double before, now;
    yCInfo(SERVERINERTIAL, "Starting server Inertial thread");
    while (!isStopping())
    {
        before = yarp::os::Time::now();
        if (IMU!=nullptr)
        {
            Bottle imuData;
            bool res = getInertial(imuData);

            // publish data on YARP port if required
            if(useROS != ROS_only)
            {
                yarp::os::Bottle& bot = writer.get();
                bot = imuData;
                if (res)
                {
                    static yarp::os::Stamp ts;
                    if (iTimed)
                        ts=iTimed->getLastInputStamp();
                    else
                        ts.update();


                    curr_timestamp_counter = ts.getCount();

                    if (curr_timestamp_counter!=prev_timestamp_counter)
                    {
                        if (!spoke)
                        {
                            yCDebug(SERVERINERTIAL, "Writing an Inertial measurement.");
                            spoke = true;
                        }
                        p.setEnvelope(ts);
                        writer.write(strict);
                    }
                    else
                    {
                        trap++;
                    }

                    prev_timestamp_counter = curr_timestamp_counter;
                }
            }

            // publish ROS topic if required
            if(useROS != ROS_disabled)
            {
                double euler_xyz[3], quaternion[4];

                euler_xyz[0] = imuData.get(0).asFloat64();
                euler_xyz[1] = imuData.get(1).asFloat64();
                euler_xyz[2] = imuData.get(2).asFloat64();

                convertEulerAngleYXZdegrees_to_quaternion(euler_xyz, quaternion);

                yarp::rosmsg::sensor_msgs::Imu &rosData = rosPublisherPort.prepare();

                rosData.header.seq = rosMsgCounter++;
                rosData.header.stamp = yarp::os::Time::now();
                rosData.header.frame_id = frame_id;

                rosData.orientation.x = quaternion[0];
                rosData.orientation.y = quaternion[1];
                rosData.orientation.z = quaternion[2];
                rosData.orientation.w = quaternion[3];
                rosData.orientation_covariance = covariance;

                rosData.linear_acceleration.x = imuData.get(3).asFloat64();   // [m/s^2]
                rosData.linear_acceleration.y = imuData.get(4).asFloat64();   // [m/s^2]
                rosData.linear_acceleration.z = imuData.get(5).asFloat64();   // [m/s^2]
                rosData.linear_acceleration_covariance = covariance;

                rosData.angular_velocity.x = imuData.get(6).asFloat64();   // to be converted into rad/s (?) - verify with users
                rosData.angular_velocity.y = imuData.get(7).asFloat64();   // to be converted into rad/s (?) - verify with users
                rosData.angular_velocity.z = imuData.get(8).asFloat64();   // to be converted into rad/s (?) - verify with users
                rosData.angular_velocity_covariance = covariance;

                rosPublisherPort.write();
            }
        }

        /// wait 5 ms.
        now = yarp::os::Time::now();
        if ((now - before) < period) {
            double k = period-(now-before);
            yarp::os::Time::delay(k);
        }
    }
    yCInfo(SERVERINERTIAL, "Server Intertial thread finished");
}

bool ServerInertial::read(ConnectionReader& connection)
{
    yarp::os::Bottle cmd, response;
    cmd.read(connection);
    yCTrace(SERVERINERTIAL, "command received: %s", cmd.toString().c_str());

    // We receive a command but don't do anything with it.
    return true;
}

bool ServerInertial::read(yarp::sig::Vector &out)
{
    if (IMU == nullptr) { return false; }
    return IMU->read (out);
}

bool ServerInertial::getChannels(int *nc)
{
    if (IMU == nullptr) { return false; }
    return IMU->getChannels (nc);
}

bool ServerInertial::calibrate(int ch, double v)
{
    if (IMU==nullptr) {return false;}
    return IMU->calibrate(ch, v);
}


bool ServerInertial::attach(PolyDriver* poly)
{
    yCTrace(SERVERINERTIAL);
    if(!poly)
    {
        yCError(SERVERINERTIAL, "ServerInertial: device to attach to is not valid!");
        return false;
    }
    IMU_polydriver = poly;
    IMU_polydriver->view(IMU);

    // iTimed interface
    IMU_polydriver->view(iTimed);  // not mandatory

    if(IMU != nullptr)
    {
        if(!Thread::isRunning())
            start();
    }
    else
    {
        yCError(SERVERINERTIAL, "ControlBoardWrapper: attach to subdevice failed");
    }
    return true;
}

bool ServerInertial::detach()
{
    return true;
}

bool ServerInertial::attachAll(const PolyDriverList &imuToAttachTo)
{
    if (imuToAttachTo.size() != 1)
    {
        yCError(SERVERINERTIAL, "ServerInertial: cannot attach more than one device");
        return false;
    }

    return attach(imuToAttachTo[0]->poly);
}

bool ServerInertial::detachAll()
{
    IMU = nullptr;
    return true;
}
