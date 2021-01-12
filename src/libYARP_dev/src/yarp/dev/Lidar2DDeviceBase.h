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
#include <yarp/dev/IPreciselyTimed.h>
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
class YARP_dev_API Lidar2DDeviceBase: public yarp::dev::IRangefinder2D,
                                      public yarp::dev::IPreciselyTimed
{

protected:
    //internal data
    yarp::sig::Vector     m_laser_data;
    yarp::os::Stamp       m_timestamp;
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
    bool           m_do_not_clip_and_allow_infinity_enable;
    std::vector <Range_t> m_range_skip_vector;

public:
    // the main parse function
    bool parseConfiguration(yarp::os::Searchable& config);

public:
    //constructor
    Lidar2DDeviceBase();

protected:
    //utility methods called by laser devices
    virtual bool updateLidarData();
    virtual bool updateTimestamp();
    virtual bool applyLimitsOnLaserData();
    virtual bool updateLogic() = 0;

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector& data) override;
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData>& data) override;
    virtual bool getDeviceStatus(Device_status& status) override;
    virtual bool getDeviceInfo(std::string& device_info) override;
    virtual bool getDistanceRange(double& min, double& max) override;
    virtual bool getScanLimits(double& min, double& max) override;
    virtual bool getHorizontalResolution(double& step) override;
    virtual bool getScanRate(double& rate) override;

    //IPreciselyTimed interface
    virtual  yarp::os::Stamp getLastInputStamp() override;

private:
    //utility methods called internally by Lidar2DDeviceBase
    virtual bool checkSkipAngle(const double& angle, double& distance);
};

} // dev
} // yarp

#endif
