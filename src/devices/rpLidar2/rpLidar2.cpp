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
using namespace rp::standalone::rplidar;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

//-------------------------------------------------------------------------------------

bool RpLidar2::open(yarp::os::Searchable& config)
{
    string   serial;
    int      baudrate;
    u_result result;

    device_status = DEVICE_OK_STANBY;
    min_distance  = 0.1; //m
    max_distance  = 2.5;  //m
    bool br       = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        clip_max_enable = general_config.check("clip_max");
        clip_min_enable = general_config.check("clip_min");
        if (clip_max_enable)                                      { max_distance = general_config.find("clip_max").asDouble(); }
        if (clip_min_enable)                                      { min_distance = general_config.find("clip_min").asDouble(); }
        if (general_config.check("max_angle")   == false)         { yError()  << "Missing max_angle param in GENERAL group"; return false;    }
        if (general_config.check("min_angle")   == false)         { yError()  << "Missing min_angle param in GENERAL group"; return false;    }
        if (general_config.check("resolution")  == false)         { yError()  << "Missing resolution param in GENERAL group"; return false;  }
        if (general_config.check("serial_port") == false)         { yError()  << "Missing serial_port param in GENERAL group"; return false; }
        if (general_config.check("serial_baudrate") == false)     { yError()  << "Missing serial_baudrate param in GENERAL group"; return false; }
        if (general_config.check("sample_buffer_life") == false)  { yError()  << "Missing sample_buffer_life param in GENERAL group"; return false; }

        baudrate    = general_config.find("serial_baudrate").asInt();
        serial      = general_config.find("serial_port").asString();
        max_angle   = general_config.find("max_angle").asDouble();
        min_angle   = general_config.find("min_angle").asDouble();
        resolution  = general_config.find("resolution").asDouble();
        buffer_life = general_config.find("sample_buffer_life").asInt();

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

        Bottle mins   = skip_config.findGroup("min");
        Bottle maxs   = skip_config.findGroup("max");
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

    drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);
    if (!drv)
    {
            yError() << "Create Driver fail, exit\n";
            return false;
    }



    if(IS_FAIL(drv->connect(serial.c_str(), (_u32)baudrate)))
    {
        yError() << "Error, cannot bind to the specified serial port:", serial.c_str();
        RPlidarDriver::DisposeDriver(drv);
        return false;
    }

    info = deviceinfo();
    yInfo("max_dist %f, min_dist %f",   max_distance, min_distance);

    result = drv->startMotor();
    if(result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yInfo() << "Motor started succesfully";
    result = drv->startScan();
    if(result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yInfo() << "Scan started succesfully";

    yInfo() << "Device info:" << info;
    yInfo("max_angle %f, min_angle %f", max_angle, min_angle);
    yInfo("resolution %f",              resolution);
    yInfo("sensors %d",                 sensorsNum);
    Time::turboBoost();
    RateThread::start();
    return true;
}

bool RpLidar2::close()
{
    drv->stopMotor();
    RPlidarDriver::DisposeDriver(drv);
    RateThread::stop();
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
    out           = laser_data;
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool RpLidar2::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    LockGuard guard(mutex);
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

    return true;
}

//#define DEBUG_LOCKING 1
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof (_Array[0]))
#endif

void RpLidar2::run()
{
    u_result                            op_result;
    rplidar_response_measurement_node_t nodes[360*2];
    size_t                              count = _countof(nodes);
    static int                          life = 0;
    op_result = drv->grabScanData(nodes, count);
    if (op_result != RESULT_OK)
    {
        yError() << "grabbing scan data failed";
        handleError(op_result);
        return;
    }
    drv->ascendScanData(nodes, count);
    if (op_result != RESULT_OK)
    {
        yError() << "ascending scan data failed\n";
        handleError(op_result);
        return;
    }

    if(buffer_life && life%buffer_life == 0)
    {
        laser_data.zero();
    }

    for(size_t i = 0; i < count; ++i)
    {

        double distance = nodes[i].distance_q2 / 4.0f / 1000.0; //m
        double angle    = (float)((nodes[i].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f); //deg
        double quality  = nodes[i].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;
        angle = (360 - angle);

        if (angle >= 360) angle -= 360;

        if (quality == 0)
        {
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

        int elem = (int)(angle / resolution);

        if (elem >= 0 && elem < (int)laser_data.size())
        {
            laser_data[elem] = distance;
        }
        else
        {
            yDebug() << "RpLidar::run() invalid angle: elem" << elem << ">" << "laser_data.size()" << laser_data.size();
        }
    }

    life++;
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

void RpLidar2::handleError(u_result error)
{
    switch(error)
    {
    case RESULT_FAIL_BIT:
        yError() << "error: 'FAIL BIT'";
        break;
    case RESULT_ALREADY_DONE:
        yError() << "error: 'ALREADY DONE'";
        break;
    case RESULT_INVALID_DATA:
        yError() << "error: 'INVALID DATA'";
        break;
    case RESULT_OPERATION_FAIL:
        yError() << "error: 'OPERATION FAIL'";
        break;
    case RESULT_OPERATION_TIMEOUT:
        yError() << "error: 'OPERATION TIMEOUT'";
        break;
    case RESULT_OPERATION_STOP:
        yError() << "error: 'OPERATION STOP'";
        break;
    case RESULT_OPERATION_NOT_SUPPORT:
        yError() << "error: 'OPERATION NOT SUPPORT'";
        break;
    case RESULT_FORMAT_NOT_SUPPORT:
        yError() << "error: 'FORMAT NOT SUPPORT'";
        break;
    case RESULT_INSUFFICIENT_MEMORY:
        yError() << "error: 'INSUFFICENT MEMORY'";
        break;
    }
}

ConstString RpLidar2::deviceinfo()
{
    if(drv)
    {
        u_result                       result;
        rplidar_response_device_info_t info;
        string                         serialNumber;

        result = drv->getDeviceInfo(info);
        if(result != RESULT_OK)
        {
            handleError(result);
            return "";
        }

        for(int i = 0; i < 16; ++i)
        {
            serialNumber += to_string(info.serialnum[i]);
        }

        return
                "Firmware Version: "   + to_string(info.firmware_version) +
                "\nHardware Version: " + to_string(info.hardware_version) +
                "\nModel: "            + to_string(info.model) +
                "\nSerial Number:"     + serialNumber;
    }
    return "";
}

bool RpLidar2::getDeviceInfo(yarp::os::ConstString &device_info)
{
    LockGuard guard(mutex);
    device_info = info;
    return true;
}
