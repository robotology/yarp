/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H
#define YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>


#define DEFAULT_THREAD_PERIOD 20 //ms
const int BATTERY_TIMEOUT=100; //ms


class BatteryInputPortProcessor : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    yarp::os::Bottle lastBottle;
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

    BatteryInputPortProcessor();

    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle &v) override;

    inline int getLast(yarp::os::Bottle &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    double getVoltage();
    double getCurrent();
    double getCharge();
    double getTemperature();
    int getStatus();

};

/**
* @ingroup dev_impl_network_clients
*
* \brief `batteryClient`: The client side of any IBattery capable device.
* Still single thread! concurrent access is unsafe.
*/
class BatteryClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IBattery
{
protected:
    BatteryInputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    std::string local;
    std::string remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string deviceId;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;


    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp() override;

    /**
    * Get the instantaneous voltage measurement
    * @param voltage the voltage measurement
    * @return true/false.
    */
    bool getBatteryVoltage(double &voltage) override;

    /**
    * Get the instantaneous current measurement
    * @param current the current measurement
    * @return true/false.
    */
    bool getBatteryCurrent(double &current) override;

    /**
    * get the battery status of charge
    * @param charge the charge measurement (0-100%)
    * @return true/false.
    */
    bool getBatteryCharge(double &charge) override;

    /**
    * get the battery status
    * @param status the battery status
    * @return true/false.
    */
    bool getBatteryStatus(Battery_status &status) override;

    /**
    * get the battery hardware charactestics (e.g. max voltage etc)
    * @param a string containing the battery infos
    * @return true/false.
    */
    bool getBatteryInfo(std::string &battery_info) override;

    /**
    * get the battery temperature
    * @param temprature the battery temperature
    * @return true/false.
    */
    bool getBatteryTemperature(double &temperature) override;
};

#endif // YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H
