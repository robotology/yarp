/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_DEV_POSEROSPUBLISHER_H
#define YARP_DEV_POSEROSPUBLISHER_H

#include "GenericSensorRosPublisher.h"
#include <yarp/rosmsg/geometry_msgs/PoseStamped.h>

    /**
 * @ingroup dev_impl_wrapper
 *
 * \brief This wrapper connects to a device and publishes a ROS topic of type geometry_msgs::PoseStamped.
 *
 * | YARP device name |
 * |:-----------------:|
 * | `PoseRosPublisher` |
 *
 * The parameters accepted by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | Prefix of the port opened by this device                          | MUST start with a '/' character |
 * | period         |      -         | int     | ms             |   -           | Yes                          | Refresh period of the broadcasted values in ms                    |  |
 */
class PoseRosPublisher : public GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>
{
    // Interface of the wrapped device
    yarp::dev::IOrientationSensors* m_iOrientationSensors{ nullptr };
    yarp::dev::IPositionSensors*    m_iPositionSensors {nullptr};

public:
    using GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>::GenericSensorRosPublisher;

    using GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>::open;
    using GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>::close;

    using GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>::attachAll;
    using GenericSensorRosPublisher<yarp::rosmsg::geometry_msgs::PoseStamped>::detachAll;

    /* RateThread methods */
    void run() override;

protected:
    bool viewInterfaces() override;
};

#endif

