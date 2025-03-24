/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H
#define YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H

#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/BatteryData.h>

#include "IBatteryMsgs.h"
#include "Battery_nwc_yarp_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 20 //ms

class Battery_InputPortProcessor : public yarp::os::BufferedPort<yarp::dev::BatteryData>
{
    const int BATTERY_TIMEOUT=100; //ms

    yarp::dev::BatteryData lastData;
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

    Battery_InputPortProcessor();

    using yarp::os::BufferedPort<yarp::dev::BatteryData>::onRead;
    void onRead(yarp::dev::BatteryData &v) override;

    inline int getLast(yarp::dev::BatteryData &data, yarp::os::Stamp &stmp);

    inline int getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);

    double getVoltage();
    double getCurrent();
    double getCharge();
    double getTemperature();
    yarp::dev::IBattery::Battery_status getStatus();

};

/**
* @ingroup dev_impl_network_clients
*
* \brief `Battery_nwc_yarp`: The client side of any IBattery capable device.
*
* Parameters required by this device are shown in class: Battery_nwc_yarp_ParamsParser
*
*/
class Battery_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IBattery,
        public Battery_nwc_yarp_ParamsParser
{
protected:
    Battery_InputPortProcessor inputPort;
    yarp::os::Port rpcPort;
    IBatteryMsgs   m_battery_RPC;
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
    yarp::dev::ReturnValue getBatteryVoltage(double &voltage) override;

    /**
    * Get the instantaneous current measurement
    * @param current the current measurement
    * @return true/false.
    */
    yarp::dev::ReturnValue getBatteryCurrent(double &current) override;

    /**
    * get the battery status of charge
    * @param charge the charge measurement (0-100%)
    * @return true/false.
    */
    yarp::dev::ReturnValue getBatteryCharge(double &charge) override;

    /**
    * get the battery status
    * @param status the battery status
    * @return true/false.
    */
    yarp::dev::ReturnValue getBatteryStatus(yarp::dev::IBattery::Battery_status &status) override;

    /**
    * get the battery hardware characteristics (e.g. max voltage etc)
    * @param a string containing the battery infos
    * @return true/false.
    */
    yarp::dev::ReturnValue getBatteryInfo(std::string &battery_info) override;

    /**
    * get the battery temperature
    * @param temperature the battery temperature
    * @return true/false.
    */
    yarp::dev::ReturnValue getBatteryTemperature(double &temperature) override;
};

#endif // YARP_DEV_BATTERYCLIENT_BATTERYCLIENT_H
