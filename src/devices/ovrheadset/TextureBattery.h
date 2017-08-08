/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_TEXTUREBATTERY_H
#define YARP_OVRHEADSET_TEXTUREBATTERY_H

#include <GL/glew.h>
#include <OVR_CAPI.h>

#include <yarp/os/RateThread.h>

#include <array>

class TextureStatic;
namespace yarp { namespace dev { class PolyDriver; } }
namespace yarp { namespace dev { class IBattery; } }

class TextureBattery : public yarp::os::SystemRateThread
{
public:
    enum BatteryStatus {
        BatteryStatusMissing         = 0,
        BatteryStatus100             = 1,
        BatteryStatus080             = 2,
        BatteryStatus060             = 3,
        BatteryStatus040             = 4,
        BatteryStatusCaution         = 5,
        BatteryStatusLow             = 6,
        BatteryStatusCharging        = 7,
        BatteryStatusCharging080     = 8,
        BatteryStatusCharging060     = 9,
        BatteryStatusCharging040     = 10,
        BatteryStatusChargingCaution = 11,
        BatteryStatusChargingLow     = 12,
    };

    TextureBattery(ovrSession session, bool enabled);
    ~TextureBattery();

    ovrSession session;

    TextureStatic* currentTexture;

    virtual void run();

private:
    std::array<TextureStatic*, 13> textures;
    BatteryStatus currentStatus;

    yarp::dev::PolyDriver* drv;
    yarp::dev::IBattery* ibat;

    bool initBatteryClient();
};

#endif // YARP_OVRHEADSET_TEXTUREBATTERY_H
