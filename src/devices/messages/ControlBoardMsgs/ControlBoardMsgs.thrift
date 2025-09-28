/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yInfoPid {
} (
  yarp.name = "yarp::dev::PidExtraInfo"
  yarp.includefile = "yarp/dev/ControlBoardPid.h"
)

struct yPid {
} (
  yarp.name = "yarp::dev::Pid"
  yarp.includefile = "yarp/dev/ControlBoardPid.h"
)

enum yPidControlTypeEnum {
} (
  yarp.name = "yarp::dev::PidControlTypeEnum"
  yarp.includefile = "yarp/dev/PidEnums.h"
  yarp.enumbase = "int32_t"
)


//-------------------------------------------------
// IJointBrake
struct return_isJointBraked {
  1: yReturnValue ret;
  2: bool isBraked;
}

struct return_getAutoBrakeEnabled {
  1: yReturnValue ret;
  2: bool enabled;
}

//-------------------------------------------------
// IVelocityDirect
struct return_getRefVelocityOne {
  1: yReturnValue ret;
  2: double vel;
}

struct return_getRefVelocityAll {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getRefVelocityGroup {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getAxes {
  1: yReturnValue ret;
  2: i32 axes;
}

//-------------------------------------------------
// IPidControl
struct return_getPid {
  1: yReturnValue ret;
  2: yPid pid;
}

struct return_getPids {
  1: yReturnValue ret;
  2: list<yPid> pids;
}

struct return_getPidExtraInfo {
  1: yReturnValue ret;
  2: yInfoPid info;
}

struct return_getPidExtraInfos {
  1: yReturnValue ret;
  2: list<yInfoPid> info;
}

struct return_getPidOffset {
  1: yReturnValue ret;
  2: double offset;
}

struct return_getPidFeedforward {
  1: yReturnValue ret;
  2: double feedforward;
}

struct return_isPidEnabled {
  1: yReturnValue ret;
  2: bool isEnabled;
}

struct return_getPidError {
  1: yReturnValue ret;
  2: double err;
}

struct return_getPidErrors {
  1: yReturnValue ret;
  2: list<double> errs;
}

struct return_getPidReference {
  1: yReturnValue ret;
  2: double ref;
}

struct return_getPidReferences {
  1: yReturnValue ret;
  2: list<double> refs;
}

struct return_getPidErrorLimit {
  1: yReturnValue ret;
  2: double lim;
}

struct return_getPidErrorLimits {
  1: yReturnValue ret;
  2: list<double> lims;
}

struct return_getPidOutput {
  1: yReturnValue ret;
  2: double out;
}

struct return_getPidOutputs {
  1: yReturnValue ret;
  2: list<double> outs;
}

//-------------------------------------------------
// ILimits

struct return_getPosLimits {
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

struct return_getVelLimits {
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

//-------------------------------------------------
service ControlBoardMsgs
{
    yReturnValue               setManualBrakeActiveRPC(1: i32 j, 2:bool active);
    yReturnValue               setAutoBrakeEnabledRPC(1: i32 j, 2:bool enabled);
    return_isJointBraked       isJointBrakedRPC(1:i32 j) (yarp.qualifier = "const");
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(1: i32 j) (yarp.qualifier = "const");

    return_getAxes              getAxesRPC() (yarp.qualifier = "const");
    return_getRefVelocityOne    getRefVelocityOneRPC(1:i32 j) (yarp.qualifier = "const");
    return_getRefVelocityAll    getRefVelocityAllRPC() (yarp.qualifier = "const");
    return_getRefVelocityGroup  getRefVelocityGroupRPC(1: list<i32> j) (yarp.qualifier = "const");

    yReturnValue               setPosLimitsRPC(1: i16 j, 2: double min, 3: double max);
    yReturnValue               setVelLimitsRPC(1: i16 j, 2: double min, 3: double max);
    return_getPosLimits        getPosLimitsRPC(1: i16 j);
    return_getVelLimits        getVelLimitsRPC(2: i16 j);

    yReturnValue               enablePidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               disablePidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               resetPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               setPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: yPid pid);
    yReturnValue               setPidsRPC(1: yPidControlTypeEnum pidtype, 2: list<yPid> pids);
    yReturnValue               setPidReferenceRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double ref);
    yReturnValue               setPidReferencesRPC(1: yPidControlTypeEnum pidtype, 2: list<double> refs);
    yReturnValue               setPidErrorLimitRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double limit);
    yReturnValue               setPidErrorLimitsRPC(1: yPidControlTypeEnum pidtype, 2: list<double> limits);
    return_isPidEnabled        isPidEnabledRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidError         getPidErrorRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidErrors        getPidErrorsRPC(1: yPidControlTypeEnum pidtype);
    return_getPidReference     getPidReferenceRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidReferences    getPidReferencesRPC(1: yPidControlTypeEnum pidtype);
    return_getPidErrorLimit    getPidErrorLimitRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidErrorLimits   getPidErrorLimitsRPC(1: yPidControlTypeEnum pidtype);
    return_getPidOutput        getPidOutputRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidOutputs       getPidOutputsRPC(1: yPidControlTypeEnum pidtype);
    return_getPid              getPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPids             getPidsRPC(1: yPidControlTypeEnum pidtype);
    return_getPidExtraInfo     getPidExtraInfoRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidExtraInfos    getPidExtraInfosRPC(1: yPidControlTypeEnum pidtype);
    return_getPidOffset        getPidOffsetRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    return_getPidFeedforward   getPidFeedforwardRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
}
