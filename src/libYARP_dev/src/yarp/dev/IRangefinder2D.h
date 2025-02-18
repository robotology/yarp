/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IRANGEFINDER2D_H
#define YARP_DEV_IRANGEFINDER2D_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/LaserMeasurementData.h>
#include <vector>
#include <string>

constexpr yarp::conf::vocab32_t VOCAB_ILASER2D             = yarp::os::createVocab32('i','l','a','s');
constexpr yarp::conf::vocab32_t VOCAB_DEVICE_INFO          = yarp::os::createVocab32('l','s','n','f');
constexpr yarp::conf::vocab32_t VOCAB_LASER_DISTANCE_RANGE = yarp::os::createVocab32('l','s','d','r');
constexpr yarp::conf::vocab32_t VOCAB_LASER_ANGULAR_RANGE  = yarp::os::createVocab32('l','s','a','r');
constexpr yarp::conf::vocab32_t VOCAB_LASER_ANGULAR_STEP   = yarp::os::createVocab32('l','s','a','s');
constexpr yarp::conf::vocab32_t VOCAB_LASER_SCAN_RATE      = yarp::os::createVocab32('l','s','s','r');

namespace yarp::dev {
class IRangefinder2D;
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
        DEVICE_OK_STANDBY       = 0,
        DEVICE_OK_IN_USE        = 1,
        DEVICE_GENERAL_ERROR    = 2,
        DEVICE_TIMEOUT          = 3
    };

    virtual ~IRangefinder2D();

    /**
    * Get the device measurements
    * @param data a vector containing the measurement data, expressed in Cartesian/polar format
    * @param timestamp the timestamp of the retrieved data.
    * @return true/false
    */
    virtual ReturnValue getLaserMeasurement(std::vector<yarp::sig::LaserMeasurementData> &data, double* timestamp = nullptr) = 0;

    /**
    * Get the device measurements
    * @param ranges the vector containing the raw measurement data, as acquired by the device.
    * @param timestamp the timestamp of the retrieved data.
    * @return true/false.
    */
    virtual ReturnValue getRawData(yarp::sig::Vector &data, double* timestamp = nullptr) = 0;

    /**
    * get the device status
    * @param status the device status
    * @return true/false.
    */
    virtual ReturnValue getDeviceStatus(Device_status& status) = 0;

    /**
    * get the device detection range
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false.
    */
    virtual ReturnValue getDistanceRange(double& min, double& max) = 0;

    /**
    * set the device detection range. Invalid setting will be discarded.
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false on success/failure.
    */
    virtual ReturnValue setDistanceRange(double min, double max) = 0;

    /**
    * get the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false.
    */
    virtual ReturnValue getScanLimits(double& min, double& max) = 0;

    /**
    * set the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false on success/failure.
    */
    virtual ReturnValue setScanLimits(double min, double max) = 0;

    /**
    * get the angular step between two measurements.
    * @param step the angular step between two measurements
    * @return true/false.
    */
    virtual ReturnValue getHorizontalResolution(double& step) = 0;

    /**
    * get the angular step between two measurements (if available)
    * @param step the angular step between two measurements
    * @return true/false on success/failure.
    */
    virtual ReturnValue setHorizontalResolution(double step) = 0;

    /**
    * get the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false.
    */
    virtual ReturnValue getScanRate(double& rate) = 0;

    /**
    * set the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false on success/failure.
    */
    virtual ReturnValue setScanRate(double rate) = 0;

    /**
    * get the device hardware characteristics
    * @param device_info string containing the device infos
    * @return true/false.
    */
    virtual ReturnValue getDeviceInfo(std::string &device_info) = 0;
};

#endif // YARP_DEV_IRANGEFINDER2D_H
