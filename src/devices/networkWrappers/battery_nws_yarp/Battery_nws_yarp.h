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
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/api.h>
#include <yarp/dev/BatteryData.h>

#include "IBatteryMsgs.h"

#include "Battery_nws_yarp_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 0.02 //s

// rpc commands
class IBatteryMsgsImpl : public IBatteryMsgs
{
private:
    std::mutex                m_mutex;
    yarp::dev::IBattery*      m_iBat{nullptr};

public:
    return_get_BatteryInfo    getBatteryInfoRPC() override;
    IBatteryMsgsImpl          (yarp::dev::IBattery* iBattery);
    ~IBatteryMsgsImpl() = default;

public:
    std::mutex* getMutex() { return &m_mutex; }
};


 /**
 *  @ingroup dev_impl_nws_yarp
 *
 * \brief `Battery_nws_yarp`: Device that expose a battery sensor (using the IBattery interface) on the YARP network.
 *
 * It reads the data from a battery sensor and sends them on a port, acting as a streaming server for a batteryClient device.
 * It creates one rpc port and its related handler for every output port.
 *
 * Parameters required by this device are shown in class: Battery_nws_yarp_ParamsParser
 */
class Battery_nws_yarp :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        Battery_nws_yarp_ParamsParser
{
public:
    Battery_nws_yarp();
    ~Battery_nws_yarp();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:

    yarp::dev::IBattery *m_ibattery_p = nullptr;
    std::unique_ptr<IBatteryMsgsImpl>  m_msgsImpl;

    //ports stuff
    std::string m_streamingPortName;
    std::string m_rpcPortName;
    yarp::os::Port m_rpcPort;
    yarp::os::BufferedPort<yarp::dev::BatteryData> m_streamingPort;

    //data
    double m_battery_charge = std::nan("");
    double m_battery_voltage = std::nan("");
    double m_battery_current = std::nan("");
    double m_battery_temperature = std::nan("");
    yarp::dev::IBattery::Battery_status m_battery_status = yarp::dev::IBattery::Battery_status::BATTERY_TIMEOUT;

    yarp::os::Stamp m_lastStateStamp;
    double m_period;
    std::string m_sensorId;

    //log stuff
    char                m_log_buffer[1024];
    FILE                *m_logFile=nullptr;

    //public methods
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    //private methods
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    //internal methods to handle particular statuses of the battery
    bool initialize_YARP(yarp::os::Searchable &config);
    void notify_message(std::string msg);
    void emergency_shutdown(std::string msg);
    void check_battery_status(double battery_charge);
    void stop_robot(std::string quit_port);
};

#endif // YARP_DEV_BATTERYWRAPPER_H
