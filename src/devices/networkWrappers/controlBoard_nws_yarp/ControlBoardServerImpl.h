/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_CONTROLBOARDSERVERIMPL_H
#define YARP_DEV_CONTROLBOARDSERVERIMPL_H

#include <mutex>

#include "ControlBoardMsgs.h"
#include <yarp/dev/ControlBoardInterfaces.h>

class ControlBoardRPCd : public ControlBoardMsgs
{
    private:
    mutable std::mutex             m_mutex;
    yarp::dev::IJointBrake*        m_iJointBrake = nullptr;
    yarp::dev::IVelocityDirect*    m_iVelocityDirect = nullptr;

    public:
    ControlBoardRPCd(yarp::dev::IJointBrake* iJointBrake,
                     yarp::dev::IVelocityDirect* IVelocityDirect)
    {
        m_iJointBrake = iJointBrake;
        m_iVelocityDirect = IVelocityDirect;
    }

    //IJointBrake
    return_isJointBraked isJointBrakedRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setManualBrakeActiveRPC(const std::int32_t j, const bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabledRPC(const std::int32_t j, const bool enabled) override;
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(const std::int32_t j) const override;

    // IVelocityDirect
    return_getAxes               getAxesRPC() const override;
    return_getDesiredVelocityOne getDesiredVelocityOneRPC(const std::int32_t j) const override;
    return_getDesiredVelocityAll getDesiredVelocityAllRPC() const override;
    return_getDesiredVelocityGroup getDesiredVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
