/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeBattery.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <iostream>
#include <cstring>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKEBATTERY, "yarp.device.fakeBattery")
constexpr double default_period = 0.02;
constexpr double default_charge = 50.0;
constexpr double default_voltage = 30.0;
constexpr double default_current = 3.0;
constexpr double default_temperature = 20.0;
constexpr const char* default_info = "Fake battery system v2.0";
}

FakeBattery::FakeBattery() :
        PeriodicThread(default_period)
{
}


bool FakeBattery::open(yarp::os::Searchable& config)
{
    double period = config.check("thread_period", Value(default_period), "Thread period (smaller implies faster charge/discharge)").asFloat64();
    setPeriod(period);

    double charge = config.check("charge", Value(default_charge), "Initial charge (%)").asFloat64();
    double voltage = config.check("voltage", Value(default_voltage), "Initial voltage (V)").asFloat64();
    double current = config.check("current", Value(default_current), "Initial current (A)").asFloat64();
    double temperature = config.check("temperature", Value(default_temperature), "Initial temperature (°C)").asFloat64();
    std::string info = config.check("info", Value(default_info), "Initial battery information").asString();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        battery_charge = charge;
        battery_voltage = voltage;
        battery_current = current;
        battery_temperature = temperature;
        battery_info = std::move(info);
        updateStatus();
    }

    std::string name = config.find("name").asString();
    this->yarp().attachAsServer(ctrl_port);
    if (!ctrl_port.open(name + "/control/rpc:i")) {
        yCError(FAKEBATTERY, "Could not open rpc port");
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

double FakeBattery::getBatteryVoltage()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return battery_voltage;
}

double FakeBattery::getBatteryCurrent()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return battery_current;
}

double FakeBattery::getBatteryCharge()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return battery_charge;
}

std::string FakeBattery::getBatteryStatus()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    switch (battery_status) {
    case BATTERY_OK_STANBY:
        return "0: BATTERY_OK_STANBY";
    case BATTERY_OK_IN_CHARGE:
        return "1: BATTERY_OK_IN_CHARGE";
    case BATTERY_OK_IN_USE:
        return "2: BATTERY_OK_IN_USE";
    case BATTERY_GENERAL_ERROR:
        return "3: BATTERY_GENERAL_ERROR";
    case BATTERY_TIMEOUT:
        return "4: BATTERY_TIMEOUT";
    case BATTERY_LOW_WARNING:
        return "5: BATTERY_LOW_WARNING";
    case BATTERY_CRITICAL_WARNING:
        return "6: BATTERY_CRITICAL_WARNING";
    default:
        return "Invalid battery status";
    };
}

std::string FakeBattery::getBatteryInfo()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return battery_info;
}

double FakeBattery::getBatteryTemperature()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return battery_temperature;
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
