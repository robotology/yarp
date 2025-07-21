/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// ---------------------------------------------------------------
// TYPES DEFINITIONS
// ---------------------------------------------------------------

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yMatrix{
} (
  yarp.name = "yarp::sig::Matrix"
  yarp.includefile = "yarp/sig/Matrix.h"
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

struct yStamp{
} (
  yarp.name = "yarp::os::Stamp"
  yarp.includefile = "yarp/os/Stamp.h"
)

struct yFlexImage{
} (
  yarp.name = "yarp::sig::FlexImage"
  yarp.includefile = "yarp/sig/Image.h"
)

struct yDepthImage{
} (
  yarp.name = "yarp::sig::ImageOf<yarp::sig::PixelFloat>"
  yarp.includefile = "yarp/sig/Image.h"
)

enum ySensorStatus {
} (
  yarp.name = "yarp::dev::IRGBDSensor::RGBDSensor_status"
  yarp.includefile = "yarp/dev/IRGBDSensor.h"
  yarp.enumbase = "int32_t"
)

enum yFeatureMode {
} (
  yarp.name = "yarp::dev::FeatureMode"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
)

enum yBusType {
} (
  yarp.name = "yarp::dev::BusType"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
)

struct yCameraDescriptor {
  1: yBusType busType;
  2: string deviceDescription;
}

// ---------------------------------------------------------------
// IRGBDMsgs return types
// ---------------------------------------------------------------

struct IRGBDMsgs_return_getExtrinsic {
  1: yReturnValue ret;
  2: yMatrix matrix;
}

struct IRGBDMsgs_return_getLastErrorMsg {
  1: yReturnValue ret;
  2: string errorMsg;
  3: yStamp stamp;
}

struct IRGBDMsgs_return_getRgbImage {
  1: yReturnValue ret;
  2: yFlexImage img;
  3: yStamp stamp;
}

struct IRGBDMsgs_return_getDepthImage {
  1: yReturnValue ret;
  2: yDepthImage img;
  3: yStamp stamp;
}

struct IRGBDMsgs_return_getImages {
  1: yReturnValue ret;
  2: yFlexImage  rgbimg;
  3: yDepthImage depthimg;
  4: yStamp rgbstamp;
  5: yStamp depthstamp;
}

struct IRGBDMsgs_return_getSensorStatus {
  1: yReturnValue ret;
  2: ySensorStatus status;
}

// ---------------------------------------------------------------
// IRGBVisualParamsMsgs return types
// ---------------------------------------------------------------

struct IRGBVisualParamsMsgs_return_getRgbWidth {
  1: yReturnValue ret;
  2: i32 width;
}

struct IRGBVisualParamsMsgs_return_getRgbHeight {
  1: yReturnValue ret;
  2: i32 height;
}

struct IRGBVisualParamsMsgs_return_getRgbResolution {
  1: yReturnValue ret;
  2: i32 width;
  3: i32 height;
}

struct IRGBVisualParamsMsgs_return_getRgbSupportedCfg {
  1: yReturnValue ret;
  2: list<yCameraConfig> configuration;
}

struct IRGBVisualParamsMsgs_return_getRgbFOV {
  1: yReturnValue ret;
  2: double horizontalFov;
  3: double verticalFOV;
}

struct IRGBVisualParamsMsgs_return_getRgbIntrinsicParam {
  1: yReturnValue ret;
  2: yProperty params;
}

struct IRGBVisualParamsMsgs_return_getRgbMirroring {
  1: yReturnValue ret;
  2: bool mirror;
}

// ---------------------------------------------------------------
// IDepthVisualParamsMsgs return types
// ---------------------------------------------------------------

struct IDepthVisualParamsMsgs_return_getDepthWidth {
  1: yReturnValue ret;
  2: i32 width;
}

struct IDepthVisualParamsMsgs_return_getDepthHeight {
  1: yReturnValue ret;
  2: i32 height;
}

struct IDepthVisualParamsMsgs_return_getDepthResolution {
  1: yReturnValue ret;
  2: i32 width;
  3: i32 height;
}

struct IDepthVisualParamsMsgs_return_getDepthFOV {
  1: yReturnValue ret;
  2: double horizontalFov;
  3: double verticalFOV;
}

struct IDepthVisualParamsMsgs_return_getDepthAccuracy {
  1: yReturnValue ret;
  2: double accuracy;
}

struct IDepthVisualParamsMsgs_return_getDepthClipPlanes {
  1: yReturnValue ret;
  2: double nearPlane;
  3: double farPlane;
}

struct IDepthVisualParamsMsgs_return_getDepthMirroring {
  1: yReturnValue ret;
  2: bool mirror;
}

struct IDepthVisualParamsMsgs_return_getDepthIntrinsicParam {
  1: yReturnValue ret;
  2: yProperty params;
}

// ---------------------------------------------------------------
// IFrameGrabberControlMsgs return types
// ---------------------------------------------------------------

struct IFrameGrabberControlMsgs_return_getCameraDescription {
  1: yReturnValue ret;
  2: yCameraDescriptor camera;
}

struct IFrameGrabberControlMsgs_return_hasFeature {
  1: yReturnValue ret;
  2: bool hasFeature;
}

struct IFrameGrabberControlMsgs_return_getFeature1 {
  1: yReturnValue ret;
  2: double value;
}

struct IFrameGrabberControlMsgs_return_getFeature2 {
  1: yReturnValue ret;
  2: double value1;
  3: double value2;
}

struct IFrameGrabberControlMsgs_return_hasOnOff {
  1: yReturnValue ret;
  2: bool HasOnOff;
}

struct IFrameGrabberControlMsgs_return_getActive {
  1: yReturnValue ret;
  2: bool isActive;
}

struct IFrameGrabberControlMsgs_return_hasAuto {
  1: yReturnValue ret;
  3: bool hasAuto;
}

struct IFrameGrabberControlMsgs_return_hasManual {
  1: yReturnValue ret;
  2: bool hasManual;
}

struct IFrameGrabberControlMsgs_return_hasOnePush {
  1: yReturnValue ret;
  2: bool hasOnePush;
}

struct IFrameGrabberControlMsgs_return_getMode {
  1: yReturnValue ret;
  2: yFeatureMode mode;
}

// ---------------------------------------------------------------
// RGBDSensorMsgs service
// ---------------------------------------------------------------

service RGBDSensorMsgs
{
  IRGBDMsgs_return_getExtrinsic                getExtrinsicParamRPC();
  IRGBDMsgs_return_getLastErrorMsg             getLastErrorMsgRPC();
  IRGBDMsgs_return_getRgbImage                 getRgbImageRPC();
  IRGBDMsgs_return_getDepthImage               getDepthImageRPC();
  IRGBDMsgs_return_getImages                   getImagesRPC();
  IRGBDMsgs_return_getSensorStatus             getSensorStatusRPC();

  IRGBVisualParamsMsgs_return_getRgbWidth                 getRgbWidthRPC();
  IRGBVisualParamsMsgs_return_getRgbHeight                getRgbHeightRPC();
  IRGBVisualParamsMsgs_return_getRgbSupportedCfg          getRgbSupportedConfigurationsRPC();
  IRGBVisualParamsMsgs_return_getRgbResolution            getRgbResolutionRPC();
  yReturnValue                                            setRgbResolutionRPC(1:i32 width, 2:i32 height);
  IRGBVisualParamsMsgs_return_getRgbFOV                   getRgbFOVRPC();
  yReturnValue                                            setRgbFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IRGBVisualParamsMsgs_return_getRgbIntrinsicParam        getRgbIntrinsicParamRPC();
  IRGBVisualParamsMsgs_return_getRgbMirroring             getRgbMirroringRPC();
  yReturnValue                                            setRgbMirroringRPC(1:bool mirror);

  IDepthVisualParamsMsgs_return_getDepthWidth               getDepthWidthRPC();
  IDepthVisualParamsMsgs_return_getDepthHeight              getDepthHeightRPC();
  IDepthVisualParamsMsgs_return_getDepthResolution          getDepthResolutionRPC();
  yReturnValue                                              setDepthResolutionRPC(1:i32 width, 2:i32 height);
  IDepthVisualParamsMsgs_return_getDepthFOV                 getDepthFOVRPC();
  yReturnValue                                              setDepthFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IDepthVisualParamsMsgs_return_getDepthAccuracy            getDepthAccuracyRPC();
  yReturnValue                                              setDepthAccuracyRPC(1:double accuracy);
  IDepthVisualParamsMsgs_return_getDepthClipPlanes          getDepthClipPlanesRPC();
  yReturnValue                                              setDepthClipPlanesRPC(1:double nearPlane, 2:double farPlane);
  IDepthVisualParamsMsgs_return_getDepthMirroring           getDepthMirroringRPC();
  yReturnValue                                              setDepthMirroringRPC(1:bool mirror);
  IDepthVisualParamsMsgs_return_getDepthIntrinsicParam      getDepthIntrinsicParamRPC();

  IFrameGrabberControlMsgs_return_getCameraDescription      getCameraDescriptionRPC();
  IFrameGrabberControlMsgs_return_hasFeature                hasFeatureRPC(1:i32 feature);
  yReturnValue                                              setFeature1RPC(1:i32 feature, 2:double value);
  IFrameGrabberControlMsgs_return_getFeature1               getFeature1RPC(1:i32 feature);
  yReturnValue                                              setFeature2RPC(1:i32 feature, 2:double value1, 3:double value2);
  IFrameGrabberControlMsgs_return_getFeature2               getFeature2RPC(1:i32 feature);
  IFrameGrabberControlMsgs_return_hasOnOff                  hasOnOffRPC(1:i32 feature);
  yReturnValue                                              setActiveRPC(1:i32 feature, 2:bool onoff);
  IFrameGrabberControlMsgs_return_getActive                 getActiveRPC(1:i32 feature);
  IFrameGrabberControlMsgs_return_hasAuto                   hasAutoRPC(1:i32 feature );
  IFrameGrabberControlMsgs_return_hasManual                 hasManualRPC(1:i32 feature);
  IFrameGrabberControlMsgs_return_hasOnePush                hasOnePushRPC(1:i32 feature );
  yReturnValue                                              setModeRPC(1:i32 feature, 2:yFeatureMode mode);
  IFrameGrabberControlMsgs_return_getMode                   getModeRPC(1:i32 feature);
  yReturnValue                                              setOnePushRPC(1:i32 feature);
}
