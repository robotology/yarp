/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_RANGEFINDER2D_NWC_YARP_H
#define YARP_DEV_RANGEFINDER2D_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/LaserScan2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>

#include "Rangefinder2D_nwc_yarp_ParamsParser.h"

class Rangefinder2DInputPortProcessor :
        public yarp::os::BufferedPort<yarp::dev::LaserScan2D>
{
    yarp::dev::LaserScan2D lastScan;
    std::mutex mutex;
    yarp::os::Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat();

    Rangefinder2DInputPortProcessor();

    using yarp::os::BufferedPort<yarp::dev::LaserScan2D>::onRead;
    void onRead(yarp::dev::LaserScan2D&v) override;

    inline int getLast(yarp::dev::LaserScan2D &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    yarp::dev::IRangefinder2D::Device_status getStatus();

};

/**
* @ingroup dev_impl_network_clients dev_impl_network_lidar
*
* \brief `Rangefinder2D_nwc_yarp`: The client side of any ILaserRangefinder2D capable device.
*
* Parameters required by this device are shown in class: Rangefinder2D_nwc_yarp_ParamsParser
*
*/
class Rangefinder2D_nwc_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRangefinder2D,
        public Rangefinder2D_nwc_yarp_ParamsParser
{
protected:
    Rangefinder2DInputPortProcessor m_inputPort;
    yarp::os::Port m_rpcPort;
    yarp::os::Stamp m_lastTs;
    std::string m_deviceId;

    double m_scan_angle_min;
    double m_scan_angle_max;
    std::string m_laser_frame_name;
    std::string m_robot_frame_name;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IRangefinder2D methods */
    bool getLaserMeasurement(std::vector<yarp::dev::LaserMeasurementData> &data, double* timestamp = nullptr) override;
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
};

#endif // YARP_DEV_RANGEFINDER2D_NWC_YARP_H
