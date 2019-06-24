/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#ifndef YARP_DEV_RANGEFINDER2DCLIENT_RANGEFINDER2DCLIENT_H
#define YARP_DEV_RANGEFINDER2DCLIENT_RANGEFINDER2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>


#define DEFAULT_THREAD_PERIOD 20 //ms
const int LASER_TIMEOUT=100; //ms

class Rangefinder2DInputPortProcessor :
        public yarp::os::BufferedPort<yarp::os::Bottle>
{
    yarp::os::Bottle lastBottle;
    yarp::os::Mutex mutex;
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

    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle &v) override;

    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    bool getData(yarp::sig::Vector &data);
    yarp::dev::IRangefinder2D::Device_status getStatus();

};

/**
* @ingroup dev_impl_network_clients
*
* The client side of any ILaserRangefinder2D capable device.
* Still single thread! concurrent access is unsafe.
*/
class Rangefinder2DClient:
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IRangefinder2D
{
protected:
    Rangefinder2DInputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    std::string local;
    std::string remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string deviceId;

    double scan_angle_min;
    double scan_angle_max;
    double device_position_x;
    double device_position_y;
    double device_position_theta;
    std::string laser_frame_name;
    std::string robot_frame_name;

public:

    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;


    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp() override;

    /**
    * Get the device measurements
    * @param data a vector containing the measurement data, expressed in cartesian/polar format
    * @return true/false..
    */
    bool getLaserMeasurement(std::vector<yarp::dev::LaserMeasurementData> &data) override;

    /**
    * Get the device measurements
    * @param ranges the vector containing the raw measurement data, as acquired by the device.
    * @return true/false.
    */
    bool getRawData(yarp::sig::Vector &data) override;

    /**
    * get the device status
    * @param status the device status
    * @return true/false.
    */
    bool getDeviceStatus(Device_status &status) override;

    /**
    * get the device detection range
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false.
    */
    bool getDistanceRange(double& min, double& max) override;

    /**
    * set the device detection range. Invalid setting will be discarded.
    * @param min the minimum detection distance
    * @param max the maximum detection distance
    * @return true/false on success/failure.
    */
    bool setDistanceRange(double min, double max) override;

    /**
    * get the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false.
    */
    bool getScanLimits(double& min, double& max) override;

    /**
    * set the scan angular range.
    * @param min start angle of the scan
    * @param max end angle of the scan
    * @return true/false on success/failure.
    */
    bool setScanLimits(double min, double max) override;

    /**
    * get the angular step between two measurments.
    * @param step the angular step between two measurments
    * @return true/false.
    */
    bool getHorizontalResolution(double& step) override;

    /**
    * get the angular step between two measurments (if available)
    * @param step the angular step between two measurments
    * @return true/false on success/failure.
    */
    bool setHorizontalResolution(double step) override;

    /**
    * get the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false.
    */
    bool getScanRate(double& rate) override;

    /**
    * set the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true/false on success/failure.
    */
    bool setScanRate(double rate) override;

    /**
    * get the device hardware charactestics
    * @param device_info string containing the device infos
    * @return true/false.
    */
    bool getDeviceInfo(std::string &device_info) override;

};

#endif // YARP_DEV_RANGEFINDER2DCLIENT_RANGEFINDER2DCLIENT_H
