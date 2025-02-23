/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

// ---------------------------------------------------------------

struct return_getVideoModeMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getVideoModeDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getFPSMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getFPSDC1394 {
  1: yReturnValue ret;
  2: i32 fps;
}

struct return_getISOSpeedDC1394 {
  1: yReturnValue ret;
  2: i32 speed;
}

struct return_getColorCodingMaskDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getColorCodingDC1394 {
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getFormat7MaxWindowDC1394 {
  1: yReturnValue ret;
  2: i32 xdim;
  3: i32 ydim;
  4: i32 xstep;
  5: i32 ystep;
  6: i32 xoffstep;
  7: i32 yoffstep;
}

struct return_getFormat7WindowDC1394 {
  1: yReturnValue ret;
  2: i32 xdim;
  3: i32 ydim;
  4: i32 x0;
  5: i32 y0;
}

struct return_getOperationModeDC1394 {
  1: yReturnValue ret;
  2: bool b1394b;
}

struct return_getTransmissionDC1394 {
  1: yReturnValue ret;
  2: bool bTxON;
}

struct return_getBytesPerPacketDC1394 {
  1: yReturnValue ret;
  2: i32 bpp;
}



// ---------------------------------------------------------------

service IFrameGrabberControlDC1394Msgs
{
  return_getVideoModeMaskDC1394    getVideoModeMaskDC1394RPC();
  return_getVideoModeDC1394        getVideoModeDC1394RPC();
  yReturnValue                     setVideoModeDC1394RPC(1:i32 videomode);

  return_getFPSMaskDC1394          getFPSMaskDC1394RPC();
  return_getFPSDC1394              getFPSDC1394RPC();
  yReturnValue                     setFPSDC1394RPC(1:i32 fps);

  return_getISOSpeedDC1394         getISOSpeedDC1394RPC();
  yReturnValue                     setISOSpeedDC1394RPC(1:i32 speed);

  return_getColorCodingMaskDC1394  getColorCodingMaskDC1394RPC(1:i32 videomode);
  return_getColorCodingDC1394      getColorCodingDC1394RPC();
  yReturnValue                     setColorCodingDC1394RPC(1:i32 coding);

  return_getFormat7MaxWindowDC1394 getFormat7MaxWindowDC1394RPC();
  return_getFormat7WindowDC1394    getFormat7WindowDC1394RPC();
  yReturnValue                     setFormat7WindowDC1394RPC(1:i32 xdim, 2:i32 ydim, 3:i32 x0, 4:i32 y0);

  yReturnValue                     setOperationModeDC1394RPC(1:bool b1394b);
  return_getOperationModeDC1394    getOperationModeDC1394RPC();
  yReturnValue                     setTransmissionDC1394RPC(1:bool bTxON);
  return_getTransmissionDC1394     getTransmissionDC1394RPC();

  yReturnValue                     setBroadcastDC1394RPC(1:bool onoff);
  yReturnValue                     setDefaultsDC1394RPC();
  yReturnValue                     setResetDC1394RPC();
  yReturnValue                     setPowerDC1394RPC(1:bool onoff);

  yReturnValue                     setCaptureDC1394RPC (1:bool bON);
  return_getBytesPerPacketDC1394   getBytesPerPacketDC1394RPC();
  yReturnValue                     setBytesPerPacketDC1394RPC(1: i32 bpp);
}
