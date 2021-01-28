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

#ifndef YARP_DEV_RANGEFINDER2D_NWS_ROS_H
#define YARP_DEV_RANGEFINDER2D_NWS_ROS_H

 //#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/LaserScan2D.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/api.h>
#include <yarp/dev/IPreciselyTimed.h>

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>


#define DEFAULT_THREAD_PERIOD 0.02 //s


  /**
   *  @ingroup dev_impl_wrapper dev_impl_network_wrapper dev_impl_lidar
   *
   * \section Rangefinder2D_nws_ros_device_parameters Description of input parameters
   * A Network grabber for 2D Rangefinder devices.
   * This device will publish data on the specified ROS topic.
   *
   * This device does not accepts YARP RPC commands, it is dedicated only to data publishing.
   *
   *   Parameters required by this device are:
   * | Parameter name | SubParameter            | Type    | Units          | Default Value | Required                       | Description                                                           | Notes        |
   * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:---------------------------------------------------------------------:|:------------:|
   * | period         |      -                  | int     | ms             |   20          | No                             | refresh period of the broadcasted values in ms                        | default 20ms |
   * | subdevice      |      -                  | string  | -              |   -           | alternative to 'attach' action | name of the subdevice to use as a data source                         | when used, parameters for the subdevice must be provided as well |
   * | nodeName       |      -                  | string  | -              |   -           | Yes                            | name of ROS node,  e.g. /myRobotName                                  | -           |
   * | namtopicNamee  |      -                  | string  | -              |   -           | Yes '                          | name of ROS topic, e.g. /Rangefinder2DSensor                          | -           |
   * | frame_id       |      -                  | string  | -              |   -           | Yes                            | name of the attached frame                                            | -           |
   *
   * Example of configuration file using .ini format.
   *
   * \code{.unparsed}
   * device rangefinder2D_nws_ros
   * subdevice <Rangefinder2DSensor>
   * period 20
   * name /<robotName>/Rangefinder2DSensor
   * \endcode
   */
class Rangefinder2D_nws_ros :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper
{
public:
    Rangefinder2D_nws_ros();
    ~Rangefinder2D_nws_ros();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IRangefinder2D *s);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    // ROS streaming data
    std::string                                               frame_id;             // name of the frame measures are referred to
    std::string                                               rosNodeName;          // name of the rosNode
    std::string                                               rosTopicName;         // name of the rosTopic
    yarp::os::Node*                                           rosNode;              // add a ROS node
    yarp::os::NetUint32                                       rosMsgCounter;        // incremental counter in the ROS message
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::LaserScan> rosPublisherPort;     // Dedicated ROS topic publisher

private:
    //interfaces
    yarp::dev::PolyDriver driver;
    yarp::dev::IRangefinder2D* sens_p;
    yarp::dev::IPreciselyTimed* iTimed;

private:
    //device data
    yarp::os::Stamp lastStateStamp;
    double _period;
    double minAngle, maxAngle;
    double minDistance, maxDistance;
    double resolution;
    bool   isDeviceOwned;

private:
    //private methods
    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
};

#endif //YARP_DEV_RANGEFINDER2D_NWS_ROS_H
