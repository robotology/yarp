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
    size_t                         m_njoints = 0;
    yarp::dev::IJointBrake*        m_iJointBrake = nullptr;
    yarp::dev::IVelocityDirect*    m_iVelocityDirect = nullptr;
    yarp::dev::IPidControl*        m_iPidControl = nullptr;

    public:
    ControlBoardRPCd(size_t njoints,
                     yarp::dev::IJointBrake* iJointBrake,
                     yarp::dev::IVelocityDirect* IVelocityDirect,
                     yarp::dev::IPidControl* iPid)
    {
        m_njoints = njoints;
        m_iJointBrake = iJointBrake;
        m_iVelocityDirect = IVelocityDirect;
        m_iPidControl = iPid;
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

    // IPidControl
    yarp::dev::ReturnValue setPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const yarp::dev::Pid& pid) override;
    yarp::dev::ReturnValue setPidsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<yarp::dev::Pid>& pids) override;
    return_getPid getPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_getPids getPidsRPC(const yarp::dev::PidControlTypeEnum pidtype) override;
    return_getInfoPid getPidExtraInfoRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_getInfoPids getPidExtraInfosRPC(const yarp::dev::PidControlTypeEnum pidtypeo) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
