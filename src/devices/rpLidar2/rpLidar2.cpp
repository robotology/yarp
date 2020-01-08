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

#include "rpLidar2.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <mutex>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

//#define LASER_DEBUG
//#define FORCE_SCAN

using namespace std;
using namespace rp::standalone::rplidar;


//-------------------------------------------------------------------------------------

bool RpLidar2::open(yarp::os::Searchable& config)
{
    string   serial;
    int      baudrate;
    u_result result;

    m_device_status = DEVICE_OK_STANBY;
    m_min_distance  = 0.1; //m
    m_max_distance  = 2.5;  //m
    m_pwm_val       = 0;
    bool br       = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        m_clip_max_enable = general_config.check("clip_max");
        m_clip_min_enable = general_config.check("clip_min");
        if (m_clip_max_enable)                                    { m_max_distance = general_config.find("clip_max").asFloat64(); }
        if (m_clip_min_enable)                                    { m_min_distance = general_config.find("clip_min").asFloat64(); }
        if (general_config.check("max_angle")   == false)         { yError()  << "Missing max_angle param in GENERAL group"; return false;    }
        if (general_config.check("min_angle")   == false)         { yError()  << "Missing min_angle param in GENERAL group"; return false;    }
        if (general_config.check("resolution")  == false)         { yError()  << "Missing resolution param in GENERAL group"; return false;  }
        if (general_config.check("serial_port") == false)         { yError()  << "Missing serial_port param in GENERAL group"; return false; }
        if (general_config.check("serial_baudrate") == false)     { yError()  << "Missing serial_baudrate param in GENERAL group"; return false; }
        if (general_config.check("sample_buffer_life") == false)  { yError()  << "Missing sample_buffer_life param in GENERAL group"; return false; }

        baudrate    = general_config.find("serial_baudrate").asInt32();
        m_serialPort  = general_config.find("serial_port").asString();
        m_max_angle   = general_config.find("max_angle").asFloat64();
        m_min_angle   = general_config.find("min_angle").asFloat64();
        m_resolution  = general_config.find("resolution").asFloat64();
        m_buffer_life = general_config.find("sample_buffer_life").asInt32();
        m_do_not_clip_infinity_enable = (general_config.find("allow_infinity").asInt32()!=0);
        if (general_config.check("motor_pwm"))
        {
            m_pwm_val     = general_config.find("motor_pwm").asInt32();
        }
        if (general_config.check("thread_period"))
        {
            double thread_period = general_config.find("thread_period").asInt32() / 1000.0;
            this->setPeriod(thread_period);
        }
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

    if (m_max_angle <= m_min_angle)            { yError() << "max_angle should be > min_angle";  return false; }
    double fov = (m_max_angle - m_min_angle);
    if (fov >360)                          { yError() << "max_angle - min_angle <= 360";  return false; }
    m_sensorsNum = (int)(fov/m_resolution);
    m_laser_data.resize(m_sensorsNum, 0.0);

    m_drv = RPlidarDriver::CreateDriver(rp::standalone::rplidar::DRIVER_TYPE_SERIALPORT);
    if (!m_drv)
    {
            yError() << "Create Driver fail, exit\n";
            return false;
    }

    if (IS_FAIL(m_drv->connect(m_serialPort.c_str(), (_u32)baudrate)))
    {
        yError() << "Error, cannot bind to the specified serial port:", m_serialPort.c_str();
        RPlidarDriver::DisposeDriver(m_drv);
        return false;
    }

    m_info = deviceinfo();
    yInfo("max_dist %f, min_dist %f",   m_max_distance, m_min_distance);

    bool m_inExpressMode=false;
    result = m_drv->checkExpressScanSupported(m_inExpressMode);
    if (result == RESULT_OK && m_inExpressMode==true)
    {
        yInfo() << "Express scan mode is supported";
    }
    else
    {
        yWarning() << "Device does not supports express scan mode";
    }

    result = m_drv->startMotor();
    if (result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yInfo() << "Motor started successfully";

    if (m_pwm_val!=0)
    {
        if (m_pwm_val>0 && m_pwm_val<1023)
        {
            result = m_drv->setMotorPWM(m_pwm_val);
            if (result != RESULT_OK)
            {
                handleError(result);
                return false;
            }
            yInfo() << "Motor pwm set to "<< m_pwm_val;
        }
        else
        {
            yError() << "Invalid motor pwm request " << m_pwm_val <<". It should be a value between 0 and 1023.";
            return false;
        }
    }
    else
    {
        yInfo() << "Motor pwm set to default value (600?)";
    }

    bool forceScan =false;
    result = m_drv->startScan(forceScan,m_inExpressMode);

    if (result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yInfo() << "Scan started successfully";

    yInfo() << "Device info:" << m_info;
    yInfo("max_angle %f, min_angle %f", m_max_angle, m_min_angle);
    yInfo("resolution %f",              m_resolution);
    yInfo("sensors %d",                 m_sensorsNum);
    PeriodicThread::start();
    return true;
}

bool RpLidar2::close()
{
    m_drv->stopMotor();
    RPlidarDriver::DisposeDriver(m_drv);
    PeriodicThread::stop();
    yInfo() << "rpLidar closed";
    return true;
}

bool RpLidar2::getDistanceRange(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    min = m_min_distance;
    max = m_max_distance;
    return true;
}

bool RpLidar2::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool RpLidar2::getScanLimits(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    min = m_min_angle;
    max = m_max_angle;
    return true;
}

bool RpLidar2::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_angle = min;
    m_max_angle = max;
    return true;
}

bool RpLidar2::getHorizontalResolution(double& step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    step = m_resolution;
    return true;
}

bool RpLidar2::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_resolution = step;
    return true;
}

bool RpLidar2::getScanRate(double& rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yWarning("getScanRate not yet implemented");
    return true;
}

bool RpLidar2::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yWarning("setScanRate not yet implemented");
    return false;
}


bool RpLidar2::getRawData(yarp::sig::Vector &out)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    out           = m_laser_data;
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool RpLidar2::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    std::lock_guard<std::mutex> guard(m_mutex);
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
bool RpLidar2::getDeviceStatus(Device_status &status)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    status = m_device_status;
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
    rplidar_response_measurement_node_t nodes[2048];
    size_t                              count = _countof(nodes);
    static int                          life = 0;
    op_result = m_drv->grabScanData(nodes, count);
    if (op_result != RESULT_OK)
    {
        yError() << m_serialPort << ": grabbing scan data failed";
        handleError(op_result);
        return;
    }

    float frequency=0;
    bool is4kmode=false;
    op_result = m_drv->getFrequency(m_inExpressMode, count, frequency, is4kmode);
    if (op_result != RESULT_OK)
    {
        yError() << "getFrequency failed";
    }

    m_drv->ascendScanData(nodes, count);

    if (op_result != RESULT_OK)
    {
        yError() << "ascending scan data failed\n";
        handleError(op_result);
        return;
    }

    if (m_buffer_life && life%m_buffer_life == 0)
    {
        for (size_t i=0 ;i<m_laser_data.size(); i++)
        {
            //m_laser_data[i]=0; //0 is a terribly unsafe value and should be avoided.
            m_laser_data[i]=std::numeric_limits<double>::infinity();
        }
    }

    for (size_t i = 0; i < count; ++i)
    {

        double distance = nodes[i].distance_q2 / 4.0f / 1000.0; //m
        double angle    = (float)((nodes[i].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f); //deg
        double quality  = nodes[i].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;
        angle = (360 - angle);

        if (angle >= 360)
        {
            angle -= 360;
        }

        if (quality == 0 || distance == 0)
        {
            distance = std::numeric_limits<double>::infinity();
        }

        if (angle > 360)
        {
            yWarning() << "Invalid angle";
        }

        if (m_clip_min_enable)
        {
            if (distance < m_min_distance)
            {
                //laser angular measurements not read by the device are now set to infinity and not to zero
                distance = std::numeric_limits<double>::infinity();
            }
        }
        if (m_clip_max_enable)
        {
            if (distance > m_max_distance)
            {
                if (!m_do_not_clip_infinity_enable && distance <= std::numeric_limits<double>::infinity())
                {
                    distance = m_max_distance;
                }
            }
        }

        for (auto& i : m_range_skip_vector)
        {
            if (angle > i.min && angle < i.max)
            {
                distance = std::numeric_limits<double>::infinity();
            }
        }

        int elem = (int)(angle / m_resolution);

        if (elem >= 0 && elem < (int)m_laser_data.size())
        {
            m_laser_data[elem] = distance;
        }
        else
        {
            yDebug() << "RpLidar::run() invalid angle: elem" << elem << ">" << "laser_data.size()" << m_laser_data.size();
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
    switch (error)
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

std::string RpLidar2::deviceinfo()
{
    if (m_drv)
    {
        u_result                       result;
        rplidar_response_device_info_t info;
        string                         serialNumber;

        result = m_drv->getDeviceInfo(info);
        if (result != RESULT_OK)
        {
            handleError(result);
            return {};
        }

        for (unsigned char i : info.serialnum)
        {
            serialNumber += to_string(i);
        }

        return "Firmware Version: "   + to_string(info.firmware_version) +
               "\nHardware Version: " + to_string(info.hardware_version) +
               "\nModel: "            + to_string(info.model) +
               "\nSerial Number:"     + serialNumber;
    }
    return {};
}

bool RpLidar2::getDeviceInfo(std::string &device_info)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    device_info = m_info;
    return true;
}
