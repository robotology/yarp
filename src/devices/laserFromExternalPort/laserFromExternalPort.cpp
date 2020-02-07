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

InputPortProcessor::InputPortProcessor()
{
}

void InputPortProcessor::onRead(yarp::dev::LaserScan2D& b)
{
    mutex.lock();
        lastScan = b;
        getEnvelope(lastStamp);
    mutex.unlock();
}

inline void InputPortProcessor::getLast(yarp::dev::LaserScan2D& data, Stamp& stmp)
{
    mutex.lock();
        data = lastScan;
        stmp = lastStamp;
    mutex.unlock();
}

//-------------------------------------------------------------------------------------

bool LaserFromExternalPort::open(yarp::os::Searchable& config)
{
    Property subConfig;
    m_info = "LaserFromExternalPort device";
    m_device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    m_min_distance = 0.1; //m
    m_max_distance = 2.5;  //m
    bool br = config.check("SUBDEVICE");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("SUBDEVICE");
        m_clip_max_enable = general_config.check("clip_max");
        m_clip_min_enable = general_config.check("clip_min");
        if (m_clip_max_enable) { m_max_distance = general_config.find("clip_max").asFloat64(); }
        if (m_clip_min_enable) { m_min_distance = general_config.find("clip_min").asFloat64(); }
        m_do_not_clip_infinity_enable = (general_config.find("allow_infinity").asInt32()!=0);
    }
    else
    {
        yError() << "Missing SUBDEVICE section";
        return false;
    }
    bool bs = config.check("SKIP");
    if (bs != false)
    {
        yarp::os::Searchable& skip_config = config.findGroup("SKIP");
        Bottle mins = skip_config.findGroup("min");
        Bottle maxs = skip_config.findGroup("max");
        size_t s_mins = mins.size();
        size_t s_maxs = mins.size();
        if (s_mins == s_maxs && s_maxs > 1 )
        {
            for (size_t s = 1; s < s_maxs; s++)
            {
                Range_t range;
                range.max = maxs.get(s).asFloat64();
                range.min = mins.get(s).asFloat64();
                if (range.max >= 0 && range.max <= 360 &&
                    range.min >= 0 && range.min <= 360 &&
                    range.max > range.min)
                {
                    m_range_skip_vector.push_back(range);
                }
                else
                {
                    yError() << "Invalid range in SKIP section";
                    return false;
                }
            }
        }

    }

    m_input_port.open("/iiiii");

    PeriodicThread::start();

    yInfo("Sensor ready");
    return true;
}

bool LaserFromExternalPort::close()
{
    PeriodicThread::stop();

    m_input_port.close();

    yInfo() << "LaserFromExternalPort closed";
    return true;
}

bool LaserFromExternalPort::getDistanceRange(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(mutex);
    min = m_min_distance;
    max = m_max_distance;
    return true;
}

bool LaserFromExternalPort::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool LaserFromExternalPort::getScanLimits(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(mutex);
    min = m_min_angle;
    max = m_max_angle;
    return true;
}

bool LaserFromExternalPort::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setScanLimits not yet implemented");
    return true;
}

bool LaserFromExternalPort::getHorizontalResolution(double& step)
{
    std::lock_guard<std::mutex> guard(mutex);
    step = m_resolution;
    return true;
}

bool LaserFromExternalPort::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setHorizontalResolution not yet implemented");
    return true;
}

bool LaserFromExternalPort::getScanRate(double& rate)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("getScanRate not yet implemented");
    return true;
}

bool LaserFromExternalPort::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setScanRate not yet implemented");
    return false;
}


bool LaserFromExternalPort::getRawData(yarp::sig::Vector &out)
{
    std::lock_guard<std::mutex> guard(mutex);
    out = m_laser_data;
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool LaserFromExternalPort::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    std::lock_guard<std::mutex> guard(mutex);
#ifdef LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = m_laser_data.size();
    data.resize(size);
    if (m_max_angle < m_min_angle) { yError() << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = m_max_angle - m_min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + m_min_angle)* DEG2RAD;
        data[i].set_polar(m_laser_data[i], angle);
    }
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}
bool LaserFromExternalPort::getDeviceStatus(Device_status &status)
{
    std::lock_guard<std::mutex> guard(mutex);
    status = m_device_status;
    return true;
}

bool LaserFromExternalPort::threadInit()
{
#ifdef LASER_DEBUG
    yDebug("LaserFromExternalPort:: thread initialising...\n");
    yDebug("... done!\n");
#endif

    return true;
}

void LaserFromExternalPort::run()
{
#ifdef DEBUG_TIMING
    double t1 = yarp::os::Time::now();
#endif
    std::lock_guard<std::mutex> guard(mutex);

    m_input_port.getLast(m_last_scan_data, m_last_stamp);

    double angle, distance;
    size_t i;

    const double infinity   = std::numeric_limits<double>::infinity();

    for (int elem = 0; elem < m_sensorsNum; elem++)
    {
        angle    = elem * m_resolution;    //deg
        distance = m_last_scan_data.scans[elem]; //m

        if (m_clip_min_enable && distance < m_min_distance)
        {
            distance = m_max_distance;
        }

        if (m_clip_max_enable              &&
            distance > m_max_distance      &&
            !m_do_not_clip_infinity_enable &&
            distance <= infinity)
        {
            distance = m_max_distance;
        }

        for (i = 0; i < m_range_skip_vector.size(); i++)
        {
            if (angle > m_range_skip_vector[i].min && angle < m_range_skip_vector[i].max)
            {
                distance = infinity;
            }
        }

        m_laser_data[elem] = distance;
    }

    return;
}

void LaserFromExternalPort::threadRelease()
{
#ifdef LASER_DEBUG
    yDebug("LaserFromExternalPort Thread releasing...");
    yDebug("... done.");
#endif

    return;
}

bool LaserFromExternalPort::getDeviceInfo(std::string &device_info)
{
    std::lock_guard<std::mutex> guard(mutex);
    device_info = m_info;
    return true;
}
