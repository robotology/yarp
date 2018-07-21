/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

#ifndef LASER_FROM_DEPTH_H
#define LASER_FROM_DEPTH_H

#include <string>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IRGBDSensor.h>
#include <vector>

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

class LaserFromDepth : public PeriodicThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    IRGBDSensor* iRGBD;
    yarp::os::Mutex mutex;

    int m_depth_width;
    int m_depth_height;
    yarp::sig::ImageOf<float> m_depth_image;

    int m_sensorsNum;
    double m_min_angle;
    double m_max_angle;
    double m_min_distance;
    double m_max_distance;
    double m_resolution;
    bool m_clip_max_enable;
    bool m_clip_min_enable;
    bool m_do_not_clip_infinity_enable;
    std::vector <Range_t> m_range_skip_vector;

    std::string m_info;
    Device_status m_device_status;

    yarp::sig::Vector m_laser_data;

public:
    LaserFromDepth(double period = 0.01) : PeriodicThread(period),
        iRGBD(nullptr),
        m_depth_width(0),
        m_depth_height(0),
        m_sensorsNum(0),
        m_min_angle(0.0),
        m_max_angle(0.0),
        m_min_distance(0.0),
        m_max_distance(0.0),
        m_resolution(0.0),
        m_clip_max_enable(false),
        m_clip_min_enable(false),
        m_do_not_clip_infinity_enable(false),
        m_device_status(Device_status::DEVICE_OK_STANBY)
    {}

    ~LaserFromDepth()
    {
    }

    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector &data) override;
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) override;
    virtual bool getDeviceStatus     (Device_status &status) override;
    virtual bool getDeviceInfo       (std::string &device_info) override;
    virtual bool getDistanceRange    (double& min, double& max) override;
    virtual bool setDistanceRange    (double min, double max) override;
    virtual bool getScanLimits        (double& min, double& max) override;
    virtual bool setScanLimits        (double min, double max) override;
    virtual bool getHorizontalResolution      (double& step) override;
    virtual bool setHorizontalResolution      (double step) override;
    virtual bool getScanRate         (double& rate) override;
    virtual bool setScanRate         (double rate) override;
};

#endif
