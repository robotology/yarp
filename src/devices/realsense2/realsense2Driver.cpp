/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdint>

#include <yarp/os/Value.h>

#include "realsense2Driver.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

using namespace std;

constexpr char accuracy       [] = "accuracy";
constexpr char clipPlanes     [] = "clipPlanes";
constexpr char depthRes       [] = "depthResolution";
constexpr char rgbRes         [] = "rgbResolution";

static std::string get_device_information(const rs2::device& dev)
{

    std::stringstream ss;
    ss << "Device information: " << std::endl;

    for (int i = 0; i < static_cast<int>(RS2_CAMERA_INFO_COUNT); i++)
    {
        rs2_camera_info info_type = static_cast<rs2_camera_info>(i);
        ss << "  " << std::left << std::setw(20) << info_type << " : ";

        if (dev.supports(info_type))
            ss << dev.get_info(info_type) << std::endl;
        else
            ss << "N/A" << std::endl;
    }
    return ss.str();
}

static int pixFormatToCode(const rs2_format p)
{
    switch(p)
    {
    case (RS2_FORMAT_RGB8):
        return VOCAB_PIXEL_RGB;

    case (RS2_FORMAT_BGR8):
        return VOCAB_PIXEL_BGR;

    case (RS2_FORMAT_Z16):
        return VOCAB_PIXEL_MONO16;

    case (RS2_FORMAT_DISPARITY16):
        return VOCAB_PIXEL_MONO16;

    case (RS2_FORMAT_RGBA8):
        return VOCAB_PIXEL_RGBA;

    case (RS2_FORMAT_BGRA8):
        return VOCAB_PIXEL_BGRA;

    case (RS2_FORMAT_Y8):
        return VOCAB_PIXEL_MONO;

    case (RS2_FORMAT_Y16):
        return VOCAB_PIXEL_MONO16;;

    case (RS2_FORMAT_RAW16):
        return VOCAB_PIXEL_MONO16;

    case (RS2_FORMAT_RAW8):
        return VOCAB_PIXEL_MONO;
    default:
        return VOCAB_PIXEL_INVALID;

    }
}

static size_t bytesPerPixel(const rs2_format format)
{
    size_t bytes_per_pixel = 0;
    switch (format)
    {
    case RS2_FORMAT_RAW8:
    case RS2_FORMAT_Y8:
        bytes_per_pixel = 1;
        break;
    case RS2_FORMAT_Z16:
    case RS2_FORMAT_DISPARITY16:
    case RS2_FORMAT_Y16:
    case RS2_FORMAT_RAW16:
        bytes_per_pixel = 2;
        break;
    case RS2_FORMAT_RGB8:
    case RS2_FORMAT_BGR8:
        bytes_per_pixel = 3;
        break;
    case RS2_FORMAT_RGBA8:
    case RS2_FORMAT_BGRA8:
        bytes_per_pixel = 4;
        break;
    default:
        break;
    }
    return bytes_per_pixel;
}


realsense2Driver::realsense2Driver() : m_paramParser(nullptr), m_depthRegistration(false),
                                       m_period(0)
{

    m_params_map =
    {
        {accuracy,       RGBDSensorParamParser::RGBDParam(accuracy,        1)},
        {clipPlanes,     RGBDSensorParamParser::RGBDParam(clipPlanes,      2)},
        {depthRes,       RGBDSensorParamParser::RGBDParam(depthRes,        2)},
        {rgbRes,         RGBDSensorParamParser::RGBDParam(rgbRes,          2)}

    };

    m_depthRegistration = true;

    m_paramParser = new RGBDSensorParamParser();

    // realsense SDK already provides them
    m_paramParser->depthIntrinsic.isOptional = true;
    m_paramParser->rgbIntrinsic.isOptional   = true;
    m_paramParser->isOptionalExtrinsic       = true;

    m_supportedFeatures.push_back(YARP_FEATURE_EXPOSURE);
    m_supportedFeatures.push_back(YARP_FEATURE_WHITE_BALANCE);
    m_supportedFeatures.push_back(YARP_FEATURE_GAIN);
    m_supportedFeatures.push_back(YARP_FEATURE_FRAME_RATE);
    m_supportedFeatures.push_back(YARP_FEATURE_MIRROR);
}

realsense2Driver::~realsense2Driver()
{
    close();

    if (m_paramParser)
    {
        delete m_paramParser;
        m_paramParser = nullptr;
    }

    return;
}

bool realsense2Driver::initializeRealsenseDevice()
{
    // TODO get configurations of the device, and read the value from the conf file
    double colorW = m_params_map[rgbRes].val[0].asDouble();
    double colorH = m_params_map[rgbRes].val[1].asDouble();
    double depthW = m_params_map[depthRes].val[0].asDouble();
    double depthH = m_params_map[depthRes].val[1].asDouble();
    m_cfg.enable_stream(RS2_STREAM_COLOR, colorW, colorH, RS2_FORMAT_RGB8, m_period);
    m_cfg.enable_stream(RS2_STREAM_DEPTH, depthW, depthH, RS2_FORMAT_Z16, m_period);

    m_pipeline.start(m_cfg);

    // Camera warmup - Dropped frames to allow stabilization
    yInfo()<<"realsenseDriver: sensor warm-up....";
    for (int i = 0; i < 30; i++)
    {
        m_pipeline.wait_for_frames();
    }
    yInfo()<<"realsenseDriver: .... device ready!";
    // First, create a rs2::context.
    rs2::device_list devices = m_ctx.query_devices();

    rs2::device selected_device;
    if (devices.size() == 0)
    {
        yError() << "realsense2Driver: No device connected, please connect a RealSense device";

        rs2::device_hub device_hub(m_ctx);

        //Using the device_hub we can block the program until a device connects
        m_device = device_hub.wait_for_device();
    }
    else
    {
        //TODO: if more are connected?!
        // Update the selected device
        m_device = devices[0];
    }


    // Given a device, we can query its sensors using:
    m_sensors = m_device.query_sensors();

    yInfo()<< "realsense2Driver: Device consists of" << m_sensors.size()<<"sensors";

    // Get stream intrinsics & extrinsics
    rs2::pipeline_profile pipeline_profile = m_pipeline.get_active_profile();
    rs2::video_stream_profile depth_stream_profile = rs2::video_stream_profile(pipeline_profile.get_stream(RS2_STREAM_DEPTH));
    rs2::video_stream_profile color_stream_profile = rs2::video_stream_profile(pipeline_profile.get_stream(RS2_STREAM_COLOR));

    m_depth_intrin = depth_stream_profile.get_intrinsics();
    m_color_intrin = color_stream_profile.get_intrinsics();
    m_depth_to_color = depth_stream_profile.get_extrinsics_to(color_stream_profile);
    m_color_to_depth = color_stream_profile.get_extrinsics_to(depth_stream_profile);

    //TODO definire cosa fare se riesco a leggere i parametri intrinseci e estrinseci dalla camera...

    return true;
}


void realsense2Driver::settingErrorMsg(const string& error, bool& ret)
{
    yError() << "realsense2Driver:" << error.c_str();
    ret = false;
}

bool realsense2Driver::setParams()
{
    return true;
}


bool realsense2Driver::open(Searchable& config)
{
    std::vector<RGBDSensorParamParser::RGBDParam*> params;
    for (auto& p:m_params_map)
    {
        params.push_back(&(p.second));
    }

    m_period = config.check("period", yarp::os::Value(30), "period of the camera").asInt();

    if (!m_paramParser->parseParam(config, params))
    {
        yError()<<"realsense2Driver: failed to parse the parameters";
        return false;
    }

    //"registered" is a hidden parameter for debugging pourpose
    m_depthRegistration = !(config.check("registered") && config.find("registered").isBool() && config.find("registered").asBool() == false);

    if (!initializeRealsenseDevice())
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

bool realsense2Driver::close()
{
    //m_pipeline.stop(); problemi, se non sta girando lancia una eccezione.
    return true;
}

int realsense2Driver::getRgbHeight()
{
    return 0;
}

int realsense2Driver::getRgbWidth()
{
    return 0;
}

bool realsense2Driver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yWarning()<<"realsense2Driver:getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool realsense2Driver::getRgbResolution(int &width, int &height)
{
    return true;
}

bool realsense2Driver::setDepthResolution(int width, int height)
{
    return setResolution(width, height);
}

bool realsense2Driver::setResolution(int width, int height)
{
    bool      bRet;
    return bRet;
}

bool realsense2Driver::setRgbResolution(int width, int height)
{
    return setResolution(width, height);
}

bool realsense2Driver::setFOV(double horizontalFov, double verticalFov)
{
    return true;
}

bool realsense2Driver::setRgbFOV(double horizontalFov, double verticalFov)
{
    return setFOV(horizontalFov, verticalFov);
}

bool realsense2Driver::setDepthFOV(double horizontalFov, double verticalFov)
{
    return setFOV(horizontalFov, verticalFov);
}

bool realsense2Driver::setDepthAccuracy(double accuracy)
{
    bool ret = true;
    return ret;
}

bool realsense2Driver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    return true;
}

bool realsense2Driver::getRgbMirroring(bool& mirror)
{
    return true;
}

bool realsense2Driver::setRgbMirroring(bool mirror)
{
    return false;
}

bool realsense2Driver::setIntrinsic(Property& intrinsic, const rs2_intrinsics &values)
{
    intrinsic.put("focalLengthX",       values.fx);
    intrinsic.put("focalLengthY",       values.fy);
    intrinsic.put("principalPointX",    values.ppx);
    intrinsic.put("principalPointY",    values.ppy);

    intrinsic.put("distortionModel", "plumb_bob");
    intrinsic.put("k1", values.coeffs[0]);
    intrinsic.put("k2", values.coeffs[1]);
    intrinsic.put("t1", values.coeffs[2]);
    intrinsic.put("t2", values.coeffs[3]);
    intrinsic.put("k3", values.coeffs[4]);

    intrinsic.put("stamp", yarp::os::Time::now());
    return true;
}

bool realsense2Driver::getRgbIntrinsicParam(Property& intrinsic)
{
    return setIntrinsic(intrinsic, m_color_intrin);
}

int  realsense2Driver::getDepthHeight()
{
    return 0;
}

int  realsense2Driver::getDepthWidth()
{
    return 0;
}

bool realsense2Driver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    return true;
}

bool realsense2Driver::getDepthIntrinsicParam(Property& intrinsic)
{
    return setIntrinsic(intrinsic, m_depth_intrin);;
}

double realsense2Driver::getDepthAccuracy()
{
    return 0;
}

bool realsense2Driver::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    return true;
}

bool realsense2Driver::setDepthClipPlanes(double nearPlane, double farPlane)
{
    return true;
}

bool realsense2Driver::getDepthMirroring(bool& mirror)
{
    return true;
}

bool realsense2Driver::setDepthMirroring(bool mirror)
{
    return true;
}

bool realsense2Driver::getExtrinsicParam(Matrix& extrinsic)
{
    extrinsic = m_paramParser->transformationMatrix;
    return true;
}

bool realsense2Driver::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    rs2::frameset data = m_pipeline.wait_for_frames();
    return getImage(rgbImage, timeStamp, data);
}

bool realsense2Driver::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    rs2::frameset data = m_pipeline.wait_for_frames();
    return getImage(depthImage, timeStamp, data);
}

bool realsense2Driver::getImage(FlexImage& Frame, Stamp *timeStamp, rs2::frameset &sourceFrame)
{
    rs2::video_frame color_frm = sourceFrame.get_color_frame();
    rs2_format format = color_frm.get_profile().format();

    int pixCode = pixFormatToCode(format);
    size_t mem_to_wrt = color_frm.get_width() * color_frm.get_height() * bytesPerPixel(format);

    if (pixCode == VOCAB_PIXEL_INVALID)
    {
        yError() << "realsense2Driver: Pixel Format not recognized";
        return false;
    }

    Frame.setPixelCode(pixCode);
    Frame.resize(m_color_intrin.width, m_color_intrin.height);

    if ((size_t) Frame.getRawImageSize() != mem_to_wrt)
    {
        yError() << "realsense2Driver: device and local copy data size doesn't match";
        return false;
    }

    memcpy((void*)Frame.getRawImage(), (void*)color_frm.get_data(), mem_to_wrt);
    m_rgb_stamp.update();
    *timeStamp = m_rgb_stamp;
    return true;
}

bool realsense2Driver::getImage(depthImage& Frame, Stamp *timeStamp, const rs2::frameset &sourceFrame)
{
    rs2::depth_frame depth_frm = sourceFrame.get_depth_frame();
    rs2_format format = depth_frm.get_profile().format();

    int pixCode = pixFormatToCode(format);

    int w = depth_frm.get_width();
    int h = depth_frm.get_height();

    if (pixCode == VOCAB_PIXEL_INVALID)
    {
        yError() << "realsense2Driver: Pixel Format not recognized";
        return false;
    }

    Frame.resize(w, h);

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h ; y++)
        {
            Frame.safePixel(x, y) = depth_frm.get_distance(x, y);
        }
    }

    m_depth_stamp.update();
    *timeStamp   = m_depth_stamp;
    return true;
}

bool realsense2Driver::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    rs2::frameset data = m_pipeline.wait_for_frames();
    return getImage(colorFrame, colorStamp, data) & getImage(depthFrame, depthStamp, data);
}

IRGBDSensor::RGBDSensor_status realsense2Driver::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

ConstString realsense2Driver::getLastErrorMsg(Stamp* timeStamp)
{
    return "AAAA to implement";
}

bool realsense2Driver::getCameraDescription(CameraDescriptor* camera)
{
    camera->deviceDescription = get_device_information(m_device);
    camera->busType = BUS_USB;
    return true;
}

bool realsense2Driver::hasFeature(int feature, bool* hasFeature)
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

bool realsense2Driver::setFeature(int feature, double value)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    switch(f)
    {
    case YARP_FEATURE_EXPOSURE:
        break;
    case YARP_FEATURE_GAIN:
        break;
    case YARP_FEATURE_FRAME_RATE:
    {
        break;
    }
    case YARP_FEATURE_WHITE_BALANCE:
        return false;
    default:
        yError() << "feature not supported!";
        return false;
    }
    return true;
}

bool realsense2Driver::getFeature(int feature, double *value)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    switch(f)
    {
    case YARP_FEATURE_EXPOSURE:
        break;
    case YARP_FEATURE_GAIN:
        break;
    case YARP_FEATURE_FRAME_RATE:
        break;
    case YARP_FEATURE_WHITE_BALANCE:
        return false;
    default:
        return false;
    }
    return true;
}

bool realsense2Driver::setFeature(int feature, double value1, double value2)
{
    yError() << "no 2-valued feature are supported";
    return false;
}

bool realsense2Driver::getFeature(int feature, double *value1, double *value2)
{
    yError() << "no 2-valued feature are supported";
    return false;
}

bool realsense2Driver::hasOnOff(  int feature, bool *HasOnOff)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_WHITE_BALANCE || f == YARP_FEATURE_MIRROR)
    {
        *HasOnOff = true;
        return true;
    }
    *HasOnOff = false;
    return true;
}

bool realsense2Driver::setActive( int feature, bool onoff)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if (!hasOnOff(feature, &b) || !b)
    {
        yError() << "feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }

    switch(feature)
    {
    case YARP_FEATURE_MIRROR:
        break;
    case YARP_FEATURE_WHITE_BALANCE:
        break;
    default:
        return false;
    }


    return true;
}

bool realsense2Driver::getActive( int feature, bool *isActive)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if (!hasOnOff(feature, &b) || !b)
    {
        yError() << "feature does not have OnOff.. call hasOnOff() to know if a specific feature support OnOff mode";
        return false;
    }
    // see depthcamera
    return true;
}

bool realsense2Driver::hasAuto(int feature, bool *hasAuto)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_WHITE_BALANCE)
    {
        *hasAuto = true;
        return true;
    }
    *hasAuto = false;
    return true;
}

bool realsense2Driver::hasManual( int feature, bool* hasManual)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE || f == YARP_FEATURE_FRAME_RATE || f == YARP_FEATURE_GAIN)
    {
        *hasManual = true;
        return true;
    }
    *hasManual = false;
    return true;
}

bool realsense2Driver::hasOnePush(int feature, bool* hasOnePush)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    return hasAuto(feature, hasOnePush);
}

bool realsense2Driver::setMode(int feature, FeatureMode mode)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE)
    {
        switch(mode)
        {
        case MODE_AUTO:
            break;
        case MODE_MANUAL:
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

bool realsense2Driver::getMode(int feature, FeatureMode* mode)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    cameraFeature_id_t f = static_cast<cameraFeature_id_t>(feature);
    if (f == YARP_FEATURE_EXPOSURE)
    {
        return true;
    }

    yError() << "feature does not have both auto and manual mode";
    return false;
}

bool realsense2Driver::setOnePush(int feature)
{
    bool b;
    if (!hasFeature(feature, &b) || !b)
    {
        yError() << "feature not supported!";
        return false;
    }

    if (!hasOnePush(feature, &b) || !b)
    {
        yError() << "feature doesn't have OnePush";
        return false;
    }

    setMode(feature, MODE_AUTO);
    setMode(feature, MODE_MANUAL);

    return true;
}
