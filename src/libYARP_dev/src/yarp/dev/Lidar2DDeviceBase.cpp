/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define _USE_MATH_DEFINES

#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <limits>
#include <cmath>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

bool Lidar2DDeviceBase::getScanLimits(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    min = m_min_angle;
    max = m_max_angle;
    return true;
}

bool Lidar2DDeviceBase::getDistanceRange(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    min = m_min_distance;
    max = m_max_distance;
    return true;
}

bool Lidar2DDeviceBase::getHorizontalResolution(double& step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    step = m_resolution;
    return true;
}

bool Lidar2DDeviceBase::getDeviceStatus(Device_status& status)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    status = m_device_status;
    return true;
}

bool Lidar2DDeviceBase::getRawData(yarp::sig::Vector& out)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    out = m_laser_data;
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool Lidar2DDeviceBase::getScanRate(double& rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    rate = m_scan_rate;
    return true;
}

bool Lidar2DDeviceBase::getDeviceInfo(std::string& device_info)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    device_info = m_info;
    return true;
}

bool Lidar2DDeviceBase::getLaserMeasurement(std::vector<LaserMeasurementData>& data)
{
    std::lock_guard<std::mutex> guard(m_mutex);
#ifdef LASER_DEBUG
    //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = m_laser_data.size();
    data.resize(size);
    if (m_max_angle < m_min_angle) { yError() << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = m_max_angle - m_min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size) * laser_angle_of_view + m_min_angle) * DEG2RAD;
        data[i].set_polar(m_laser_data[i], angle);
    }
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

Lidar2DDeviceBase::Lidar2DDeviceBase() :
    m_device_status(yarp::dev::IRangefinder2D::Device_status::DEVICE_OK_STANBY),
    m_scan_rate(0.0),
    m_sensorsNum(0),
    m_min_angle(0.0),
    m_max_angle(0.0),
    m_min_distance(0.1),
    m_max_distance(30.0),
    m_resolution(0.0),
    m_clip_max_enable(false),
    m_clip_min_enable(false),
    m_do_not_clip_infinity_enable(false)
{}

bool Lidar2DDeviceBase::parse(yarp::os::Searchable& config)
{
    bool br = config.check("SENSOR");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("SENSOR");
        m_clip_max_enable = general_config.check("clip_max");
        m_clip_min_enable = general_config.check("clip_min");
        if (m_clip_max_enable) { m_max_distance = general_config.find("clip_max").asFloat64(); }
        if (m_clip_min_enable) { m_min_distance = general_config.find("clip_min").asFloat64(); }
        m_do_not_clip_infinity_enable = (general_config.find("allow_infinity").asInt32() != 0);
    }
    else
    {
        yError() << "Missing SENSOR section";
        return false;
    }
    bool bs = config.check("SKIP");
    if (bs == true)
    {
        yarp::os::Searchable& skip_config = config.findGroup("SKIP");
        Bottle mins = skip_config.findGroup("min");
        Bottle maxs = skip_config.findGroup("max");
        size_t s_mins = mins.size();
        size_t s_maxs = mins.size();
        if (s_mins == s_maxs && s_maxs > 1)
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
        //yDebug() << "Skip section set sucesfully";
    }
    else
    {
        //yDebug() << "No Skip section required";
    }
    return true;
}