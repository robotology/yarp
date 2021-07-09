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

    bool getBatteryVoltage(double& voltage) override;
    bool getBatteryCurrent(double& current) override;
    bool getBatteryCharge(double& charge) override;
    bool getBatteryStatus(Battery_status& status) override;
    bool getBatteryInfo(std::string& info) override;
    bool getBatteryTemperature(double& temperature) override;

private:
    QDBusInterface *m_interface { nullptr };
};

#endif
