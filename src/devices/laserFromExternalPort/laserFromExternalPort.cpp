/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#define _USE_MATH_DEFINES

#include "laserFromExternalPort.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <mutex>

using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

YARP_LOG_COMPONENT(LASER_FROM_EXTERNAL_PORT, "yarp.devices.laserFromExternalPort")

InputPortProcessor::InputPortProcessor()
{
}

void InputPortProcessor::onRead(yarp::dev::LaserScan2D& b)
{
    m_port_mutex.lock();
        m_lastScan = b;
        getEnvelope(m_lastStamp);
    m_port_mutex.unlock();
}

inline void InputPortProcessor::getLast(yarp::dev::LaserScan2D& data, Stamp& stmp)
{
    m_port_mutex.lock();
        data = m_lastScan;
        stmp = m_lastStamp;
    m_port_mutex.unlock();
}

//-------------------------------------------------------------------------------------

bool LaserFromExternalPort::open(yarp::os::Searchable& config)
{
    Property subConfig;
    m_info = "LaserFromExternalPort device";
    m_port_name = "/laserFromExternalPort:i";

#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_EXTERNAL_PORT) << "%s\n", config.toString().c_str());
#endif

    if (this->parseConfiguration(config) == false)
    {
        yCError(LASER_FROM_EXTERNAL_PORT) << "Error parsing parameters";
        return false;
    }

    yarp::os::Searchable& general_config = config.findGroup("SENSOR");
    if (general_config.check("input_port_name"))
       {m_port_name = general_config.find("input_port_name").asString();} //this parameter is optional

    m_input_port.useCallback();
    m_input_port.open(m_port_name);

    PeriodicThread::start();

    yInfo("LaserFromExternalPort: Sensor ready");
    return true;
}

bool LaserFromExternalPort::close()
{
    PeriodicThread::stop();

    m_input_port.close();

    yInfo() << "LaserFromExternalPort closed";
    return true;
}



bool LaserFromExternalPort::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool LaserFromExternalPort::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_EXTERNAL_PORT) << "setScanLimits not yet implemented";
    return true;
}



bool LaserFromExternalPort::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_EXTERNAL_PORT, "setHorizontalResolution not yet implemented");
    return true;
}

bool LaserFromExternalPort::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_EXTERNAL_PORT, "setScanRate not yet implemented");
    return false;
}



bool LaserFromExternalPort::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_EXTERNAL_PORT) <<"LaserFromExternalPort:: thread initialising...\n");
    yCDebug(LASER_FROM_EXTERNAL_PORT) <<"... done!\n");
#endif

    return true;
}

void LaserFromExternalPort::run()
{
#ifdef DEBUG_TIMING
    double t1 = yarp::os::Time::now();
#endif
    std::lock_guard<std::mutex> guard(m_mutex);

    m_input_port.getLast(m_last_scan_data, m_last_stamp);
    size_t received_scans = m_last_scan_data.scans.size();

    if (1)
    {
       //this ovverrides user setting with parameters received from the port
       m_sensorsNum = received_scans;
       m_max_angle = m_last_scan_data.angle_max;
       m_min_angle = m_last_scan_data.angle_min;
       m_max_distance = m_last_scan_data.range_max;
       m_min_distance = m_last_scan_data.range_min;
       m_resolution   = received_scans/(m_max_angle - m_min_angle);
       if (m_laser_data.size() != m_sensorsNum) m_laser_data.resize(m_sensorsNum);
    }

    for (size_t elem = 0; elem < m_sensorsNum; elem++)
    {
        m_laser_data[elem] = m_last_scan_data.scans[elem]; //m
    }
    applyLimitsOnLaserData();

    return;
}

void LaserFromExternalPort::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_EXTERNAL_PORT) <<"Thread releasing...");
    yCDebug(LASER_FROM_EXTERNAL_PORT) <<"... done.");
#endif

    return;
}
