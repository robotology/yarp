/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPIDCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPIDCONTROL_H

#include <yarp/dev/IPidControl.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperPidControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IPidControl
{
public:
    bool setPid(const yarp::dev::PidControlTypeEnum& pidtype, int j, const yarp::dev::Pid& p) override;
    bool setPids(const yarp::dev::PidControlTypeEnum& pidtype, const yarp::dev::Pid* ps) override;
    bool setPidReference(const yarp::dev::PidControlTypeEnum& pidtype, int j, double ref) override;
    bool setPidReferences(const yarp::dev::PidControlTypeEnum& pidtype, const double* refs) override;
    bool setPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype, int j, double limit) override;
    bool setPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype, const double* limits) override;
    bool getPidError(const yarp::dev::PidControlTypeEnum& pidtype, int j, double* err) override;
    bool getPidErrors(const yarp::dev::PidControlTypeEnum& pidtype, double* errs) override;
    bool getPidOutput(const yarp::dev::PidControlTypeEnum& pidtype, int j, double* out) override;
    bool getPidOutputs(const yarp::dev::PidControlTypeEnum& pidtype, double* outs) override;
    bool setPidOffset(const yarp::dev::PidControlTypeEnum& pidtype, int j, double v) override;
    bool getPid(const yarp::dev::PidControlTypeEnum& pidtype, int j, yarp::dev::Pid* p) override;
    bool getPids(const yarp::dev::PidControlTypeEnum& pidtype, yarp::dev::Pid* pids) override;
    bool getPidReference(const yarp::dev::PidControlTypeEnum& pidtype, int j, double* ref) override;
    bool getPidReferences(const yarp::dev::PidControlTypeEnum& pidtype, double* refs) override;
    bool getPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype, int j, double* limit) override;
    bool getPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype, double* limits) override;
    bool resetPid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool disablePid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool enablePid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool isPidEnabled(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool* enabled) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPIDCONTROL_H
