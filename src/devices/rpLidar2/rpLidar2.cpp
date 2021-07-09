/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

YARP_LOG_COMPONENT(RP2_LIDAR, "yarp.devices.rpLidar2")

//-------------------------------------------------------------------------------------

bool RpLidar2::open(yarp::os::Searchable& config)
{
    string   serial;
    int      baudrate;
    u_result result;

    m_device_status = DEVICE_OK_STANBY;
    m_min_distance  = 0.1; //m
    m_max_distance  = 5;  //m
    m_pwm_val       = 0;

    //parse all the parameters related to the linear/angular range of the sensor
    if (this->parseConfiguration(config) == false)
    {
        yCError(RP2_LIDAR) << ": error parsing parameters";
        return false;
    }

    bool br       = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("serial_port") == false)         { yCError(RP2_LIDAR)  << "Missing serial_port param in GENERAL group"; return false; }
        if (general_config.check("serial_baudrate") == false)     { yCError(RP2_LIDAR)  << "Missing serial_baudrate param in GENERAL group"; return false; }
        if (general_config.check("sample_buffer_life") == false)  { yCError(RP2_LIDAR)  << "Missing sample_buffer_life param in GENERAL group"; return false; }

        baudrate    = general_config.find("serial_baudrate").asInt32();
        m_serialPort  = general_config.find("serial_port").asString();
        m_buffer_life = general_config.find("sample_buffer_life").asInt32();
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
        yCError(RP2_LIDAR) << "Missing GENERAL section";
        return false;
    }

    m_drv = RPlidarDriver::CreateDriver(rp::standalone::rplidar::DRIVER_TYPE_SERIALPORT);
    if (!m_drv)
    {
            yCError(RP2_LIDAR) << "Create Driver fail, exit\n";
            return false;
    }

    if (IS_FAIL(m_drv->connect(m_serialPort.c_str(), (_u32)baudrate)))
    {
        yCError(RP2_LIDAR) << "Error, cannot bind to the specified serial port:", m_serialPort.c_str();
        RPlidarDriver::DisposeDriver(m_drv);
        return false;
    }

    m_info = deviceinfo();
    yCInfo(RP2_LIDAR, "max_dist %f, min_dist %f",   m_max_distance, m_min_distance);

    bool m_inExpressMode=false;
    result = m_drv->checkExpressScanSupported(m_inExpressMode);
    if (result == RESULT_OK && m_inExpressMode==true)
    {
        yCInfo(RP2_LIDAR) << "Express scan mode is supported";
    }
    else
    {
        yCWarning(RP2_LIDAR) << "Device does not supports express scan mode";
    }

    result = m_drv->startMotor();
    if (result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yCInfo(RP2_LIDAR) << "Motor started successfully";

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
            yCInfo(RP2_LIDAR) << "Motor pwm set to "<< m_pwm_val;
        }
        else
        {
            yCError(RP2_LIDAR) << "Invalid motor pwm request " << m_pwm_val <<". It should be a value between 0 and 1023.";
            return false;
        }
    }
    else
    {
        yCInfo(RP2_LIDAR) << "Motor pwm set to default value (600?)";
    }

    bool forceScan =false;
    result = m_drv->startScan(forceScan,m_inExpressMode);

    if (result != RESULT_OK)
    {
        handleError(result);
        return false;
    }
    yCInfo(RP2_LIDAR) << "Scan started successfully";

    yCInfo(RP2_LIDAR) << "Device info:" << m_info;
    yCInfo(RP2_LIDAR,"max_angle %f, min_angle %f", m_max_angle, m_min_angle);
    yCInfo(RP2_LIDAR,"resolution %f",              m_resolution);
    yCInfo(RP2_LIDAR,"sensors %zu",                m_sensorsNum);
    PeriodicThread::start();
    return true;
}

bool RpLidar2::close()
{
    m_drv->stopMotor();
    RPlidarDriver::DisposeDriver(m_drv);
    PeriodicThread::stop();
    yCInfo(RP2_LIDAR) << "rpLidar closed";
    return true;
}

bool RpLidar2::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool RpLidar2::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_angle = min;
    m_max_angle = max;
    return true;
}

bool RpLidar2::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_resolution = step;
    return true;
}

bool RpLidar2::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(RP2_LIDAR, "setScanRate not yet implemented");
    return false;
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
    updateLidarData();
    m_mutex.unlock();
    return;
}

bool RpLidar2::acquireDataFromHW()
{
    u_result                            op_result;
    rplidar_response_measurement_node_t nodes[2048];
    size_t                              count = _countof(nodes);
    static int                          life = 0;
    op_result = m_drv->grabScanData(nodes, count);
    if (op_result != RESULT_OK)
    {
        yCError(RP2_LIDAR) << m_serialPort << ": grabbing scan data failed";
        handleError(op_result);
        return false;
    }

    float frequency = 0;
    bool is4kmode = false;
    op_result = m_drv->getFrequency(m_inExpressMode, count, frequency, is4kmode);
    if (op_result != RESULT_OK)
    {
        yCError(RP2_LIDAR) << "getFrequency failed";
        return false;
    }

    m_drv->ascendScanData(nodes, count);

    if (op_result != RESULT_OK)
    {
        yCError(RP2_LIDAR) << "ascending scan data failed\n";
        handleError(op_result);
        return false;
    }

    if (m_buffer_life && life % m_buffer_life == 0)
    {
        for (size_t i = 0; i < m_laser_data.size(); i++)
        {
            //m_laser_data[i]=0; //0 is a terribly unsafe value and should be avoided.
            m_laser_data[i] = std::numeric_limits<double>::infinity();
        }
    }

    //this lock protects m_laser_data. It is released at the end of the run(),
    //after that the following methods are called: applyLimitsOnLaserData(), updateTimestamp()
    m_mutex.lock();

    for (size_t i = 0; i < count; ++i)
    {

        double distance = nodes[i].distance_q2 / 4.0f / 1000.0; //m
        double angle = (float)((nodes[i].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f); //deg
        double quality = nodes[i].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;
        angle = (360 - angle);

        if (angle >= 360)
        {
            angle -= 360;
        }

        //checkme, is it useful?
        if (angle > 360)
        {
            yCWarning(RP2_LIDAR) << "Invalid angle";
        }

        if (quality == 0 || distance == 0)
        {
            distance = std::numeric_limits<double>::infinity();
        }

        int elem = (int)(angle / m_resolution);
        if (elem >= 0 && elem < (int)m_laser_data.size())
        {
            m_laser_data[elem] = distance;
        }
        else
        {
            yCDebug(RP2_LIDAR) << "RpLidar::run() invalid angle: elem" << elem << ">" << "laser_data.size()" << m_laser_data.size();
        }
    }

    life++;
    return true;
}

void RpLidar2::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(RP2_LIDAR,"RpLidar Thread releasing...");
    yCDebug(RP2_LIDAR,"... done.");
#endif

    return;
}

void RpLidar2::handleError(u_result error)
{
    switch (error)
    {
    case RESULT_FAIL_BIT:
        yCError(RP2_LIDAR) << "error: 'FAIL BIT'";
        break;
    case RESULT_ALREADY_DONE:
        yCError(RP2_LIDAR) << "error: 'ALREADY DONE'";
        break;
    case RESULT_INVALID_DATA:
        yCError(RP2_LIDAR) << "error: 'INVALID DATA'";
        break;
    case RESULT_OPERATION_FAIL:
        yCError(RP2_LIDAR) << "error: 'OPERATION FAIL'";
        break;
    case RESULT_OPERATION_TIMEOUT:
        yCError(RP2_LIDAR) << "error: 'OPERATION TIMEOUT'";
        break;
    case RESULT_OPERATION_STOP:
        yCError(RP2_LIDAR) << "error: 'OPERATION STOP'";
        break;
    case RESULT_OPERATION_NOT_SUPPORT:
        yCError(RP2_LIDAR) << "error: 'OPERATION NOT SUPPORT'";
        break;
    case RESULT_FORMAT_NOT_SUPPORT:
        yCError(RP2_LIDAR) << "error: 'FORMAT NOT SUPPORT'";
        break;
    case RESULT_INSUFFICIENT_MEMORY:
        yCError(RP2_LIDAR) << "error: 'INSUFFICENT MEMORY'";
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
