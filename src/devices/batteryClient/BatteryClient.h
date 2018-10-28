/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

#ifndef YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H
#define YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class BatteryClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms
const int BATTERY_TIMEOUT=100; //ms

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class BatteryInputPortProcessor : public yarp::os::BufferedPort<yarp::os::Bottle>
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

    BatteryInputPortProcessor();

    using yarp::os::BufferedPort<yarp::os::Bottle>::onRead;
    virtual void onRead(yarp::os::Bottle &v) override;

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
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
* @ingroup dev_impl_network_clients
*
* The client side of any IBattery capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::BatteryClient: public DeviceDriver,
                          public IPreciselyTimed,
                          public IBattery
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    BatteryInputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    std::string local;
    std::string remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string deviceId;
    int _rate;
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

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
