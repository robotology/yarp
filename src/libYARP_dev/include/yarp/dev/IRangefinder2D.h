/*
 * Copyright (C) 2015 ICub Facility - Istituto italiano di Tecnologia
 * Author: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPRANGEFINDER2DINTERFACE
#define YARPRANGEFINDER2DINTERFACE

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>
#include <vector>

#define VOCAB_ILASER2D     VOCAB4('i','l','a','s')
#define VOCAB_DEVICE_INFO  VOCAB4('l','s','n','f')
#define VOCAB_LASER_DISTANCE_RANGE VOCAB4('l','s','d','r')
#define VOCAB_LASER_ANGULAR_RANGE  VOCAB4('l','s','a','r')
#define VOCAB_LASER_ANGULAR_STEP   VOCAB4('l','s','a','s')
#define VOCAB_LASER_SCAN_RATE      VOCAB4('l','s','s','r')

namespace yarp {
    namespace dev {
        class IRangefinder2D;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface for planar laser range finders
 */
class YARP_dev_API yarp::dev::IRangefinder2D
{
public:
    enum Device_status
    {
        DEVICE_OK_STANBY        = 0,
        DEVICE_OK_IN_USE        = 1,
        DEVICE_GENERAL_ERROR    = 2,
        DEVICE_TIMEOUT          = 3
    };

    class LaserMeasurementData
    {
        double stored_x;
        double stored_y;
        double stored_angle;
        double stored_distance;
        public:
        LaserMeasurementData()    { stored_x = stored_y = stored_angle = stored_distance = 0; }
        inline void set_cartesian(const double x, const double y)       { stored_x = x; stored_y = y; stored_angle = 0; stored_distance = 0; stored_distance = std::sqrt(x*x + y*y); stored_angle = std::atan2(y, x); }
        inline void set_polar(const double rho, const double theta)     { stored_angle = theta; stored_distance = rho; stored_y = rho*std::sin(theta); stored_x = rho*std::cos(theta); }
        inline void get_cartesian (double& x, double& y)       { x = stored_x; y = stored_y; }
        inline void get_polar     (double& rho, double& theta) { rho = stored_angle; theta = stored_distance; }
    };

    struct PolarMeasurementData
    {
        double angle;
        double distance;
    };

    virtual ~IRangefinder2D(){}

    /**
    * Get the device measurements
    * @param data a vector containing the measurement data, expressed in cartesian/polar format
    * @return true/false..
    */
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) = 0;

    /**
    * Get the device measurements
    * @param ranges the vector containing the raw measurement data, as acquired by the device.
    * @return true/false.
    */
    virtual bool getRawData(yarp::sig::Vector &data) = 0;

    /**
    * get the device status
    * @param status the device status
    * @return true/false.
    */
    virtual bool getDeviceStatus(Device_status& status) = 0;

    /**
    * get the device detection range
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false.
    */
    virtual bool getDistanceRange(double& min, double& max) = 0;

    /**
    * set the device detection range. Invalid setting will be discarded.
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false on success/failure.
    */
    virtual bool setDistanceRange(double min, double max) = 0;

    /**
    * get the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false.
    */
    virtual bool getScanLimits(double& min, double& max) = 0;

    /**
    * set the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false on success/failure.
    */
    virtual bool setScanLimits(double min, double max) = 0;

    /**
    * get the angular step between two measurments.
    * @param step the angular step between two measurments
    * @return true/false.
    */
    virtual bool getHorizontalResolution(double& step) = 0;

    /**
    * get the angular step between two measurments (if available)
    * @param step the angular step between two measurments
    * @return true/false on success/failure.
    */
    virtual bool setHorizontalResolution(double step) = 0;

    /**
    * get the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false.
    */
    virtual bool getScanRate(double& rate) = 0;

    /**
    * set the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false on success/failure.
    */
    virtual bool setScanRate(double rate) = 0;

    /**
    * get the device hardware charactestics
    * @param device_info string containing the device infos
    * @return true/false.
    */
    virtual bool getDeviceInfo(yarp::os::ConstString &device_info) = 0;
};

#endif
