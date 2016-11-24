#include "depthCameraDriver.h"
#include <algorithm>
#include <math.h>
#include <yarp/os/Value.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace openni;
using namespace std;

#define RETURN_FALSE_STATUS_NOT_OK(s) if(s != STATUS_OK){yError() << OpenNI::getExtendedError(); return false;}

streamFrameListener::streamFrameListener()
{
    image.setPixelCode(VOCAB_PIXEL_RGB);
    w        = 0;
    h        = 0;
    dataSize = 0;
    isReady    = false;
}

void streamFrameListener::onNewFrame(openni::VideoStream& stream)
{
    LockGuard guard(mutex);
    stream.readFrame(&frameRef);

    if (!frameRef.isValid() || !frameRef.getData())
    {
        yInfo() << "frame lost";
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

    if(pixC == VOCAB_PIXEL_INVALID)
    {
        yError() << "depthCameraDriver: Pixel Format not recognized";
        return;
    }

    image.setPixelCode(pixC);
    image.resize(w, h);
    if(image.getRawImageSize() != frameRef.getDataSize())
    {
        yError() << "depthCameraDriver:device and local copy data size doesn't match";
        return;
    }
    memcpy((void*)image.getRawImage(), (void*)frameRef.getData(), frameRef.getDataSize());
    stamp.update();
    return;
}


depthCameraDriver::depthCameraDriver()
{
    supportedFeatures.push_back(YARP_FEATURE_EXPOSURE);
    supportedFeatures.push_back(YARP_FEATURE_WHITE_BALANCE);
    supportedFeatures.push_back(YARP_FEATURE_GAIN);
    supportedFeatures.push_back(YARP_FEATURE_FRAME_RATE);

    // initialize struct for params
    cameraDescription.clipPlanes.name = "clipPlanes";
    cameraDescription.clipPlanes.size = 2;
    cameraDescription.accuracy.name = "accuracy";
    cameraDescription.depthRes.name = "depthResolution";
    cameraDescription.depthRes.size = 2;
    cameraDescription.depth_hFov.name = "depth_hFOV";
    cameraDescription.depth_vFov.name = "depth_vFOV";

    cameraDescription.rgbRes.name = "rgbResolution";
    cameraDescription.rgbRes.size = 2;
    cameraDescription.rgb_hFov.name = "rgb_hFOV";
    cameraDescription.rgb_vFov.name = "rgb_vFOV";
    return;
}

depthCameraDriver::~depthCameraDriver()
{
    return;
}

bool depthCameraDriver::initializeOpeNIDevice()
{
    Status      rc;

    rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        yError() << "depthCameraDriver: Initialize failed," << OpenNI::getExtendedError();
        return false;
    }

    rc = m_device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        yError() << "depthCameraDriver: Couldn't open device," << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_COLOR) != NULL)
    {
        rc = m_imageStream.create(m_device, SENSOR_COLOR);
        if (rc != STATUS_OK)
        {
            yError() << "depthCameraDriver: Couldn't create color stream," << OpenNI::getExtendedError();
            return false;
        }
    }

    rc = m_imageStream.start();
    if (rc != STATUS_OK)
    {
        yError() << "depthCameraDriver: Couldn't start the color stream," << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = m_depthStream.create(m_device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            yError() << "depthCameraDriver: Couldn't create depth stream," << OpenNI::getExtendedError();
            return false;
        }
    }

    rc = m_depthStream.start();
    if (rc != STATUS_OK)
    {
        yError() << "depthCameraDriver: Couldn't start the depth stream," << OpenNI::getExtendedError();
        return false;
    }

    m_imageStream.addNewFrameListener(&m_imageFrame);
    m_depthStream.addNewFrameListener(&m_depthFrame);

    return true;
}

bool depthCameraDriver::checkParam(const Bottle& settings, const Bottle& description, RGBDParam &param)
{
    bool ret1, ret2, ret3;

    // look for settings
    ret1 = checkParam(settings,    param, param.isSetting);

    // look for HW_DESCRIPTION
    ret2 = checkParam(description, param, param.isDescription);

    if( (param.isSetting) && (param.isDescription) )
    {
        yError() << "Setting " << param.name << " can either be a 'SETTING' or 'HW_DESCRIPTION', not both. Fix the config file. \
                    Look for documentation online.";
        ret3 = false;
    }
    return (ret1 && ret2 && ret3);
}

bool depthCameraDriver::checkParam(const Bottle& input, RGBDParam &param, bool &found)
{
    bool ret = false;
    yarp::os::Value v;

    if(input->check(param.name))
    {
        v = input->find(param.name);
        if(v.isNull())
        {
            yError() << "Parameter " << param.name << " malformed. Check your config file.";
            return false;
        }

        if(v.isList() )
        {
            // check single or more params
            if(param.size ==1)
            {
                yError() << "Parameter " << param.name << " should be a single value.";
                return false;
            }

            // check size of data match
            Bottle b = v.asList();
            if(b.size() != param.size)
            {
                yError() << "Parameter " << param.name << " size should be " << param.size << ", got " << b.size() << "instead. Check your config file";
                return false;
            }

            // All ok here, fill the data
            param.val.resize(param.size);
            for(int i=0; i<param.size; i++)
            {
                param.val[i] = b.get(i); // maybe i=1? check...
            }
            return true;
        }
        else  // got a single value from file
        {
            ret = true;
            param.val[0] = v;
            found = true;
        }
    }
    else
    {
        ret = true;
        found = true;
    }
    return ret;
}

void depthCameraDriver::settingErrorMsg(RGBDParam& param, bool& ret)
{
    yError() << "msg";
    ret = false;
}


bool depthCameraDriver::open(Searchable& config)
{
    if(!initializeOpeNIDevice())
    {
        return false;
    }

    bool   ret;
    Bottle settings;
    Bottle description;
    string error, success;
    Value  v1, v2;

    error   = "unable to set";
    success = "succesfully setted";
    ret     = true;
    //"accuracy";
    //"d_Resolution";
    //"c_Resolution";
    //"nearPlane";
    //"farPlane";
    //"d_hFov";
    //"d_vFov";
    //"c_hFov";
    //"c_vFov";

    if(!config.check("SETTINGS"))
    {
        yError() << "depthCameraDriver: missing SETTINGS section on the configuration file";
        return false;
    }
    settings = config.findGroup("SETTINGS");

    if(!config.check("HW_DESCRIPTION"))
    {
        yError() << "depthCameraDriver: missing SETTINGS section on the configuration file";
        return false;
    }
    description = config.findGroup("HW_DESCRIPTION");


    cameraDescription.accuracy.name = "name";
    // Check input file
    if(!checkParam(settings, description, cameraDescription.accuracy)   )    ret = false;
    if(!checkParam(settings, description, cameraDescription.clipPlanes) )    ret = false;
    if(!checkParam(settings, description, cameraDescription.depth_hFov) )    ret = false;
    if(!checkParam(settings, description, cameraDescription.depth_vFov) )    ret = false;
    if(!checkParam(settings, description, cameraDescription.depthRes)   )    ret = false;

    if(!checkParam(settings, description, cameraDescription.rgb_hFov)   )    ret = false;
    if(!checkParam(settings, description, cameraDescription.rgb_vFov)   )    ret = false;
    if(!checkParam(settings, description, cameraDescription.rgbRes)     )    ret = false;

    if(!ret)
    {
        yError() << "depthCamera driver input file not correct, please fix it!";
        return false;
    }

    ret = true;
    // Do all required settings
    if(cameraDescription.accuracy.isSetting)
    {
        if(!cameraDescription.accuracy.val[0].isDouble() )
            settingErrorMsg("Param " << cameraDescription.accuracy.name << " is not a double as it should be.", ret);

        if(! setDepthAccuracy(cameraDescription.accuracy.val[0].asDouble() ) )
            settingErrorMsg("Setting param " << cameraDescription.accuracy.name << " failed... quitting.", ret);
    }

    if(cameraDescription.clipPlanes.isSetting)
    {
        if(!cameraDescription.clipPlanes.val[0].isDouble() )
            settingErrorMsg("Param " << cameraDescription.clipPlanes.name << " is not a double as it should be.", ret);

        if(!cameraDescription.clipPlanes.val[1].isDouble() )
            settingErrorMsg("Param " << cameraDescription.clipPlanes.name << " is not a double as it should be.", ret);

        if(! setDepthClipPlanes(cameraDescription.clipPlanes.val[0].asDouble(), cameraDescription.clipPlanes.val[1].asDouble() ) )
            settingErrorMsg("Setting param " << cameraDescription.clipPlanes.name << " failed... quitting.", ret);
    }

    if(cameraDescription.depth_hFov.isSetting)
    {
        if(!cameraDescription.depth_hFov.val[0].isDouble() )
            settingErrorMsg("Param " << cameraDescription.depth_hFov.name << " is not a double as it should be.", ret);

        if(! setDepthFOV(cameraDescription.depth_hFov.val[0].asDouble() ) )
            settingErrorMsg("Setting param " << cameraDescription.depth_hFov.name << " failed... quitting.", ret);
    }

    if(cameraDescription.depth_vFov.isSetting)
    {
        if(!cameraDescription.depth_vFov.val[0].isDouble() )
            settingErrorMsg("Param " << cameraDescription.depth_vFov.name << " is not a double as it should be.", ret);

        if(! setDepthFOV(cameraDescription.depth_vFov.val[0].asDouble() ) )
            settingErrorMsg("Setting param " << cameraDescription.depth_vFov.name << " failed... quitting.", ret);
    }

    if(cameraDescription.accuracy.isSetting)
    {
        if(!cameraDescription.accuracy.val[0].isDouble() )
            settingErrorMsg("Param " << cameraDescription.accuracy.name << " is not a double as it should be.", ret);

        if(! setDepthAccuracy(cameraDescription.accuracy.val[0].asDouble() ) )
            settingErrorMsg("Setting param " << cameraDescription.accuracy.name << " failed... quitting.", ret);
    }


    /*
     * cameraDescription.clipPlanes
    cameraDescription.depth_hFov
    cameraDescription.depth_vFov
    cameraDescription.depthRes

    cameraDescription.rgb_hFov
    cameraDescription.rgb_vFov
    cameraDescription.rgbRes
      */
     if( (cameraDescription.accuracy.isSetting)
         setta;

    if(paramSetter(settings, description, "accuracy", m_description.accuracy.first, v1))
    {
        if(m_description.accuracy.first)
        {
            if(setDepthAccuracy(v1.asDouble()))
            {
                yInfo() << "accuracy" << success;
            }
            else
            {
                yError() << error << "accuracy";
            }
        }
        else
        {
            m_description.accuracy.second = v1.asDouble();
        }
    }

    if(paramSetter(settings, description,"rgb_height", m_description.c_H.first, v1)
    && paramSetter(settings, description, "rgb_width", m_description.c_W.first, v2))
    {
        if(m_description.c_H.first && m_description.c_W.first)
        {
            if(setRgbResolution(v1.asInt(), v2.asInt()))
            {
                yInfo() << "RGB resolution" << success;
            }
            else
            {
                yError() << error << "RGB resolution";
            }
        }
        else
        {
            m_description.c_H.second = v1.asInt();
            m_description.c_W.second = v2.asInt();
        }
    }

    return true;
}

bool depthCameraDriver::close()
{
    m_imageStream.destroy();
    m_depthStream.destroy();
    m_device.close();
    OpenNI::shutdown();
    return true;
}

int depthCameraDriver::getRgbHeight()
{
    if(m_description.c_H.first)
    {
        return m_description.c_H.second;
    }

    return m_imageStream.getVideoMode().getResolutionY();
}

int depthCameraDriver::getRgbWidth()
{    
    if(m_description.c_W.first)
    {
        return m_description.c_W.second;
    }

    return m_imageStream.getVideoMode().getResolutionX();
}

bool depthCameraDriver::setDepthResolution(int width, int height)
{
    if(m_description.d_W.first)
    {
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
    bRet = stream.setVideoMode(vm) == STATUS_OK;
    RETURN_FALSE_STATUS_NOT_OK(stream.start());

    if(!bRet)
    {
        yError() << OpenNI::getExtendedError();
    }

    return bRet;
}

bool depthCameraDriver::setRgbResolution(int width, int height)
{
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
    return setFOV(horizontalFov, verticalFov, m_depthStream);
}

bool depthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    return setFOV(horizontalFov, verticalFov, m_depthStream);
}

bool depthCameraDriver::setDepthAccuracy(double accuracy)
{
    bool a1, a2;
    a1 = fabs(accuracy - 0.001)  < 0.00001;
    a2 = fabs(accuracy - 0.0001) < 0.00001;
    if(!a1 && !a2)
    {
        yError() << "depthCameraDriver: supporting accuracy of 1mm (0.001) or 100um (0.0001) only at the moment";
        return false;
    }

    PixelFormat pf;
    VideoMode   vm;
    bool        ret;

    vm = m_imageStream.getVideoMode();
    pf = fabs(accuracy - 0.001) < 0.00001 ? PIXEL_FORMAT_DEPTH_1_MM : PIXEL_FORMAT_DEPTH_100_UM;

    vm.setPixelFormat(pf);
    m_depthStream.stop();
    ret = m_depthStream.setVideoMode(vm) == STATUS_OK;
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.start());

    if(!ret)
    {
        yError() << OpenNI::getExtendedError();
    }
    return ret;
}

bool depthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    horizontalFov = m_imageStream.getHorizontalFieldOfView() * RAD2DEG;
    verticalFov   = m_imageStream.getVerticalFieldOfView()   * RAD2DEG;
    return true;
}

bool depthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
{
    return false;
}

int  depthCameraDriver::getDepthHeight()
{
    return m_depthStream.getVideoMode().getResolutionY();
}

int  depthCameraDriver::getDepthWidth()
{
    return m_depthStream.getVideoMode().getResolutionX();
}

bool depthCameraDriver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    horizontalFov = m_depthStream.getHorizontalFieldOfView() * RAD2DEG;
    verticalFov   = m_depthStream.getVerticalFieldOfView()   * RAD2DEG;
    return true;
}

bool depthCameraDriver::getDepthIntrinsicParam(Property& intrinsic)
{
    return false;
}

double depthCameraDriver::getDepthAccuracy()
{
    return m_depthStream.getVideoMode().getPixelFormat() == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;
}

bool depthCameraDriver::getDepthClipPlanes(double& near, double& far)
{
    double factor;
    factor = getDepthAccuracy();
    near   = m_depthStream.getMinPixelValue() * factor;
    far    = m_depthStream.getMaxPixelValue() * factor;
    return true;

}

bool depthCameraDriver::setDepthClipPlanes(double near, double far)
{
    double factor;
    factor = getDepthAccuracy();
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setProperty(STREAM_PROPERTY_MAX_VALUE, int(far  / factor)));
    RETURN_FALSE_STATUS_NOT_OK(m_depthStream.setProperty(STREAM_PROPERTY_MIN_VALUE, int(near / factor)));
    return true;
}

bool depthCameraDriver::getExtrinsicParam(Property& extrinsic)
{
    return false;
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

bool depthCameraDriver::getImage(FlexImage& Frame, Stamp* Stamp, streamFrameListener& sourceFrame)
{
    LockGuard guard(sourceFrame.mutex);

    bool ret = Frame.copy(sourceFrame.image);
    *Stamp   = sourceFrame.stamp;
    return ret;
}

bool depthCameraDriver::getImage(ImageOf<PixelFloat>& Frame, Stamp* Stamp, streamFrameListener& sourceFrame)
{
    LockGuard guard(sourceFrame.mutex);
    if(!sourceFrame.isReady)
    {
        yError() << "device not ready";
        return false;
    }
    int w, h, i;
    w = sourceFrame.w;
    h = sourceFrame.h;

    if(sourceFrame.dataSize != size_t(h * w * sizeof(short)) ||
       (sourceFrame.pixF != PIXEL_FORMAT_DEPTH_100_UM && sourceFrame.pixF != PIXEL_FORMAT_DEPTH_1_MM))
    {
        yError() << "depthCameraDriver::getImage: image format error";
        return false;
    }

    float  factor;
    float* rawImage;
    short* srcRawImage;

    srcRawImage = (short*)(sourceFrame.image.getRawImage());
    factor      = sourceFrame.pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;

    Frame.resize(w, h);
    rawImage = (float*)(Frame.getRawImage());

    //TODO: optimize short-to-float cast and multiplication using SSE/SIMD instruction
    for(i = 0; i < w * h; i++)
    {
        rawImage[i] = srcRawImage[i] * factor;
    }
    *Stamp   = sourceFrame.stamp;
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
    if(m_device.isValid()      &&
       m_imageStream.isValid() &&
       m_depthStream.isValid() &&
       m_imageFrame.isValid()  &&
       m_depthFrame.isValid())
    {
        status = DEVICE_STATE_OK;
    }
    switch(status)
    {
    case DEVICE_STATE_OK:
        return RGBD_SENSOR_OK_IN_USE;

    case DEVICE_STATE_NOT_READY:
        return RGBD_SENSOR_NOT_READY;

    case DEVICE_STATE_ERROR:
        return RGBD_SENSOR_GENERIC_ERROR;

    default:
        return RGBD_SENSOR_GENERIC_ERROR;
    }

    return RGBD_SENSOR_GENERIC_ERROR;
}

ConstString depthCameraDriver::getLastErrorMsg(Stamp* timeStamp)
{
    return OpenNI::getExtendedError();
}

bool depthCameraDriver::getCameraDescription(CameraDescriptor* camera)
{
    camera->deviceDescription = m_device.getDeviceInfo().getName();
    camera->busType = BUS_USB;
    return true;
}

bool depthCameraDriver::hasFeature(int feature, bool *hasFeature)
{
    cameraFeature_id_t f;
    f = static_cast<cameraFeature_id_t>(feature);
    if (f < YARP_FEATURE_BRIGHTNESS || f > YARP_FEATURE_NUMBER_OF-1)
    {
        return false;
    }

    if(std::find(supportedFeatures.begin(), supportedFeatures.end(), f) != supportedFeatures.end())
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
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
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
        yError() << "no manual mode for white_balance. call hasManual() to know if a specific feature support Manual mode instead of wasting my time";
        return false;
    default:
        yError() << "feature not supported!";
        return false;
    }
    return true;
}

bool depthCameraDriver::getFeature(int feature, double *value)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
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
        yError() << "no manual mode for white_balance. call hasManual() to know if a specific feature support Manual mode";
        return false;
    default:
        return false;
    }
    return true;
}

bool depthCameraDriver::setFeature(int feature, double value1, double value2)
{
    yError() << "no 2-valued feature are supported";
    return false;
}

bool depthCameraDriver::getFeature(int feature, double *value1, double *value2)
{
    yError() << "no 2-valued feature are supported";
    return false;
}

bool depthCameraDriver::hasOnOff(  int feature, bool *HasOnOff)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if(f == YARP_FEATURE_WHITE_BALANCE)
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
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if(!hasOnOff(feature, &b) || !b)
    {
        yError() << "feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }

    RETURN_FALSE_STATUS_NOT_OK(m_imageStream.getCameraSettings()->setAutoWhiteBalanceEnabled(onoff));

    return true;
}

bool depthCameraDriver::getActive( int feature, bool *isActive)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if(!hasOnOff(feature, &b) || !b)
    {
        yError() << "feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }

    *isActive = m_imageStream.getCameraSettings()->getAutoWhiteBalanceEnabled();
    return true;
}

bool depthCameraDriver::hasAuto(int feature, bool *hasAuto)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if(f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_WHITE_BALANCE)
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
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if(f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_FRAME_RATE || f == YARP_FEATURE_GAIN)
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
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    return hasAuto(feature, hasOnePush);
}

bool depthCameraDriver::setMode(int feature, FeatureMode mode)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if(f == YARP_FEATURE_EXPOSURE)
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

    yError() << "feature does not have both auto and manual mode";
    return false;
}

bool depthCameraDriver::getMode(int feature, FeatureMode* mode)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if(f == YARP_FEATURE_EXPOSURE)
    {
        *mode = m_imageStream.getCameraSettings()->getAutoExposureEnabled() ? MODE_AUTO : MODE_MANUAL;
        return true;
    }

    yError() << "feature does not have both auto and manual mode";
    return false;
}

bool depthCameraDriver::setOnePush(int feature)
{
    bool b;
    if(!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if(!hasOnePush(feature, &b) || !b)
    {
        yError() << "feature doesn't have OnePush";
        return false;
    }

    setMode(feature, MODE_AUTO);
    setMode(feature, MODE_MANUAL);

    return true;
}
