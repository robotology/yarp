/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_RANGEFINDER2DSERVERIMPL_H
#define YARP_DEV_RANGEFINDER2DSERVERIMPL_H

#include "IRangefinder2DMsgs.h"

class IRangefinder2DRPCd : public IRangefinder2DMsgs
{
    private:
    yarp::dev::IRangefinder2D* m_irf = nullptr;
    std::mutex                 m_mutex;

    public:
    void setInterface(yarp::dev::IRangefinder2D* _irf)
    {
        m_irf = _irf;
    }

    return_getDeviceStatus getDeviceStatus_RPC() override;
    return_getDistanceRange getDistanceRange_RPC() override;
    return_getScanLimits getScanLimits_RPC() override;
    return_getHorizontalResolution getHorizontalResolution_RPC() override;
    return_getScanRate getScanRate_RPC() override;
    return_getDeviceInfo getDeviceInfo_RPC() override;
    bool setDistanceRange_RPC(const double min, const double max) override;
    bool setScanLimits_RPC(const double min, const double max) override;
    bool setHorizontalResolution_RPC(const double step) override;
    bool setScanRate_RPC(const double rate) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_RANGEFINDER2DSERVERIMPL_H
