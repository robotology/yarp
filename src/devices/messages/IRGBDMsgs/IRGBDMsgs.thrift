/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

// ---------------------------------------------------------------

struct return_getExtrinsic {
  1: yReturnValue ret;
  2: yMatrix matrix;
}

struct return_getLastErrorMsg {
  1: yReturnValue ret;
  2: string errorMsg;
  3: yStamp stamp;
}

struct return_getRgbImage {
  1: yReturnValue ret;
  2: yFlexImage img;
  3: yStamp stamp;
}

struct return_getDepthImage {
  1: yReturnValue ret;
  2: yDepthImage img;
  3: yStamp stamp;
}

struct return_getImages {
  1: yReturnValue ret;
  2: yFlexImage  rgbimg;
  3: yDepthImage depthimg;
  4: yStamp rgbstamp;
  5: yStamp depthstamp;
}

struct return_getSensorStatus {
  1: yReturnValue ret;
  2: ySensorStatus status;
}

// ---------------------------------------------------------------

service IRGBDMsgs
{
  return_getExtrinsic         getExtrinsicParamRPC();
  return_getLastErrorMsg      getLastErrorMsgRPC();
  return_getRgbImage          getRgbImageRPC();
  return_getDepthImage        getDepthImageRPC();
  return_getImages            getImagesRPC();
  return_getSensorStatus      getSensorStatusRPC();
}
