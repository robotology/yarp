#include "depthCameraDriver.h"
using namespace yarp::dev;
using namespace yarp::sig;
using namespace openni;

void streamFrameListener::onNewFrame(openni::VideoStream& stream)
{

    mutex.lock();
    if (!frameRef.isValid() || !frameRef.getData())
    {
        yInfo() << "frame lost";
        return;
    }

    PixelFormat pixF;
    int         pixC;
    int         w, h, i;

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

    if((pixC == VOCAB_PIXEL_MONO_FLOAT && pixF == PIXEL_FORMAT_DEPTH_1_MM)   ||
       (pixC == VOCAB_PIXEL_MONO_FLOAT && pixF == PIXEL_FORMAT_DEPTH_100_UM))
    {
        if(frameRef.getDataSize() != frameRef.getHeight() * frameRef.getWidth() * sizeof(short))
        {
            yError() << "image format error";
        }

        float  factor;
        float* rawImage;
        short* srcRawImage;

        srcRawImage = (short*)(frameRef.getData());
        factor      = pixF == PIXEL_FORMAT_DEPTH_1_MM ? 0.001 : 0.0001;


        image.resize(w, h);
        rawImage = (float*)(image.getRawImage());

        //TODO: optimize short-to-float cast and multiplication using SSE/SIMD instruction
        for(i = 0; i < w * h; i++)
        {
            rawImage[i] = srcRawImage[i] * factor;
        }
    }
    else if(pixC == VOCAB_PIXEL_RGB)
    {
        image.resize(w, h);
        if(frameRef.getDataSize() != image.getRawImageSize())
        {
            yError() << "image format error";
        }
        memcpy((void*)image.getRawImage(), (void*)frameRef.getData(), frameRef.getDataSize());

    }

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
        return VOCAB_PIXEL_MONO_FLOAT;

    case (PIXEL_FORMAT_DEPTH_100_UM):
        return VOCAB_PIXEL_MONO_FLOAT;

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
    sourceFrame.mutex.unlock();
    return ret;
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
