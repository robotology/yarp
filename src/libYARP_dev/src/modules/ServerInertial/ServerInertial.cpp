// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Alexis Maldonado, Radu Bogdan Rusu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <ServerInertial.h>


using namespace yarp::dev;
using namespace yarp::os;

// needed for the driver factory.
yarp::dev::DriverCreator *createServerInertial()
{
    return new yarp::dev::DriverCreatorOf<yarp::dev::ServerInertial>
            ("inertial", "inertial", "yarp::dev::ServerInertial");
}

/**
 * Constructor.
 */
yarp::dev::ServerInertial::ServerInertial()
{
    IMU = NULL;
    spoke = false;
    iTimed=0;
    period = 0.005;
    prev_timestamp_counter=0;
    curr_timestamp_counter=0;
    trap = 0;
    partName = "Server Inertial";

    // init ROS data
    frame_id = "";
    rosNodeName = "";
    rosTopicName = "";
    rosNode = NULL;
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

    std::cout << "covariance size is " << covariance.size() << std::endl;
}

yarp::dev::ServerInertial::~ServerInertial()
{
    if (IMU != NULL) close();
}


bool ServerInertial::checkROSParams(yarp::os::Searchable &config)
{
    // check for ROS parameter group
    if(!config.check("ROS") )
    {
        useROS = ROS_disabled;
        yInfo()  << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }
    else
    {
        yInfo()  << "ROS group was FOUND in config file.";

        Bottle &rosGroup = config.findGroup("ROS");
        if(rosGroup.isNull())
        {
            yError() << partName << "ROS group params is not a valid group or empty";
            useROS = ROS_config_error;
            return false;
        }

        // check for useROS parameter
        if(!rosGroup.check("useROS"))
        {
            yError() << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                        Allowed values are true, false, ROS_only";
            useROS = ROS_config_error;
            return false;
        }
        yarp::os::ConstString ros_use_type = rosGroup.find("useROS").asString();
        if(ros_use_type == "false")
        {
            yInfo() << partName << "useROS topic if set to 'false'";
            useROS = ROS_disabled;
            return true;
        }
        else if(ros_use_type == "true")
        {
            yInfo() << partName << "useROS topic if set to 'true'";
            useROS = ROS_enabled;
        }
        else if(ros_use_type == "only")
        {
            yInfo() << partName << "useROS topic if set to 'only";
            useROS = ROS_only;
        }
        else
        {
            yInfo() << partName << "useROS parameter is seet to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
            useROS = ROS_config_error;
            return false;
        }

        // check for ROS_nodeName parameter
        if(!rosGroup.check("ROS_nodeName"))
        {
            yError() << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
        yInfo() << partName << "rosNodeName is " << rosNodeName;

        // check for ROS_topicName parameter
        if(!rosGroup.check("ROS_topicName"))
        {
            yError() << partName << " cannot find rosTopicName parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        rosTopicName = rosGroup.find("ROS_topicName").asString();
        yInfo() << partName << "rosTopicName is " << rosTopicName;

        // check for frame_id parameter
        if(!rosGroup.check("frame_id"))
        {
            yError() << partName << " cannot find rosTopicName parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        frame_id = rosGroup.find("frame_id").asString();
        yInfo() << partName << "frame_id is " << frame_id;

        return true;
    }
    yError() << partName << "should never get here!" << __LINE__;
    return false;  // should never get here
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
            yInfo() << partName << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yError() << partName << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yError() << partName << " something went wrong with ROS configuration, we should never be here!!!";
            success = false;
        } break;
    }
    return success;
}

/**
 * Configure with a set of options. These are:
 * <TABLE>
 * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
 * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
 * </TABLE>
 *
 * @param config The options to use
 * @return true iff the object could be configured.
 */
bool yarp::dev::ServerInertial::open(yarp::os::Searchable& config)
{
    p.setReader(*this);

    period = config.check("period",yarp::os::Value(0.005),"maximum period").asDouble();
    //Look for the device name (serial Port). Usually /dev/ttyUSB0
    yarp::os::Value *name;
    if (config.check("subdevice",name))
    {
        yDebug("Subdevice %s\n", name->toString().c_str());
        if (name->isString())
        {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(), "subdevice"); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device",name->toString());
            poly.open(p);
        }
        else
            poly.open(*name);
        if (!poly.isValid())
        {
            yError("cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    }
    else
    {
        yError("\"--subdevice <name>\" not set for server_inertial\n");
        return false;
    }

    // if we are here, poly is valid
    poly.view(IMU);
    if(IMU == NULL)
    {
        yError("Error, subdevice <%s> has no valid IMU interface\n", name->toString().c_str());
        return false;
    }
    checkROSParams(config);


    // iTimed interface
    poly.view(iTimed);  // not mandatory

    //Look for the portname to register (--name option), defaulting to /inertial if missing
    yarp::os::ConstString portName;
    if(useROS != ROS_only)
    {
        if (config.check("name",name))
            portName = name->asString();
        else
        {
            yInfo() << "Using default values for port name, you can change it using '--name /myPortName' parameter";
            portName = "/inertial";
        }

        if(!p.open(portName))
        {
            yError() << "ServerInertial, cannot open port " << portName;
            return false;
        }
        writer.attach(p);
    }

    // call ROS node/topic initilization, if needed
    if(!initialize_ROS() )
    {
        return false;
    }

    if (IMU!=NULL)
    {
        start();
        return true;
    }
    else
        return false;
}

bool yarp::dev::ServerInertial::close()
{
    yInfo("Closing Server Inertial...\n");
    if (IMU != NULL)
    {
        stop();
        IMU = NULL;
        return true;
    }
    return false;
}


bool yarp::dev::ServerInertial::getInertial(yarp::os::Bottle &bot)
{
    if (IMU==NULL)
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
                bot.addDouble (indata[i]);
        }
        else
        {
            bot.clear(); //dummy info.
        }

        return(worked);
    }
}

void yarp::dev::ServerInertial::run()
{
    double before, now;
    yInfo("Server Inertial starting\n");
    while (!isStopping())
    {
        before = yarp::os::Time::now();
        if (IMU!=NULL)
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
                            yDebug("Writing an Inertial measurement.\n");
                            spoke = true;
                        }
                        p.setEnvelope(ts);
                        writer.write();
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

                euler_xyz[0] = imuData.get(0).asDouble();
                euler_xyz[1] = imuData.get(1).asDouble();
                euler_xyz[2] = imuData.get(2).asDouble();

                convertEulerAngleYXZdegrees_to_quaternion(euler_xyz, quaternion);

                sensor_msgs_Imu &rosData = rosPublisherPort.prepare();

                rosData.header.seq = rosMsgCounter++;
                rosData.header.stamp = normalizeSecNSec(yarp::os::Time::now());
                rosData.header.frame_id = frame_id;

                rosData.orientation.x = quaternion[0];  // to be converted into quaternion
                rosData.orientation.y = quaternion[1];  // to be converted into quaternion
                rosData.orientation.z = quaternion[2];  // to be converted into quaternion
                rosData.orientation.w = quaternion[3];  // to be converted into quaternion
                rosData.orientation_covariance = covariance;

                rosData.linear_acceleration.x = imuData.get(3).asDouble();  // to be converted into radiants
                rosData.linear_acceleration.y = imuData.get(4).asDouble();  // to be converted into radiants
                rosData.linear_acceleration.z = imuData.get(5).asDouble();  // to be converted into radiants
                rosData.linear_acceleration_covariance = covariance;

                rosData.angular_velocity.x = imuData.get(6).asDouble();   // to be converted into radiants
                rosData.angular_velocity.y = imuData.get(7).asDouble();   // to be converted into radiants
                rosData.angular_velocity.z = imuData.get(8).asDouble();   // to be converted into radiants
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
    yDebug("Server Intertial closed\n");
}

bool yarp::dev::ServerInertial::read(ConnectionReader& connection)
{
    yarp::os::Bottle cmd, response;
    cmd.read(connection);
    // printf("command received: %s\n", cmd.toString().c_str());

    // We receive a command but don't do anything with it.
    return true;
}

bool yarp::dev::ServerInertial::read(yarp::sig::Vector &out)
{
    if (IMU == NULL) { return false; }
    return IMU->read (out);
}

bool yarp::dev::ServerInertial::getChannels(int *nc)
{
    if (IMU == NULL) { return false; }
    return IMU->getChannels (nc);
}

bool yarp::dev::ServerInertial::calibrate(int ch, double v)
{
    if (IMU==NULL) {return false;}
    return IMU->calibrate(ch, v);
}
