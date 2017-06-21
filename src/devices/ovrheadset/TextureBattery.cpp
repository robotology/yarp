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

TextureBattery::TextureBattery(ovrSession session, bool enabled) :
        SystemRateThread(5000),
        session(session),
        currentTexture(nullptr),
        currentStatus(BatteryStatusMissing),
        drv(nullptr),
        ibat(nullptr)
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

    if (!enabled) {
        // Calling suspend() before start should ensure that run() is never called
        suspend();
    }
    start();
}

TextureBattery::~TextureBattery()
{
    stop();

    if (drv) {
        drv->close();
        delete drv;
        drv = nullptr;
        ibat = nullptr;
    }

    for (TextureStatic* texture : textures) {
        delete texture;
        texture = nullptr;
    }

    currentTexture = nullptr;
}

bool TextureBattery::initBatteryClient()
{
    if (ibat) {
        return true;
    }

    // FIXME
    std::string robot_name = "foo";
    std::string localPort = "/oculus/battery:i";
    std::string remotePort = "/" + robot_name + "/battery:o";

    yarp::os::Property options;
    options.put("robot", robot_name.c_str());
    options.put("device", "batteryClient");
    options.put("local", localPort.c_str());
    options.put("remote", remotePort.c_str());
    options.put("period", getRate());
    options.put("quiet", true);

    drv = new yarp::dev::PolyDriver(options);

    if (!drv || !(drv->isValid())) {
        yError("Problems instantiating the device driver");
        delete drv;
        drv = nullptr;
        ibat = nullptr;
        return false;
    }

    drv->view(ibat);
    if (!ibat) {
        yError("Problems viewing the battery interface");
        drv->close();
        delete drv;
        drv = nullptr;
        ibat = nullptr;
        return false;
    }

    return true;
}


void TextureBattery::run()
{
    //    currentStatus = (BatteryStatus)(((int)currentStatus + 1) % textures.size());
    //    currentTexture = textures[currentStatus];
    //    return;

    if (!ibat) {
        if (!initBatteryClient()) {
            yWarning() << "Cannot connect to battery. Suspending thread.";
            currentTexture = textures[BatteryStatusMissing];
            suspend();
            return;
        }
    }
    yAssert(ibat);

    int status;
    double charge;
    bool ret = true;
    ret &= ibat->getBatteryCharge(charge);
    ret &= ibat->getBatteryStatus(status);

    if (!ret) {
        currentTexture = textures[BatteryStatusMissing];
        return;
    }

    if (charge > 95.0) {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging];
        } else {
            currentTexture = textures[BatteryStatus100];
        }
    } else if (charge > 80.0) {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging080];
        } else {
            currentTexture = textures[BatteryStatus080];
        }
    } else if (charge > 60.0) {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging060];
        } else {
            currentTexture = textures[BatteryStatus060];
        }
    } else if (charge > 40.0) {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusCharging040];
        } else {
            currentTexture = textures[BatteryStatus040];
        }
    } else if (charge > 20.0) {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusChargingCaution];
        } else {
            currentTexture = textures[BatteryStatusCaution];
        }
    } else {
        if (status == yarp::dev::IBattery::BATTERY_OK_IN_CHARGE) {
            currentTexture = textures[BatteryStatusChargingLow];
        } else {
            currentTexture = textures[BatteryStatusLow];
        }
    }
}
