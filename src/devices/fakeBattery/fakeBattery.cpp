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
#include <string.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

FakeBattery::FakeBattery() :
        PeriodicThread(0.02)
{
    debugEnable = false;
}

FakeBattery::~FakeBattery()
{
}

bool FakeBattery::open(yarp::os::Searchable& config)
{
    Bottle& group_general = config.findGroup("GENERAL");

    if (config.check("thread_period")) {
        int period = config.find("thread_period").asInt32();
        setPeriod((double)period / 1000.0);
    }

    if (group_general.isNull()) {
        yWarning() << "GENERAL group parameters missing, assuming default";
    } else {
        // Other options
        this->debugEnable = group_general.check("debug", Value(0), "enable/disable the debug mode").asBool();
    }

    PeriodicThread::start();
    return true;
}

bool FakeBattery::close()
{
    //stop the thread
    PeriodicThread::stop();

    return true;
}

bool FakeBattery::threadInit()
{
    battery_info = "fake battery system v1.0";
    battery_voltage = 50.0;
    battery_current = 51.0;
    battery_charge = 52.0;
    battery_temperature = 53.0;
    timeStamp = yarp::os::Time::now();
    return true;
}

void FakeBattery::run()
{
    double timeNow = yarp::os::Time::now();
    static int counter = 0;

    std::lock_guard<std::mutex> lock(m_mutex);
    if (timeNow - timeStamp > 2.0) {
        timeStamp = timeNow;
        if (counter >= 4) {
            battery_voltage = 50.0;
            battery_current = 51.0;
            battery_charge = 52.0;
            battery_temperature = 53.0;
            counter = 0;
        } else {
            battery_voltage += 10;
            battery_current += 10;
            battery_charge += 10;
            battery_temperature += 10;
            counter++;
        }
    }
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
    status = BATTERY_OK_IN_USE;
    return true;
}

bool FakeBattery::getBatteryTemperature(double& temperature)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    temperature = 20;
    return true;
}

bool FakeBattery::getBatteryInfo(string& info)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    info = battery_info;
    return true;
}

void FakeBattery::threadRelease()
{
    yTrace("FakeBattery Thread released\n");
}
