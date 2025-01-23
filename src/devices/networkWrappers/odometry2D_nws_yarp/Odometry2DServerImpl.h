/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_ODOM2DSERVERIMPL_H
#define YARP_DEV_ODOM2DSERVERIMPL_H

#include "IOdometry2DMsgs.h"
#include <yarp/dev/IOdometry2D.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ReturnValue.h>

class IOdometry2DRPCd : public IOdometry2DMsgs
{
    private:
    yarp::dev::Nav2D::IOdometry2D* m_iOdom = nullptr;
    std::mutex                     m_mutex;

    public:
    IOdometry2DRPCd (yarp::dev::Nav2D::IOdometry2D* _odom) { m_iOdom = _odom; }

    yarp::dev::ReturnValue reset_odometry_RPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_ODOM2DSERVERIMPL_H
