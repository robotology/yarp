/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

service FakeBatteryService
{
    oneway void setBatteryVoltage(1: double voltage);
    oneway void setBatteryCurrent(1: double current);
    oneway void setBatteryCharge(1: double charge);
    oneway void setBatteryInfo(1: string info);
    oneway void setBatteryTemperature(1: double temperature);

    double getBatteryVoltage();
    double getBatteryCurrent();
    double getBatteryCharge();
    string getBatteryStatus();
    string getBatteryInfo();
    double getBatteryTemperature();
}
