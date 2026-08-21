/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ODOMETRY_H
#define YARP_DEV_ODOMETRY_H

#include <yarp/dev/OdometryData.h>

namespace yarp::dev::Nav2D {
class YARP_dev_API Odometry : public yarp::dev::OdometryData
{
public:

    Odometry()=default;
    virtual ~Odometry()=default;
};
} // namespace yarp::dev::Nav2D

#endif // YARP_DEV_ODOMETRY_H
