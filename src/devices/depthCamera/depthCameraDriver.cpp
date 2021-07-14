/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "depthCameraDriver.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include <algorithm>
#include <cmath>
#include <mutex>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace openni;
using namespace std;

#ifndef RETURN_FALSE_STATUS_NOT_OK
#define RETURN_FALSE_STATUS_NOT_OK(s) if(s != STATUS_OK) { yCError(DEPTHCAMERA) << OpenNI::getExtendedError(); return false; }
#endif

namespace {
YARP_LOG_COMPONENT(DEPTHCAMERA, "yarp.device.depthCamera")
constexpr char accuracy       [] = "accuracy";
constexpr char clipPlanes     [] = "clipPlanes";
constexpr char depth_Fov      [] = "depth_Fov";
constexpr char depthRes       [] = "depthResolution";
constexpr char rgb_Fov        [] = "rgb_Fov";
constexpr char rgbRes         [] = "rgbResolution";
constexpr char rgbMirroring   [] = "rgbMirroring";
constexpr char depthMirroring [] = "depthMirroring";
}

static std::map<std::string, RGBDSensorParamParser::RGBDParam> params_map =
{
    {accuracy,       RGBDSensorParamParser::RGBDParam(accuracy,        1)},
    {clipPlanes,     RGBDSensorParamParser::RGBDParam(clipPlanes,      2)},
    {depth_Fov,      RGBDSensorParamParser::RGBDParam(depth_Fov,       2)},
    {depthRes,       RGBDSensorParamParser::RGBDParam(depthRes,        2)},
    {rgb_Fov,        RGBDSensorParamParser::RGBDParam(rgb_Fov,         2)},
    {rgbRes,         RGBDSensorParamParser::RGBDParam(rgbRes,          2)},
    {rgbMirroring,   RGBDSensorParamParser::RGBDParam(rgbMirroring,    1)},
    {depthMirroring, RGBDSensorParamParser::RGBDParam(depthMirroring,  1)}
};

class streamFrameListener : public openni::VideoStream::NewFrameListener
{
public:

    //Properties
    std::mutex         mutex;
    yarp::os::Stamp         stamp;
    yarp::sig::FlexImage    image;
    openni::PixelFormat     pixF{openni::PixelFormat::PIXEL_FORMAT_DEPTH_1_MM};
    int                     w{0};
    int                     h{0};
    size_t                  dataSize{0};
    bool                    isReady{false};

    //Method
    streamFrameListener();
    bool isValid(){return frameRef.isValid() & isReady;}
    void destroy(){frameRef.release();}
    bool getImage(FlexImage& inputImage)
    {
        std::lock_guard<std::mutex> guard(mutex);
        return inputImage.copy(image);
    }

    yarp::os::Stamp getStamp()
    {
        std::lock_guard<std::mutex> guard(mutex);
        return stamp;
    }

private:
    void onNewFrame(openni::VideoStream& stream) override;
    openni::VideoFrameRef   frameRef;
};

streamFrameListener::streamFrameListener()
{
    image.setPixelCode(VOCAB_PIXEL_RGB);
}

void streamFrameListener::onNewFrame(openni::VideoStream& stream)
{
    std::lock_guard<std::mutex> guard(mutex);
    stream.readFrame(&frameRef);

    if (!frameRef.isValid() || !frameRef.getData())
    {
        yCInfo(DEPTHCAMERA) << "Frame lost";
        return;
    }

    int pixC;

    pixF     = stream.getVideoMode().getPixelFormat();
    pixC     = depthCameraDriver::pixFormatToCode(pixF);
    w        = frameRef.getWidth();
    h        = frameRef.getHeight();
    dataSize = frameRef.getDataSize();

    if (isReady == false)
    {
        isReady = true;
    }

    if (pixC == VOCAB_PIXEL_INVALID)
    {
        yCError(DEPTHCAMERA) << "Pixel Format not recognized";
        return;
    }

    image.setPixelCode(pixC);
    image.resize(w, h);

    if (image.getRawImageSize() != (size_t) frameRef.getDataSize())
    {
        yCError(DEPTHCAMERA) << "Device and local copy data size doesn't match";
        return;
    }

    memcpy((void*)image.getRawImage(), (void*)frameRef.getData(), frameRef.getDataSize());
    stamp.update();
    return;
}


depthCameraDriver::depthCameraDriver() : m_depthFrame(nullptr), m_imageFrame(nullptr), m_paramParser(nullptr)
{

    m_depthRegistration = true;
    m_depthFrame        = new streamFrameListener();
    m_imageFrame        = new streamFrameListener();
    m_paramParser       = new RGBDSensorParamParser();

    m_supportedFeatures.push_back(YARP_FEATURE_EXPOSURE);
    m_supportedFeatures.push_back(YARP_FEATURE_WHITE_BALANCE);
    m_supportedFeatures.push_back(YARP_FEATURE_GAIN);
    m_supportedFeatures.push_back(YARP_FEATURE_FRAME_RATE);
    m_supportedFeatures.push_back(YARP_FEATURE_MIRROR);
}

depthCameraDriver::~depthCameraDriver()
{
    close();
    if (m_depthFrame) {
        delete m_depthFrame;
    }
    if (m_imageFrame) {
        delete m_imageFrame;
    }
    if (m_paramParser) {
        delete m_paramParser;
    }
}

bool depthCameraDriver::initializeOpeNIDevice()
{
    Status      rc;

    rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        yCError(DEPTHCAMERA) << "Initialize failed," << OpenNI::getExtendedError();
        return false;
    }

    rc = m_device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        yCError(DEPTHCAMERA) << "Couldn't open device," << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_COLOR) != nullptr)
    {
        rc = m_imageStream.create(m_device, SENSOR_COLOR);
        if (rc != STATUS_OK)
        {
            yCError(DEPTHCAMERA) << "Couldn't create color stream," << OpenNI::getExtendedError();
            return false;
        }
    }

    rc = m_imageStream.start();
    if (rc != STATUS_OK)
    {
        yCError(DEPTHCAMERA) << "Couldn't start the color stream," << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_DEPTH) != nullptr)
    {
        rc = m_depthStream.create(m_device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            yCError(DEPTHCAMERA) << "Couldn't create depth stream," << OpenNI::getExtendedError();
            return false;
        }
    }

    if (m_depthRegistration)
    {
        if (m_device.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR))
        {
            if (m_device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR) == STATUS_OK)
            {
                yCInfo(DEPTHCAMERA) << "Depth successfully registered on rgb sensor";
            }
            else
            {
                yCWarning(DEPTHCAMERA) << "Depth registration failed.. sending  unregistered images";
            }
        }
        else
        {
            yCWarning(DEPTHCAMERA) << "Depth image registration not supported by this device";
        }
    }

    rc = m_depthStream.start();
    if (rc != STATUS_OK)
    {
        yCError(DEPTHCAMERA) << "Couldn't start the depth stream," << OpenNI::getExtendedError();
        return false;
    }

    m_imageStream.addNewFrameListener(m_imageFrame);
    m_depthStream.addNewFrameListener(m_depthFrame);

    return true;
}


void depthCameraDriver::settingErrorMsg(const string& error, bool& ret)
{
    yCError(DEPTHCAMERA) << error;
    ret = false;
}

bool depthCameraDriver::setParams()
{
    bool ret;
    ret = true;

    // Do all required settings

    //ACCURACY
    if (params_map[accuracy].isSetting && ret)
    {
        if (!params_map[accuracy].val[0].isFloat64()) {
            settingErrorMsg("Param " + params_map[accuracy].name + " is not a double as it should be.", ret);
        }

        if (!setDepthAccuracy(params_map[accuracy].val[0].asFloat64())) {
            settingErrorMsg("Setting param " + params_map[accuracy].name + " failed... quitting.", ret);
        }
    }

    //CLIP_PLANES
    if (params_map[clipPlanes].isSetting && ret)
    {
        if (!params_map[clipPlanes].val[0].isFloat64()) {
            settingErrorMsg("Param " + params_map[clipPlanes].name + " is not a double as it should be.", ret);
        }

        if (!params_map[clipPlanes].val[1].isFloat64()) {
            settingErrorMsg("Param " + params_map[clipPlanes].name + " is not a double as it should be.", ret);
        }

        if (!setDepthClipPlanes(params_map[clipPlanes].val[0].asFloat64(), params_map[clipPlanes].val[1].asFloat64())) {
            settingErrorMsg("Setting param " + params_map[clipPlanes].name + " failed... quitting.", ret);
        }
    }

    //DEPTH_FOV
    if (params_map[depth_Fov].isSetting && ret)
    {
        Value p1, p2;
        p1 = params_map[depth_Fov].val[0];
        p2 = params_map[depth_Fov].val[1];

        if (!p1.isFloat64() || !p2.isFloat64()) {
            settingErrorMsg("Param " + params_map[depth_Fov].name + " is not a double as it should be.", ret);
        }

        if (!setDepthFOV(p1.asFloat64(), p2.asFloat64())) {
            settingErrorMsg("Setting param " + params_map[depth_Fov].name + " failed... quitting.", ret);
        }
    }



    //RGB_FOV
    if (params_map[rgb_Fov].isSetting && ret)
    {
        Value p1, p2;
        p1 = params_map[rgb_Fov].val[0];
        p2 = params_map[rgb_Fov].val[1];

        if (!p1.isFloat64() || !p2.isFloat64() )
        {
            settingErrorMsg("Param " + params_map[rgb_Fov].name + " is not a double as it should be.", ret);
        }

        if (! setRgbFOV(p1.asFloat64(), p2.asFloat64() ) )
        {
            settingErrorMsg("Setting param " + params_map[rgb_Fov].name + " failed... quitting.", ret);
        }
    }

    //DEPTH_RES
    if (params_map[depthRes].isSetting && ret)
    {
        Value p1, p2;
        p1 = params_map[depthRes].val[0];
        p2 = params_map[depthRes].val[1];

        if (!p1.isInt32() || !p2.isInt32() )
        {
            settingErrorMsg("Param " + params_map[depthRes].name + " is not a int as it should be.", ret);
        }

        if (! setDepthResolution(p1.asInt32(), p2.asInt32()))
        {
            settingErrorMsg("Setting param " + params_map[depthRes].name + " failed... quitting.", ret);
        }
    }

    //RGB_RES
    if (params_map[rgbRes].isSetting && ret)
    {
        Value p1, p2;
        p1 = params_map[rgbRes].val[0];
        p2 = params_map[rgbRes].val[1];

        if (!p1.isInt32() || !p2.isInt32() )
        {
            settingErrorMsg("Param " + params_map[rgbRes].name + " is not a int as it should be.", ret);
        }

        if (! setRgbResolution(p1.asInt32(), p2.asInt32()))
        {
            settingErrorMsg("Setting param " + params_map[rgbRes].name + " failed... quitting.", ret);
        }
    }

    // rgb MIRRORING
    if (params_map[rgbMirroring].isSetting && ret)
    {
        //the device usually fail to set the mirror properties at the start.
        //so we will try to set it for 5 times with a little delay before returning false
        bool mirrorOk;
        Value& v = params_map[rgbMirroring].val[0];
        mirrorOk = false;

        if (!v.isBool())
        {
            settingErrorMsg("Param " + params_map[rgbMirroring].name + " is not a bool as it should be.", ret);
            return false;
        }

        for (int t = 0; t < 5; t++)
        {
            yCInfo(DEPTHCAMERA) << "Trying to set rgb mirroring parameter for the" << t+1 << "time/s";
            yarp::os::SystemClock::delaySystem(0.5);
            if (setRgbMirroring(v.asBool()))
            {
                yCInfo(DEPTHCAMERA) << "Rgb mirroring parameter set successfully";
                mirrorOk = true;
                break;
            }
        }
        if (!mirrorOk)
        {
            settingErrorMsg("Setting param " + params_map[rgbMirroring].name + " failed... quitting.", ret);
        }
    }
    // depth MIRRORING
    if (params_map[depthMirroring].isSetting && ret)
    {
        //the device usually fail to set the mirror properties at the start.
        //so we will try to set it for 5 times with a little delay before returning false
        bool mirrorOk;
        Value& v = params_map[depthMirroring].val[0];
        mirrorOk = false;

        if (!v.isBool() )
        {
            settingErrorMsg("Param " + params_map[depthMirroring].name + " is not a bool as it should be.", ret);
        }

        for (int t = 0; t < 5; t++)
        {
            yCInfo(DEPTHCAMERA) << "Trying to set depth mirroring parameter for the" << t+1 << "time/s";
            yarp::os::SystemClock::delaySystem(0.5);
            if (setDepthMirroring(v.asBool()))
            {
                yCInfo(DEPTHCAMERA) << "Depth mirroring parameter set successfully";
                mirrorOk = true;
                break;
            }
        }
        if (!mirrorOk)
        {
            settingErrorMsg("Setting param " + params_map[depthMirroring].name + " failed... quitting.", ret);
        }
    }
    return ret;
}


bool depthCameraDriver::open(Searchable& config)
{
    std::vector<RGBDSensorParamParser::RGBDParam*> params;
    for (auto& p:params_map)
    {
        params.push_back(&(p.second));
    }

    if (!m_paramParser->parseParam(config, params))
    {
        yCError(DEPTHCAMERA) << "Failed to parse the parameters";
        return false;
    }

    //"registered" is a hidden parameter for debugging pourpose
    m_depthRegistration = !(config.check("registered") && config.find("registered").isBool() && config.find("registered").asBool() == false);

    if (!initializeOpeNIDevice())
    {
        return false;
    }

    // setting Parameters
    if (!setParams())
    {
        return false;
    }

    return true;
}

bool depthCameraDriver::close()
{
    m_imageStream.stop();
    m_imageStream.destroy();
    m_depthStream.stop();
    m_depthStream.destroy();
    m_device.close();
    OpenNI::shutdown();
    return true;
}

int depthCameraDriver::getRgbHeight()
{
    if (params_map[rgbRes].isDescription)
    {
        return params_map[rgbRes].val.at(1).asInt32();
    }

    return m_imageStream.getVideoMode().getResolutionY();
}

int depthCameraDriver::getRgbWidth()
{
    if (params_map[rgbRes].isDescription)
    {
        return params_map[rgbRes].val.at(0).asInt32();
    }

    return m_imageStream.getVideoMode().getResolutionX();
}

bool depthCameraDriver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yCWarning(DEPTHCAMERA) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool depthCameraDriver::getRgbResolution(int &width, int &height)
{
    if (params_map[rgbRes].isDescription)
    {
        return params_map[rgbRes].val.at(0).asInt32();
    }
    else{
        width  = m_imageStream.getVideoMode().getResolutionX();
        height = m_imageStream.getVideoMode().getResolutionY();
    }
    return true;
}

bool depthCameraDriver::setDepthResolution(int width, int height)
{
    if (params_map[depthRes].isDescription)
    {
        yCError(DEPTHCAMERA) << "Cannot set. Depth resolution is a description!";
        return false;
    }

    return setResolution(width, height, m_depthStream);
}

bool depthCameraDriver::setResolution(int width, int height, VideoStream& stream)
{
    VideoMode vm;
    bool      bRet;

    vm = stream.getVideoMode();
    vm.setResolution(width, height);
    stream.stop();
    bRet = (stream.setVideoMode(vm) == STATUS_OK);
    RETURN_FALSE_STATUS_NOT_OK(stream.start());

    if (!bRet)
    {
        yCError(DEPTHCAMERA) << OpenNI::getExtendedError();
    }

    return bRet;
}

bool depthCameraDriver::setRgbResolution(int width, int height)
{
    if (params_map[rgbRes].isDescription)
    {
        return false;
    }

    return setResolution(width, height, m_imageStream);
}

bool depthCameraDriver::setFOV(double horizontalFov, double verticalFov, VideoStream& stream)
{
    RETURN_FALSE_STATUS_NOT_OK(stream.setProperty(STREAM_PROPERTY_VERTICAL_FOV, verticalFov * DEG2RAD));
    RETURN_FALSE_STATUS_NOT_OK(stream.setProperty(STREAM_PROPERTY_HORIZONTAL_FOV, horizontalFov * DEG2RAD));
    return true;
}

bool depthCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    if (params_map[rgb_Fov].isDescription)
    {
        return false;
    }
    return setFOV(horizontalFov, verticalFov, m_depthStream);
}

bool depthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    if (params_map[depth_Fov].isDescription)
    {
        return false;
    }
    return setFOV(horizontalFov, verticalFov, m_depthStream);
}

bool depthCameraDriver::setDepthAccuracy(double _accuracy)
{
    if (params_map[accuracy].isDescription)
    {
        return false;
    }
    bool a1, a2;
    a1 = fabs(_accuracy - 0.001)  < 0.00001;
    a2 = fabs(_accuracy - 0.0001) < 0.00001;
    if (!a1 && !a2)
    {
        yCError(DEPTHCAMERA) << "Supporting accuracy of 1mm (0.001) or 100um (0.0001) only at the moment";
        return false;
    }

    PixelFormat pf;
    VideoMode   vm;
    bool        ret;

    vm = m_imageStream.getVideoMode();
    pf = fabs(_accuracy - 0.001) < 0.00001 ? PIXEL_FORMAT_DEPTH_1_MM : PIXEL_FORMAT_DEPTH_100_UM;

    vm.setPixelFormat(pf);
    m_depthStream.stop();
    ret = m_depthStream.setVideoMode(vm) == STATUS_OK;
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.start());

    if (!ret)
    {
        yCError(DEPTHCAMERA) << OpenNI::getExtendedError();
    }
    return ret;
}

bool depthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    if (params_map[rgb_Fov].isDescription)
    {
        horizontalFov = params_map[rgb_Fov].val[0].asFloat64();
        verticalFov   = params_map[rgb_Fov].val[1].asFloat64();
        return true;
    }
    horizontalFov = m_imageStream.getHorizontalFieldOfView() * RAD2DEG;
    verticalFov   = m_imageStream.getVerticalFieldOfView()   * RAD2DEG;
    return true;
}

bool depthCameraDriver::getRgbMirroring(bool& mirror)
{
    if (params_map[rgbMirroring].isDescription)
    {
        mirror = params_map[rgbMirroring].val[0].asBool();
        return true;
    }

    mirror = m_imageStream.getMirroringEnabled();
    return true;
}

bool depthCameraDriver::setRgbMirroring(bool mirror)
{
    if (params_map[rgbMirroring].isDescription)
    {
        return false;
    }

    if (m_imageStream.setMirroringEnabled(mirror) != STATUS_OK)
    {
        return false;
    }

    bool ret;
    getRgbMirroring(ret);
    return (ret == mirror);
}

bool depthCameraDriver::setIntrinsic(Property& intrinsic, const yarp::sig::IntrinsicParams& values)
{
    values.toProperty(intrinsic);
    return true;
}

bool depthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
{
    return setIntrinsic(intrinsic, m_paramParser->rgbIntrinsic);
}

int  depthCameraDriver::getDepthHeight()
{
    if (params_map[depthRes].isDescription)
    {
        return params_map[depthRes].val[1].asFloat64();
    }
    return m_depthStream.getVideoMode().getResolutionY();
}

int  depthCameraDriver::getDepthWidth()
{
    if (params_map[depthRes].isDescription)
    {
        return params_map[depthRes].val[0].asFloat64();
    }
    return m_depthStream.getVideoMode().getResolutionX();
}

bool depthCameraDriver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    if (params_map[depth_Fov].isDescription)
    {
        horizontalFov = params_map[depth_Fov].val[0].asFloat64();
        verticalFov   = params_map[depth_Fov].val[1].asFloat64();
        return true;
    }
    horizontalFov = m_depthStream.getHorizontalFieldOfView() * RAD2DEG;
    verticalFov   = m_depthStream.getVerticalFieldOfView()   * RAD2DEG;
    return true;
}

bool depthCameraDriver::getDepthIntrinsicParam(Property& intrinsic)
{
    return setIntrinsic(intrinsic, m_paramParser->rgbIntrinsic);
}

double depthCameraDriver::getDepthAccuracy()
{
    if (params_map[accuracy].isDescription)
    {
        return params_map[accuracy].val[0].asFloat64();
    }
    return m_depthStream.getVideoMode().getPixelFormat() == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;
}

bool depthCameraDriver::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    if (params_map[clipPlanes].isDescription)
    {
        nearPlane = params_map[clipPlanes].val[0].asFloat64();
        farPlane  = params_map[clipPlanes].val[1].asFloat64();
        return true;
    }
    double factor;
    factor = getDepthAccuracy();
    nearPlane   = m_depthStream.getMinPixelValue() * factor;
    farPlane    = m_depthStream.getMaxPixelValue() * factor;
    return true;
}

bool depthCameraDriver::setDepthClipPlanes(double nearPlane, double farPlane)
{
    if (params_map[clipPlanes].isDescription)
    {
        return false;
    }
    double factor;
    factor = getDepthAccuracy();
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setProperty(STREAM_PROPERTY_MAX_VALUE, int(farPlane  / factor)));
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setProperty(STREAM_PROPERTY_MIN_VALUE, int(nearPlane / factor)));
    return true;
}

bool depthCameraDriver::getDepthMirroring(bool& mirror)
{
    if (params_map[depthMirroring].isDescription)
    {
        return params_map[depthMirroring].val[0].asBool();
    }
    mirror = m_depthStream.getMirroringEnabled();
    return true;
}

bool depthCameraDriver::setDepthMirroring(bool mirror)
{
    if (params_map[depthMirroring].isDescription)
    {
        return false;
    }
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setMirroringEnabled(mirror));

    bool ret;
    getDepthMirroring(ret);
    return (ret == mirror);
}

bool depthCameraDriver::getExtrinsicParam(Matrix& extrinsic)
{
    extrinsic = m_paramParser->transformationMatrix;
    return true;
}

bool depthCameraDriver::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    return getImage(rgbImage, timeStamp, m_imageFrame);
}

bool depthCameraDriver::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    return getImage(depthImage, timeStamp, m_depthFrame);
}

int depthCameraDriver::pixFormatToCode(PixelFormat p)
{
    //pixel size interpretation based on  openni2_camera/src/openni2_frame_listener.cpp:
    /*switch (video_mode.getPixelFormat())
        {
          case openni::PIXEL_FORMAT_DEPTH_1_MM:
            image->encoding = sensor_msgs::image_encodings::TYPE_16UC1;
            image->step = sizeof(unsigned char) * 2 * image->width;
            break;
          case openni::PIXEL_FORMAT_DEPTH_100_UM:
            image->encoding = sensor_msgs::image_encodings::TYPE_16UC1;
            image->step = sizeof(unsigned char) * 2 * image->width;
            break;
          case openni::PIXEL_FORMAT_SHIFT_9_2:
            image->encoding = sensor_msgs::image_encodings::TYPE_16UC1;
            image->step = sizeof(unsigned char) * 2 * image->width;
            break;
          case openni::PIXEL_FORMAT_SHIFT_9_3:
            image->encoding = sensor_msgs::image_encodings::TYPE_16UC1;
            image->step = sizeof(unsigned char) * 2 * image->width;
            break;
    */
    switch(p)
    {
    case (PIXEL_FORMAT_RGB888):
        return VOCAB_PIXEL_RGB;

    case (PIXEL_FORMAT_DEPTH_1_MM):
        return VOCAB_PIXEL_MONO16;

    case (PIXEL_FORMAT_DEPTH_100_UM):
        return VOCAB_PIXEL_MONO16;

    case (PIXEL_FORMAT_SHIFT_9_2):
        return VOCAB_PIXEL_INVALID;

    case (PIXEL_FORMAT_SHIFT_9_3):
        return VOCAB_PIXEL_INVALID;

    case (PIXEL_FORMAT_GRAY8):
        return VOCAB_PIXEL_MONO;

    case (PIXEL_FORMAT_GRAY16):
        return VOCAB_PIXEL_MONO16;

    default:
        return VOCAB_PIXEL_INVALID;
    }
    return VOCAB_PIXEL_INVALID;
}

bool depthCameraDriver::getImage(FlexImage& Frame, Stamp* Stamp, streamFrameListener* sourceFrame)
{
    bool ret = sourceFrame->getImage(Frame);
    *Stamp   = sourceFrame->getStamp();
    return ret;
}

bool depthCameraDriver::getImage(ImageOf<PixelFloat>& Frame, Stamp* Stamp, streamFrameListener* sourceFrame)
{
    std::lock_guard<std::mutex> guard(sourceFrame->mutex);
    if (!sourceFrame->isReady)
    {
        yCError(DEPTHCAMERA) << "Device not ready";
        return false;
    }
    int w, h, i;
    w = sourceFrame->w;
    h = sourceFrame->h;

    if (sourceFrame->dataSize != size_t(h * w * sizeof(short)) ||
       (sourceFrame->pixF != PIXEL_FORMAT_DEPTH_100_UM && sourceFrame->pixF != PIXEL_FORMAT_DEPTH_1_MM))
    {
        yCError(DEPTHCAMERA) << "getImage: image format error";
        return false;
    }

    float  factor;
    float* rawImage;
    short* srcRawImage;

    srcRawImage = reinterpret_cast<short*> (sourceFrame->image.getRawImage());
    factor      = sourceFrame->pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;

    Frame.resize(w, h);
    rawImage = reinterpret_cast<float*> (Frame.getRawImage());

    //TODO: optimize short-to-float cast and multiplication using SSE/SIMD instruction
    for(i = 0; i < w * h; i++)
    {
        rawImage[i] = srcRawImage[i] * factor;
    }
    *Stamp   = sourceFrame->stamp;
    return true;
}

bool depthCameraDriver::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    return getImage(colorFrame, colorStamp, m_imageFrame) & getImage(depthFrame, depthStamp, m_depthFrame);
}

IRGBDSensor::RGBDSensor_status depthCameraDriver::getSensorStatus()
{
    openni::DeviceState status;
    status = DEVICE_STATE_NOT_READY;
    if (m_device.isValid()      &&
       m_imageStream.isValid() &&
       m_depthStream.isValid() &&
       m_imageFrame->isValid()  &&
       m_depthFrame->isValid())
    {
        status = DEVICE_STATE_OK;
    }
    switch(status)
    {
    case DEVICE_STATE_OK:
        return RGBD_SENSOR_OK_IN_USE;

    case DEVICE_STATE_NOT_READY:
        return RGBD_SENSOR_NOT_READY;

    default:
        return RGBD_SENSOR_GENERIC_ERROR;
    }

    return RGBD_SENSOR_GENERIC_ERROR;
}

std::string depthCameraDriver::getLastErrorMsg(Stamp* timeStamp)
{
    return OpenNI::getExtendedError();
}

bool depthCameraDriver::getCameraDescription(CameraDescriptor* camera)
{
    camera->deviceDescription = m_device.getDeviceInfo().getName();
    camera->busType = BUS_USB;
    return true;
}

bool depthCameraDriver::hasFeature(int feature, bool* hasFeature)
{
    cameraFeature_id_t f;
    f = static_cast<cameraFeature_id_t>(feature);
    if (f < YARP_FEATURE_BRIGHTNESS || f > YARP_FEATURE_NUMBER_OF-1)
    {
        return false;
    }

    if (std::find(m_supportedFeatures.begin(), m_supportedFeatures.end(), f) != m_supportedFeatures.end())
    {
        *hasFeature = true;
    }
    else
    {
        *hasFeature = false;
    }

    return true;
}

bool depthCameraDriver::setFeature(int feature, double value)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    switch(f)
    {
    case YARP_FEATURE_EXPOSURE:
        RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setExposure(int(value * 100)+1));
        break;
    case YARP_FEATURE_GAIN:
        RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setGain(int(value * 100)+1));
        break;
    case YARP_FEATURE_FRAME_RATE:
    {
        VideoMode vm;

        vm = m_imageStream.getVideoMode();
        vm.setFps(int(value));
        RETURN_FALSE_STATUS_NOT_OK(m_imageStream.setVideoMode(vm));

        m_depthStream.getVideoMode();
        vm.setFps(int(value));
        RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setVideoMode(vm));
        break;
    }
    case YARP_FEATURE_WHITE_BALANCE:
        yCError(DEPTHCAMERA) << "No manual mode for white_balance. call hasManual() to know if a specific feature support Manual mode instead of wasting my time";
        return false;
    default:
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }
    return true;
}

bool depthCameraDriver::getFeature(int feature, double *value)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    switch(f)
    {
    case YARP_FEATURE_EXPOSURE:
        *value = m_imageStream.getCameraSettings()->getExposure();
        break;
    case YARP_FEATURE_GAIN:
        *value = m_imageStream.getCameraSettings()->getGain();
        break;
    case YARP_FEATURE_FRAME_RATE:
        *value = (m_imageStream.getVideoMode().getFps());
        break;
    case YARP_FEATURE_WHITE_BALANCE:
        yCError(DEPTHCAMERA) << "No manual mode for white_balance. call hasManual() to know if a specific feature support Manual mode";
        return false;
    default:
        return false;
    }
    return true;
}

bool depthCameraDriver::setFeature(int feature, double value1, double value2)
{
    yCError(DEPTHCAMERA) << "No 2-valued feature are supported";
    return false;
}

bool depthCameraDriver::getFeature(int feature, double *value1, double *value2)
{
    yCError(DEPTHCAMERA) << "No 2-valued feature are supported";
    return false;
}

bool depthCameraDriver::hasOnOff(  int feature, bool *HasOnOff)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_WHITE_BALANCE || f == YARP_FEATURE_MIRROR)
    {
        *HasOnOff = true;
        return true;
    }
    *HasOnOff = false;
    return true;
}

bool depthCameraDriver::setActive( int feature, bool onoff)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    if (!hasOnOff(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }

    switch(feature)
    {
    case YARP_FEATURE_MIRROR:
        RETURN_FALSE_STATUS_NOT_OK(m_imageStream.setProperty(STREAM_PROPERTY_MIRRORING, onoff));
        RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setProperty(STREAM_PROPERTY_MIRRORING, onoff));
        break;
    case YARP_FEATURE_WHITE_BALANCE:
        RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setAutoWhiteBalanceEnabled(onoff));
        break;
    default:
        return false;
    }


    return true;
}

bool depthCameraDriver::getActive( int feature, bool *isActive)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    if (!hasOnOff(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }

    *isActive = m_imageStream.getCameraSettings()->getAutoWhiteBalanceEnabled();
    return true;
}

bool depthCameraDriver::hasAuto(int feature, bool *hasAuto)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_WHITE_BALANCE)
    {
        *hasAuto = true;
        return true;
    }
    *hasAuto = false;
    return true;
}

bool depthCameraDriver::hasManual( int feature, bool* hasManual)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_FRAME_RATE || f == YARP_FEATURE_GAIN)
    {
        *hasManual = true;
        return true;
    }
    *hasManual = false;
    return true;
}

bool depthCameraDriver::hasOnePush(int feature, bool* hasOnePush)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    return hasAuto(feature, hasOnePush);
}

bool depthCameraDriver::setMode(int feature, FeatureMode mode)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE)
    {
        switch(mode)
        {
        case MODE_AUTO:
            RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setAutoExposureEnabled(true));
            break;
        case MODE_MANUAL:
            RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setAutoExposureEnabled(false));
            break;
        case MODE_UNKNOWN:
            return false;
        default:
            return false;
        }
        return true;
    }

    yCError(DEPTHCAMERA) << "Feature does not have both auto and manual mode";
    return false;
}

bool depthCameraDriver::getMode(int feature, FeatureMode* mode)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    auto f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE)
    {
        *mode = m_imageStream.getCameraSettings()->getAutoExposureEnabled() ? MODE_AUTO : MODE_MANUAL;
        return true;
    }

    yCError(DEPTHCAMERA) << "Feature does not have both auto and manual mode";
    return false;
}

bool depthCameraDriver::setOnePush(int feature)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature not supported!";
        return false;
    }

    if (!hasOnePush(feature, &b) || !b)
    {
        yCError(DEPTHCAMERA) << "Feature doesn't have OnePush";
        return false;
    }

    setMode(feature, MODE_AUTO);
    setMode(feature, MODE_MANUAL);

    return true;
}
