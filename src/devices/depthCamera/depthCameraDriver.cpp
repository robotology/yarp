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
    m_supportedFeatures.push_back(YARP_FEATURE_EXPOSURE);
    m_supportedFeatures.push_back(YARP_FEATURE_WHITE_BALANCE);
    m_supportedFeatures.push_back(YARP_FEATURE_GAIN);
    m_supportedFeatures.push_back(YARP_FEATURE_FRAME_RATE);
    m_supportedFeatures.push_back(YARP_FEATURE_MIRROR);

    // initialize struct for params
    m_cameraDescription.accuracy.name   = "accuracy";
    m_cameraDescription.clipPlanes.name = "clipPlanes";
    m_cameraDescription.clipPlanes.size = 2;
    m_cameraDescription.depth_Fov.name  = "depthFov";
    m_cameraDescription.depth_Fov.size  = 2;
    m_cameraDescription.depthRes.name   = "depthResolution";
    m_cameraDescription.depthRes.size   = 2;
    m_cameraDescription.rgb_Fov.name    = "rgbFov";
    m_cameraDescription.rgb_Fov.size    = 2;
    m_cameraDescription.rgbRes.name     = "rgbResolution";
    m_cameraDescription.rgbRes.size     = 2;
    m_cameraDescription.mirroring.name  = "mirroring";
    m_cameraDescription.transformationMatrix.resize(4, 4);
    m_cameraDescription.depthIntrinsic.retificationMatrix.resize(4, 4);
    m_cameraDescription.rgbIntrinsic.retificationMatrix.resize(4, 4);

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

    ret3 = true;


    ret1 = checkParam(settings,    param, param.isSetting);    // look for settings
    ret2 = checkParam(description, param, param.isDescription);// look for HW_DESCRIPTION

    if( (param.isSetting) && (param.isDescription) )
    {
        yError() << "Setting " << param.name << " can either be a 'SETTING' or 'HW_DESCRIPTION', not both. Fix the config file. \
                    Look for documentation online.";
        ret3 = false;
    }
    return (ret1 && ret2 && ret3);
}

bool depthCameraDriver::checkParam(const Bottle& input, RGBDParam& param, bool& found)
{
    bool ret = false;
    yarp::os::Value v;

    if(input.check(param.name))
    {
        v = input.find(param.name);
        if(v.isNull())
        {
            yError() << "Parameter " << param.name << " malformed. Check your config file.";
            return false;
        }

        if(param.size > 1 && !v.isList())
        {
            yError() << "depthCameraDriver: parameter" << param.name << "size should be" << param.size <<  \
                        ". suggestion: parameter with more than 1 value should be between '(' parentheses";
            return false;
        }
        if(v.isList() )
        {
            yDebug() << param.name << "is list";
            // check single or more params
            if(param.size == 1)
            {
                yError() << "Parameter " << param.name << " should be a single value.";
                return false;
            }

            // check size of data match
            Bottle* b = v.asList();
            if(b->size() != param.size)
            {
                yError() << "Parameter " << param.name << " size should be " << param.size << ", got " << b->size() << "instead. Check your config file";
                return false;
            }

            // All ok here, fill the data
            param.val.resize(param.size);
            for(int i=0; i<param.size; i++)
            {
                yDebug() << b->get(i).toString();
                param.val[i] = b->get(i); // maybe i=1? check...
            }
            return true;
        }
        else  // got a single value from file
        {
            yDebug() << param.name << "is not list";

            ret          = true;
            param.val[0] = v;
            found        = true;
        }
    }
    else
    {
        ret   = true;
        found = false;
    }
    return ret;
}

void depthCameraDriver::settingErrorMsg(const string& error, bool& ret)
{
    yError() << error.c_str();
    ret = false;
}

bool depthCameraDriver::setParams(const Bottle& settings, const Bottle& description)
{
    bool ret;
    ret = true;

    if(!checkParam(settings, description, m_cameraDescription.accuracy)   ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.clipPlanes) ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.depth_Fov)  ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.depthRes)   ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.rgb_Fov)    ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.rgbRes)     ) ret = false;
    if(!checkParam(settings, description, m_cameraDescription.mirroring)  ) ret = false;

    if(!ret)
    {
        yError() << "depthCamera driver input file not correct, please fix it!";
        return false;
    }

    // Do all required settings

    //ACCURACY
    if(m_cameraDescription.accuracy.isSetting)
    {
        if(!m_cameraDescription.accuracy.val[0].isDouble() )
            settingErrorMsg("Param " + m_cameraDescription.accuracy.name + " is not a double as it should be.", ret);

        if(! setDepthAccuracy(m_cameraDescription.accuracy.val[0].asDouble() ) )
            settingErrorMsg("Setting param " + m_cameraDescription.accuracy.name + " failed... quitting.", ret);
    }

    //CLIP_PLANES
    if(m_cameraDescription.clipPlanes.isSetting)
    {
        if(!m_cameraDescription.clipPlanes.val[0].isDouble() )
            settingErrorMsg("Param " + m_cameraDescription.clipPlanes.name + " is not a double as it should be.", ret);

        if(!m_cameraDescription.clipPlanes.val[1].isDouble() )
            settingErrorMsg("Param " + m_cameraDescription.clipPlanes.name + " is not a double as it should be.", ret);

        if(! setDepthClipPlanes(m_cameraDescription.clipPlanes.val[0].asDouble(), m_cameraDescription.clipPlanes.val[1].asDouble() ) )
            settingErrorMsg("Setting param " + m_cameraDescription.clipPlanes.name + " failed... quitting.", ret);
    }

    //DEPTH_FOV
    if(m_cameraDescription.depth_Fov.isSetting)
    {
        Value p1, p2;
        p1 = m_cameraDescription.depth_Fov.val[0];
        p2 = m_cameraDescription.depth_Fov.val[1];

        if(!p1.isDouble() || !p2.isDouble() )
            settingErrorMsg("Param " + m_cameraDescription.depth_Fov.name + " is not a double as it should be.", ret);

        if(! setDepthFOV(p1.asDouble(), p2.asDouble() ) )
            settingErrorMsg("Setting param " + m_cameraDescription.depth_Fov.name + " failed... quitting.", ret);
    }

    //DEPTH_RES
    if(m_cameraDescription.depthRes.isSetting)
    {
        Value p1, p2;
        p1 = m_cameraDescription.depthRes.val[0];
        p2 = m_cameraDescription.depthRes.val[1];

        if(!p1.isInt() || !p2.isInt() )
            settingErrorMsg("Param " + m_cameraDescription.depthRes.name + " is not a double as it should be.", ret);

        if(! setDepthResolution(p1.asInt(), p2.asInt()))
            settingErrorMsg("Setting param " + m_cameraDescription.depthRes.name + " failed... quitting.", ret);
    }

    //RGB_FOV
    if(m_cameraDescription.rgb_Fov.isSetting)
    {
        Value p1, p2;
        p1 = m_cameraDescription.rgb_Fov.val[0];
        p2 = m_cameraDescription.rgb_Fov.val[1];

        if(!p1.isDouble() || !p2.isDouble() )
            settingErrorMsg("Param " + m_cameraDescription.rgb_Fov.name + " is not a double as it should be.", ret);

        if(! setRgbFOV(p1.asDouble(), p2.asDouble() ) )
            settingErrorMsg("Setting param " + m_cameraDescription.rgb_Fov.name + " failed... quitting.", ret);
    }

    //RGB_RES
    if(m_cameraDescription.rgbRes.isSetting)
    {
        Value p1, p2;
        p1 = m_cameraDescription.rgbRes.val[0];
        p2 = m_cameraDescription.rgbRes.val[1];

        if(!p1.isInt() || !p2.isInt() )
            settingErrorMsg("Param " + m_cameraDescription.rgbRes.name + " is not a double as it should be.", ret);

        if(! setRgbResolution(p1.asInt(), p2.asInt()))
            settingErrorMsg("Setting param " + m_cameraDescription.rgbRes.name + " failed... quitting.", ret);
    }

    //MIRRORING
    if(m_cameraDescription.mirroring.isSetting)
    {
        Value& v = m_cameraDescription.mirroring.val[0];
        if(!v.isBool() )
            settingErrorMsg("Param " + m_cameraDescription.mirroring.name + " is not a double as it should be.", ret);

        if(!this->setActive(YARP_FEATURE_MIRROR, v.asBool()))
            settingErrorMsg("Setting param " + m_cameraDescription.mirroring.name + " failed... quitting.", ret);
    }

    return ret;
}

bool depthCameraDriver::parseIntrinsic(const Searchable& config, const string& groupName, intrinsicParams &params)
{

    pair<string, double*>          realparam;
    vector<pair<string, double*> > realParams;
    size_t                         i, j;

    if(!config.check(groupName))
    {
        yError() << "depthCameraDriver: missing" << groupName << "section on the configuration file";
        return false;
    }
    Bottle& intrinsic = config.findGroup(groupName);

    if(!intrinsic.check("retificationMatrix"))
    {
        yError() << "depthCameraDriver: retificationMatrix not present!";
        return false;
    }
    Bottle* ret_m;
    ret_m = intrinsic.find("retificationMatrix").asList();
    if(!(ret_m->size() == 4*4))
    {
        yError() << "depthCameraDriver: the size of the retification matrix is wrong";
        return false;
    }
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            int k = i*4+j;
            Value& v = ret_m->get(k);
            if(!v.isDouble())
            {
                yError() << "wrong data format on retification matrix (position" << k << ")";
                return false;
            }
            params.retificationMatrix[i][j] = v.asDouble();
        }
    }

    realparam.first = "focalLenghtX";       realparam.second = &params.focalLenghtX;    realParams.push_back(realparam);
    realparam.first = "focalLenghtY";       realparam.second = &params.focalLenghtY;    realParams.push_back(realparam);
    realparam.first = "principalPointX";    realparam.second = &params.principalPointX; realParams.push_back(realparam);
    realparam.first = "principalPointY";    realparam.second = &params.principalPointY; realParams.push_back(realparam);
    for(i = 0; i < realParams.size(); i++)
    {
        if(!intrinsic.check(realParams[i].first))
        {
            yError() << "depthCameraDriver: missing" << realParams[i].first << "param in" << groupName << "group in the configuration file";
        }
        *(realParams[i].second) = intrinsic.find(realParams[i].first).asDouble();
    }

    if(!intrinsic.check("distortionModel"))
    {
        yError() << "missing distortionModel param in configuration";
        return false;
    }

    if(!config.check(intrinsic.find("distortionModel").asString()))
    {
        yError() << "missing" << intrinsic.find("distortionModel").asString() << "group in configuration file";
        return false;
    }

    Bottle& distortion = config.findGroup(intrinsic.find("distortionModel").asString());

    if(!distortion.check("name"))
    {
        yError() << "depthCameraDriver: missing name param in" << config.find("distortionModel").asString() << "group in configuration file";
        return false;
    }
    if(distortion.find("name").asString() != "plumb_bob")
    {
        yError() << "depthCameraDriver: only plumb_bob distortion model is supported at the moment";
        return false;
    }

    realParams.clear();
    realparam.first = "k1";    realparam.second = &params.distortionModel.k1; realParams.push_back(realparam);
    realparam.first = "k2";    realparam.second = &params.distortionModel.k2; realParams.push_back(realparam);
    realparam.first = "t1";    realparam.second = &params.distortionModel.t1; realParams.push_back(realparam);
    realparam.first = "t2";    realparam.second = &params.distortionModel.t2; realParams.push_back(realparam);
    realparam.first = "k3";    realparam.second = &params.distortionModel.k3; realParams.push_back(realparam);

    for(i = 0; i < realParams.size(); i++)
    {
        if(!distortion.check(realParams[i].first))
        {
            yError() << "depthCameraDriver: missing" << realParams[i].first << "param in" << intrinsic.find("distortionModel").asString() << "group in the configuration file";
        }
        *(realParams[i].second) = distortion.find(realParams[i].first).asDouble();
    }

    return true;
}

bool depthCameraDriver::open(Searchable& config)
{
    if(!initializeOpeNIDevice())
    {
        return false;
    }

    if(!config.check("SETTINGS"))
    {
        yError() << "depthCameraDriver: missing SETTINGS section on the configuration file";
        return false;
    }
    Bottle& settings = config.findGroup("SETTINGS");

    if(!config.check("HW_DESCRIPTION"))
    {
        yError() << "depthCameraDriver: missing HW_DESCRIPTION section on the configuration file";
        return false;
    }
    Bottle& description = config.findGroup("HW_DESCRIPTION");

    if(!config.check("DEPTH_INTRINSIC_PARAMETERS"))
    {
        yError() << "depthCameraDriver: missing DEPTH_INTRINSIC_PARAMETERS section on the configuration file";
        return false;
    }
    Bottle& depth_intrinsic = config.findGroup("DEPTH_INTRINSIC_PARAMETERS");

    // setting Parameters
    if(!setParams(settings, description))
    {
        return false;
    }

    if(!parseIntrinsic(config, "RGB_INTRINSIC_PARAMETERS", m_cameraDescription.rgbIntrinsic))
    {
        return false;
    }

    if(!parseIntrinsic(config, "DEPTH_INTRINSIC_PARAMETERS", m_cameraDescription.depthIntrinsic))
    {
        return false;
    }

    if(!config.check("EXTRINSIC_PARAMETERS"))
    {
        yError() << "depthCameraDriver: missing EXTRINSIC_PARAMETERS section on the configuration file";
        return false;
    }
    Bottle& extrinsic = config.findGroup("EXTRINSIC_PARAMETERS");

    if(!extrinsic.check("transformation"))
    {
        yError() << "depthCameraDriver: missing transformation parameter under EXTRINSIC_PARAMETERS group in configuration file";
        return false;
    }

    Bottle* transformation;
    transformation = extrinsic.find("transformation").asList();
    if(!(transformation->size() == 4*4))
    {
        yError() << "depthCameraDriver: the size of the transformation matrix is wrong";
        return false;
    }
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int k = i*4+j;
            Value& v = transformation->get(k);
            if(!v.isDouble())
            {
                yError() << "wrong data format on transformation matrix (position" << k << ")";
                return false;
            }
            m_cameraDescription.transformationMatrix[i][j] = v.asDouble();
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
    if(m_cameraDescription.rgbRes.isDescription)
    {
        return m_cameraDescription.rgbRes.val.at(0).asInt();
    }

    return m_imageStream.getVideoMode().getResolutionY();
}

int depthCameraDriver::getRgbWidth()
{    
    if(m_cameraDescription.rgbRes.isDescription)
    {
        return m_cameraDescription.rgbRes.val.at(1).asInt();
    }

    return m_imageStream.getVideoMode().getResolutionX();
}

bool depthCameraDriver::setDepthResolution(int width, int height)
{
    if(m_cameraDescription.depthRes.isDescription)
    {
        yError() << "depthCameraDriver: cannot set. Depth resolution is a description!";
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

bool depthCameraDriver::setIntrinsic(Property& intrinsic, const intrinsicParams& values)
{
    intrinsic.put("focalLenghtX",       values.focalLenghtX);
    intrinsic.put("focalLenghtY",       values.focalLenghtY);
    intrinsic.put("principalPointX",    values.principalPointX);
    intrinsic.put("principalPointY",    values.principalPointY);
    Bottle mat;
    for(int i = 0; i < 4; i++)
    {
        mat.addDouble(values.retificationMatrix[i][0]);
        mat.addDouble(values.retificationMatrix[i][1]);
        mat.addDouble(values.retificationMatrix[i][2]);
        mat.addDouble(values.retificationMatrix[i][3]);
    }

    intrinsic.put("retificationMatrix", Value(mat.toString()));
    return true;
}

bool depthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
{
    return setIntrinsic(intrinsic, m_cameraDescription.rgbIntrinsic);
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
    return setIntrinsic(intrinsic, m_cameraDescription.rgbIntrinsic);;
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

bool depthCameraDriver::getExtrinsicParam(Matrix& extrinsic)
{
    extrinsic = m_cameraDescription.transformationMatrix;
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

    if(std::find(m_supportedFeatures.begin(), m_supportedFeatures.end(), f) != m_supportedFeatures.end())
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
    if(f == YARP_FEATURE_WHITE_BALANCE || f == YARP_FEATURE_MIRROR)
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
