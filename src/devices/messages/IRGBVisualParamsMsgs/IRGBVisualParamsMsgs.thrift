/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yCameraConfig {
} (
  yarp.name = "yarp::dev::CameraConfig"
  yarp.includefile = "yarp/dev/IRgbVisualParams.h"
)

struct yProperty {
} (
  yarp.name = "yarp::os::Property"
  yarp.includefile = "yarp/os/Property.h"
)

// ---------------------------------------------------------------

struct return_getRgbWidth {
  1: yReturnValue ret;
  2: i32 width;
}

struct return_getRgbHeight {
  1: yReturnValue ret;
  2: i32 height;
}

struct return_getRgbResolution {
  1: yReturnValue ret;
  2: i32 width;
  3: i32 height;
}

struct return_getRgbSupportedCfg {
  1: yReturnValue ret;
  2: list<yCameraConfig> configuration;
}

struct return_getRgbFOV {
  1: yReturnValue ret;
  2: double horizontalFov;
  3: double verticalFOV;
}

struct return_getRgbIntrinsicParam {
  1: yReturnValue ret;
  2: yProperty params;
}

struct return_getRgbMirroring {
  1: yReturnValue ret;
  2: bool mirror;
}
// ---------------------------------------------------------------

service IRGBVisualParamsMsgs
{
  return_getRgbWidth                 getRgbWidthRPC();
  return_getRgbHeight                getRgbHeightRPC();
  return_getRgbSupportedCfg          getRgbSupportedConfigurationsRPC();
  return_getRgbResolution            getRgbResolutionRPC();
  yReturnValue                       setRgbResolutionRPC(1:i32 width, 2:i32 height);
  return_getRgbFOV                   getRgbFOVRPC();
  yReturnValue                       setRgbFOVRPC(1:double horizontalFov, 2:double verticalFov);
  return_getRgbIntrinsicParam        getRgbIntrinsicParamRPC();
  return_getRgbMirroring             getRgbMirroringRPC();
  yReturnValue                       setRgbMirroringRPC(1:bool mirror);
}
