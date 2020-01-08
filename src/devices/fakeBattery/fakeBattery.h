/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FAKEBATTERY_H
#define YARP_FAKEBATTERY_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/sig/Vector.h>


class FakeBattery :
        public yarp::os::PeriodicThread,
        public yarp::dev::IBattery,
        public yarp::dev::DeviceDriver
{
protected:
    yarp::os::Semaphore mutex;

    short              status;
    double             timeStamp;
    yarp::sig::Vector  data;
    double             battery_charge;
    double             battery_voltage;
    double             battery_current;
    double             battery_temperature;
    std::string        battery_info;
    unsigned char      backpack_status;

    bool debugEnable;

    yarp::os::ResourceFinder   rf;
    std::string         remoteName;
    std::string         localName;

public:
    FakeBattery(int period=20);
    virtual ~FakeBattery();

    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;

    virtual bool getBatteryVoltage     (double &voltage) override;
    virtual bool getBatteryCurrent     (double &current) override;
    virtual bool getBatteryCharge      (double &charge) override;
    virtual bool getBatteryStatus      (Battery_status &status) override;
    virtual bool getBatteryInfo        (std::string &info) override;
    virtual bool getBatteryTemperature (double &temperature) override;

    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;
};

#endif
