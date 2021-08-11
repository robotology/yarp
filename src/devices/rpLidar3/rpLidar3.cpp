/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "rpLidar3.h"

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

YARP_LOG_COMPONENT(RP_LIDAR3, "yarp.devices.RpLidar3")

//-------------------------------------------------------------------------------------
bool RpLidar3::startMotor()
{
    u_result result = m_drv->startMotor();
    if (result != RESULT_OK)
    {
        handleError(result);
        yCError(RP_LIDAR3) << "Unable to start motor";
        return false;
    }
    yCInfo(RP_LIDAR3) << "Motor started successfully";

    if (m_pwm_val > 0 && m_pwm_val < 1023)
    {
        result = m_drv->setMotorPWM(m_pwm_val);
        if (result != RESULT_OK)
        {
            handleError(result);
            yCError(RP_LIDAR3) << "Unable to setMotorPWM";
            return false;
        }
        yCInfo(RP_LIDAR3) << "Motor pwm set to " << m_pwm_val;
    }
    else
    {
        yCError(RP_LIDAR3) << "Invalid motor pwm request " << m_pwm_val << ". It should be a value between 0 and 1023.";
        return false;
    }

    return true;
}

bool RpLidar3::startScan()
{
    RplidarScanMode current_scan_mode;
    std::vector<RplidarScanMode> scanModes;
    u_result op_result = m_drv->getAllSupportedScanModes(scanModes);
    yCInfo(RP_LIDAR3) << "List of Scan Modes";
    for (size_t i = 0; i < scanModes.size(); i++)
    {
        yCInfo(RP_LIDAR3) << "id:" << scanModes[i].id
            << ", mode:" << scanModes[i].scan_mode
            << ", max distance:" << scanModes[i].max_distance
            << ", us per sample:" << scanModes[i].us_per_sample
            << ", samples/s:" << 1.0 / scanModes[i].us_per_sample * 1000000;
    }

    yInfo() << "Using scan mode: " << m_scan_mode;

    if (IS_OK(op_result))
    {
        _u16 selectedScanMode = _u16(-1);
        for (std::vector<RplidarScanMode>::iterator iter = scanModes.begin(); iter != scanModes.end(); iter++)
        {
            if (iter->scan_mode == m_scan_mode)
            {
                selectedScanMode = iter->id;
                break;
            }
        }

        if (selectedScanMode == _u16(-1))
        {
            yCInfo(RP_LIDAR3, "scan mode `%s' is not supported by lidar, supported modes:", m_scan_mode.c_str());
            for (std::vector<RplidarScanMode>::iterator iter = scanModes.begin(); iter != scanModes.end(); iter++)
            {
                yCInfo(RP_LIDAR3, "\t%s: max_distance: %.1f m, Point number: %.1fK", iter->scan_mode, iter->max_distance, (1000 / iter->us_per_sample));
            }
            op_result = RESULT_OPERATION_FAIL;
        }
        else
        {
            op_result = m_drv->startScanExpress(false /* not force scan */, selectedScanMode, 0, &current_scan_mode);
        }
    }

    if (op_result != RESULT_OK)
    {
        handleError(op_result);
        return false;
    }

    yCInfo(RP_LIDAR3) << "Scan started successfully";
    return true;
}

bool RpLidar3::open(yarp::os::Searchable& config)
{
    string   serial;
    int      baudrate;

    m_device_status = DEVICE_OK_STANBY;

    //parse all the parameters related to the linear/angular range of the sensor
    if (this->parseConfiguration(config) == false)
    {
        yCError(RP_LIDAR3) << ": error parsing parameters";
        return false;
    }

    bool br       = config.check("GENERAL");
    if (br != false)
    {
        //general options
        {
            yarp::os::Searchable& general_config = config.findGroup("GENERAL");
            if (general_config.check("serial_port") == false)         { yCError(RP_LIDAR3)  << "Missing serial_port param in GENERAL group"; return false; }
            if (general_config.check("serial_baudrate") == false)     { yCError(RP_LIDAR3)  << "Missing serial_baudrate param in GENERAL group"; return false; }
            if (general_config.check("sample_buffer_life") == false)  { yCError(RP_LIDAR3)  << "Missing sample_buffer_life param in GENERAL group"; return false; }
            if (general_config.check("thread_period"))
            {
                double thread_period = general_config.find("thread_period").asInt32() / 1000.0;
                this->setPeriod(thread_period);
            }

            baudrate    = general_config.find("serial_baudrate").asInt32();
            m_serialPort  = general_config.find("serial_port").asString();
            m_buffer_life = general_config.find("sample_buffer_life").asInt32();
        }

        //options specific to the rplidar sdk
        {
            yarp::os::Searchable& rplidar_config = config.findGroup("RPLIDAR");
            if (rplidar_config.check("motor_pwm"))
            {
                m_pwm_val     = rplidar_config.find("motor_pwm").asInt32();
            }
            if (rplidar_config.check("express_mode"))
            {
                m_inExpressMode = rplidar_config.find("express_mode").asInt32();
            }
            if (rplidar_config.check("scan_mode"))
            {
                m_scan_mode = rplidar_config.find("scan_mode").asString();
            }
            if (rplidar_config.check("force_scan"))
            {
                m_force_scan = rplidar_config.find("force_scan").asInt32();
            }
        }
    }
    else
    {
        yCError(RP_LIDAR3) << "Missing GENERAL section";
        return false;
    }

    m_drv = RPlidarDriver::CreateDriver(rp::standalone::rplidar::DRIVER_TYPE_SERIALPORT);
    if (!m_drv)
    {
            yCError(RP_LIDAR3) << "Create Driver fail, exit\n";
            return false;
    }

    if (IS_FAIL(m_drv->connect(m_serialPort.c_str(), (_u32)baudrate)))
    {
        yCError(RP_LIDAR3) << "Error, cannot bind to the specified serial port:", m_serialPort.c_str();
        RPlidarDriver::DisposeDriver(m_drv);
        return false;
    }

    if (!deviceinfo()) return false;
    if (!startMotor()) return false;
    if (!startScan()) return false;

    yCInfo(RP_LIDAR3) << "Device info:" << m_info;
    yCInfo(RP_LIDAR3,"max_dist %f, min_dist %f",   m_max_distance, m_min_distance);
    yCInfo(RP_LIDAR3,"max_angle %f, min_angle %f", m_max_angle, m_min_angle);
    yCInfo(RP_LIDAR3,"resolution %f",              m_resolution);
    yCInfo(RP_LIDAR3,"sensors %zu",                m_sensorsNum);
    PeriodicThread::start();
    return true;
}

bool RpLidar3::close()
{
    yCDebug(RP_LIDAR3) << "closing..";
    PeriodicThread::stop();

    m_drv->stopMotor();
    m_drv->stop();
    RPlidarDriver::DisposeDriver(m_drv);
    yCInfo(RP_LIDAR3) << "rpLidar closed";
    return true;
}

bool RpLidar3::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool RpLidar3::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_angle = min;
    m_max_angle = max;
    return true;
}

bool RpLidar3::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_resolution = step;
    return true;
}

bool RpLidar3::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(RP_LIDAR3, "setScanRate not yet implemented");
    return false;
}

bool RpLidar3::threadInit()
{
    return true;
}

//#define DEBUG_LOCKING 1
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof (_Array[0]))
#endif

void RpLidar3::run()
{
    bool b = updateLidarData();
    if (b)
    {
        m_mutex.unlock();
    }
    else
    {
        yCWarning(RP_LIDAR3, "updateLidarData() failed.");
    }
    return;
}

bool RpLidar3::acquireDataFromHW()
{
    u_result                            op_result;
    size_t                              count = m_nodes_num;
    static int                          life = 0;
    op_result = m_drv->grabScanDataHq(m_nodes, count);
    if (op_result != RESULT_OK)
    {
        yCError(RP_LIDAR3) << m_serialPort << ": grabbing scan data failed";
        handleError(op_result);
        return false;
    }

    m_drv->ascendScanData(m_nodes, count);

    if (op_result != RESULT_OK)
    {
        yCError(RP_LIDAR3) << "ascending scan data failed\n";
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

        double distance = (m_nodes[i].dist_mm_q2 / 1000.f / (1 << 2)); //m
        double angle = (m_nodes[i].angle_z_q14 * 90.f / (1 << 14)); //deg
        double quality = (m_nodes[i].quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT); //to be verified
        angle = (360 - angle);

        if (angle >= 360)
        {
            angle -= 360;
        }

        //checkme, is it useful?
        if (angle > 360)
        {
            yCWarning(RP_LIDAR3) << "Invalid angle";
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
            yCDebug(RP_LIDAR3) << "RpLidar::run() invalid angle: elem" << elem << ">" << "laser_data.size()" << m_laser_data.size();
        }
    }

    life++;
    return true;
}

void RpLidar3::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(RP_LIDAR3,"RpLidar Thread releasing...");
    yCDebug(RP_LIDAR3,"... done.");
#endif

    return;
}

void RpLidar3::handleError(u_result error)
{
    switch (error)
    {
    case RESULT_FAIL_BIT:
        yCError(RP_LIDAR3) << "error: 'FAIL BIT'";
        break;
    case RESULT_ALREADY_DONE:
        yCError(RP_LIDAR3) << "error: 'ALREADY DONE'";
        break;
    case RESULT_INVALID_DATA:
        yCError(RP_LIDAR3) << "error: 'INVALID DATA'";
        break;
    case RESULT_OPERATION_FAIL:
        yCError(RP_LIDAR3) << "error: 'OPERATION FAIL'";
        break;
    case RESULT_OPERATION_TIMEOUT:
        yCError(RP_LIDAR3) << "error: 'OPERATION TIMEOUT'";
        break;
    case RESULT_OPERATION_STOP:
        yCError(RP_LIDAR3) << "error: 'OPERATION STOP'";
        break;
    case RESULT_OPERATION_NOT_SUPPORT:
        yCError(RP_LIDAR3) << "error: 'OPERATION NOT SUPPORT'";
        break;
    case RESULT_FORMAT_NOT_SUPPORT:
        yCError(RP_LIDAR3) << "error: 'FORMAT NOT SUPPORT'";
        break;
    case RESULT_INSUFFICIENT_MEMORY:
        yCError(RP_LIDAR3) << "error: 'INSUFFICENT MEMORY'";
        break;
    }
}

bool RpLidar3::deviceinfo()
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
            yCError(RP_LIDAR3) << "Unable to getDeviceInfo";
            return false;
        }

        for (unsigned char i : info.serialnum)
        {
            serialNumber += to_string(i);
        }

        m_info = std::string("Firmware Version: ")  + to_string(info.firmware_version) +
                 "\nHardware Version: " + to_string(info.hardware_version) +
                 "\nModel: "            + to_string(info.model) +
                 "\nSerial Number:"     + serialNumber;
        return true;
    }
    yCError(RP_LIDAR3) << "sdk driver not intialised?!";
    return false;
}
