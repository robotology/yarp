/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "Odometry2D_nws_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Stamp.h>
#include <math.h>

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

    if (driver->isValid())
    {
        driver->view(m_odometry2D_interface);
    }

    if (m_odometry2D_interface == nullptr)
    {
        yCError(ODOMETRY2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    bool b = PeriodicThread::start();

    return b;
}


bool Odometry2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
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
    yarp::os::Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        yCWarning(ODOMETRY2D_NWS_YARP) << "missing 'period' parameter, using default value of" << DEFAULT_THREAD_PERIOD;
    } else {
        m_period = config.find("period").asFloat64();
    }

    if (!config.check("odometer_port_name"))
    {
        yCError(ODOMETRY2D_NWS_YARP) << "missing odometer_port_name parameter";
        return false;
    }
    m_odometerStreamingPortName  = config.find("odometer_port_name").asString();
    if (m_odometerStreamingPortName[0] != '/') {
        yCError(ODOMETRY2D_NWS_YARP) << "missing initial / in odometer_port_name parameter";
        return false;
    }

    if (!config.check("odometry_port_name"))
    {
        yCError(ODOMETRY2D_NWS_YARP) << "missing odometry_port_name parameter";
        return false;
    }
    m_odometryStreamingPortName  = config.find("odometry_port_name").asString();
    if (m_odometryStreamingPortName[0] != '/') {
        yCError(ODOMETRY2D_NWS_YARP) << "missing initial / in odometry_port_name parameter";
        return false;
    }

    if (!config.check("velocity_port_name"))
    {
        yCError(ODOMETRY2D_NWS_YARP) << "missing velocity_port_name parameter";
        return false;
    }
    m_velocityStreamingPortName  = config.find("velocity_port_name").asString();
    if (m_velocityStreamingPortName[0] != '/') {
        yCError(ODOMETRY2D_NWS_YARP) << "missing initial / in velocity_port_name parameter";
        return false;
    }

    if(config.check("subdevice"))
    {
        yarp::os::Property p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if(!m_driver.open(p) || !m_driver.isValid())
        {
            yCError(ODOMETRY2D_NWS_YARP) << "failed to open subdevice.. check params";
            return false;
        }

        if(!attach(&m_driver))
        {
            yCError(ODOMETRY2D_NWS_YARP) << "failed to open subdevice.. check params";
            return false;
        }
    }

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
    if (m_odometry2D_interface!=nullptr)
    {
        yarp::os::Stamp timeStamp(static_cast<int>(m_stampCount++), yarp::os::Time::now());
        yarp::dev::OdometryData odometryData;
        m_odometry2D_interface->getOdometry(odometryData);
        if (m_port_odometry.getOutputCount()>0)
        {
            m_port_odometry.setEnvelope(timeStamp);
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
            m_port_odometer.setEnvelope(timeStamp);
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
            m_port_velocity.setEnvelope(timeStamp);
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
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    yCDebug(ODOMETRY2D_NWS_YARP) << "read() Command failed";
    return false;
}