/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_BATTERYWRAPPER_H
#define YARP_DEV_BATTERYWRAPPER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/api.h>

#define DEFAULT_THREAD_PERIOD 0.02 //s

 /**
 *  @ingroup dev_impl_wrapper
 *
 * \brief `batteryWrapper`: Device that expose a battery sensor (using the IBattery interface) on the YARP network.
 *
 * \section batteryWrapper_device_parameters Description of input parameters
 *
 * It reads the data from a battery sensor and sends them on a port, acting as a streaming server for a batteryClient device.
 * It creates one rpc port and its related handler for every output port.
 *
 *
 * Parameters required by this device are:
 * | Parameter name    | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                                      | Notes |
 * |:-----------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:--------------------------------------------------------------------------------:|:-----:|
 * | name              |      -         | string  | -              |   -           | Yes                         | prefix of the ports opened by the device, e.g. /robotName/battery1               | MUST start with a '/' character. /data:o and /rpc:i is automatically appended by the wrapper at the end |
 * | period            |      -         | double  | s              |   1.0         | No                          | refresh period of the broadcasted values in seconds                              | optional, default 1.0s |
 * | subdevice         |      -         | string  | -              |   -           | No                          | name of the subdevice to instantiate                                             | when used, parameters for the subdevice must be provided as well |
 * | enable_shutdown   |      -         | bool    | -              |   false       | No                          | if enabled, batteryWrapper will start a system shutdown when charge is below 5%  | - |
 * | enable_log        |      -         | bool    | -              |   false       | No                          | if enabled, stores a log of battery usage on disk                                | data stored on file batteryLog.txt |
 * | quitPortName      |      -         | string  | -              |   -           | No                          | name of port used to terminate the execution of yarpRobotInterface               | used only if enable_shutdown=true |
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 *  device  batteryWrapper
 *  subdevice fakeBattery
 *  name /myBatterySensor
 *
 * ** parameter for 'fakeBattery' subdevice follows here **
 * ...
 * \endcode
 *
 *
 * Configuration file using .xml format.
 *
 * \code{.xml}
 *  <device name="battery1" type="batteryWrapper">
 *      <param name="period">   20                  </param>
 *      <param name="name">   /myBatterySensor       </param>
 *
 *      <action phase="startup" level="5" type="attach">
 *          <paramlist name="networks">
 *              <elem name="my_battery">  my_battery </elem>
 *          </paramlist>
 *      </action>
 *
 *      <action phase="shutdown" level="5" type="detach" />
 *  </device>
 * \endcode
 * */
class BatteryWrapper :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
public:
    BatteryWrapper();
    ~BatteryWrapper();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:

    //driver stuff
    yarp::dev::PolyDriver m_driver;
    yarp::dev::IBattery *m_ibattery_p;             // the battery read from

    //ports stuff
    std::string m_streamingPortName;
    std::string m_rpcPortName;
    std::string m_quitPortName;
    yarp::os::Port m_rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> m_streamingPort;

    //data
    double m_battery_charge = std::nan("");
    double m_battery_voltage = std::nan("");
    double m_battery_current = std::nan("");
    double m_battery_temperature = std::nan("");
    yarp::dev::IBattery::Battery_status m_battery_status = yarp::dev::IBattery::Battery_status::BATTERY_TIMEOUT;

    yarp::os::Stamp m_lastStateStamp;             // the last reading time stamp
    double m_period;
    bool m_ownDevices;
    std::string m_sensorId;

    //behavior controls
    bool m_enable_shutdown;
    bool m_enable_log;

    //log stuff
    char                m_log_buffer[1024];
    FILE                *m_logFile;

    //public methods
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    //private methods
    void attach(yarp::dev::IBattery *s);
    void detach();

    //internal methods to handle particular statuses of the battery
    bool initialize_YARP(yarp::os::Searchable &config);
    void notify_message(std::string msg);
    void emergency_shutdown(std::string msg);
    void check_battery_status(double battery_charge);
    void stop_robot(std::string quit_port);
};

#endif // YARP_DEV_BATTERYWRAPPER_H
