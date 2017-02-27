/*
* Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
* Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include "TextureBattery.h"

#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IBattery.h>

#include "img-battery-missing.h"
#include "img-battery-100.h"
#include "img-battery-080.h"
#include "img-battery-060.h"
#include "img-battery-040.h"
#include "img-battery-caution.h"
#include "img-battery-low.h"
#include "img-battery-charging.h"
#include "img-battery-charging-080.h"
#include "img-battery-charging-060.h"
#include "img-battery-charging-040.h"
#include "img-battery-charging-caution.h"
#include "img-battery-charging-low.h"

TextureBattery::TextureBattery(ovrSession session) :
        RateThread(2000),
        session(session),
        currentTexture(nullptr),
        currentStatus(BatteryStatusMissing),
        drv(nullptr),
        batteryClient(nullptr)
{
    textures[BatteryStatusMissing] = new TextureStatic(session, battery_missing);
    textures[BatteryStatus100] = new TextureStatic(session, battery_100);
    textures[BatteryStatus080] = new TextureStatic(session, battery_080);
    textures[BatteryStatus060] = new TextureStatic(session, battery_060);
    textures[BatteryStatus040] = new TextureStatic(session, battery_040);
    textures[BatteryStatusCaution] = new TextureStatic(session, battery_caution);
    textures[BatteryStatusLow] = new TextureStatic(session, battery_low);
    textures[BatteryStatusCharging] = new TextureStatic(session, battery_charging);
    textures[BatteryStatusCharging080] = new TextureStatic(session, battery_charging_080);
    textures[BatteryStatusCharging060] = new TextureStatic(session, battery_charging_060);
    textures[BatteryStatusCharging040] = new TextureStatic(session, battery_charging_040);
    textures[BatteryStatusChargingCaution] = new TextureStatic(session, battery_charging_caution);
    textures[BatteryStatusChargingLow] = new TextureStatic(session, battery_charging_low);

    currentTexture = textures[currentStatus];
    start();
}

TextureBattery::~TextureBattery()
{
    stop();
    for (TextureStatic* texture : textures) {
        delete texture;
    }
    currentTexture = nullptr;
}

bool TextureBattery::initBatteryClient()
{
    if (batteryClient) {
        return true;
    }

    std::string robot_name = "foo";
    std::string localPort = "/oculus/battery:i";
    std::string remotePort = "/" + robot_name + "/battery:o";

    yarp::os::Property options;
    options.put("robot", robot_name.c_str());
    options.put("device", "batteryClient");
    options.put("local", localPort.c_str());
    options.put("remote", remotePort.c_str());
    options.put("period", getRate());

    drv = new yarp::dev::PolyDriver(options);

    if (!drv || !(drv->isValid())) {
        yError("Problems instantiating the device driver");
        delete drv;
        drv = nullptr;
        batteryClient = nullptr;
        return false;
    }

//    drv->view(batteryClient);
    if (!batteryClient) {
        yError("Problems viewing the battery interface");
        drv->close();
        delete drv;
        drv = nullptr;
        batteryClient = nullptr;
        return false;
    }

    return true;
}

bool TextureBattery::threadInit()
{
    initBatteryClient();
    // Do not fail if we cannot connect to the battery
    return true;
}

void TextureBattery::threadRelease()
{
    if (drv) {
        drv->close();
        delete drv;
        drv = nullptr;
        batteryClient = nullptr;
    }
}

void TextureBattery::run() {
    currentStatus = (BatteryStatus)(((int)currentStatus + 1) % 13);
    currentTexture = textures[currentStatus];
    return;

#if 0
    if (batteryClient) {
        if (!initBatteryClient) {
            currentTexture = textures[BatteryStatusMissing];
            return;
        }
    }
    yAssert(batteryClient);

    int status;
    double charge;
    bool ret = true;
    ret &= batteryClient->getBatteryCharge(&charge);
    ret &= batteryClient->getStatus(&status);

    if (!ret) {
        currentTexture = textures[BatteryStatusMissing];
        return;
    }

    if (charge > 0.90) {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging];
        } else {
            currentTexture = textures[BatteryStatus100];
        }
    } else if (charge > 0.80) {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging080];
        } else {
            currentTexture = textures[BatteryStatus080];
        }
    } else if (charge > 0.60) {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging060];
        } else {
            currentTexture = textures[BatteryStatus060];
        }
    } else if (charge > 0.40) {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging040];
        } else {
            currentTexture = textures[BatteryStatus040];
        }
    } else if (charge > 0.20) {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusChargingCaution];
        } else {
            currentTexture = textures[BatteryStatusCaution];
        }
    } else {
        if (status == BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusChargingLow];
        } else {
            currentTexture = textures[BatteryStatusLow];
        }
#endif
}
