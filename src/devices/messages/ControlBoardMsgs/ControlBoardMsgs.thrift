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
//-------------------------------------------------

service ControlBoardMsgs
{
    return_isJointBraked       isJointBrakedRPC(1:i16 j) (yarp.qualifier = "const");
    yReturnValue               setManualBrakeActiveRPC(1: i16 j, 2:bool active);
    yReturnValue               setAutoBrakeEnabledRPC(1: i16 j, 2:bool enabled);
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(1: i16 j) (yarp.qualifier = "const");

}
