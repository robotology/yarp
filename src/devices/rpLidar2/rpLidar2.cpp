/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#include <rpLidar2.h>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <yarp/os/ResourceFinder.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <limits>

#define _USE_MATH_DEFINES
#include <math.h>

//#define LASER_DEBUG
//#define FORCE_SCAN

using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

//-------------------------------------------------------------------------------------

bool RpLidar2::open(yarp::os::Searchable& config)
{
    info = "Fake Laser device for test/debugging";
    device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    min_distance = 0.1; //m
    max_distance = 2.5;  //m

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        clip_max_enable = general_config.check("clip_max");
        clip_min_enable = general_config.check("clip_min");
        if (clip_max_enable) { max_distance = general_config.find("clip_max").asDouble(); }
        if (clip_min_enable) { min_distance = general_config.find("clip_min").asDouble(); }
        if (general_config.check("max_angle") == false) { yError() << "Missing max_angle param"; return false; }
        if (general_config.check("min_angle") == false) { yError() << "Missing min_angle param"; return false; }
        if (general_config.check("resolution") == false) { yError() << "Missing resolution param"; return false; }
        max_angle = general_config.find("max_angle").asDouble();
        min_angle = general_config.find("min_angle").asDouble();
        resolution = general_config.find("resolution").asDouble();
        do_not_clip_infinity_enable = (general_config.find("allow_infinity").asInt()!=0);
    }
    else
    {
        yError() << "Missing GENERAL section";
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
                range.max = maxs.get(s).asDouble();
                range.min = mins.get(s).asDouble();
                if (range.max >= 0 && range.max <= 360 &&
                    range.min >= 0 && range.min <= 360 &&
                    range.max > range.min)
                {
                    range_skip_vector.push_back(range);
                }
                else
                {
                    yError() << "Invalid range in SKIP section";
                    return false;
                }
            }
        }

    }

    if (max_angle <= min_angle)            { yError() << "max_angle should be > min_angle";  return false; }
    double fov = (max_angle - min_angle);
    if (fov >360)                          { yError() << "max_angle - min_angle <= 360";  return false; }
    sensorsNum = (int)(fov/resolution);
    laser_data.resize(sensorsNum,0.0);

    yInfo("Starting debug mode");
    yInfo("max_dist %f, min_dist %f", max_distance, min_distance);
    yInfo("max_angle %f, min_angle %f", max_angle, min_angle);
    yInfo("resolution %f", resolution);
    yInfo("sensors %d", sensorsNum);
    Time::turboBoost();

    yarp::os::Searchable& general_config = config.findGroup("GENERAL");
    bool ok = general_config.check("Serial_Configuration");
    if (!ok)
    {
        yError("Cannot find configuration file for serial port communication!");
        return false;
    }
    yarp::os::ConstString serial_filename = general_config.find("Serial_Configuration").asString();

    Property prop;
    ResourceFinder rf;
    rf.setVerbose();
    yarp::os::ConstString serial_completefilename= rf.findFileByName(serial_filename.c_str());

    prop.put("device", "serialport");
    ok = prop.fromConfigFile(serial_completefilename.c_str(), config, false);
    if (!ok)
    {
        yError("Unable to read from serial port configuration file");
        return false;
    }

    driver.open(prop);
    if (!driver.isValid())
    {
        yError("Error opening PolyDriver check parameters");
        return false;
    }

    RateThread::start();
    return true;
}

bool RpLidar2::close()
{
    RateThread::stop();

    if(driver.isValid())
        driver.close();

    yInfo() << "rpLidar closed";
    return true;
}

bool RpLidar2::getDistanceRange(double& min, double& max)
{
    LockGuard guard(mutex);
    min = min_distance;
    max = max_distance;
    return true;
}

bool RpLidar2::setDistanceRange(double min, double max)
{
    LockGuard guard(mutex);
    min_distance = min;
    max_distance = max;
    return true;
}

bool RpLidar2::getScanLimits(double& min, double& max)
{
    LockGuard guard(mutex);
    min = min_angle;
    max = max_angle;
    return true;
}

bool RpLidar2::setScanLimits(double min, double max)
{
    LockGuard guard(mutex);
    min_angle = min;
    max_angle = max;
    return true;
}

bool RpLidar2::getHorizontalResolution(double& step)
{
    LockGuard guard(mutex);
    step = resolution;
    return true;
}

bool RpLidar2::setHorizontalResolution(double step)
{
    LockGuard guard(mutex);
    resolution = step;
    return true;
}

bool RpLidar2::getScanRate(double& rate)
{
    LockGuard guard(mutex);
    yWarning("getScanRate not yet implemented");
    return true;
}

bool RpLidar2::setScanRate(double rate)
{
    LockGuard guard(mutex);
    yWarning("setScanRate not yet implemented");
    return false;
}


bool RpLidar2::getRawData(yarp::sig::Vector &out)
{
    LockGuard guard(mutex);
    out = laser_data;
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool RpLidar2::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    LockGuard guard(mutex);
#ifdef LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = laser_data.size();
    data.resize(size);
    if (max_angle < min_angle) { yError() << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = max_angle - min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + min_angle)* DEG2RAD;
        data[i].set_polar(laser_data[i], angle);
    }
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}
bool RpLidar2::getDeviceStatus(Device_status &status)
{
    LockGuard guard(mutex);
    status = device_status;
    return true;
}

bool RpLidar2::threadInit()
{
#ifdef LASER_DEBUG
    yDebug("RpLidar:: thread initialising...\n");
    yDebug("... done!\n");
#endif
    return true;
}

#define DEBUG_LOCKING 1

void RpLidar2::run()
{
    double distance = 0; //m
    double angle = 0; //deg
    double quality = 0;
    angle = (360 - angle) + 90;
    if (angle >= 360) angle -= 360;

    if (quality == 0)
    {
        //      yWarning() << "Quality Low" << i / 5;
        distance = std::numeric_limits<double>::infinity();
    }
    if (angle > 360)
    {
        yWarning() << "Invalid angle";
    }

    if (clip_min_enable)
    {
        if (distance < min_distance)
            distance = max_distance;
    }
    if (clip_max_enable)
    {
        if (distance > max_distance)
        {
            if (!do_not_clip_infinity_enable && distance <= std::numeric_limits<double>::infinity())
            {
                distance = max_distance;
            }
        }
    }

    for (size_t i = 0; i < range_skip_vector.size(); i++)
    {
        if (angle>range_skip_vector[i].min && angle < range_skip_vector[i].max)
        {
            distance = std::numeric_limits<double>::infinity();
        }
    }

    //int m_elem = (int)((max_angle - min_angle) / resolution);
    int elem = (int)(angle / resolution);
    if (elem >= 0 && elem < (int)laser_data.size())
    {
        laser_data[elem] = distance;
    }
    else
    {
        yDebug() << "RpLidar::run() invalid angle: elem" << elem << ">" << "laser_data.size()" << laser_data.size();
    }

#ifdef DEBUG_TIMING
    double t2 = yarp::os::Time::now();
    yDebug( "Time %f",  (t2 - t1) * 1000.0);
#endif
    return;
}

void RpLidar2::threadRelease()
{
#ifdef LASER_DEBUG
    yDebug("RpLidar Thread releasing...");
    yDebug("... done.");
#endif

    return;
}

bool RpLidar2::getDeviceInfo(yarp::os::ConstString &device_info)
{
    LockGuard guard(mutex);
    device_info = info;
    return true;
}
