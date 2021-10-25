/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEODOMETRY_H
#define YARP_FAKEODOMETRY_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>

constexpr double default_period = 0.02;

/**
 * @ingroup dev_impl_fake dev_impl_navigation
 *
 * \section fakeOdometry_parameters Device description
 * \brief `FakeOdometry`: A device for generating a fake odometry.
 * This device will generate the odometry and then the user can retrieve it by calling `getOdometry`.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter            | Type    | Units          | Default Value | Required                       | Description                                                                                         | Notes |
 * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:---------------------------------------------------------------------------------------------------:|:-----:|
 * | period         |      -                  | double  | s              |   0.02        | No                             | refresh period of the broadcasted values in s                                                      | default 0.02s |
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FakeOdometry
 * period 0.02
 * \endcode
 *
 * example of xml file
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="fakeOdometry" build="2" portprefix="test" xmlns:xi="http://www.w3.org/2001/XInclude">
 *   <devices>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="fakeOdometry_device" type="fakeOdometry">
 *     </device>
 *   </devices>
 * </robot>
 * \endcode
 *
 * example of command via terminal.
 *
 * \code{.unparsed}
 * yarpdev --device fakeOdometry
 * \endcode
 */

class FakeOdometry :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::IOdometry2D
{
public:
    FakeOdometry();

    // PeriodicThread
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IOdometry2D
    bool   getOdometry(yarp::dev::OdometryData& odom) override;
    bool   resetOdometry() override;

private:
    yarp::dev::OdometryData m_odometryData;

    std::mutex m_odometry_mutex;
    double m_period;
};

#endif // YARP_FAKEODOMETRY_H
