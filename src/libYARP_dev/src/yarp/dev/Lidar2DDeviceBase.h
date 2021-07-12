/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
 * @brief The Lidar2DDeviceBase class.
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
    //This section contains utility methods called by laser devices

    /**
    * This utility method calls in sequence: grabDataFromHW(), updateTimestamp and applyLimitsOnLaserData().
    * It is recommended that the device driver implementation calls this method to perform the
    * aforementioned three operations.
    * @return true/false if the methods called internally are all successful
    */
    virtual bool updateLidarData();

    /**
    * By default, it automatically updates the internal timestamp with the yarp time.
    * It can be redefined by the user if the timestamp source is for example the hardware device.
    * @return true/false if the update is successful.
    */
    virtual bool updateTimestamp();

    /**
    * Apply the limits on the internally stored lidar measurements.
    * @return true/false if the method is successful.
    */
    virtual bool applyLimitsOnLaserData();

    /**
    * This method should be implemented by the user, and contain the logic to grab data from the hardware.
    * @return true/false if the method is successful.
    */
    virtual bool acquireDataFromHW() = 0;

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

    //IPreciselyTimed interface
    virtual  yarp::os::Stamp getLastInputStamp() override;

private:
    //utility methods called internally by Lidar2DDeviceBase
    virtual bool checkSkipAngle(const double& angle, double& distance);
};

} // dev
} // yarp

#endif
