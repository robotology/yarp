#include "depthCameraDriver.h"
#include <algorithm>

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

bool depthCameraDriver::open(Searchable& config)
{
    if(!initializeOpeNIDevice())
    {
        return false;
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
    m_imageFrame.mutex.lock();
    int ret = m_imageFrame.image.height();
    m_imageFrame.mutex.unlock();
    return ret;
}

int depthCameraDriver::getRgbWidth()
{
    m_imageFrame.mutex.lock();
    int ret = m_imageFrame.image.width();
    m_imageFrame.mutex.unlock();
    return ret;
}

bool depthCameraDriver::setDepthResolution(int width, int height)
{
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

bool depthCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    return false;
}

bool depthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    return false;
}

bool depthCameraDriver::setDepthAccuracy(double accuracy)
{
    if(!(fabs(accuracy - 0.001) < 0.00001) || !(fabs(accuracy - 0.0001) < 0.00001) )
    {
        yError() << "depthCameraDriver: supporting accuracy of 1mm (0.001) or 100um (0.0001) only at the moment";
        return false;
    }

    PixelFormat pf;
    VideoMode   vm;

    vm = m_imageStream.getVideoMode();
    pf = fabs(accuracy - 0.001) < 0.00001 ? PIXEL_FORMAT_DEPTH_1_MM : PIXEL_FORMAT_DEPTH_100_UM;

    vm.setPixelFormat(pf);
    RETURN_FALSE_STATUS_NOT_OK(m_imageStream.setVideoMode(vm));
    return true;
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
    m_depthFrame.mutex.lock();

    int ret = m_depthFrame.image.height();

    m_depthFrame.mutex.unlock();
    return ret;
}

int  depthCameraDriver::getDepthWidth()
{
    m_depthFrame.mutex.lock();

    int ret = m_depthFrame.image.width();

    m_depthFrame.mutex.unlock();
    return ret;
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
    return m_depthFrame.pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;
}

bool depthCameraDriver::getDepthClipPlanes(double& near, double& far)
{
    near = m_depthStream.getMinPixelValue();
    far  = m_depthStream.getMaxPixelValue();
    return true;

}

bool depthCameraDriver::setDepthClipPlanes(double near, double far)
{
    yError() << "impossible to set clip planes for OpenNI2 devices";
    return false;
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
