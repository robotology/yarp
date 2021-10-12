/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IODOMETRY2D_H
#define YARP_DEV_IODOMETRY2D_H

#include <yarp/dev/api.h>
#include <yarp/dev/OdometryData.h>

namespace yarp::dev::Nav2D {
class IOdometry2D;
} // namespace yarp

/**
 * @ingroup dev_iface_navigation
 *
 * IOdometry2D interface. Provides methods to obtain the odometry data from a robot.
 */
class YARP_dev_API yarp::dev::Nav2D::IOdometry2D
{
public:
    /**
     * Destructor.
     */
    virtual ~IOdometry2D();

    /**
    * Gets the odometry of the robot, including its velocity expressed in the world and in the local reference frame.
    * @param odom the odometry.
    * @return true/false
    */
    virtual bool   getOdometry(yarp::dev::OdometryData& odom) = 0;

    /**
    * Resets the odometry of the robot to zero
    * @return true/false
    */
    virtual bool   resetOdometry() = 0;
};

#endif // YARP_DEV_IODOMETRY2D_H
