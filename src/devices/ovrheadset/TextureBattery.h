/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_OVRHEADSET_TEXTUREBATTERY_H
#define YARP_OVRHEADSET_TEXTUREBATTERY_H

#include <GL/glew.h>
#include <OVR_CAPI.h>

#include <yarp/os/PeriodicThread.h>

#include <array>

class TextureStatic;
namespace yarp { namespace dev { class PolyDriver; } }
namespace yarp { namespace dev { class IBattery; } }

class TextureBattery : public yarp::os::PeriodicThread
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
