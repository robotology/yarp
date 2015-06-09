/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Marco Randazzo
* email:   marco.ranndazzo@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
*/

#ifndef BATTERY_CLIENT_H
#define BATTERY_CLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class BatteryClient;
    }
}

/**
*  @ingroup yarp_dev_modules
*  \defgroup BatteryClient BatteryClient
*
* \section BatteryClient_parameter Description of input parameters
*
*  This device will connect to the proper analogServer and read the data broadcasted making them available to use for the user application. It also made available some function to check and control the state of the remote sensor.
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units | Default Value | Required  | Description   | Notes |
* |:--------------:|:------:|:-----:|:-------------:|:--------: |:-------------:|:-----:|
* | local          | string |       |               | Yes       | full name if the port opened by the device  | must start with a '/' character |
* | remote         | string |       |               | Yes       | full name of the port the device need to connect to | must start with a '/' character |
* | carrier        | string |       | udp           | No        | type of carrier to use, like tcp, udp and so on ...  | - |
* | period         | int    | ms    | 20            | No        | Publication period (in ms) of the sensor reading on the Can Bus | - |
*  The device will create a port with name <local> and will connect to a port colled <remote> at startup,
* ex: <b> /myModule/left_arm/ForceTorque </b>, and will connect to a port called <b> /icub/left_arm/ForceTorque<b>.
*
**/

#define DEFAULT_THREAD_PERIOD 20 //ms
const int ANALOG_TIMEOUT=100; //ms

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class BatteryInputPortProcessor : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    yarp::os::Bottle lastBottle;
    yarp::os::Semaphore mutex;
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

    virtual void onRead(yarp::os::Bottle &v);

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
* @ingroup dev_impl_wrapper
*
* The client side of any IAnalogSensor capable device.
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
    yarp::os::ConstString local;
    yarp::os::ConstString remote;
    yarp::os::Stamp lastTs; //used by IPreciselyTimed
    std::string robotName;
    std::string deviceId;
    std::string sensorType;
    std::string portPrefix;
    int _rate;

    void  removeLeadingTrailingSlashesOnly(std::string &name);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    
    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    yarp::os::Stamp getLastInputStamp();

    /**
    * Get the instantaneous voltage measurement
    * @param voltage the voltage measurement
    * @return true/false.
    */
    bool getBatteryVoltage(double &voltage);

    /**
    * Get the instantaneous current measurement
    * @param current the current measurement
    * @return true/false.
    */
    bool getBatteryCurrent(double &current);

    /**
    * get the battery status of charge
    * @param charge the charge measurement (0-100%)
    * @return true/false.
    */
    bool getBatteryCharge(double &charge);

    /**
    * get the battery status
    * @param status the battery status
    * @return true/false.
    */
    bool getBatteryStatus(int &status);

    /**
    * get the battery hardware charactestics (e.g. max voltage etc)
    * @param a string containing the battery infos
    * @return true/false.
    */
    bool getBatteryInfo(yarp::os::ConstString &battery_info);

    /**
    * get the battery temperature
    * @param temprature the battery temperature
    * @return true/false.
    */
    bool getBatteryTemperature(double &temperature);
};

#endif // BATTERY_CLIENT_H
