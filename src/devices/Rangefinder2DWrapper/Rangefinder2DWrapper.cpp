/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Rangefinder2DWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RANGEFINDER2DWRAPPER, "yarp.devices.Rangefinder2DWrapper")


/**
  * It reads the data from a rangefinder sensor and sends them on one port.
  * It also creates one rpc port.
  */

Rangefinder2DWrapper::Rangefinder2DWrapper() : PeriodicThread(DEFAULT_THREAD_PERIOD),
    partName("Rangefinder2DWrapper"),
    sens_p(nullptr),
    iTimed(nullptr),
    _period(DEFAULT_THREAD_PERIOD),
    minAngle(0),
    maxAngle(0),
    minDistance(0),
    maxDistance(0),
    resolution(0),
    isDeviceOwned(false),
    // init ROS data
    useROS(ROS_disabled),
    frame_id(""),
    rosNodeName(""),
    rosTopicName(""),
    rosNode(nullptr),
    rosMsgCounter(0)
{}

Rangefinder2DWrapper::~Rangefinder2DWrapper()
{
    sens_p = nullptr;
}

bool Rangefinder2DWrapper::checkROSParams(yarp::os::Searchable &config)
{
    // check for ROS parameter group
    if (!config.check("ROS"))
    {
        useROS = ROS_disabled;
        yCInfo(RANGEFINDER2DWRAPPER) << "No ROS group found in config file ... skipping ROS initialization.";
        return true;
    }

    yCInfo(RANGEFINDER2DWRAPPER) << "ROS group was FOUND in config file.";

    Bottle &rosGroup = config.findGroup("ROS");
    if (rosGroup.isNull())
    {
        yCError(RANGEFINDER2DWRAPPER) << partName << "ROS group params is not a valid group or empty";
        useROS = ROS_config_error;
        return false;
    }

    // check for useROS parameter
    if (!rosGroup.check("useROS"))
    {
        yCError(RANGEFINDER2DWRAPPER) << partName << " cannot find useROS parameter, mandatory when using ROS message. \n \
                                                        Allowed values are true, false, ROS_only";
        useROS = ROS_config_error;
        return false;
    }
    std::string ros_use_type = rosGroup.find("useROS").asString();
    if (ros_use_type == "false")
    {
        yCInfo(RANGEFINDER2DWRAPPER) << partName << "useROS topic if set to 'false'";
        useROS = ROS_disabled;
        return true;
    }
    else if (ros_use_type == "true")
    {
        yCInfo(RANGEFINDER2DWRAPPER) << partName << "useROS topic if set to 'true'";
        useROS = ROS_enabled;
    }
    else if (ros_use_type == "only")
    {
        yCInfo(RANGEFINDER2DWRAPPER) << partName << "useROS topic if set to 'only";
        useROS = ROS_only;
    }
    else
    {
        yCInfo(RANGEFINDER2DWRAPPER) << partName << "useROS parameter is set to invalid value ('" << ros_use_type << "'), supported values are 'true', 'false', 'only'";
        useROS = ROS_config_error;
        return false;
    }

    // check for ROS_nodeName parameter
    if (!rosGroup.check("ROS_nodeName"))
    {
        yCError(RANGEFINDER2DWRAPPER) << partName << " cannot find ROS_nodeName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosNodeName = rosGroup.find("ROS_nodeName").asString();
    yCInfo(RANGEFINDER2DWRAPPER) << partName << "rosNodeName is " << rosNodeName;
    if(rosNodeName[0] != '/'){
        yCError(RANGEFINDER2DWRAPPER) << "node name must begin with an initial /";
        return false;
    }
    // check for ROS_topicName parameter
    if (!rosGroup.check("ROS_topicName"))
    {
        yCError(RANGEFINDER2DWRAPPER) << partName << " cannot find ROS_topicName parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    rosTopicName = rosGroup.find("ROS_topicName").asString();
    yCInfo(RANGEFINDER2DWRAPPER) << partName << "rosTopicName is " << rosTopicName;

    // check for frame_id parameter
    if (!rosGroup.check("frame_id"))
    {
        yCError(RANGEFINDER2DWRAPPER) << partName << " cannot find frame_id parameter, mandatory when using ROS message";
        useROS = ROS_config_error;
        return false;
    }
    frame_id = rosGroup.find("frame_id").asString();
    yCInfo(RANGEFINDER2DWRAPPER) << partName << "frame_id is " << frame_id;

    return true;
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
            if (rosNode == nullptr)
            {
                yCError(RANGEFINDER2DWRAPPER) << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }
            if (!rosPublisherPort.topic(rosTopicName))
            {
                yCError(RANGEFINDER2DWRAPPER) << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration\n";
                success = false;
                break;
            }
            success = true;
        } break;

        case ROS_disabled:
        {
            yCInfo(RANGEFINDER2DWRAPPER) << partName << ": no ROS initialization required";
            success = true;
        } break;

        case ROS_config_error:
        {
            yCError(RANGEFINDER2DWRAPPER) << partName << " ROS parameter are not correct, check your configuration file";
            success = false;
        } break;

        default:
        {
            yCError(RANGEFINDER2DWRAPPER) << partName << " something went wrong with ROS configuration, we should never be here!!!";
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
        yCError(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(sens_p);
        Idevice2attach->view(iTimed);
    }

    if (nullptr == sens_p)
    {
        yCError(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(sens_p);

    if(!sens_p->getDistanceRange(minDistance, maxDistance))
    {
        yCError(RANGEFINDER2DWRAPPER) << "Laser device does not provide min & max distance range.";
        return false;
    }

    if(!sens_p->getScanLimits(minAngle, maxAngle))
    {
        yCError(RANGEFINDER2DWRAPPER) << "Laser device does not provide min & max angle scan range.";
        return false;
    }

    if (!sens_p->getHorizontalResolution(resolution))
    {
        yCError(RANGEFINDER2DWRAPPER) << "Laser device does not provide horizontal resolution ";
        return false;
    }

    PeriodicThread::setPeriod(_period);
    return PeriodicThread::start();
}

bool Rangefinder2DWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
    return true;
}

void Rangefinder2DWrapper::attach(yarp::dev::IRangefinder2D *s)
{
    sens_p = s;
}

void Rangefinder2DWrapper::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
}

bool Rangefinder2DWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) {
        return false;
    }

    // parse in, prepare out
    int action = in.get(0).asVocab32();
    int inter  = in.get(1).asVocab32();
    bool ret = false;

    if (inter == VOCAB_ILASER2D)
    {
        if (action == VOCAB_GET)
        {
            int cmd = in.get(2).asVocab32();
            if (cmd == VOCAB_DEVICE_INFO)
            {
                if (sens_p)
                {
                    std::string info;
                    if (sens_p->getDeviceInfo(info))
                    {
                        out.addVocab32(VOCAB_IS);
                        out.addVocab32(cmd);
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
                        out.addVocab32(VOCAB_IS);
                        out.addVocab32(cmd);
                        out.addFloat64(min);
                        out.addFloat64(max);
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
                        out.addVocab32(VOCAB_IS);
                        out.addVocab32(cmd);
                        out.addFloat64(min);
                        out.addFloat64(max);
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
                        out.addVocab32(VOCAB_IS);
                        out.addVocab32(cmd);
                        out.addFloat64(step);
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
                        out.addVocab32(VOCAB_IS);
                        out.addVocab32(cmd);
                        out.addFloat64(rate);
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
                yCError(RANGEFINDER2DWRAPPER, "Invalid command received in Rangefinder2DWrapper");
            }
        }
        else if (action == VOCAB_SET)
        {
            int cmd = in.get(2).asVocab32();
            if (cmd == VOCAB_LASER_DISTANCE_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt32();
                    double max = in.get(4).asInt32();
                    sens_p->setDistanceRange(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt32();
                    double max = in.get(4).asInt32();
                    sens_p->setScanLimits(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_SCAN_RATE)
            {
                if (sens_p)
                {
                    double rate = in.get(3).asInt32();
                    sens_p->setScanRate(rate);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_STEP)
            {
                if (sens_p)
                {
                    double step = in.get(3).asFloat64();
                    sens_p->setHorizontalResolution(step);
                    ret = true;
                }
            }
            else
            {
                yCError(RANGEFINDER2DWRAPPER, "Invalid command received in Rangefinder2DWrapper");
            }
        }
        else
        {
            yCError(RANGEFINDER2DWRAPPER, "Invalid action received in Rangefinder2DWrapper");
        }
    }
    else
    {
        yCError(RANGEFINDER2DWRAPPER, "Invalid interface vocab received in Rangefinder2DWrapper");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab32(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr) {
        out.write(*returnToSender);
    }
    return true;
}

bool Rangefinder2DWrapper::threadInit()
{
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
    yCWarning(RANGEFINDER2DWRAPPER) << "The 'Rangefinder2DWrapper' device is deprecated in favour of 'rangefinder2D_nws_yarp'.";
    yCWarning(RANGEFINDER2DWRAPPER) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(RANGEFINDER2DWRAPPER) << "Please update your scripts.";

    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        yCError(RANGEFINDER2DWRAPPER) << "Rangefinder2DWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    } else {
        _period = config.find("period").asInt32() / 1000.0;
    }

    if (!config.check("name"))
    {
        yCError(RANGEFINDER2DWRAPPER) << "Rangefinder2DWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yCError(RANGEFINDER2DWRAPPER) << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString();
        rpcPortName = streamingPortName + "/rpc:i";
        setId("Rangefinder2DWrapper");
    }

    checkROSParams(config);

    if(!initialize_YARP(config) )
    {
        yCError(RANGEFINDER2DWRAPPER) << sensorId << "Error initializing YARP ports";
        return false;
    }


    // call ROS node/topic initialization, if needed
    if (!initialize_ROS())
    {
        return false;
    }

    if(config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if(!driver.open(p) || !driver.isValid())
        {
            yCError(RANGEFINDER2DWRAPPER) << "RangeFinder2DWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&driver, "1");
        if(!attachAll(driverlist))
        {
            yCError(RANGEFINDER2DWRAPPER) << "RangeFinder2DWrapper: failed to open subdevice.. check params";
            return false;
        }
        isDeviceOwned = true;
    }
    return true;
}

bool Rangefinder2DWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    if(useROS != ROS_only)
    {
        if (!streamingPort.open(streamingPortName))
            {
                yCError(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper: failed to open port %s", streamingPortName.c_str());
                return false;
            }
        if (!rpcPort.open(rpcPortName))
            {
                yCError(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper: failed to open port %s", rpcPortName.c_str());
                return false;
            }
        rpcPort.setReader(*this);
    }
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
    if (sens_p!=nullptr)
    {
        bool ret = true;
        IRangefinder2D::Device_status status;
        yarp::sig::Vector ranges;
        ret &= sens_p->getRawData(ranges);
        ret &= sens_p->getDeviceStatus(status);

        if (ret)
        {
            if (iTimed) {
                lastStateStamp = iTimed->getLastInputStamp();
            } else {
                lastStateStamp.update(yarp::os::Time::now());
            }

            int ranges_size = ranges.size();

            yarp::dev::LaserScan2D& b = streamingPort.prepare();
            //b.clear();
            b.scans=ranges;
            b.angle_min= minAngle;
            b.angle_max= maxAngle;
            b.range_min= minDistance;
            b.range_max= maxDistance;
            b.status=status;
            streamingPort.setEnvelope(lastStateStamp);
            streamingPort.write();

            // publish ROS topic if required
            if (useROS != ROS_disabled)
            {
                yarp::rosmsg::sensor_msgs::LaserScan &rosData = rosPublisherPort.prepare();
                rosData.header.seq = rosMsgCounter++;
                rosData.header.stamp = lastStateStamp.getTime();
                rosData.header.frame_id = frame_id;

                rosData.angle_min = minAngle * M_PI / 180.0;
                rosData.angle_max = maxAngle * M_PI / 180.0;
                rosData.angle_increment = resolution * M_PI / 180.0;
                rosData.time_increment = 0;             // all points in a single scan are considered took at the very same time
                rosData.scan_time = getPeriod();        // time elapsed between two successive readings
                rosData.range_min = minDistance;
                rosData.range_max = maxDistance;
                rosData.ranges.resize(ranges_size);
                rosData.intensities.resize(ranges_size);

                for (int i = 0; i < ranges_size; i++)
                {
                    // in yarp, NaN is used when a scan value is missing. For example when the angular range of the rangefinder is smaller than 360.
                    // is ros, NaN is not used. Hence this check replaces NaN with inf.
                    if (std::isnan(ranges[i]))
                    {
                       rosData.ranges[i] = std::numeric_limits<double>::infinity();
                       rosData.intensities[i] = 0.0;
                    }
                    else
                    {
                       rosData.ranges[i] = ranges[i];
                       rosData.intensities[i] = 0.0;
                    }
                }
                rosPublisherPort.write();
            }
        }
        else
        {
            yCError(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper: %s: Sensor returned error", sensorId.c_str());
        }
    }
}

bool Rangefinder2DWrapper::close()
{
    yCTrace(RANGEFINDER2DWRAPPER, "Rangefinder2DWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    if(rosNode!=nullptr) {
        rosNode->interrupt();
        delete rosNode;
        rosNode = nullptr;
    }

    detachAll();
    return true;
}
