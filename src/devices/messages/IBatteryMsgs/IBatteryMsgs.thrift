/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_get_BatteryInfo{
    1: yReturnValue result;
    2: string info;
}

//-------------------------------------------------

service IBatteryMsgs
{
    return_get_BatteryInfo getBatteryInfoRPC ();
}
