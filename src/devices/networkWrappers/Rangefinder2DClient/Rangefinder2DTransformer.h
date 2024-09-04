/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_RANGEFINDER2DTRANSFORMER
#define YARP_DEV_RANGEFINDER2DTRANSFORMER


#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/LaserScan2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include "Rangefinder2DTransformer_ParamsParser.h"
#include <mutex>


#define DEFAULT_THREAD_PERIOD 20 //ms
const int LASER_TIMEOUT=100; //ms

/**
* @ingroup dev_impl_network_clients dev_impl_network_lidar
*
* \brief `Rangefinder2DClient`: The client side of any ILaserRangefinder2D capable device.
*
* Still single thread! concurrent access is unsafe.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the Rangefinder2DClient device.          |       |
* | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the Rangefinder2DClient connects to.    |     |
* | carrier        |     -          | string  | -              | tcp           | No           | The carier used for the connection with the server.               |       |
*/
class Rangefinder2DTransformer:
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRangefinder2D,
        public yarp::dev::WrapperSingle,
        public Rangefinder2DTransformer_ParamsParser
{
protected:
    // interfaces
    yarp::dev::IRangefinder2D* sens_p=nullptr;

    //data
    double m_scan_angle_min;
    double m_scan_angle_max;
    double m_device_position_x=0;
    double m_device_position_y=0;
    double m_device_position_theta=0;
    std::string m_laser_frame_name;
    std::string m_robot_frame_name;

public:

    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IRangefinder2D methods */
    bool getLaserMeasurement(std::vector<yarp::sig::LaserMeasurementData> &data, double* timestamp = nullptr) override;
    bool getRawData(yarp::sig::Vector &data, double* timestamp = nullptr) override;
    bool getDeviceStatus(Device_status &status) override;
    bool getDistanceRange(double& min, double& max) override;
    bool setDistanceRange(double min, double max) override;
    bool getScanLimits(double& min, double& max) override;
    bool setScanLimits(double min, double max) override;
    bool getHorizontalResolution(double& step) override;
    bool setHorizontalResolution(double step) override;
    bool getScanRate(double& rate) override;
    bool setScanRate(double rate) override;
    bool getDeviceInfo(std::string &device_info) override;

    /* WrapperSingle  methods */
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;
};

#endif // YARP_DEV_RANGEFINDER2DCLIENT_RANGEFINDER2DCLIENT_H
