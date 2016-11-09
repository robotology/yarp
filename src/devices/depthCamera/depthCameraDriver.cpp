#include "depthCameraDriver.h"
using namespace yarp::dev;
using namespace yarp::sig;
using namespace openni;
using namespace std;

void streamFrameListener::onNewFrame(openni::VideoStream& stream)
{

    mutex.lock();
    if (!frameRef.isValid() || !frameRef.getData())
    {
        yInfo() << "frame lost";
        return;
    }
    int         pixC;

    stream.readFrame(&frameRef);
    pixF = stream.getVideoMode().getPixelFormat();
    pixC = yarp::dev::depthCameraDriver::pixFormatToCode(pixF);
    w    = frameRef.getWidth();
    h    = frameRef.getHeight();

    if(pixC == VOCAB_PIXEL_INVALID)
    {
        yError() << "depthCameraDriver: Pixel Format not recognized";
        return;
    }

    image.setPixelCode(pixC);
    image.setExternal((void*)frameRef.getData(), w, h);
    stamp.update();
    mutex.unlock();
}


depthCameraDriver::depthCameraDriver()
{

}

depthCameraDriver::~depthCameraDriver()
{

}

bool depthCameraDriver::initializeOpeNIDevice()
{
    Status      rc;

    rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        yError() << "Initialize failed\n%s\n" << OpenNI::getExtendedError();
        return false;
    }

    rc = m_device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        yError() << "Couldn't open device\n%s\n" << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_COLOR) != NULL)
    {
        rc = m_imageStream.create(m_device, SENSOR_COLOR);
        if (rc != STATUS_OK)
        {
            yError() << "Couldn't create color stream\n%s\n" << OpenNI::getExtendedError();
            return false;
        }
    }

    rc = m_imageStream.start();
    if (rc != STATUS_OK)
    {
        yError() << "Couldn't start the color stream\n%s\n" << OpenNI::getExtendedError();
        return false;
    }

    if (m_device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = m_depthStream.create(m_device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            yError() << "Couldn't create depth stream\n%s\n" << OpenNI::getExtendedError();
            return false;
        }
    }

    rc = m_depthStream.start();
    if (rc != STATUS_OK)
    {
        yError() << "Couldn't start the depth stream\n%s\n" << OpenNI::getExtendedError();
        return false;
    }

    m_imageStream.addNewFrameListener(&m_imageFrame);
    m_depthStream.addNewFrameListener(&m_depthFrame);

    return true;
}

bool depthCameraDriver::open(yarp::os::Searchable& config)
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

bool depthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    horizontalFov = m_imageStream.getHorizontalFieldOfView() * RAD2DEG;
    verticalFov   = m_imageStream.getVerticalFieldOfView()   * RAD2DEG;

}

bool depthCameraDriver::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{

}

bool depthCameraDriver::getRgbSensorInfo(yarp::os::Property& info)
{

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

}

bool depthCameraDriver::getDepthIntrinsicParam(yarp::os::Property& intrinsic)
{

}

bool depthCameraDriver::getDepthSensorInfo(yarp::os::Property info)
{
    string sensorType;
    switch(m_depthStream.getSensorInfo().getSensorType())
    {
    case SENSOR_COLOR:
        sensorType = "SENSOR_COLOR";
    case SENSOR_DEPTH:
        sensorType = "SENSOR_DEPTH";
    case SENSOR_IR:
        sensorType = "SENSOR_IR";
    }

    info.put("SensorType", sensorType);
    return true;
}

double depthCameraDriver::getDepthAccuracy()
{
    double precision;
    precision = m_depthFrame.pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;
    return precision;
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

bool depthCameraDriver::getExtrinsicParam(yarp::os::Property& extrinsic)
{

}

bool depthCameraDriver::getRgbImage(yarp::sig::FlexImage& rgbImage, yarp::os::Stamp* timeStamp)
{
    return getImage(rgbImage, timeStamp, m_imageFrame);
}

bool depthCameraDriver::getDepthImage(yarp::sig::ImageOf<PixelFloat>& depthImage, yarp::os::Stamp* timeStamp)
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
            break;*/
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

bool depthCameraDriver::getImage(yarp::sig::FlexImage& Frame, yarp::os::Stamp* Stamp, streamFrameListener& sourceFrame)
{
    sourceFrame.mutex.lock();
    bool ret = Frame.copy(sourceFrame.image);
    *Stamp = sourceFrame.stamp;
    sourceFrame.mutex.unlock();
    return ret;
}

bool depthCameraDriver::getImage(yarp::sig::ImageOf<PixelFloat>& Frame, yarp::os::Stamp* Stamp, streamFrameListener& sourceFrame)
{
    sourceFrame.mutex.lock();


    if(sourceFrame.frameRef.getDataSize() != sourceFrame.frameRef.getHeight() * sourceFrame.frameRef.getWidth() * sizeof(short))
    {
        yError() << "image format error";
    }
    int         w, h, i;
    w    = sourceFrame.w;
    h    = sourceFrame.h;

    float  factor;
    float* rawImage;
    short* srcRawImage;

    srcRawImage = (short*)(sourceFrame.frameRef.getData());
    factor      = sourceFrame.pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;

    Frame.resize(w, h);
    rawImage = (float*)(Frame.getRawImage());

    //TODO: optimize short-to-float cast and multiplication using SSE/SIMD instruction
    for(i = 0; i < w * h; i++)
    {
        rawImage[i] = srcRawImage[i] * factor;
    }
    *Stamp   = sourceFrame.stamp;
    sourceFrame.mutex.unlock();
    return true;
}

bool depthCameraDriver::getImages(yarp::sig::FlexImage& colorFrame, yarp::sig::ImageOf<PixelFloat>& depthFrame, yarp::os::Stamp* colorStamp, yarp::os::Stamp* depthStamp)
{
    return getImage(colorFrame, colorStamp, m_imageFrame) & getImage(depthFrame, depthStamp, m_depthFrame);
}

IRGBDSensor::RGBDSensor_status depthCameraDriver::getSensorStatus()
{
    openni::DeviceState status = DEVICE_STATE_OK;
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

yarp::os::ConstString depthCameraDriver::getLastErrorMsg(yarp::os::Stamp* timeStamp)
{

}

bool depthCameraDriver::getCameraDescription(CameraDescriptor *camera)
{
    return false;
}

bool depthCameraDriver::hasFeature(int feature, bool *hasFeature)
{
    return false;
}
bool depthCameraDriver::setFeature(int feature, double value)
{
    return false;
}
bool depthCameraDriver::getFeature(int feature, double *value)
{
    return false;
}
bool depthCameraDriver::setFeature(int feature, double value1, double value2)
{
    return false;
}
bool depthCameraDriver::getFeature(int feature, double *value1, double *value2)
{
    return false;
}
bool depthCameraDriver::hasOnOff(  int feature, bool *HasOnOff)
{
    return false;
}
bool depthCameraDriver::setActive( int feature, bool onoff)
{
    return false;
}
bool depthCameraDriver::getActive( int feature, bool *isActive)
{
    return false;
}
bool depthCameraDriver::hasAuto(   int feature, bool *hasAuto)
{
    return false;
}
bool depthCameraDriver::hasManual( int feature, bool *hasManual)
{
    return false;
}
bool depthCameraDriver::hasOnePush(int feature, bool *hasOnePush)
{
    return false;
}
bool depthCameraDriver::setMode(   int feature, FeatureMode mode)
{
    return false;
}
bool depthCameraDriver::getMode(   int feature, FeatureMode *mode)
{
    return false;
}
bool depthCameraDriver::setOnePush(int feature)
{
    return false;
}
