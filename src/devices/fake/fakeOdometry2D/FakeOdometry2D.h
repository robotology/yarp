/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEODOMETRY2D_H
#define YARP_FAKEODOMETRY2D_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>

#include "FakeOdometry2D_ParamsParser.h"

constexpr double default_period = 0.02;

/**
 * @ingroup dev_impl_fake dev_impl_navigation
 *
 * \section fakeOdometry_parameters Device description
 * \brief `fakeOdometry2D`: A device for generating a fake odometry.
 * This device will generate the odometry and then the user can retrieve it by calling `getOdometry`.
 *
 * Parameters required by this device are shown in class: FakeOdometry2D_ParamsParser
 *
 */

class FakeOdometry2D :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::IOdometry2D,
        public FakeOdometry2D_ParamsParser
{
public:
    FakeOdometry2D();

    // PeriodicThread
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IOdometry2D
    bool   getOdometry(yarp::dev::OdometryData& odom, double* timestamp=nullptr) override;
    bool   resetOdometry() override;

private:
    yarp::dev::OdometryData m_odometryData;

    std::mutex m_odometry_mutex;
    double m_period;
    double m_timestamp=0;
};

#endif // YARP_FAKEODOMETRY2D_H
