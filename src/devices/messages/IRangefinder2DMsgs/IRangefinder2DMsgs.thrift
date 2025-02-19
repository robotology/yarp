/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

enum yarp_dev_IRangefinder2D_Device_status{
} (
  yarp.name = "yarp::dev::IRangefinder2D::Device_status"
  yarp.includefile="yarp/dev/IRangefinder2D.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

//-------------------------------------------------

struct return_getDeviceStatus {
  1: yReturnValue retval;
  2: yarp_dev_IRangefinder2D_Device_status status;
}
struct return_getDistanceRange {
  1: yReturnValue retval;
  2: double min;
  3: double max;
}
struct return_getScanLimits {
  1: yReturnValue retval;
  2: double min;
  3: double max;
}
struct return_getHorizontalResolution {
  1: yReturnValue retval;
  2: double step;
}
struct return_getScanRate {
  1: yReturnValue retval;
  2: double rate;
}
struct return_getDeviceInfo {
  1: yReturnValue retval;
  2: string device_info
}

//-------------------------------------------------

service IRangefinder2DMsgs
{
    return_getDeviceStatus getDeviceStatus_RPC();
    return_getDistanceRange getDistanceRange_RPC();
    return_getScanLimits getScanLimits_RPC();
    return_getHorizontalResolution getHorizontalResolution_RPC();
    return_getScanRate getScanRate_RPC();
    return_getDeviceInfo getDeviceInfo_RPC();
    yReturnValue setDistanceRange_RPC(1:double min, 2:double max);
    yReturnValue setScanLimits_RPC(1:double min, 2:double max);
    yReturnValue setHorizontalResolution_RPC(1:double step);
    yReturnValue setScanRate_RPC(1:double rate);
}
