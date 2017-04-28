/*
* Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
* Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#include <math.h>
#include <algorithm>
#include <yarp/os/Value.h>
#include <map>
#include "fakeDepthCameraDriver.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace std;

fakeDepthCameraDriver::fakeDepthCameraDriver()
{
    rgb_h{480};
    rgb_w{640};
    dep_h{480};
    dep_w{640};
    accuracy{0.001};
    rgb_Vfov{36};
    rgb_Hfov{50};
    dep_Vfov{36};
    dep_Hfov{50};
    dep_near{0.4};
    dep_far{6};
}

fakeDepthCameraDriver::~fakeDepthCameraDriver()
{
}

bool fakeDepthCameraDriver::open(Searchable& config)
{
    Property cfg;
    cfg.put("device", "test_grabber");
    testgrabber.open(cfg);
    testgrabber.view(image);

    vector<tuple<double*, string, double> > param;
    param.push_back(make_tuple(&rgb_h,    "rgb_h",    480.0));
    param.push_back(make_tuple(&rgb_w,    "rgb_w",    640.0));
    param.push_back(make_tuple(&dep_h,    "rgb_h",    480.0));
    param.push_back(make_tuple(&dep_w,    "rgb_w",    640.0));
    param.push_back(make_tuple(&accuracy, "accuracy", 0.001));
    param.push_back(make_tuple(&rgb_Vfov, "rgb_Vfov", 50.0));
    param.push_back(make_tuple(&rgb_Hfov, "rgb_Hfov", 36.0));
    param.push_back(make_tuple(&dep_Vfov, "dep_Vfov", 50.0));
    param.push_back(make_tuple(&dep_Hfov, "dep_Hfov", 36.0));
    param.push_back(make_tuple(&dep_near, "dep_near", 0.2));
    param.push_back(make_tuple(&dep_far,  "dep_far",  6.0));
    for (auto p : param)
    {
        if (config.check(get<1>(p)))
        {
            *get<0>(p) = config.find(get<1>(p)).asDouble();
        }
        else
        {
            *get<0>(p) = get<2>(p);
        }
        
    }
    
    return true;
}

bool fakeDepthCameraDriver::close()
{
    return true;
}

int fakeDepthCameraDriver::getRgbHeight()
{
    return image->height();
}

int fakeDepthCameraDriver::getRgbWidth()
{
    return image->width();
}

bool  fakeDepthCameraDriver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations){
    yWarning()<<"fakeDepthCameraDriver:getRgbSupportedConfigurations not implemented yet";
    return false;
}
bool fakeDepthCameraDriver::getRgbResolution(int &width, int &height)
{
    width  = image->width();
    height = image->height();
    return true;
}

bool yarp::dev::fakeDepthCameraDriver::setRgbResolution(int width, int height)
{
    return false;
}

bool fakeDepthCameraDriver::setDepthResolution(int width, int height)
{
    return false;
}

bool fakeDepthCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    rgb_Hfov = horizontalFov;
    rgb_Vfov = verticalFov;
    return true;
}

bool fakeDepthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    dep_Hfov = horizontalFov;
    dep_Vfov = verticalFov;
    return true;
}

bool fakeDepthCameraDriver::setDepthAccuracy(double in_accuracy)
{
    accuracy = in_accuracy;
    return true;
}

bool fakeDepthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    horizontalFov = rgb_Hfov;
    verticalFov   = rgb_Vfov;
    return false;
}

bool fakeDepthCameraDriver::getRgbMirroring(bool& mirror)
{
    return false;
}

bool fakeDepthCameraDriver::setRgbMirroring(bool mirror)
{
    return false;
}

bool fakeDepthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
{
    return false;
}

int  fakeDepthCameraDriver::getDepthHeight()
{
    return image->height();
}

int  fakeDepthCameraDriver::getDepthWidth()
{
    return image->width();
}

bool fakeDepthCameraDriver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    horizontalFov = dep_Hfov;
    verticalFov   = dep_Vfov;
    return false;
}

bool fakeDepthCameraDriver::getDepthIntrinsicParam(Property& intrinsic)
{
    return false;
}

double fakeDepthCameraDriver::getDepthAccuracy()
{
    return accuracy;
}

bool fakeDepthCameraDriver::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    nearPlane = dep_near;
    farPlane  = dep_far;
    return true;
}

bool fakeDepthCameraDriver::setDepthClipPlanes(double nearPlane, double farPlane)
{
    dep_near = nearPlane;
    dep_far  = farPlane;
    return true;
}

bool fakeDepthCameraDriver::getDepthMirroring(bool& mirror)
{
    return false;
}

bool fakeDepthCameraDriver::setDepthMirroring(bool mirror)
{
    return false;
}

bool fakeDepthCameraDriver::getExtrinsicParam(Matrix& extrinsic)
{
    return false;
}

bool fakeDepthCameraDriver::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    ImageOf<yarp::sig::PixelRgb> imageof;
    if (!image->getImage(imageof)) {return false;}
    rgbImage.setPixelCode(VOCAB_PIXEL_RGB);
    rgbImage.resize(imageof);
    memcpy((void*)rgbImage.getRawImage(), (void*)imageof.getRawImage(), imageof.getRawImageSize());
    timeStamp->update(yarp::os::Time::now());
    return true;
}

bool fakeDepthCameraDriver::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    ImageOf<yarp::sig::PixelRgb> imageof;
    if (!image->getImage(imageof)) {return false;}
    depthImage.resize(imageof);
    for (size_t i = 0; i < imageof.width(); i++)
    {
        for (size_t j = 0; j < imageof.height(); j++)
        {
            PixelRgb pix = (*(PixelRgb*)imageof.getPixelAddress(i, j));
            *(PixelFloat*)depthImage.getPixelAddress(i, j) = (float(pix.b) / 255.0)/3.0 + (float(pix.g) / 255.0) / 3.0 + (float(pix.r) / 255.0) / 3.0;
        }
    }
    timeStamp->update(yarp::os::Time::now());
    return true;
}

bool fakeDepthCameraDriver::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    return getRgbImage(colorFrame, colorStamp) & getDepthImage(depthFrame, depthStamp);
}

IRGBDSensor::RGBDSensor_status fakeDepthCameraDriver::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

ConstString fakeDepthCameraDriver::getLastErrorMsg(Stamp* timeStamp)
{
    return "no error";
}
