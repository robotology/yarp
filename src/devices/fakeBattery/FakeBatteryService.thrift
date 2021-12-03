/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

enum Battery_status{}(
  yarp.name = "yarp::dev::IBattery::Battery_status"
  yarp.includefile = "yarp/dev/IBattery.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

service FakeBatteryService
{
    oneway void setBatteryVoltage(1: double voltage);
    oneway void setBatteryCurrent(1: double current);
    oneway void setBatteryCharge(1: double charge);
    oneway void setBatteryStatus(1: Battery_status status);
    oneway void setBatteryInfo(1: string info);
    oneway void setBatteryTemperature(1: double temperature);

    double getBatteryVoltage();
    double getBatteryCurrent();
    double getBatteryCharge();
    Battery_status getBatteryStatus();
    string getBatteryStatusString();
    string getBatteryInfo();
    double getBatteryTemperature();
} (
    yarp.monitor = "true"
)
