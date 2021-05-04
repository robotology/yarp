/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include "rangefinder2D_nws_ros.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RANGEFINDER2D_NWS_ROS, "yarp.devices.rangefinder2D_nws_ros")


/**
  * It reads the data from a rangefinder sensor and sends them on one port.
  * It also creates one rpc port.
  */

Rangefinder2D_nws_ros::Rangefinder2D_nws_ros() : PeriodicThread(DEFAULT_THREAD_PERIOD),
    rosNode(nullptr),
    rosMsgCounter(0),
    sens_p(nullptr),
    iTimed(nullptr),
    _period(DEFAULT_THREAD_PERIOD),
    minAngle(0),
    maxAngle(0),
    minDistance(0),
    maxDistance(0),
    resolution(0),
    isDeviceOwned(false)
{}

Rangefinder2D_nws_ros::~Rangefinder2D_nws_ros()
{
    sens_p = nullptr;
}

bool Rangefinder2D_nws_ros::checkROSParams(yarp::os::Searchable &config)
{
    // check for ROS_nodeName parameter
    if (!config.check("nodeName"))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Cannot find ROS_nodeName parameter, mandatory when using ROS message";
        return false;
    }
    rosNodeName = config.find("nodeName").asString();  // TODO: check name is correct
    yCInfo(RANGEFINDER2D_NWS_ROS) << "NodeName is " << rosNodeName;

    // check for ROS_topicName parameter
    if (!config.check("topicName"))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Cannot find ROS_topicName parameter, mandatory when using ROS message";
        return false;
    }
    rosTopicName = config.find("topicName").asString();
    yCInfo(RANGEFINDER2D_NWS_ROS) << "rosTopicName is " << rosTopicName;

    // check for frame_id parameter
    if (!config.check("frame_id"))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Cannot find frame_id parameter, mandatory when using ROS message";
        return false;
    }
    frame_id = config.find("frame_id").asString();
    yCInfo(RANGEFINDER2D_NWS_ROS) << "Frame_id is " << frame_id;

    return true;
}

bool Rangefinder2D_nws_ros::initialize_ROS()
{
    rosNode = new yarp::os::Node(rosNodeName);   // add a ROS node
    if (rosNode == nullptr)
    {
        yCError(RANGEFINDER2D_NWS_ROS) << " opening " << rosNodeName << " Node, check your yarp-ROS network configuration\n";
        return false;
    }
    if (!rosPublisherPort.topic(rosTopicName))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << " opening " << rosTopicName << " Topic, check your yarp-ROS network configuration\n";
        return false;
    }
    return true;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool Rangefinder2D_nws_ros::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(RANGEFINDER2D_NWS_ROS, "Rangefinder2DWrapper: cannot attach more than one device");
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
        yCError(RANGEFINDER2D_NWS_ROS, "Rangefinder2DWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(sens_p);

    if(!sens_p->getDistanceRange(minDistance, maxDistance))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Laser device does not provide min & max distance range.";
        return false;
    }

    if(!sens_p->getScanLimits(minAngle, maxAngle))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Laser device does not provide min & max angle scan range.";
        return false;
    }

    if (!sens_p->getHorizontalResolution(resolution))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Laser device does not provide horizontal resolution ";
        return false;
    }

    PeriodicThread::setPeriod(_period);
    return PeriodicThread::start();
}

bool Rangefinder2D_nws_ros::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
    return true;
}

void Rangefinder2D_nws_ros::attach(yarp::dev::IRangefinder2D *s)
{
    sens_p = s;
}

void Rangefinder2D_nws_ros::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
}

bool Rangefinder2D_nws_ros::threadInit()
{
    return true;
}

bool Rangefinder2D_nws_ros::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        yCError(RANGEFINDER2D_NWS_ROS) << "Rangefinder2DWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
        _period = config.find("period").asInt32() / 1000.0;

    checkROSParams(config);

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
            yCError(RANGEFINDER2D_NWS_ROS) << "failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&driver, "1");
        if(!attachAll(driverlist))
        {
            yCError(RANGEFINDER2D_NWS_ROS) << "failed to open subdevice.. check params";
            return false;
        }
        isDeviceOwned = true;
    }
    return true;
}


void Rangefinder2D_nws_ros::threadRelease()
{
    rosPublisherPort.close();
}

void Rangefinder2D_nws_ros::run()
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
            if(iTimed)
                lastStateStamp = iTimed->getLastInputStamp();
            else
                lastStateStamp.update(yarp::os::Time::now());

            int ranges_size = ranges.size();

            // publish ROS topic if required
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
        else
        {
            yCError(RANGEFINDER2D_NWS_ROS, "Rangefinder2D_nws_ros: %s: Sensor returned error", rosTopicName.c_str());
        }
    }
}

bool Rangefinder2D_nws_ros::close()
{
    yCTrace(RANGEFINDER2D_NWS_ROS, "Rangefinder2DWrapperROSROS::Close");
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
