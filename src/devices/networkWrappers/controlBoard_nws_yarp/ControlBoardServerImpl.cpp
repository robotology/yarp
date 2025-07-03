/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "ControlBoardServerImpl.h"

/*! \file ControlBoardServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

namespace {
YARP_LOG_COMPONENT(CB_RPC, "yarp.device.controlBoard_nws_yarp")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(ODOM2D_RPC, "Invalid interface"); return false;}}

ReturnValue ControlBoardRPCd::dummyTest_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    ReturnValue ret = ReturnValue_ok;
    return ret;
}
