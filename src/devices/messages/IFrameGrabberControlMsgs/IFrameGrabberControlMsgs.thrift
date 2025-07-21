/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

enum yBusType {
} (
  yarp.name = "yarp::dev::BusType"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
)

enum yFeatureMode {
} (
  yarp.name = "yarp::dev::FeatureMode"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
)

// ---------------------------------------------------------------
struct yCameraDescriptor {
  1: yBusType busType;
  2: string deviceDescription;
}

struct return_getCameraDescription {
  1: yReturnValue ret;
  2: yCameraDescriptor camera;
}

struct return_hasFeature {
  1: yReturnValue ret;
  2: bool hasFeature;
}

struct return_getFeature1 {
  1: yReturnValue ret;
  2: double value;
}

struct return_getFeature2 {
  1: yReturnValue ret;
  2: double value1;
  3: double value2;
}

struct return_hasOnOff {
  1: yReturnValue ret;
  2: bool HasOnOff;
}

struct return_getActive {
  1: yReturnValue ret;
  2: bool isActive;
}

struct return_hasAuto {
  1: yReturnValue ret;
  3: bool hasAuto;
}

struct return_hasManual {
  1: yReturnValue ret;
  2: bool hasManual;
}

struct return_hasOnePush {
  1: yReturnValue ret;
  2: bool hasOnePush;
}

struct return_getMode {
  1: yReturnValue ret;
  2: yFeatureMode mode;
}

// ---------------------------------------------------------------

service IFrameGrabberControlMsgs
{
  return_getCameraDescription getCameraDescriptionRPC();
  return_hasFeature  hasFeatureRPC(1:i32 feature);
  yReturnValue       setFeature1RPC(1:i32 feature, 2:double value);
  return_getFeature1 getFeature1RPC(1:i32 feature);
  yReturnValue       setFeature2RPC(1:i32 feature, 2:double value1, 3:double value2);
  return_getFeature2 getFeature2RPC(1:i32 feature);
  return_hasOnOff    hasOnOffRPC(1:i32 feature);
  yReturnValue       setActiveRPC(1:i32 feature, 2:bool onoff);
  return_getActive   getActiveRPC(1:i32 feature);
  return_hasAuto     hasAutoRPC(1:i32 feature );
  return_hasManual   hasManualRPC(1:i32 feature);
  return_hasOnePush  hasOnePushRPC(1:i32 feature );
  yReturnValue       setModeRPC(1:i32 feature, 2:yFeatureMode mode);
  return_getMode     getModeRPC(1:i32 feature);
  yReturnValue       setOnePushRPC(1:i32 feature);
}
