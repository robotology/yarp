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
#include <yarp/sig/LaserScan2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>

#include "IRangefinder2DMsgs.h"
#include "Rangefinder2D_nwc_yarp_ParamsParser.h"

class Rangefinder2D_InputPortProcessor :
        public yarp::os::BufferedPort<yarp::sig::LaserScan2D>
{
    yarp::sig::LaserScan2D lastScan;
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

    Rangefinder2D_InputPortProcessor();

    using yarp::os::BufferedPort<yarp::sig::LaserScan2D>::onRead;
    void onRead(yarp::sig::LaserScan2D& v) override;

    inline int getLast(yarp::sig::LaserScan2D& data, yarp::os::Stamp& stmp);

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
    Rangefinder2D_InputPortProcessor m_inputPort;
    IRangefinder2DMsgs m_RPC;
    std::mutex         m_mutex;

    yarp::os::Port m_rpcPort;
    yarp::os::Stamp m_lastTs;
    std::string m_deviceId;

    double m_scan_angle_min = std::nan("1");
    double m_scan_angle_max = std::nan("1");
    std::string m_laser_frame_name;
    std::string m_robot_frame_name;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IRangefinder2D methods */
    yarp::dev::ReturnValue getLaserMeasurement(std::vector<yarp::sig::LaserMeasurementData> &data, double* timestamp = nullptr) override;
    yarp::dev::ReturnValue getRawData(yarp::sig::Vector &data, double* timestamp = nullptr) override;
    yarp::dev::ReturnValue getDeviceStatus(Device_status &status) override;
    yarp::dev::ReturnValue getDistanceRange(double& min, double& max) override;
    yarp::dev::ReturnValue setDistanceRange(double min, double max) override;
    yarp::dev::ReturnValue getScanLimits(double& min, double& max) override;
    yarp::dev::ReturnValue setScanLimits(double min, double max) override;
    yarp::dev::ReturnValue getHorizontalResolution(double& step) override;
    yarp::dev::ReturnValue setHorizontalResolution(double step) override;
    yarp::dev::ReturnValue getScanRate(double& rate) override;
    yarp::dev::ReturnValue setScanRate(double rate) override;
    yarp::dev::ReturnValue getDeviceInfo(std::string &device_info) override;
};

#endif // YARP_DEV_RANGEFINDER2D_NWC_YARP_H
