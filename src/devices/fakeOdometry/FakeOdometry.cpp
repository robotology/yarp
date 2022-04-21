/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FakeOdometry.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Rand.h>


namespace {
    YARP_LOG_COMPONENT(FAKEODOMETRY, "yarp.device.FakeOdometry")
}


FakeOdometry::FakeOdometry():
PeriodicThread(default_period)
{
    m_period = default_period;
    yCTrace(FAKEODOMETRY);
}


bool FakeOdometry::threadInit()
{
    yCTrace(FAKEODOMETRY);
    return true;
}


void FakeOdometry::run()
{
    std::lock_guard lock(m_odometry_mutex);
    m_odometryData.base_vel_x = yarp::math::Rand::scalar(0,5);
    m_odometryData.base_vel_y = yarp::math::Rand::scalar(0,5);
    m_odometryData.base_vel_theta = yarp::math::Rand::scalar(0,5);
    m_odometryData.odom_vel_x = m_odometryData.base_vel_x;
    m_odometryData.odom_vel_y =  m_odometryData.base_vel_y;
    m_odometryData.odom_vel_theta = m_odometryData.base_vel_theta;
    m_odometryData.odom_x = m_odometryData.odom_x + m_period * m_odometryData.base_vel_x;
    m_odometryData.odom_y =  m_odometryData.odom_y + m_period * m_odometryData.base_vel_y;
    m_odometryData.odom_theta = m_odometryData.odom_theta + m_period * m_odometryData.base_vel_theta;

}


void FakeOdometry::threadRelease()
{
}


bool FakeOdometry::open(yarp::os::Searchable& config)
{
    // check period
    if (!config.check("period", "refresh period of the broadcasted values in s")) {
        yCInfo(FAKEODOMETRY) << "Using default 'period' parameter";
    }  else {
        m_period = config.find("period").asFloat64();
    }
    yCInfo(FAKEODOMETRY) << "thread period set to " << m_period << "s";
    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}


bool FakeOdometry::close()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    return true;
}


bool FakeOdometry::getOdometry(yarp::dev::OdometryData& odom)
{
    std::lock_guard lock(m_odometry_mutex);
    odom.odom_x = m_odometryData.odom_x;
    odom.odom_y = m_odometryData.odom_y;
    odom.odom_theta  = m_odometryData.odom_theta;
    odom.base_vel_x = m_odometryData.base_vel_x;
    odom.base_vel_y = m_odometryData.base_vel_y;
    odom.base_vel_theta = m_odometryData.base_vel_theta;
    odom.odom_vel_x  = m_odometryData.odom_vel_x;
    odom.odom_vel_y = m_odometryData.odom_vel_y;
    odom.odom_vel_theta = m_odometryData.odom_vel_theta;
    return true;
}


bool FakeOdometry::resetOdometry()
{
    std::lock_guard lock(m_odometry_mutex);
    m_odometryData.odom_x = 0;
    m_odometryData.odom_y = 0;
    m_odometryData.odom_theta = 0;
    m_odometryData.base_vel_x = 0;
    m_odometryData.base_vel_y = 0;
    m_odometryData.base_vel_theta = 0;
    m_odometryData.odom_vel_x = 0;
    m_odometryData.odom_vel_y = 0;
    m_odometryData.odom_vel_theta = 0;
    return true;
}
