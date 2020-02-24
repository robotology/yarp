/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <vector>

#include <yarp/os/Searchable.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/api.h>
#include <mutex>

#ifndef YARP_DEV_2DLIDARDEVICETEMPLATE_H
#define YARP_DEV_2DLIDARDEVICETEMPLATE_H

namespace yarp {
namespace dev {

struct Range_t
{
    double min;
    double max;
};

/**
 * @brief The DLidarDeviceTemplate class.
 * This class has been designed to uniform the parsing of lidar yarp devices.
 */
class YARP_dev_API Lidar2DDeviceBase: public yarp::dev::IRangefinder2D
{

protected:
    //internal data
    yarp::sig::Vector     m_laser_data;
    yarp::dev::IRangefinder2D::Device_status m_device_status;
    std::mutex     m_mutex;

protected:
    //internal parameters
    std::string    m_info;
    double         m_scan_rate;
    size_t         m_sensorsNum;
    double         m_min_angle;
    double         m_max_angle;
    double         m_min_distance;
    double         m_max_distance;
    double         m_resolution;
    bool           m_clip_max_enable;
    bool           m_clip_min_enable;
    bool           m_do_not_clip_infinity_enable;
    std::vector <Range_t> m_range_skip_vector;

public:
    // the main parse function
    bool parse (yarp::os::Searchable& config);

public:
    //constructor
    Lidar2DDeviceBase();

public:
    //IRangefinder2D interface
    bool getRawData(yarp::sig::Vector& data) override;
    bool getLaserMeasurement(std::vector<LaserMeasurementData>& data) override;
    bool getDeviceStatus(Device_status& status) override;
    bool getDeviceInfo(std::string& device_info) override;
    bool getDistanceRange(double& min, double& max) override;
    bool getScanLimits(double& min, double& max) override;
    bool getHorizontalResolution(double& step) override;
    bool getScanRate(double& rate) override;
};

} // dev
} // yarp

#endif
