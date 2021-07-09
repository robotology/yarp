/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "upowerBattery.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <iostream>
#include <cstring>

#include <QString>
#include <QDBusInterface>
#include <QDBusReply>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(UPOWERBATTERY, "yarp.device.upowerBattery")
const QString UPOWER_SERVICE = QStringLiteral("org.freedesktop.UPower");
const QString UPOWER_OBJECT = QStringLiteral("org.freedesktop.UPower.Device");
}

bool UpowerBattery::open(yarp::os::Searchable& config)
{
    std::string device_path = config.check("device_path",
                                           Value("/org/freedesktop/UPower/devices/battery_BAT0"),
                                           "Battery device path (as returned by 'upower -e', for example '/org/freedesktop/UPower/devices/battery_BAT0')").asString();

    m_interface = new QDBusInterface(UPOWER_SERVICE, QString(device_path.c_str()), UPOWER_OBJECT, QDBusConnection::systemBus());
    if (!m_interface->isValid()) {
        yCError(UPOWERBATTERY) << "Interface not found";
        delete m_interface;
        return false;
    }

    return true;
}

bool UpowerBattery::close()
{
    delete m_interface;
    return true;
}

bool UpowerBattery::getBatteryVoltage(double& voltage)
{
    voltage = m_interface->property("Voltage").toDouble();
    return true;
}

bool UpowerBattery::getBatteryCurrent(double& current)
{
    auto energyrate = m_interface->property("EnergyRate").toDouble();
    auto voltage = m_interface->property("Voltage").toDouble();
    auto state = m_interface->property("State").toUInt();
    current = energyrate / voltage;
    if (current > 0 && state == 1 /* Charging */) {
        current = -current;
    }
    return true;
}

bool UpowerBattery::getBatteryCharge(double& charge)
{
    charge = m_interface->property("Percentage").toInt();
    return true;
}

bool UpowerBattery::getBatteryStatus(Battery_status& status)
{
    status = yarp::dev::IBattery::BATTERY_OK_STANBY;
    auto st = m_interface->property("State").toUInt();
    auto wl = m_interface->property("WarningLevel").toUInt();

    switch (st) {
    case 1 /* Charging */: [[fallthrough]];
    case 5 /* Pending charge */:
        status = yarp::dev::IBattery::BATTERY_OK_IN_CHARGE;
        break;
    case 2 /* Discharging */:
    case 6 /* Pending discharge */:
        switch (wl) {
            case 1 /* None */: [[fallthrough]];
            case 2 /* Discharging (only for UPSes) */:
                status = yarp::dev::IBattery::BATTERY_OK_IN_USE;
                break;
            case 3 /* Low */:
                status = yarp::dev::IBattery::BATTERY_LOW_WARNING;
                break;
            case 4 /* Critical */: [[fallthrough]];
            case 5 /* Action */:
                status = yarp::dev::IBattery::BATTERY_CRITICAL_WARNING;
                break;
            case 0 /* Unknown */: [[fallthrough]];
            default:
                status = yarp::dev::IBattery::BATTERY_GENERAL_ERROR;
        }
        break;
    case 3 /* Empty */:
        status = yarp::dev::IBattery::BATTERY_CRITICAL_WARNING;
        break;
    case 4 /* Fully charged */:
        status = yarp::dev::IBattery::BATTERY_OK_STANBY;
        break;
    case 0 /* Unknown */: [[fallthrough]];
    default:
        status = yarp::dev::IBattery::BATTERY_GENERAL_ERROR;
        break;
    }

    return true;
}

bool UpowerBattery::getBatteryTemperature(double& temperature)
{
    temperature = m_interface->property("Temperature").toDouble();
    return true;
}

bool UpowerBattery::getBatteryInfo(string& info)
{
    info = QStringLiteral("Vendor: %1, Model: %2, Serial: %3")
                .arg(m_interface->property("Vendor").toString())
                .arg(m_interface->property("Model").toString())
                .arg(m_interface->property("Serial").toString())
                .toStdString();
    yCDebug(UPOWERBATTERY) << info;
    return true;
}
