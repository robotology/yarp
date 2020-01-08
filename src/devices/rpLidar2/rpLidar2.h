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

#ifndef RPLIDAR_H
#define RPLIDAR_H


#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ISerialDevice.h>

#include <mutex>
#include <string>
#include <vector>

#include <rplidar.h>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
struct Range_t
{
    double min;
    double max;
};

//---------------------------------------------------------------------------------------------------------------

class RpLidar2 : public PeriodicThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
    typedef rp::standalone::rplidar::RPlidarDriver rplidardrv;

    void                  handleError(u_result error);
    std::string deviceinfo();
protected:
    std::mutex       m_mutex;
    int                   m_sensorsNum;
    int                   m_buffer_life;
    double                m_min_angle;
    double                m_max_angle;
    double                m_min_distance;
    double                m_max_distance;
    double                m_resolution;
    bool                  m_clip_max_enable;
    bool                  m_clip_min_enable;
    bool                  m_do_not_clip_infinity_enable;
    bool                  m_inExpressMode;
    int                   m_pwm_val;
    std::vector <Range_t> m_range_skip_vector;
    std::string           m_info;
    std::string           m_serialPort;
    Device_status         m_device_status;
    yarp::sig::Vector     m_laser_data;
    rplidardrv*           m_drv;

public:
    RpLidar2(double period = 0.01) : PeriodicThread(period),
        m_sensorsNum(0),
        m_buffer_life(0),
        m_min_angle(0.0),
        m_max_angle(0.0),
        m_min_distance(0.0),
        m_max_distance(0.0),
        m_resolution(0.0),
        m_clip_max_enable(false),
        m_clip_min_enable(false),
        m_do_not_clip_infinity_enable(false),
        m_inExpressMode(false),
        m_pwm_val(0),
        m_device_status(DEVICE_OK_STANBY),
        m_drv(nullptr)
    {}


    ~RpLidar2()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool getRawData(yarp::sig::Vector &data) override;
    bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) override;
    bool getDeviceStatus     (Device_status &status) override;
    bool getDeviceInfo       (std::string &device_info) override;
    bool getDistanceRange    (double& min, double& max) override;
    bool setDistanceRange    (double min, double max) override;
    bool getScanLimits        (double& min, double& max) override;
    bool setScanLimits        (double min, double max) override;
    bool getHorizontalResolution      (double& step) override;
    bool setHorizontalResolution      (double step) override;
    bool getScanRate         (double& rate) override;
    bool setScanRate         (double rate) override;

};

#endif
