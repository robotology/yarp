/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yProperty {
} (
  yarp.name = "yarp::os::Property"
  yarp.includefile = "yarp/os/Property.h"
)

// ---------------------------------------------------------------

struct return_getDepthWidth {
  1: yReturnValue ret;
  2: i32 width;
}

struct return_getDepthHeight {
  1: yReturnValue ret;
  2: i32 height;
}

struct return_getDepthResolution {
  1: yReturnValue ret;
  2: i32 width;
  3: i32 height;
}

struct return_getDepthFOV {
  1: yReturnValue ret;
  2: double horizontalFov;
  3: double verticalFOV;
}

struct return_getDepthAccuracy {
  1: yReturnValue ret;
  2: double accuracy;
}

struct return_getDepthClipPlanes {
  1: yReturnValue ret;
  2: double nearPlane;
  3: double farPlane;
}

struct return_getDepthMirroring {
  1: yReturnValue ret;
  2: bool mirror;
}

struct return_getDepthIntrinsicParam {
  1: yReturnValue ret;
  2: yProperty params;
}

// ---------------------------------------------------------------

service IDepthVisualParamsMsgs
{
  return_getDepthWidth               getDepthWidthRPC();
  return_getDepthHeight              getDepthHeightRPC();
  return_getDepthResolution          getDepthResolutionRPC();
  yReturnValue                       setDepthResolutionRPC(1:i32 width, 2:i32 height);
  return_getDepthFOV                 getDepthFOVRPC();
  yReturnValue                       setDepthFOVRPC(1:double horizontalFov, 2:double verticalFov);
  return_getDepthAccuracy            getDepthAccuracyRPC();
  yReturnValue                       setDepthAccuracyRPC(1:double accuracy);
  return_getDepthClipPlanes          getDepthClipPlanesRPC();
  yReturnValue                       setDepthClipPlanesRPC(1:double nearPlane, 2:double farPlane);
  return_getDepthMirroring           getDepthMirroringRPC();
  yReturnValue                       setDepthMirroringRPC(1:bool mirror);
  return_getDepthIntrinsicParam      getDepthIntrinsicParamRPC();
}
