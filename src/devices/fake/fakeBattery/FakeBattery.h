/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEBATTERY_H
#define YARP_FAKEBATTERY_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>

#include "FakeBatteryService.h"
#include "FakeBattery_ParamsParser.h"

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `fakeBattery`: Documentation to be added
 *
 * Parameters required by this device are shown in class: FakeBattery_ParamsParser
 */
class FakeBattery :
        public yarp::os::PeriodicThread,
        public yarp::dev::IBattery,
        public yarp::dev::DeviceDriver,
        public FakeBatteryService,
        public FakeBattery_ParamsParser
{
protected:
    std::mutex m_mutex;
    double battery_charge {50.0};
    double battery_voltage {30.0};
    double battery_current {3.0};
    double battery_temperature {20.0};
    std::string battery_info {"Fake battery system v2.0"};
    Battery_status battery_status {BATTERY_OK_IN_USE};
    bool suspend_battery_status_update {false};

    bool debugEnable {false};

    yarp::os::RpcServer ctrl_port;
public:
    FakeBattery();
    FakeBattery(const FakeBattery&) = delete;
    FakeBattery(FakeBattery&&) = delete;
    FakeBattery& operator=(const FakeBattery&) = delete;
    FakeBattery& operator=(FakeBattery&&) = delete;

    ~FakeBattery() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::os::PeriodicThread
    void run() override;

    // yarp::dev::IBattery
    yarp::dev::ReturnValue getBatteryVoltage(double& voltage) override;
    yarp::dev::ReturnValue getBatteryCurrent(double& current) override;
    yarp::dev::ReturnValue getBatteryCharge(double& charge) override;
    yarp::dev::ReturnValue getBatteryStatus(Battery_status& status) override;
    yarp::dev::ReturnValue getBatteryInfo(std::string& info) override;
    yarp::dev::ReturnValue getBatteryTemperature(double& temperature) override;

    // FakeBatteryService
    void setBatteryVoltage(const double voltage) override;
    void setBatteryCurrent(const double current) override;
    void setBatteryCharge(const double charge) override;
    void setBatteryStatus(const yarp::dev::IBattery::Battery_status status) override;
    void setBatteryInfo(const std::string& info) override;
    void setBatteryTemperature(const double temperature) override;
    double getBatteryVoltage() override;
    double getBatteryCurrent() override;
    double getBatteryCharge() override;
    Battery_status getBatteryStatus() override;
    std::string getBatteryStatusString() override;
    std::string getBatteryInfo() override;
    double getBatteryTemperature() override;

private:
    void updateStatus();
};

#endif
