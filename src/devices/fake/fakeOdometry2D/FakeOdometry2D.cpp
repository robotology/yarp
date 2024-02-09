/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FakeOdometry2D.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Rand.h>


namespace {
    YARP_LOG_COMPONENT(FAKEODOMETRY2D, "yarp.device.FakeOdometry2D")
}


FakeOdometry2D::FakeOdometry2D():
PeriodicThread(default_period)
{
    m_period = default_period;
    yCTrace(FAKEODOMETRY2D);
}


bool FakeOdometry2D::threadInit()
{
    yCTrace(FAKEODOMETRY2D);
    return true;
}


void FakeOdometry2D::run()
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
    m_timestamp = yarp::os::Time::now();
}


void FakeOdometry2D::threadRelease()
{
}


bool FakeOdometry2D::open(yarp::os::Searchable& config)
{
    // check period
    if (!config.check("period", "refresh period of the broadcasted values in s")) {
        yCInfo(FAKEODOMETRY2D) << "Using default 'period' parameter";
    }  else {
        m_period = config.find("period").asFloat64();
    }
    yCInfo(FAKEODOMETRY2D) << "thread period set to " << m_period << "s";
    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}


bool FakeOdometry2D::close()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    return true;
}


bool FakeOdometry2D::getOdometry(yarp::dev::OdometryData& odom, double* timestamp)
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
    if (timestamp!=nullptr)
    {
        *timestamp = m_timestamp;
    }
    return true;
}


bool FakeOdometry2D::resetOdometry()
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
