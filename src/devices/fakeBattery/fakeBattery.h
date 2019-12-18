/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FAKEBATTERY_H
#define YARP_FAKEBATTERY_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>

class FakeBattery :
        public yarp::os::PeriodicThread,
        public yarp::dev::IBattery,
        public yarp::dev::DeviceDriver
{
protected:
    std::mutex m_mutex;

    short status;
    double timeStamp;
    yarp::sig::Vector data;
    double battery_charge;
    double battery_voltage;
    double battery_current;
    double battery_temperature;
    std::string battery_info;
    unsigned char backpack_status;

    bool debugEnable;

    yarp::os::ResourceFinder rf;
    std::string remoteName;
    std::string localName;

public:
    FakeBattery();
    FakeBattery(const FakeBattery&) = delete;
    FakeBattery(FakeBattery&&) = delete;
    FakeBattery& operator=(const FakeBattery&) = delete;
    FakeBattery& operator=(FakeBattery&&) = delete;

    ~FakeBattery() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    bool getBatteryVoltage(double& voltage) override;
    bool getBatteryCurrent(double& current) override;
    bool getBatteryCharge(double& charge) override;
    bool getBatteryStatus(Battery_status& status) override;
    bool getBatteryInfo(std::string& info) override;
    bool getBatteryTemperature(double& temperature) override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;
};

#endif
