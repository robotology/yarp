/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MOBBASEVEL2DSERVERIMPL_H
#define YARP_DEV_MOBBASEVEL2DSERVERIMPL_H

#include "MobileBaseVelocityControlRPC.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ReturnValue.h>

class IMobileBaseVelocityControlRPCd : public MobileBaseVelocityControlRPC
{
    private:
    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iNavVel = nullptr;
    std::mutex                m_mutex;

    public:
    IMobileBaseVelocityControlRPCd ( yarp::dev::Nav2D::INavigation2DVelocityActions* _iNavVel) { m_iNavVel = _iNavVel; }

    yarp::dev::ReturnValue applyVelocityCommandRPC(const double x_vel, const double y_vel, const double theta_vel, const double timeout) override;
    return_getLastVelocityCommand getLastVelocityCommandRPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_MOBBASEVEL2DSERVERIMPL_H
