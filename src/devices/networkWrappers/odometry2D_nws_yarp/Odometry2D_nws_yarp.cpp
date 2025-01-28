/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Odometry2D_nws_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Stamp.h>

#include <cmath>

YARP_LOG_COMPONENT(ODOMETRY2D_NWS_YARP, "yarp.devices.Odometry2D_nws_yarp")

Odometry2D_nws_yarp::Odometry2D_nws_yarp() : yarp::os::PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

Odometry2D_nws_yarp::~Odometry2D_nws_yarp()
{
    m_odometry2D_interface = nullptr;
}


bool Odometry2D_nws_yarp::attach(yarp::dev::PolyDriver* driver)
{
    std::lock_guard lock(m_mutex);

    if (driver->isValid())
    {
        driver->view(m_odometry2D_interface);
    }

    if (m_odometry2D_interface == nullptr)
    {
        yCError(ODOMETRY2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    m_RPC=new IOdometry2DRPCd(m_odometry2D_interface);

    bool b = PeriodicThread::start();

    return b;
}


bool Odometry2D_nws_yarp::detach()
{
    std::lock_guard lock(m_mutex);

    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    if (m_RPC)
    {
        delete m_RPC;
        m_RPC = nullptr;
    }
    m_odometry2D_interface = nullptr;
    return true;
}

bool Odometry2D_nws_yarp::threadInit()
{
    return true;
}

bool Odometry2D_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) { return false; }

    m_rpcPortName = m_name + "/rpc";
    m_odometerStreamingPortName = m_name + "/odometer:o";
    m_odometryStreamingPortName = m_name + "/odometry:o";
    m_velocityStreamingPortName = m_name + "/velocity:o";

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(ODOMETRY2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    if (!m_port_odometer.open(m_odometerStreamingPortName))
    {
        yCError(ODOMETRY2D_NWS_YARP, "failed to open port %s", m_odometerStreamingPortName.c_str());
        return false;
    }

    if (!m_port_odometry.open(m_odometryStreamingPortName))
    {
        yCError(ODOMETRY2D_NWS_YARP, "failed to open port %s", m_odometryStreamingPortName.c_str());
        return false;
    }

    if (!m_port_velocity.open(m_velocityStreamingPortName))
    {
        yCError(ODOMETRY2D_NWS_YARP, "failed to open port %s", m_velocityStreamingPortName.c_str());
        return false;
    }

    PeriodicThread::setPeriod(m_period);

    yCInfo(ODOMETRY2D_NWS_YARP) << "Waiting for device to attach";

    return true;
}

void Odometry2D_nws_yarp::threadRelease()
{
    m_port_velocity.interrupt();
    m_port_velocity.close();
    m_port_odometry.interrupt();
    m_port_odometry.close();
    m_port_odometer.interrupt();
    m_port_odometer.close();
}

void Odometry2D_nws_yarp::run()
{
    std::lock_guard lock(m_mutex);

    if (m_odometry2D_interface!=nullptr)
    {
        yarp::dev::OdometryData odometryData;
        double synchronized_timestamp = 0;
        m_odometry2D_interface->getOdometry(odometryData, &synchronized_timestamp);

        if (std::isnan(synchronized_timestamp) == false)
        {
            m_lastStateStamp.update(synchronized_timestamp);
        }
        else
        {
            m_lastStateStamp.update(yarp::os::Time::now());
        }

        if (m_port_odometry.getOutputCount()>0)
        {
            m_port_odometry.setEnvelope(m_lastStateStamp);
            yarp::dev::OdometryData &odometryDataFromPort = m_port_odometry.prepare();
            odometryDataFromPort.odom_x= odometryData.odom_x; //position in the odom reference frame
            odometryDataFromPort.odom_y= odometryData.odom_y;
            odometryDataFromPort.odom_theta= odometryData.odom_theta;
            odometryDataFromPort.base_vel_x= odometryData.base_vel_x; //velocity in the robot reference frame
            odometryDataFromPort.base_vel_y= odometryData.base_vel_y;
            odometryDataFromPort.base_vel_theta= odometryData.base_vel_theta;
            odometryDataFromPort.odom_vel_x= odometryData.odom_vel_x; //velocity in the odom reference frame
            odometryDataFromPort.odom_vel_y= odometryData.odom_vel_y;
            odometryDataFromPort.odom_vel_theta= odometryData.odom_vel_theta;
            m_port_odometry.write();
        }

        if (m_port_odometer.getOutputCount()>0)
        {
            m_port_odometer.setEnvelope(m_lastStateStamp);
            yarp::os::Bottle &odometer_data = m_port_odometer.prepare();
            odometer_data.clear();
            double traveled_distance = sqrt((odometryData.odom_vel_x - m_oldOdometryData.odom_x) *
                                            (odometryData.odom_vel_x - m_oldOdometryData.odom_x) +
                                            (odometryData.odom_vel_y - m_oldOdometryData.odom_y) *
                                            (odometryData.odom_vel_y - m_oldOdometryData.odom_y));
            double traveled_angle = odometryData.odom_theta - m_oldOdometryData.odom_theta;
            odometer_data.addFloat64(traveled_distance);
            odometer_data.addFloat64(traveled_angle);
            m_port_odometer.write();
        }


        if (m_port_velocity.getOutputCount()>0)
        {
            m_port_velocity.setEnvelope(m_lastStateStamp);
            yarp::os::Bottle &velocityData = m_port_velocity.prepare();
            velocityData.clear();
            velocityData.addFloat64(sqrt(odometryData.odom_vel_x * odometryData.odom_vel_x +
                                         odometryData.odom_vel_y * odometryData.odom_vel_y));
            velocityData.addFloat64(odometryData.base_vel_theta);
            m_port_velocity.write();
        }
    } else {
        yCError(ODOMETRY2D_NWS_YARP) << "no yarp::dev::Nav2D::IOdometry2D interface found";
    }
}

bool Odometry2D_nws_yarp::close()
{
    yCTrace(ODOMETRY2D_NWS_YARP, "Odometry2D_nws_yarp::Close");
    return detach();
}

bool Odometry2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_RPC) { return false;}

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_RPC)
    {
        bool b = m_RPC->read(connection);
        if (b) {
            return true;
        }
    }
    yCDebug(ODOMETRY2D_NWS_YARP) << "read() Command failed";
    return false;
}
