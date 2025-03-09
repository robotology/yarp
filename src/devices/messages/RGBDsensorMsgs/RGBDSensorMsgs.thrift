/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

include "../IRGBDMsgs/IRGBDMsgs.thrift"
include "../IRGBVisualParamsMsgs/IRGBVisualParamsMsgs.thrift"
include "../IDepthVisualParamsMsgs/IDepthVisualParamsMsgs.thrift"
include "../IFrameGrabberControlMsgs/IFrameGrabberControlMsgs.thrift"


// ---------------------------------------------------------------
// Those two definitions should be avoided and replaced by
// IRGBDMsgs.yReturnValue and IFrameGrabberControlMsgs.yFeatureMode
// Unfortunately this is not supported by thrift compiler so far, so this
// is a temp workaround. 

struct yReturnValue2 {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

enum yFeatureMode2 {
} (
  yarp.name = "yarp::dev::FeatureMode"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
)

// ---------------------------------------------------------------

service RGBDSensorMsgs
{
  IRGBDMsgs.return_getExtrinsic                getExtrinsicParamRPC();
  IRGBDMsgs.return_getLastErrorMsg             getLastErrorMsgRPC();
  IRGBDMsgs.return_getRgbImage                 getRgbImageRPC();
  IRGBDMsgs.return_getDepthImage               getDepthImageRPC();
  IRGBDMsgs.return_getImages                   getImagesRPC();
  IRGBDMsgs.return_getSensorStatus             getSensorStatusRPC();

  IRGBVisualParamsMsgs.return_getRgbWidth                 getRgbWidthRPC();
  IRGBVisualParamsMsgs.return_getRgbHeight                getRgbHeightRPC();
  IRGBVisualParamsMsgs.return_getRgbSupportedCfg          getRgbSupportedConfigurationsRPC();
  IRGBVisualParamsMsgs.return_getRgbResolution            getRgbResolutionRPC();
  yReturnValue2                                           setRgbResolutionRPC(1:i32 width, 2:i32 height);
  IRGBVisualParamsMsgs.return_getRgbFOV                   getRgbFOVRPC();
  yReturnValue2                                           setRgbFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IRGBVisualParamsMsgs.return_getRgbIntrinsicParam        getRgbIntrinsicParamRPC();
  IRGBVisualParamsMsgs.return_getRgbMirroring             getRgbMirroringRPC();
  yReturnValue2                                           setRgbMirroringRPC(1:bool mirror);
  
  IDepthVisualParamsMsgs.return_getDepthWidth               getDepthWidthRPC();
  IDepthVisualParamsMsgs.return_getDepthHeight              getDepthHeightRPC();
  IDepthVisualParamsMsgs.return_getDepthResolution          getDepthResolutionRPC();
  yReturnValue2                                             setDepthResolutionRPC(1:i32 width, 2:i32 height);
  IDepthVisualParamsMsgs.return_getDepthFOV                 getDepthFOVRPC();
  yReturnValue2                                             setDepthFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IDepthVisualParamsMsgs.return_getDepthAccuracy            getDepthAccuracyRPC();
  yReturnValue2                                             setDepthAccuracyRPC(1:double accuracy);
  IDepthVisualParamsMsgs.return_getDepthClipPlanes          getDepthClipPlanesRPC();
  yReturnValue2                                             setDepthClipPlanesRPC(1:double nearPlane, 2:double farPlane);
  IDepthVisualParamsMsgs.return_getDepthMirroring           getDepthMirroringRPC();
  yReturnValue2                                             setDepthMirroringRPC(1:bool mirror);
  IDepthVisualParamsMsgs.return_getDepthIntrinsicParam      getDepthIntrinsicParamRPC();

  IFrameGrabberControlMsgs.return_getCameraDescription      getCameraDescriptionRPC();
  IFrameGrabberControlMsgs.return_hasFeature                hasFeatureRPC(1:i32 feature);
  yReturnValue2                                             setFeature1RPC(1:i32 feature, 2:double value);
  IFrameGrabberControlMsgs.return_getFeature1               getFeature1RPC(1:i32 feature);
  yReturnValue2                                             setFeature2RPC(1:i32 feature, 2:double value1, 3:double value2);
  IFrameGrabberControlMsgs.return_getFeature2               getFeature2RPC(1:i32 feature);
  IFrameGrabberControlMsgs.return_hasOnOff                  hasOnOffRPC(1:i32 feature);
  yReturnValue2                                             setActiveRPC(1:i32 feature, 2:bool onoff);
  IFrameGrabberControlMsgs.return_getActive                 getActiveRPC(1:i32 feature);
  IFrameGrabberControlMsgs.return_hasAuto                   hasAutoRPC(1:i32 feature );
  IFrameGrabberControlMsgs.return_hasManual                 hasManualRPC(1:i32 feature);
  IFrameGrabberControlMsgs.return_hasOnePush                hasOnePushRPC(1:i32 feature );
  yReturnValue2                                             setModeRPC(1:i32 feature, 2:yFeatureMode2 mode);
  IFrameGrabberControlMsgs.return_getMode                   getModeRPC(1:i32 feature);
  yReturnValue2                                             setOnePushRPC(1:i32 feature);
}
