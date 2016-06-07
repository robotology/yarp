/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include "Rangefinder2DWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createRangefinder2DWrapper() {
    return new DriverCreatorOf<yarp::dev::Rangefinder2DWrapper>("Rangefinder2DWrapper",
        "Rangefinder2DWrapper",
        "yarp::dev::Rangefinder2Dwrapper");
}


/**
  * It reads the data from a rangefinder sensor and sends them on one port.
  * It also creates one rpc port.
  */

Rangefinder2DWrapper::Rangefinder2DWrapper() : RateThread(DEFAULT_THREAD_PERIOD)
{
    _rate = DEFAULT_THREAD_PERIOD;
    sens_p = NULL;

    // init ROS data
    frame_id = "";
    rosNodeName = "";
    rosTopicName = "";
    partName = "Rangefinder2DWrapper";
    rosNode = NULL;
    rosMsgCounter = 0;
    useROS      = ROS_disabled;
    minAngle    = 0;
    maxAngle    = 0;
    minDistance = 0;
    maxDistance = 0;
    resolution = 0;
}

Rangefinder2DWrapper::~Rangefinder2DWrapper()
{
    sens_p = NULL;
}

bool Rangefinder2DWrapper::checkROSParams(yarp::os::Searchable &config)
{
    // check for ROS parameter group
    if (!config.check("ROS"))
    {
        useROS = ROS_disabled;
        yInfo() << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }
    else
    {
        yInfo() << "ROS group was FOUND in config file.";

        Bottle &rosGroup = config.findGroup("ROS");
        if (rosGroup.isNull())
        {
            yError() << partName << "ROS group params is not a valid group or empty";
            useROS = ROS_config_error;
            return false;
        }

        // check for useROS parameter
        if (!rosGroup.check("useROS"))
        {
            yError() << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                                                            Allowed values are true, false, ROS_only";
            useROS = ROS_config_error;
            return false;
        }
        yarp::os::ConstString ros_use_type = rosGroup.find("useROS").asString();
        if (ros_use_type == "false")
        {
            yInfo() << partName << "useROS topic if set to 'false'";
            useROS = ROS_disabled;
            return true;
        }
        else if (ros_use_type == "true")
        {
            yInfo() << partName << "useROS topic if set to 'true'";
            useROS = ROS_enabled;
        }
        else if (ros_use_type == "only")
        {
            yInfo() << partName << "useROS topic if set to 'only";
            useROS = ROS_only;
        }
        else
        {
            yInfo() << partName << "useROS parameter is set to unvalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
            useROS = ROS_config_error;
            return false;
        }

        // check for ROS_nodeName parameter
        if (!rosGroup.check("ROS_nodeName"))
        {
            yError() << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        rosNodeName = rosGroup.find("ROS_nodeName").asString();  // TODO: check name is correct
        yInfo() << partName << "rosNodeName is " << rosNodeName;

        // check for ROS_topicName parameter
        if (!rosGroup.check("ROS_topicName"))
        {
            yError() << partName << " cannot find rosTopicName parameter, mandatory when using ROS message";
            useROS = ROS_config_error;
            return false;
        }
        rosTopicName = rosGroup.find("ROS_topicName").asString();
        yInfo() << partName << "rosTopicName is " << rosTopicName;

        // check for frame_id parameter
        if (!rosGroup.check("frame_id"))
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

bool Rangefinder2DWrapper::initialize_ROS()
{
    bool success = false;
    switch (useROS)
    {
        case ROS_enabled:
        case ROS_only:
        {
            rosNode = new yarp::os::Node(rosNodeName);   // add a ROS node
            if (rosNode == NULL)
            {
                yError() << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }
            if (!rosPublisherPort.topic(rosTopicName))
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
  * Specify which sensor this thread has to read from.
  */

bool Rangefinder2DWrapper::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("Rangefinder2DWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(sens_p);
        Idevice2attach->view(iTimed);
    }

    if (NULL == sens_p)
    {
        yError("Rangefinder2DWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(sens_p);

    if(!sens_p->getDistanceRange(minDistance, maxDistance))
    {
        yError() << "Laser device does not provide min & max distance range.";
        return false;
    }

    if(!sens_p->getScanLimits(minAngle, maxAngle))
    {
        yError() << "Laser device does not provide min & max angle scan range.";
        return false;
    }

    if (!sens_p->getHorizontalResolution(resolution))
    {
        yError() << "Laser device does not provide horizontal resolution ";
        return false;
    }

    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool Rangefinder2DWrapper::detachAll()
{
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }
    sens_p = NULL;
    return true;
}

void Rangefinder2DWrapper::attach(yarp::dev::IRangefinder2D *s)
{
    sens_p = s;
}

void Rangefinder2DWrapper::detach()
{
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }
    sens_p = NULL;
}

bool Rangefinder2DWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int action = in.get(0).asVocab();
    int inter  = in.get(1).asVocab();
    bool ret = false;

    if (inter == VOCAB_ILASER2D)
    {
        if (action == VOCAB_GET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_DEVICE_INFO)
            {
                if (sens_p)
                {
                    yarp::os::ConstString info;
                    if (sens_p->getDeviceInfo(info))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addString(info);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_DISTANCE_RANGE)
            {
                if (sens_p)
                {
                    double max = 0;
                    double min = 0;
                    if (sens_p->getDistanceRange(min, max))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addDouble(min);
                        out.addDouble(max);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_RANGE)
            {
                if (sens_p)
                {
                    double max = 0;
                    double min = 0;
                    if (sens_p->getScanLimits(min, max))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addDouble(min);
                        out.addDouble(max);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_STEP)
            {
                if (sens_p)
                {
                    double step = 0;
                    if (sens_p->getHorizontalResolution(step))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addDouble(step);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_SCAN_RATE)
            {
                if (sens_p)
                {
                    double rate = 0;
                    if (sens_p->getScanRate(rate))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addDouble(rate);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else
            {
                yError("Invalid command received in Rangefinder2DWrapper");
            }
        }
        else if (action == VOCAB_SET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_LASER_DISTANCE_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt();
                    double max = in.get(4).asInt();
                    sens_p->setDistanceRange(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt();
                    double max = in.get(4).asInt();
                    sens_p->setScanLimits(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_SCAN_RATE)
            {
                if (sens_p)
                {
                    double rate = in.get(3).asInt();
                    sens_p->setScanRate(rate);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_STEP)
            {
                if (sens_p)
                {
                    double step = in.get(3).asDouble();
                    sens_p->setHorizontalResolution(step);
                    ret = true;
                }
            }
            else
            {
                yError("Invalid command received in Rangefinder2DWrapper");
            }
        }
        else
        {
            yError("Invalid action received in Rangefinder2DWrapper");
        }
    }
    else
    {
        yError("Invalid interface vocab received in Rangefinder2DWrapper");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL) {
        out.write(*returnToSender);
    }
    return true;
}

bool Rangefinder2DWrapper::threadInit()
{
    // open data port
    if (!streamingPort.open(streamingPortName.c_str()))
        {
            yError("Rangefinder2DWrapper: failed to open port %s", streamingPortName.c_str());
            return false;
        }
    return true;
}

void Rangefinder2DWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string Rangefinder2DWrapper::getId()
{
    return sensorId;
}


bool Rangefinder2DWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());

    if (!config.check("period"))
    {
        yError() << "Rangefinder2DWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
        _rate = config.find("period").asInt();

    if (!config.check("name"))
    {
        yError() << "Rangefinder2DWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString().c_str();
        rpcPortName = streamingPortName + "/rpc:i";
        setId("Rangefinder2DWrapper");
    }

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }

    checkROSParams(config);

    // call ROS node/topic initilization, if needed
    if (!initialize_ROS())
    {
        return false;
    }

    return true;
}

bool Rangefinder2DWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    streamingPort.open(streamingPortName.c_str());
    rpcPort.open(rpcPortName.c_str() );
    rpcPort.setReader(*this);
    return true;
}

void Rangefinder2DWrapper::threadRelease()
{
    streamingPort.interrupt();
    streamingPort.close();
    rpcPort.interrupt();
    rpcPort.close();
}

void Rangefinder2DWrapper::run()
{
    if (sens_p!=0)
    {
        yarp::sig::Vector ranges;

        bool ret = true;
        IRangefinder2D::Device_status status;
        ret &= sens_p->getMeasurementData(ranges);
        ret &= sens_p->getDeviceStatus(status);

        if (ret)
        {
            if(iTimed)
                lastStateStamp = iTimed->getLastInputStamp();
            else
                lastStateStamp.update(yarp::os::Time::now());

            int ranges_size = ranges.size();

            yarp::os::Bottle& b = streamingPort.prepare();
            b.clear();
            Bottle& bl = b.addList();
            bl.read(ranges);
            b.addInt(status);
            streamingPort.setEnvelope(lastStateStamp);
            streamingPort.write();

            // publish ROS topic if required
            if (useROS != ROS_disabled)
            {
                sensor_msgs_LaserScan &rosData = rosPublisherPort.prepare();
                rosData.header.seq = rosMsgCounter++;
                rosData.header.stamp = normalizeSecNSec(lastStateStamp.getTime());
                rosData.header.frame_id = frame_id;

                rosData.angle_min = minAngle * 3.14 / 180.0;
                rosData.angle_max = maxAngle * 3.14 / 180.0;
                rosData.angle_increment = resolution * 3.14 / 180.0;
                rosData.time_increment = 0;             // all points in a single scan are considered took at the very same time
                rosData.scan_time = 1/getRate();        // time elapsed between two successive readings
                rosData.range_min = minDistance;
                rosData.range_max = maxDistance;
                rosData.ranges.resize(ranges_size);
                rosData.intensities.resize(ranges_size);

                for (int i = 0; i < ranges_size; i++)
                {
                    rosData.ranges[i] = ranges[i];
                    rosData.intensities[i] = 0.0;
                }

                rosPublisherPort.write();
            }
        }
        else
        {
            yError("Rangefinder2DWrapper: %s: Sensor returned error", sensorId.c_str());
        }
    }
}

bool Rangefinder2DWrapper::close()
{
    yTrace("Rangefinder2DWrapper::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    detachAll();
    return true;
}
