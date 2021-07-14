/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

YARP_LOG_COMPONENT(LASER_BASE, "yarp.devices.Lidar2DDeviceBase")

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
    if (m_max_angle < m_min_angle) { yCError(LASER_BASE) << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = m_max_angle - m_min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size) * laser_angle_of_view + m_min_angle) * DEG2RAD;
        data[i].set_polar(m_laser_data[i], angle);
    }
    return true;
}

Lidar2DDeviceBase::Lidar2DDeviceBase() :
    m_device_status(yarp::dev::IRangefinder2D::Device_status::DEVICE_GENERAL_ERROR),
    m_scan_rate(0.0),
    m_sensorsNum(0),
    m_min_angle(0.0),
    m_max_angle(0.0),
    m_min_distance(0.1),
    m_max_distance(30.0),
    m_resolution(0.0),
    m_clip_max_enable(false),
    m_clip_min_enable(false),
    m_do_not_clip_and_allow_infinity_enable(true)
{}

bool Lidar2DDeviceBase::parseConfiguration(yarp::os::Searchable& config)
{
    //sensor options (should be mandatory? TBD)
    {
        bool br = config.check("SENSOR");
        if (br != false)
        {
            yarp::os::Searchable& general_config = config.findGroup("SENSOR");
            if (general_config.check("max_angle")) { m_max_angle = general_config.find("max_angle").asFloat64(); }
            if (general_config.check("min_angle")) { m_min_angle = general_config.find("min_angle").asFloat64(); }
            if (general_config.check("resolution")) { m_resolution = general_config.find("resolution").asFloat64(); }
            m_clip_max_enable = general_config.check("max_distance");
            m_clip_min_enable = general_config.check("min_distance");
            if (m_clip_max_enable) { m_max_distance = general_config.find("max_distance").asFloat64(); }
            if (m_clip_min_enable) { m_min_distance = general_config.find("min_distance").asFloat64(); }
            m_do_not_clip_and_allow_infinity_enable = (general_config.find("allow_infinity").asInt32() == 1);
        }
        else
        {
            //yCError(LASER_BASE) << "Missing SENSOR section";
            //return false;
        }
    }

    //skip options (optional)
    {
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
                        yCError(LASER_BASE) << "Invalid range in SKIP section:"<< range.min << range.max;
                        return false;
                    }
                }
            }
            //yCDebug(LASER_BASE) << "Skip section set successfully";
        }
        else
        {
            //yCDebug(LASER_BASE) << "No Skip section required";
        }
    }

    //checks and allocation
    if (m_max_distance - m_min_distance <= 0)
    {
        yCError(LASER_BASE) << "invalid parameters: max_distance/min_distance";
        return false;
    }
    double fov = (m_max_angle - m_min_angle);
    if (fov <= 0)
    {
        yCError(LASER_BASE) << "invalid parameters: max_angle should be > min_angle";
        return false;
    }
    if (fov > 360)
    {
        yCError(LASER_BASE) << "invalid parameters: max_angle - min_angle <= 360";
        return false;
    }
    if (m_resolution <= 0)
    {
        yCError(LASER_BASE) << "invalid parameters resolution";
        return false;
    }
    m_sensorsNum = (int)(fov / m_resolution);
    m_laser_data.resize(m_sensorsNum, 0.0);

    yCInfo(LASER_BASE) << "Using the following parameters:";
    yCInfo(LASER_BASE) << "max_dist:" << m_max_distance << " min_dist:" << m_min_distance;
    yCInfo(LASER_BASE) << "max_angle:" << m_max_angle << " min_angle:" << m_min_angle;
    yCInfo(LASER_BASE) << "resolution:" << m_resolution;
    yCInfo(LASER_BASE) << "sensors:" << m_sensorsNum;
    yCInfo(LASER_BASE) << "allow_infinity:" << (m_do_not_clip_and_allow_infinity_enable ==true);
    if (m_range_skip_vector.size() >0)
    {
        for (size_t i = 0; i < m_range_skip_vector.size(); i++) {
            yCInfo(LASER_BASE) << "skip area:" << m_range_skip_vector[i].min << "->" << m_range_skip_vector[i].max;
        }
    }
    return true;
}

//this function checks if the angle is inside the allowed limits
//if not, distance value is set to NaN
bool Lidar2DDeviceBase::checkSkipAngle(const double& angle, double& distance)
{
    for (auto& it_skip : m_range_skip_vector)
    {
        if (angle > it_skip.min&& angle < it_skip.max)
        {
            distance = std::nan("");
            return true;
        }
    }
    return false;
}

bool Lidar2DDeviceBase::applyLimitsOnLaserData()
{
    for (size_t i = 0; i < m_sensorsNum; i++)
    {
        double& distance = m_laser_data[i];
        double  angle = i * m_resolution;

        if (std::isnan(distance)) {
            continue;
        }
        if (checkSkipAngle(angle, distance)) {
            continue;
        }

        if (m_clip_min_enable)
        {
            if (distance < m_min_distance)
            {
                distance = std::numeric_limits<double>::infinity();
                //the following means: if we want to clip infinity...
                if (m_do_not_clip_and_allow_infinity_enable == false)
                {
                    distance = m_min_distance;
                }
            }
        }
        if (m_clip_max_enable)
        {
            if (distance > m_max_distance)
            {
                distance = std::numeric_limits<double>::infinity();
                //the following means: if we want to clip infinity...
                if (m_do_not_clip_and_allow_infinity_enable ==false)
                {
                    distance = m_max_distance;
                }
            }
        }
    }
    return true;
}

yarp::os::Stamp Lidar2DDeviceBase::getLastInputStamp()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_timestamp;
}


bool Lidar2DDeviceBase::updateLidarData()
{
    bool b = true;
    b &= acquireDataFromHW();
    if (!b) {
        return false;
    }
    b &= applyLimitsOnLaserData();
    if (!b) {
        return false;
    }
    b &= updateTimestamp();
    return b;
}

bool Lidar2DDeviceBase::updateTimestamp()
{
    m_timestamp.update();
    return true;
}
