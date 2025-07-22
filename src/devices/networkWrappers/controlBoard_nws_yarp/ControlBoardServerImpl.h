/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_CONTROLBOARDSERVERIMPL_H
#define YARP_DEV_CONTROLBOARDSERVERIMPL_H

#include <mutex>

#include "ControlBoardMsgs.h"
#include <yarp/dev/ReturnValue.h>

#include <yarp/dev/IJointBrake.h>

class ControlBoardRPCd : public ControlBoardMsgs
{
    private:
    mutable std::mutex             m_mutex;
    yarp::dev::IJointBrake*        m_iJointBrake = nullptr;

    public:
    ControlBoardRPCd(yarp::dev::IJointBrake* iJointBrake)
    {
        m_iJointBrake = iJointBrake;
    }

    //IJointBrake
    return_isJointBraked isJointBraked_RPC(const std::int16_t j) const override;
    yarp::dev::ReturnValue setManualBrakeActive_RPC(const std::int16_t j, const bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabled_RPC(const std::int16_t j, const bool enabled) override;
    return_getAutoBrakeEnabled getAutoBrakeEnabled_RPC(const std::int16_t j) const override;


    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
