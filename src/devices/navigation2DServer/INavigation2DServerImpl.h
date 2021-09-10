/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_NAVIGATION2DSERVERIMPL_H
#define YARP_DEV_NAVIGATION2DSERVERIMPL_H

#include "INavigation2DMsgs.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Stamp.h>

class INavigation2DRPCd : public INavigation2DMsgs
{
    private:
    yarp::dev::Nav2D::INavigation2DControlActions*   m_iNav_ctrl = nullptr;
    yarp::dev::Nav2D::INavigation2DTargetActions*    m_iNav_target = nullptr;
    yarp::dev::Nav2D::INavigation2DVelocityActions*  m_iNav_vel = nullptr;
    std::mutex                                       m_mutex;

    public:
    void setInterfaces(yarp::dev::Nav2D::INavigation2DTargetActions* iNav_target, yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl, yarp::dev::Nav2D::INavigation2DVelocityActions*  iNav_vel);

    bool stop_navigation_RPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_NAVIGATION2DSERVERIMPL_H
