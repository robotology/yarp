/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "laserFromDepth.h"

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

YARP_LOG_COMPONENT(LASER_FROM_DEPTH, "yarp.devices.laserFromDepth")

//-------------------------------------------------------------------------------------

bool LaserFromDepth::open(yarp::os::Searchable& config)
{
    Property subConfig;
    m_info = "LaserFromDepth device";
    m_device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_DEPTH) << "%s\n", config.toString().c_str();
#endif

    m_min_distance = 0.1; //m
    m_max_distance = 2.5;  //m

    if (this->parseConfiguration(config) == false)
    {
        yCError(LASER_FROM_DEPTH) << "error parsing parameters";
        return false;
    }

    Property prop;
    if(!config.check("RGBD_SENSOR_CLIENT"))
    {
        yCError(LASER_FROM_DEPTH) << "missing RGBD_SENSOR_CLIENT section in configuration file!";
        return false;
    }
    prop.fromString(config.findGroup("RGBD_SENSOR_CLIENT").toString());
    prop.put("device", "RGBDSensorClient");

    driver.open(prop);
    if (!driver.isValid())
    {
        yCError(LASER_FROM_DEPTH) << "Error opening PolyDriver check parameters";
        return false;
    }
    driver.view(iRGBD);
    if (!iRGBD)
    {
        yCError(LASER_FROM_DEPTH) << "Error opening iRGBD interface. Device not available";
        return false;
    }

    m_depth_width = iRGBD->getDepthWidth();
    m_depth_height = iRGBD->getDepthHeight();
    double hfov = 0;
    double vfov = 0;
    iRGBD->getDepthFOV(hfov, vfov);
    m_sensorsNum = m_depth_width;
    m_resolution = hfov / m_depth_width;
    m_laser_data.resize(m_sensorsNum, 0.0);
    m_max_angle = +hfov / 2;
    m_min_angle = -hfov / 2;
    PeriodicThread::start();

    yCInfo(LASER_FROM_DEPTH) << "Sensor ready";
    return true;
}

bool LaserFromDepth::close()
{
    PeriodicThread::stop();

    if (driver.isValid()) {
        driver.close();
    }

    yCInfo(LASER_FROM_DEPTH) << "closed";
    return true;
}

bool LaserFromDepth::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool LaserFromDepth::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_DEPTH) << "setScanLimits not yet implemented";
    return true;
}

bool LaserFromDepth::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_DEPTH) << "setHorizontalResolution not yet implemented";
    return true;
}

bool LaserFromDepth::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_DEPTH) << "setScanRate not yet implemented";
    return false;
}

bool LaserFromDepth::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_DEPTH) << "thread initialising...\n";
    yCDebug(LASER_FROM_DEPTH) << "... done!\n";
#endif

    return true;
}

bool LaserFromDepth::acquireDataFromHW()
{
#ifdef DEBUG_TIMING
    double t1 = yarp::os::Time::now();
#endif

    iRGBD->getDepthImage(m_depth_image);
    if (m_depth_image.getRawImage() == nullptr)
    {
        yCError(LASER_FROM_DEPTH) << "invalid image received";
        return false;
    }

    if (m_depth_image.width() != m_depth_width ||
        m_depth_image.height() != m_depth_height)
    {
        yCError(LASER_FROM_DEPTH) << "invalid image size";
        return false;
    }


    auto* pointer = (float*)m_depth_image.getPixelAddress(0, m_depth_height / 2);
    double angle, distance, angleShift;

    angleShift = m_sensorsNum * m_resolution / 2;

    for (size_t elem = 0; elem < m_sensorsNum; elem++)
    {
        angle = elem * m_resolution;    //deg
        //the 1 / cos(blabla) distortion simulate the way RGBD devices calculate the distance..
        distance = *(pointer + elem);
        distance /= cos((angle - angleShift) * DEG2RAD); //m
        m_laser_data[m_sensorsNum - 1 - elem] = distance;
    }

    return true;
}

void LaserFromDepth::run()
{
    m_mutex.lock();
    updateLidarData();
    m_mutex.unlock();
    return;
}

void LaserFromDepth::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_DEPTH)<<"LaserFromDepth Thread releasing...";
    yCDebug(LASER_FROM_DEPTH) <<"... done.";
#endif

    return;
}
