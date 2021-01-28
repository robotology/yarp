/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
