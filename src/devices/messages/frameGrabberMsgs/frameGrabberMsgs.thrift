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

enum yFeatureMode {
} (
  yarp.name = "yarp::dev::FeatureMode"
  yarp.includefile = "yarp/dev/IFrameGrabberControls.h"
  yarp.enumbase = "int32_t"
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

struct yFlexImage {
} (
  yarp.name = "yarp::sig::FlexImage"
  yarp.includefile = "yarp/sig/Image.h"
)

enum yCropType {
} (
  yarp.name = "yarp::dev::cropType_id_t"
  yarp.includefile = "yarp/dev/IFrameGrabberImage.h"
  yarp.enumbase = "int32_t"
)

struct yVertex {
} (
  yarp.name = "yarp::dev::vertex_t"
  yarp.includefile = "yarp/dev/IFrameGrabberImage.h"
)

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
// IFrameGrabberControlDC1394Msgs return types
// ---------------------------------------------------------------

struct IFrameGrabberControlDC1394Msgs_return_getVideoModeMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberControlDC1394Msgs_return_getVideoModeDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberControlDC1394Msgs_return_getFPSMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberControlDC1394Msgs_return_getFPSDC1394 {
  1: yReturnValue ret;
  2: i32 fps;
}

struct IFrameGrabberControlDC1394Msgs_return_getISOSpeedDC1394 {
  1: yReturnValue ret;
  2: i32 speed;
}

struct IFrameGrabberControlDC1394Msgs_return_getColorCodingMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberControlDC1394Msgs_return_getColorCodingDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberControlDC1394Msgs_return_getFormat7MaxWindowDC1394 {
  1: yReturnValue ret;
  2: i32 xdim;
  3: i32 ydim;
  4: i32 xstep;
  5: i32 ystep;
  6: i32 xoffstep;
  7: i32 yoffstep;
}

struct IFrameGrabberControlDC1394Msgs_return_getFormat7WindowDC1394 {
  1: yReturnValue ret;
  2: i32 xdim;
  3: i32 ydim;
  4: i32 x0;
  5: i32 y0;
}

struct IFrameGrabberControlDC1394Msgs_return_getOperationModeDC1394 {
  1: yReturnValue ret;
  2: bool b1394b;
}

struct IFrameGrabberControlDC1394Msgs_return_getTransmissionDC1394 {
  1: yReturnValue ret;
  2: bool bTxON;
}

struct IFrameGrabberControlDC1394Msgs_return_getBytesPerPacketDC1394 {
  1: yReturnValue ret;
  2: i32 bpp;
}

// ---------------------------------------------------------------
// IFrameGrabberImageOf return types
// ---------------------------------------------------------------

struct IFrameGrabberImageOf_return_getHeight {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberImageOf_return_getWidth {
  1: yReturnValue ret;
  2: i32 val;
}

struct IFrameGrabberImageOf_return_getImage {
  1: yReturnValue ret;
  2: yFlexImage fImage;
}

struct IFrameGrabberImageOf_return_getImageCrop {
  1: yReturnValue ret;
  2: yFlexImage fImage;
}

// ---------------------------------------------------------------
// FrameGrabberMsgs service
// ---------------------------------------------------------------

service FrameGrabberMsgs
{
  IFrameGrabberImageOf_return_getHeight                     getHeightRPC();
  IFrameGrabberImageOf_return_getWidth                      getWidthRPC();
  IFrameGrabberImageOf_return_getImage                      getImageRPC();
  IFrameGrabberImageOf_return_getImageCrop                  getImageCropRPC(1:yCropType type, 2:list<yVertex> vertices);
  IRGBVisualParamsMsgs_return_getRgbWidth                   getRgbWidthRPC();
  IRGBVisualParamsMsgs_return_getRgbHeight                  getRgbHeightRPC();
  IRGBVisualParamsMsgs_return_getRgbSupportedCfg            getRgbSupportedConfigurationsRPC();
  IRGBVisualParamsMsgs_return_getRgbResolution              getRgbResolutionRPC();
  yReturnValue                                              setRgbResolutionRPC(1:i32 width, 2:i32 height);
  IRGBVisualParamsMsgs_return_getRgbFOV                     getRgbFOVRPC();
  yReturnValue                                              setRgbFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IRGBVisualParamsMsgs_return_getRgbIntrinsicParam          getRgbIntrinsicParamRPC();
  IRGBVisualParamsMsgs_return_getRgbMirroring               getRgbMirroringRPC();
  yReturnValue                                              setRgbMirroringRPC(1:bool mirror);

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

  IFrameGrabberControlDC1394Msgs_return_getVideoModeMaskDC1394    getVideoModeMaskDC1394RPC();
  IFrameGrabberControlDC1394Msgs_return_getVideoModeDC1394        getVideoModeDC1394RPC();
  yReturnValue                                                    setVideoModeDC1394RPC(1:i32 videomode);
  IFrameGrabberControlDC1394Msgs_return_getFPSMaskDC1394          getFPSMaskDC1394RPC();
  IFrameGrabberControlDC1394Msgs_return_getFPSDC1394              getFPSDC1394RPC();
  yReturnValue                                                    setFPSDC1394RPC(1:i32 fps);
  IFrameGrabberControlDC1394Msgs_return_getISOSpeedDC1394         getISOSpeedDC1394RPC();
  yReturnValue                                                    setISOSpeedDC1394RPC(1:i32 speed);
  IFrameGrabberControlDC1394Msgs_return_getColorCodingMaskDC1394  getColorCodingMaskDC1394RPC(1:i32 videomode);
  IFrameGrabberControlDC1394Msgs_return_getColorCodingDC1394      getColorCodingDC1394RPC();
  yReturnValue                                                    setColorCodingDC1394RPC(1:i32 coding);
  IFrameGrabberControlDC1394Msgs_return_getFormat7MaxWindowDC1394 getFormat7MaxWindowDC1394RPC();
  IFrameGrabberControlDC1394Msgs_return_getFormat7WindowDC1394    getFormat7WindowDC1394RPC();
  yReturnValue                                                    setFormat7WindowDC1394RPC(1:i32 xdim, 2:i32 ydim, 3:i32 x0, 4:i32 y0);
  yReturnValue                                                    setOperationModeDC1394RPC(1:bool b1394b);
  IFrameGrabberControlDC1394Msgs_return_getOperationModeDC1394    getOperationModeDC1394RPC();
  yReturnValue                                                    setTransmissionDC1394RPC(1:bool bTxON);
  IFrameGrabberControlDC1394Msgs_return_getTransmissionDC1394     getTransmissionDC1394RPC();
  yReturnValue                                                    setBroadcastDC1394RPC(1:bool onoff);
  yReturnValue                                                    setDefaultsDC1394RPC();
  yReturnValue                                                    setResetDC1394RPC();
  yReturnValue                                                    setPowerDC1394RPC(1:bool onoff);
  yReturnValue                                                    setCaptureDC1394RPC (1:bool bON);
  IFrameGrabberControlDC1394Msgs_return_getBytesPerPacketDC1394   getBytesPerPacketDC1394RPC();
  yReturnValue                                                    setBytesPerPacketDC1394RPC(1: i32 bpp);
}
