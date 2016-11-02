#include "depthCameraDriver.h"
using namespace yarp::dev;
using namespace yarp::sig;
using namespace openni;


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

}

int depthCameraDriver::getRgbHeight()
{

}

int depthCameraDriver::getRgbWidth()
{

}

bool depthCameraDriver::getRgbFOV(int& horizontalFov, int& verticalFov)
{

}

bool depthCameraDriver::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{

}

bool depthCameraDriver::getRgbSensorInfo(yarp::os::Property& info)
{

}

int  depthCameraDriver::getDepthHeight()
{

}

int  depthCameraDriver::getDepthWidth()
{

}

bool depthCameraDriver::getDepthFOV(int& horizontalFov, int& verticalFov)
{

}

bool depthCameraDriver::getDepthIntrinsicParam(yarp::os::Property& intrinsic)
{

}

bool depthCameraDriver::getDepthSensorInfo(yarp::os::Property info)
{

}

double depthCameraDriver::getDepthAccuracy()
{

}

bool depthCameraDriver::getDepthClipPlanes(int& near, int& far)
{
    near = 400;
    far  = 5000;

}

bool depthCameraDriver::setDepthClipPlanes(int near, int far)
{

}

bool depthCameraDriver::getExtrinsicParam(yarp::os::Property& extrinsic)
{

}

bool depthCameraDriver::getRgbImage(yarp::sig::FlexImage& rgbImage, yarp::os::Stamp* timeStamp)
{

}

bool depthCameraDriver::getDepthImage(yarp::sig::FlexImage& depthImage, yarp::os::Stamp* timeStamp)
{

}

int depthCameraDriver::pixFormatToCode(PixelFormat p)
{
    switch(p)
    {
    case (PIXEL_FORMAT_RGB888):
        return VOCAB_PIXEL_RGB;

    case (PIXEL_FORMAT_DEPTH_1_MM):
        return VOCAB_PIXEL_MONO16;

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
    int pixF;
    void* data = 0;
    pixF = pixFormatToCode(sourceFrame.pixF);
    if(pixF == VOCAB_PIXEL_INVALID)
    {
        yError() << "depthCameraDriver: Pixel Format not recognized";
        return false;
    }

    Frame.setPixelCode(pixF);
    data = (void*)sourceFrame.frameRef.getData();
    if (!data)
    {
        yInfo() << "frame lost";
        return false;
    }
    Frame.setExternal(data, sourceFrame.frameRef.getWidth(), sourceFrame.frameRef.getHeight());

    if(Stamp)
    {
        Stamp->update();
    }

    return true;
}

bool depthCameraDriver::getImages(yarp::sig::FlexImage& colorFrame, yarp::sig::FlexImage& depthFrame, yarp::os::Stamp* colorStamp, yarp::os::Stamp* depthStamp)
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
