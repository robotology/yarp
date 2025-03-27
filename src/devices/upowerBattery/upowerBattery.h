/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_UPOWERBATTERY_H
#define YARP_UPOWERBATTERY_H

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>

#include <QDBusInterface>

/**
* @ingroup dev_impl_other
*
* \brief `upowerBattery`: A device to view the battery of a linux laptop in YARP
* using the `yarp::dev::IBattery` interface.
*
* Documentation to be added
*/
class UpowerBattery :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IBattery
{
public:
    UpowerBattery() = default;
    UpowerBattery(const UpowerBattery&) = delete;
    UpowerBattery(UpowerBattery&&) = delete;
    UpowerBattery& operator=(const UpowerBattery&) = delete;
    UpowerBattery& operator=(UpowerBattery&&) = delete;

    ~UpowerBattery() override = default;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    yarp::dev::ReturnValue getBatteryVoltage(double& voltage) override;
    yarp::dev::ReturnValue getBatteryCurrent(double& current) override;
    yarp::dev::ReturnValue getBatteryCharge(double& charge) override;
    yarp::dev::ReturnValue getBatteryStatus(Battery_status& status) override;
    yarp::dev::ReturnValue getBatteryInfo(std::string& info) override;
    yarp::dev::ReturnValue getBatteryTemperature(double& temperature) override;

private:
    QDBusInterface *m_interface { nullptr };
};

#endif
