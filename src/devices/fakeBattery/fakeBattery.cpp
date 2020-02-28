/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "fakeBattery.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <iostream>
#include <cstring>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


FakeBattery::FakeBattery() :
        PeriodicThread(0.02)
{
}


bool FakeBattery::open(yarp::os::Searchable& config)
{
    double period = config.check("thread_period", Value(0.02), "Thread period (smaller implies faster charge/discharge)").asFloat64();
    setPeriod(period);

    double charge = config.check("charge", Value(50.0), "Initial charge (%)").asFloat64();
    double voltage = config.check("voltage", Value(30.0), "Initial voltage (V)").asFloat64();
    double current = config.check("current", Value(3.0), "Initial current (A)").asFloat64();
    double temperature = config.check("temperature", Value(20.0), "Initial temperature (°C)").asFloat64();
    std::string info = config.check("info", Value("Fake battery system v2.0"), "Initial battery information").asString();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        battery_charge = charge;
        battery_voltage = voltage;
        battery_current = current;
        battery_temperature = temperature;
        battery_info = std::move(info);
        updateStatus();
    }
    Bottle& group_general = config.findGroup("GENERAL");
    if (group_general.isNull()) {
        yWarning() << "GENERAL group parameters missing, assuming default";
    } else {
        // Other options
        this->debugEnable = group_general.check("debug", Value(0), "enable/disable the debug mode").asBool();
    }

    std::string name = config.find("name").asString();
    this->yarp().attachAsServer(ctrl_port);
    if (!ctrl_port.open(name + "/control/rpc:i")) {
        yError("Could not open rpc port");
        close();
        return false;
    }

    PeriodicThread::start();

    return true;
}

bool FakeBattery::close()
{
    // Stop the thread
    PeriodicThread::stop();

    // Close the RPC port
    ctrl_port.close();

    return true;
}

void FakeBattery::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (battery_current > 0.1) {
        battery_charge -= 0.001;
    } else if (battery_current < -0.1) {
        battery_charge += 0.001;
    }
    updateStatus();
}

bool FakeBattery::getBatteryVoltage(double& voltage)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    voltage = battery_voltage;
    return true;
}

bool FakeBattery::getBatteryCurrent(double& current)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    current = battery_current;
    return true;
}

bool FakeBattery::getBatteryCharge(double& charge)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    charge = battery_charge;
    return true;
}

bool FakeBattery::getBatteryStatus(Battery_status& status)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    status = battery_status;
    return true;
}

bool FakeBattery::getBatteryTemperature(double& temperature)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    temperature = battery_temperature;
    return true;
}

bool FakeBattery::getBatteryInfo(string& info)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    info = battery_info;
    return true;
}

void FakeBattery::setBatteryVoltage(const double voltage)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    battery_voltage = voltage;
    updateStatus();
}

void FakeBattery::setBatteryCurrent(const double current)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    battery_current = current;
    updateStatus();
}

void FakeBattery::setBatteryCharge(const double charge)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    battery_charge = charge;
    updateStatus();
}

void FakeBattery::setBatteryInfo(const std::string& info)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    battery_info = info;
}

void FakeBattery::setBatteryTemperature(const double temperature)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    battery_temperature = temperature;
}

void FakeBattery::updateStatus()
{
    battery_charge = yarp::conf::clamp(battery_charge, 0.0, 100.0);
    if (battery_current > 0.1) {
        if (battery_charge > 15.0) {
            battery_status = yarp::dev::IBattery::Battery_status::BATTERY_OK_IN_USE;
        } else if (battery_charge > 5.0) {
            battery_status = yarp::dev::IBattery::Battery_status::BATTERY_LOW_WARNING;
        } else {
            battery_status = yarp::dev::IBattery::Battery_status::BATTERY_CRITICAL_WARNING;
        }
    } else if (battery_current > -0.1) {
        battery_status = yarp::dev::IBattery::Battery_status::BATTERY_OK_STANBY;
    } else {
        battery_status = yarp::dev::IBattery::Battery_status::BATTERY_OK_IN_CHARGE;
    }
}
