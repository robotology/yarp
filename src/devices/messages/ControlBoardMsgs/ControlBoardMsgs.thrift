/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
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

struct return_getDesiredVelocityOne {
  1: yReturnValue ret;
  2: double vel;
}

struct return_getDesiredVelocityAll {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getDesiredVelocityGroup {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getAxes {
  1: yReturnValue ret;
  2: i32 axes;
}

//-------------------------------------------------
service ControlBoardMsgs
{
    return_isJointBraked       isJointBrakedRPC(1:i32 j) (yarp.qualifier = "const");
    yReturnValue               setManualBrakeActiveRPC(1: i32 j, 2:bool active);
    yReturnValue               setAutoBrakeEnabledRPC(1: i32 j, 2:bool enabled);
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(1: i32 j) (yarp.qualifier = "const");

    return_getAxes                  getAxesRPC() (yarp.qualifier = "const");
    return_getDesiredVelocityOne    getDesiredVelocityOneRPC(1:i32 j) (yarp.qualifier = "const");
    return_getDesiredVelocityAll    getDesiredVelocityAllRPC() (yarp.qualifier = "const");
    return_getDesiredVelocityGroup  getDesiredVelocityGroupRPC(1: list<i32> j) (yarp.qualifier = "const");
}
