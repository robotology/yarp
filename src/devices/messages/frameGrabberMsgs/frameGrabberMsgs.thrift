/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

include "../IRGBVisualParamsMsgs/IRGBVisualParamsMsgs.thrift"
include "../IFrameGrabberControlMsgs/IFrameGrabberControlMsgs.thrift"
include "../IFrameGrabberControlDC1394Msgs/IFrameGrabberControlDC1394Msgs.thrift"

// ---------------------------------------------------------------
// Those definitions should be avoided and replaced by
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

service FrameGrabberMsgs
{
  IRGBVisualParamsMsgs.return_getRgbWidth                   getRgbWidthRPC();
  IRGBVisualParamsMsgs.return_getRgbHeight                  getRgbHeightRPC();
  IRGBVisualParamsMsgs.return_getRgbSupportedCfg            getRgbSupportedConfigurationsRPC();
  IRGBVisualParamsMsgs.return_getRgbResolution              getRgbResolutionRPC();
  yReturnValue2                                             setRgbResolutionRPC(1:i32 width, 2:i32 height);
  IRGBVisualParamsMsgs.return_getRgbFOV                     getRgbFOVRPC();
  yReturnValue2                                             setRgbFOVRPC(1:double horizontalFov, 2:double verticalFov);
  IRGBVisualParamsMsgs.return_getRgbIntrinsicParam          getRgbIntrinsicParamRPC();
  IRGBVisualParamsMsgs.return_getRgbMirroring               getRgbMirroringRPC();
  yReturnValue2                                             setRgbMirroringRPC(1:bool mirror);

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

  IFrameGrabberControlDC1394Msgs.return_getVideoModeMaskDC1394    getVideoModeMaskDC1394RPC();
  IFrameGrabberControlDC1394Msgs.return_getVideoModeDC1394        getVideoModeDC1394RPC();
  yReturnValue2                                                   setVideoModeDC1394RPC(1:i32 videomode);
  IFrameGrabberControlDC1394Msgs.return_getFPSMaskDC1394          getFPSMaskDC1394RPC();
  IFrameGrabberControlDC1394Msgs.return_getFPSDC1394              getFPSDC1394RPC();
  yReturnValue2                                                   setFPSDC1394RPC(1:i32 fps);
  IFrameGrabberControlDC1394Msgs.return_getISOSpeedDC1394         getISOSpeedDC1394RPC();
  yReturnValue2                                                   setISOSpeedDC1394RPC(1:i32 speed);
  IFrameGrabberControlDC1394Msgs.return_getColorCodingMaskDC1394  getColorCodingMaskDC1394RPC(1:i32 videomode);
  IFrameGrabberControlDC1394Msgs.return_getColorCodingDC1394      getColorCodingDC1394RPC();
  yReturnValue2                                                   setColorCodingDC1394RPC(1:i32 coding);
  IFrameGrabberControlDC1394Msgs.return_getFormat7MaxWindowDC1394 getFormat7MaxWindowDC1394RPC();
  IFrameGrabberControlDC1394Msgs.return_getFormat7WindowDC1394    getFormat7WindowDC1394RPC();
  yReturnValue2                                                   setFormat7WindowDC1394RPC(1:i32 xdim, 2:i32 ydim, 3:i32 x0, 4:i32 y0);
  yReturnValue2                                                   setOperationModeDC1394RPC(1:bool b1394b);
  IFrameGrabberControlDC1394Msgs.return_getOperationModeDC1394    getOperationModeDC1394RPC();
  yReturnValue2                                                   setTransmissionDC1394RPC(1:bool bTxON);
  IFrameGrabberControlDC1394Msgs.return_getTransmissionDC1394     getTransmissionDC1394RPC();
  yReturnValue2                                                   setBroadcastDC1394RPC(1:bool onoff);
  yReturnValue2                                                   setDefaultsDC1394RPC();
  yReturnValue2                                                   setResetDC1394RPC();
  yReturnValue2                                                   setPowerDC1394RPC(1:bool onoff);
  yReturnValue2                                                   setCaptureDC1394RPC (1:bool bON);
  IFrameGrabberControlDC1394Msgs.return_getBytesPerPacketDC1394   getBytesPerPacketDC1394RPC();
  yReturnValue2                                                   setBytesPerPacketDC1394RPC(1: i32 bpp);
}
