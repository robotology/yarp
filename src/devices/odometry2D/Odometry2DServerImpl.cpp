/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "Odometry2DServerImpl.h"

/*! \file Odom2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
YARP_LOG_COMPONENT(ODOM2D_RPC, "yarp.device.map2D_nws_yarp.IMap2DRPCd")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(ODOM2D_RPC, "Invalid interface"); return false;}}

bool IOdometry2DRPCd::reset_odometry_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iOdom == nullptr) { yCError(ODOM2D_RPC, "Invalid interface"); return false; }}

    if (!m_iOdom->resetOdometry())
    {
        yCError(ODOM2D_RPC, "Unable to resetOdometry");
        return false;
    }
    return true;
}
