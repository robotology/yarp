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
    yarp::dev::IControlLimits*     m_iLimits = nullptr;

    public:
    ControlBoardRPCd(size_t njoints,
                     yarp::dev::IJointBrake* iJointBrake,
                     yarp::dev::IVelocityDirect* IVelocityDirect,
                     yarp::dev::IPidControl* iPid,
                     yarp::dev::IControlLimits* iLim)
    {
        m_njoints = njoints;
        m_iJointBrake = iJointBrake;
        m_iVelocityDirect = IVelocityDirect;
        m_iPidControl = iPid;
        m_iLimits=iLim;
    }

    //IJointBrake
    return_isJointBraked isJointBrakedRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setManualBrakeActiveRPC(const std::int32_t j, const bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabledRPC(const std::int32_t j, const bool enabled) override;
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(const std::int32_t j) const override;

    // IVelocityDirect
    return_getAxes               getAxesRPC() const override;
    return_getRefVelocityOne getRefVelocityOneRPC(const std::int32_t j) const override;
    return_getRefVelocityAll getRefVelocityAllRPC() const override;
    return_getRefVelocityGroup getRefVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const override;

    // IPidControl
    yarp::dev::ReturnValue enablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue disablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue resetPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_isPidEnabled isPidEnabledRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue setPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const yarp::dev::Pid& pid) override;
    yarp::dev::ReturnValue setPidsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<yarp::dev::Pid>& pids) override;
    return_getPid getPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_getPids getPidsRPC(const yarp::dev::PidControlTypeEnum pidtype) override;
    return_getPidExtraInfo getPidExtraInfoRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_getPidExtraInfos getPidExtraInfosRPC(const yarp::dev::PidControlTypeEnum pidtypeo) override;
    return_getPidOffset getPidOffsetRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_getPidFeedforward getPidFeedforwardRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue setPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double ref) override;
    yarp::dev::ReturnValue setPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& refs)override;
    yarp::dev::ReturnValue setPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double limit)override;
    yarp::dev::ReturnValue setPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& limits)override;
    return_getPidError getPidErrorRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)override;
    return_getPidErrors getPidErrorsRPC(const yarp::dev::PidControlTypeEnum pidtype)override;
    return_getPidReference getPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)override;
    return_getPidReferences getPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype)override;
    return_getPidErrorLimit getPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)override;
    return_getPidErrorLimits getPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype)override;
    return_getPidOutput getPidOutputRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)override;
    return_getPidOutputs getPidOutputsRPC(const yarp::dev::PidControlTypeEnum pidtype)override;

    // ILimits
    yarp::dev::ReturnValue setPosLimitsRPC(const std::int16_t j, const double min, const double max) override;
    yarp::dev::ReturnValue setVelLimitsRPC(const std::int16_t j, const double min, const double max) override;
    return_getPosLimits getPosLimitsRPC(const std::int16_t j) override;
    return_getVelLimits getVelLimitsRPC(const std::int16_t j) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
