/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


#include "V4L_camera.h"
#include "list.h"
#include "USBcameraLogComponent.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <cstdio>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>

using namespace yarp::os;
using namespace yarp::dev;


static double getEpochTimeShift()
{
    struct timeval epochtime;
    struct timespec vsTime;

    gettimeofday(&epochtime, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &vsTime);

    double uptime = vsTime.tv_sec + vsTime.tv_nsec / 1000000000.0;
    double epoch = epochtime.tv_sec + epochtime.tv_usec / 1000000.0;
    return epoch - uptime;
}


double V4L_camera::checkDouble(yarp::os::Searchable& config, const char* key)
{
    if (config.check(key)) {
        return config.find(key).asFloat64();
    }

    return -1.0;
}

#define NOT_PRESENT -1
int V4L_camera::convertYARP_to_V4L(int feature)
{
    switch (feature) {
    case YARP_FEATURE_BRIGHTNESS:
        return V4L2_CID_BRIGHTNESS;
    case YARP_FEATURE_SHUTTER: // this maps also on exposure
    case YARP_FEATURE_EXPOSURE:
        return V4L2_CID_EXPOSURE;
    case YARP_FEATURE_SHARPNESS:
        return V4L2_CID_SHARPNESS;
    case YARP_FEATURE_HUE:
        return V4L2_CID_HUE;
    case YARP_FEATURE_SATURATION:
        return V4L2_CID_SATURATION;
    case YARP_FEATURE_GAMMA:
        return V4L2_CID_GAMMA;
    case YARP_FEATURE_GAIN:
        return V4L2_CID_GAIN;
    case YARP_FEATURE_IRIS:
        return V4L2_CID_IRIS_ABSOLUTE;

        //         case YARP_FEATURE_WHITE_BALANCE:  -> this has to e mapped on the couple V4L2_CID_BLUE_BALANCE && V4L2_CID_RED_BALANCE

        //////////////////////////
        // not yet implemented  //
        //////////////////////////
        //         case YARP_FEATURE_FOCUS:          return DC1394_FEATURE_FOCUS;
        //         case YARP_FEATURE_TEMPERATURE:    return DC1394_FEATURE_TEMPERATURE;
        //         case YARP_FEATURE_TRIGGER:        return DC1394_FEATURE_TRIGGER;
        //         case YARP_FEATURE_TRIGGER_DELAY:  return DC1394_FEATURE_TRIGGER_DELAY;
        //         case YARP_FEATURE_FRAME_RATE:     return DC1394_FEATURE_FRAME_RATE;
        //         case YARP_FEATURE_ZOOM:           return DC1394_FEATURE_ZOOM;
        //         case YARP_FEATURE_PAN:            return DC1394_FEATURE_PAN;
        //         case YARP_FEATURE_TILT:           return DC1394_FEATURE_TILT;
    }
    return NOT_PRESENT;
}

V4L_camera::V4L_camera() :
        PeriodicThread(1.0 / DEFAULT_FRAMERATE), doCropping(false), toEpochOffset(getEpochTimeShift())
{
    verbose = false;
    param.fps = DEFAULT_FRAMERATE;
    param.io = IO_METHOD_MMAP;
    param.deviceId = "/dev/video0";
    param.fd = -1;
    param.n_buffers = 0;
    param.buffers = nullptr;
    param.camModel = STANDARD_UVC;
    param.dual = false;

    param.addictionalResize = false;
    param.resizeOffset_x = 0;
    param.resizeOffset_y = 0;
    param.resizeWidth = 0;
    param.resizeHeight = 0;

    _v4lconvert_data = YARP_NULLPTR;
    myCounter = 0;
    timeTot = 0;

    param.user_width = DEFAULT_WIDTH;
    param.user_height = DEFAULT_HEIGHT;
    param.raw_image = YARP_NULLPTR;
    param.raw_image_size = 0;
    param.read_image = YARP_NULLPTR;

    param.src_image = YARP_NULLPTR;
    param.src_image_size = 0;

    param.dst_image_rgb = YARP_NULLPTR;
    param.dst_image_size_rgb = 0;

    use_exposure_absolute = false;
    camMap["default"] = STANDARD_UVC;
    camMap["leopard_python"] = LEOPARD_PYTHON;

    configFx = false;
    configFy = false;
    configPPx = false;
    configPPy = false;
    configRet = false;
    configDistM = false;
    configIntrins = false;
    configured = false;

    // leopard debugging
    pixel_fmt_leo = V4L2_PIX_FMT_SGRBG8;
    bit_shift = 2; // after firmware update, the shift has to be 2 instead of 4
    bit_bayer = 8;
}

yarp::os::Stamp V4L_camera::getLastInputStamp()
{
    return timeStamp;
}

int V4L_camera::convertV4L_to_YARP_format(int format)
{
    switch (format) {
    case V4L2_PIX_FMT_GREY:
        return VOCAB_PIXEL_MONO;
    case V4L2_PIX_FMT_Y16:
        return VOCAB_PIXEL_MONO16;
    case V4L2_PIX_FMT_RGB24:
        return VOCAB_PIXEL_RGB;
//     case V4L2_PIX_FMT_ABGR32  : return VOCAB_PIXEL_BGRA; //unsupported by linux travis configuration
    case V4L2_PIX_FMT_BGR24:
        return VOCAB_PIXEL_BGR;
    case V4L2_PIX_FMT_SGRBG8:
        return VOCAB_PIXEL_ENCODING_BAYER_GRBG8;
    case V4L2_PIX_FMT_SBGGR8:
        return VOCAB_PIXEL_ENCODING_BAYER_BGGR8;
    case V4L2_PIX_FMT_SBGGR16:
        return VOCAB_PIXEL_ENCODING_BAYER_BGGR16;
    case V4L2_PIX_FMT_SGBRG8:
        return VOCAB_PIXEL_ENCODING_BAYER_GBRG8;
    case V4L2_PIX_FMT_SRGGB8:
        return VOCAB_PIXEL_ENCODING_BAYER_RGGB8;
    case V4L2_PIX_FMT_YUV420:
        return VOCAB_PIXEL_YUV_420;
    case V4L2_PIX_FMT_YUV444:
        return VOCAB_PIXEL_YUV_444;
    case V4L2_PIX_FMT_YYUV:
        return VOCAB_PIXEL_YUV_422;
    case V4L2_PIX_FMT_YUV411P:
        return VOCAB_PIXEL_YUV_411;
    }
    return NOT_PRESENT;
}

void V4L_camera::populateConfigurations()
{
    struct v4l2_fmtdesc fmt;
    struct v4l2_frmsizeenum frmsize;
    struct v4l2_frmivalenum frmival;

    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while (ioctl(param.fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
        memset(&frmsize, 0, sizeof(v4l2_frmsizeenum));
        frmsize.pixel_format = fmt.pixelformat;
        frmsize.index = 0;
        frmsize.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        while (xioctl(param.fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
            if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                memset(&frmival, 0, sizeof(v4l2_frmivalenum));
                frmival.index = 0;
                frmival.pixel_format = fmt.pixelformat;
                frmival.width = frmsize.discrete.width;
                frmival.height = frmsize.discrete.height;
                frmsize.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                while (xioctl(param.fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0) {
                    CameraConfig c;
                    c.pixelCoding = (YarpVocabPixelTypesEnum)convertV4L_to_YARP_format(frmival.pixel_format);
                    c.width = frmival.width;
                    c.height = frmival.height;
                    c.framerate = (1.0 * frmival.discrete.denominator) / frmival.discrete.numerator;
                    param.configurations.push_back(c);
                    frmival.index++;
                }
            }
            frmsize.index++;
        }
        fmt.index++;
    }
}

/**
 *    open device
 */
bool V4L_camera::open(yarp::os::Searchable& config)
{
    struct stat st;
    yCTrace(USBCAMERA) << "input params are " << config.toString();


    if (!fromConfig(config)) {
        return false;
    }

    // stat file
    if (-1 == stat(param.deviceId.c_str(), &st)) {
        yCError(USBCAMERA, "Cannot identify '%s': %d, %s", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }

    // check if it is a device
    if (!S_ISCHR(st.st_mode)) {
        yCError(USBCAMERA, "%s is no device", param.deviceId.c_str());
        return false;
    }

    // open device
    param.fd = v4l2_open(param.deviceId.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    // check if opening was successfull
    if (-1 == param.fd) {
        yCError(USBCAMERA, "Cannot open '%s': %d, %s", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }

    // if previous instance crashed, maybe will help (?)
    captureStop();
    deviceUninit();
    v4l2_close(param.fd);

    yarp::os::Time::delay(1);
    // re-open device
    param.fd = v4l2_open(param.deviceId.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    // check if opening was successfull
    if (-1 == param.fd) {
        yCError(USBCAMERA, "Cannot open '%s': %d, %s", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }


    // Initting video device
    deviceInit();
    if (verbose) {
        enumerate_controls();
    }
    if (!check_V4L2_control(V4L2_CID_EXPOSURE)) {
        use_exposure_absolute = check_V4L2_control(V4L2_CID_EXPOSURE_ABSOLUTE);
    }
    captureStart();
    yarp::os::Time::delay(0.5);
    start();

    populateConfigurations();

    // Configure the device settings from input file
    setFeature(YARP_FEATURE_GAIN, checkDouble(config, "gain"));
    setFeature(YARP_FEATURE_EXPOSURE, checkDouble(config, "exposure"));
    setFeature(YARP_FEATURE_BRIGHTNESS, checkDouble(config, "brightness"));
    setFeature(YARP_FEATURE_SHARPNESS, checkDouble(config, "sharpness"));
    yarp::os::Bottle& white_balance = config.findGroup("white_balance");
    if (!white_balance.isNull()) {
        setFeature(YARP_FEATURE_WHITE_BALANCE, white_balance.get(2).asFloat64(), white_balance.get(1).asFloat64());
    }
    setFeature(YARP_FEATURE_HUE, checkDouble(config, "hue"));
    setFeature(YARP_FEATURE_SATURATION, checkDouble(config, "saturation"));
    setFeature(YARP_FEATURE_GAMMA, checkDouble(config, "gamma"));
    setFeature(YARP_FEATURE_SHUTTER, checkDouble(config, "shutter"));
    setFeature(YARP_FEATURE_IRIS, checkDouble(config, "iris"));

    return true;
}

int V4L_camera::getRgbHeight()
{
    return height();
}

int V4L_camera::getRgbWidth()
{
    return width();
}

bool V4L_camera::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig>& configurations)
{
    configurations = param.configurations;
    return true;
}
bool V4L_camera::getRgbResolution(int& width, int& height)
{
    width = param.user_width;
    height = param.user_height;
    return true;
}

bool V4L_camera::setRgbResolution(int width, int height)
{
    mutex.wait();
    captureStop();
    deviceUninit();
    param.user_width = width;
    param.user_height = height;
    bool res = deviceInit();
    captureStart();
    mutex.post();
    return res;
}

bool V4L_camera::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    horizontalFov = param.horizontalFov;
    verticalFov = param.verticalFov;
    return configFx && configFy;
}

bool V4L_camera::setRgbFOV(double horizontalFov, double verticalFov)
{
    yCError(USBCAMERA) << "cannot set fov";
    return false;
}

bool V4L_camera::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{
    intrinsic = param.intrinsic;
    return configIntrins;
}

bool V4L_camera::getRgbMirroring(bool& mirror)
{

    mirror = (ioctl(param.fd, V4L2_CID_HFLIP) != 0);
    return true;
}

bool V4L_camera::setRgbMirroring(bool mirror)
{
    int ret = ioctl(param.fd, V4L2_CID_HFLIP, &mirror);
    if (ret < 0) {
        yCError(USBCAMERA) << "V4L2_CID_HFLIP - Unable to mirror image-" << strerror(errno);
        return false;
    }
    return true;
}

bool V4L_camera::fromConfig(yarp::os::Searchable& config)
{
    if (config.check("verbose")) {
        verbose = true;
    }

    if (!config.check("width")) {
        yCDebug(USBCAMERA) << "width parameter not found, using default value of " << DEFAULT_WIDTH;
        param.user_width = DEFAULT_WIDTH;
    } else {
        param.user_width = config.find("width").asInt32();
    }

    if (!config.check("height")) {
        yCDebug(USBCAMERA) << "height parameter not found, using default value of " << DEFAULT_HEIGHT;
        param.user_height = DEFAULT_HEIGHT;
    } else {
        param.user_height = config.find("height").asInt32();
    }

    if (!config.check("framerate")) {
        yCDebug(USBCAMERA) << "framerate parameter not found, using default value of " << DEFAULT_FRAMERATE;
        param.fps = DEFAULT_FRAMERATE;
    } else {
        param.fps = config.find("framerate").asInt32();
    }

    if (!config.check("d")) {
        yCError(USBCAMERA) << "No camera identifier was specified! (e.g. '--d /dev/video0' on Linux OS)";
        return false;
    }

    param.deviceId = config.find("d").asString();
    param.flip = config.check("flip", Value("false")).asBool();

    if (!config.check("camModel")) {
        yCInfo(USBCAMERA) << "No 'camModel' was specified, working with 'standard' uvc";
        param.camModel = STANDARD_UVC;
    } else {
        std::map<std::string, supported_cams>::iterator it = camMap.find(config.find("camModel").asString());
        if (it != camMap.end()) {
            param.camModel = it->second;
            yCDebug(USBCAMERA) << "cam model name : " << config.find("camModel").asString() << "  -- number : " << it->second;
        } else {
            yCError(USBCAMERA) << "Unknown camera model <" << config.find("camModel").asString() << ">";
            yCInfo(USBCAMERA) << "Supported models are: ";
            for (it = camMap.begin(); it != camMap.end(); it++) {
                yCInfo(USBCAMERA, " <%s>", it->first.c_str());
            }
            return false;
        }
    }

    // Check for addictional leopard parameter for debugging purpose
    if (param.camModel == LEOPARD_PYTHON) {
        yCDebug(USBCAMERA) << "-------------------------------\nusbCamera: Using leopard camera!!";
        bit_shift = config.check("shift", Value(bit_shift), "right shift of <n> bits").asInt32();
        bit_bayer = config.check("bit_bayer", Value(bit_bayer), "uses <n> bits bayer conversion").asInt32();
        switch (bit_bayer) {
        case 8:
            pixel_fmt_leo = V4L2_PIX_FMT_SGRBG8;
            break;

        case 10:
            pixel_fmt_leo = V4L2_PIX_FMT_SGRBG10;
            break;

        case 12:
            pixel_fmt_leo = V4L2_PIX_FMT_SGRBG12;
            break;

        default:
            yCError(USBCAMERA) << "bayer conversion with " << bit_bayer << "not supported";
            return false;
        }

        yCDebug(USBCAMERA) << "--------------------------------";
        yCDebug(USBCAMERA) << bit_shift << "bits of right shift applied to raw data";
        yCDebug(USBCAMERA) << "Bits used for de-bayer " << bit_bayer;
    }

    //crop is used to pass from 16:9 to 4:3
    if (config.check("crop")) {
        doCropping = true;
        yCInfo(USBCAMERA, "Cropping enabled.");
    } else {
        doCropping = false;
    }

    Value isDual = config.check("dual", Value(0), "Is this a dual camera? Two cameras merged into a single frame");

    if (config.find("dual").asBool()) {
        param.dual = true;
        yCInfo(USBCAMERA, "Using dual input camera.");
    } else {
        param.dual = false;
    }

    int type = 0;
    if (!config.check("pixelType")) {
        yCError(USBCAMERA) << "No 'pixelType' was specified!";
        return false;
    }
    {
        type = config.find("pixelType").asInt32();
    }

    switch (type) {
    case VOCAB_PIXEL_MONO:
        // Pixel type raw is the native one from the camera
        param.pixelType = convertV4L_to_YARP_format(param.src_fmt.fmt.pix.pixelformat);
        break;

    case VOCAB_PIXEL_RGB:
        // is variable param.pixelType really required??
        param.pixelType = V4L2_PIX_FMT_RGB24;
        break;

    default:
        yCError(USBCAMERA, "no valid pixel format found!! This should not happen!!");
        return false;
        break;
    }
    Value* retM;
    retM = Value::makeList("1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0");
    configFx = config.check("horizontalFov");
    configFy = config.check("verticalFov");
    configPPx = config.check("principalPointX");
    configPPy = config.check("principalPointY");
    configRet = config.check("rectificationMatrix");
    configDistM = config.check("distortionModel");
    Bottle bt;
    bt = config.findGroup("cameraDistortionModelGroup");
    if (!bt.isNull()) {
        if (bt.find("name").isNull() || bt.find("k1").isNull()
            || bt.find("k2").isNull() || bt.find("k3").isNull()
            || bt.find("t1").isNull() || bt.find("t2").isNull()) {
            yCError(USBCAMERA) << "group cameraDistortionModelGroup incomplete, "
                        "fields k1, k2, k3, t1, t2, name are required when using cameraDistortionModelGroup";
            configIntrins = false;
            return false;
        }
        {
            configIntrins = true;
        }
    } else {
        configIntrins = false;
    }
    param.horizontalFov = config.check("horizontalFov", Value(0.0), "desired horizontal fov of test image").asFloat64();
    param.verticalFov = config.check("verticalFov", Value(0.0), "desired vertical fov of test image").asFloat64();
    if (config.check("mirror")) {
        if (!setRgbMirroring(config.check("mirror",
                                          Value(0),
                                          "mirroring disabled by default")
                                 .asBool())) {
            yCError(USBCAMERA, "cannot set mirroring option");
            return false;
        }
    }

    param.intrinsic.put("focalLengthX", config.check("focalLengthX", Value(0.0), "Horizontal component of the focal lenght").asFloat64());
    param.intrinsic.put("focalLengthY", config.check("focalLengthY", Value(0.0), "Vertical component of the focal lenght").asFloat64());
    param.intrinsic.put("principalPointX", config.check("principalPointX", Value(0.0), "X coordinate of the principal point").asFloat64());
    param.intrinsic.put("principalPointY", config.check("principalPointY", Value(0.0), "Y coordinate of the principal point").asFloat64());
    param.intrinsic.put("rectificationMatrix", config.check("rectificationMatrix", *retM, "Matrix that describes the lens' distortion"));
    param.intrinsic.put("distortionModel", config.check("distortionModel", Value(""), "Reference to group of parameters describing the distortion model of the camera").asString());
    if (bt.isNull()) {
        param.intrinsic.put("name", "");
        param.intrinsic.put("k1", 0.0);
        param.intrinsic.put("k2", 0.0);
        param.intrinsic.put("k3", 0.0);
        param.intrinsic.put("t1", 0.0);
        param.intrinsic.put("t2", 0.0);
    } else {
        param.intrinsic.put("name", bt.check("name", Value(""), "Name of the distortion model").asString());
        param.intrinsic.put("k1", bt.check("k1", Value(0.0), "Radial distortion coefficient of the lens").asFloat64());
        param.intrinsic.put("k2", bt.check("k2", Value(0.0), "Radial distortion coefficient of the lens").asFloat64());
        param.intrinsic.put("k3", bt.check("k3", Value(0.0), "Radial distortion coefficient of the lens").asFloat64());
        param.intrinsic.put("t1", bt.check("t1", Value(0.0), "Tangential distortion of the lens").asFloat64());
        param.intrinsic.put("t2", bt.check("t2", Value(0.0), "Tangential distortion of the lens").asFloat64());
    }
    delete retM;

    yCDebug(USBCAMERA) << "using following device " << param.deviceId << "with the configuration: " << param.user_width << "x" << param.user_height << "; camModel is " << param.camModel;
    return true;
}

int V4L_camera::getfd()
{
    return param.fd;
}

bool V4L_camera::threadInit()
{
    yCTrace(USBCAMERA);

    timeStart = timeNow = timeElapsed = yarp::os::Time::now();
    frameCounter = 0;
    return true;
}

void V4L_camera::run()
{
    if (full_FrameRead()) {
        frameCounter++;
    } else {
        yCError(USBCAMERA) << "Failed acquiring new frame";
    }

    timeNow = yarp::os::Time::now();
    if ((timeElapsed = timeNow - timeStart) > 1.0f) {
        yCInfo(USBCAMERA, "frames acquired %d in %f sec", frameCounter, timeElapsed);
        frameCounter = 0;
        timeStart = timeNow;
    }
}

void V4L_camera::threadRelease()
{
    yCTrace(USBCAMERA);
}


/**
 *    initialize device
 */
bool V4L_camera::deviceInit()
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_streamparm frameint;
    configured = false;

    if (-1 == xioctl(param.fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            yCError(USBCAMERA, "%s is no V4L2 device", param.deviceId.c_str());
        }
        return false;
    }

    if (verbose) {
        list_cap_v4l2(param.fd);
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        yCError(USBCAMERA, "%s is no video capture device", param.deviceId.c_str());
        return false;
    }

    yCInfo(USBCAMERA, "%s is good V4L2_CAP_VIDEO_CAPTURE", param.deviceId.c_str());

    switch (param.io) {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            yCError(USBCAMERA, "%s does not support read i/o", param.deviceId.c_str());
            return false;
        }
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            yCError(USBCAMERA, "%s does not support streaming i/o", param.deviceId.c_str());
            return false;
        }
        break;

    default:
        yCError(USBCAMERA, "Unknown io method for device %s", param.deviceId.c_str());
        return false;
        break;
    }

    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(param.fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        /* Reset cropping to default if possible.
         * Don't care about errors
         */
        xioctl(param.fd, VIDIOC_S_CROP, &crop);
    }

    CLEAR(param.src_fmt);
    CLEAR(param.dst_fmt);

    _v4lconvert_data = v4lconvert_create(param.fd);
    if (_v4lconvert_data == nullptr) {
        yCError(USBCAMERA) << "Failed to initialize v4lconvert. Conversion to required format may not work";
    }

    /*
     * dst_fmt is the image format the user require.
     * With try_format, V4l does an handshake with the camera and the best match from
     * the available formats provided by the camera is selected.
     * src_fmt will contain the source format, i.e. the configuration to be sent to the
     * camera to optimize the conversion which will be done afterwards.
     *
     * VERY IMPORTANT NOTE:
     *
     * In case no match is found for the user input provided in dst_fmt, than dst_fmt
     * itself may be changed to provide the best conversion possible similar to user
     * input. In particular, pixel format conversion together with rescaling may not
     * be possible to achieve. In this case only pixel format conversion will be done
     * and we need to take care of the rescaling.
     */

    param.dst_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.dst_fmt.fmt.pix.width = param.user_width;
    param.dst_fmt.fmt.pix.height = param.user_height;
    param.dst_fmt.fmt.pix.field = V4L2_FIELD_NONE;
    param.dst_fmt.fmt.pix.pixelformat = param.pixelType;

    if (v4lconvert_try_format(_v4lconvert_data, &(param.dst_fmt), &(param.src_fmt)) != 0) {
        yCError(USBCAMERA, "v4lconvert_try_format -> Error is: %s", v4lconvert_get_error_message(_v4lconvert_data));
        return false;
    }

    // Check if dst_fmt has been changed by the v4lconvert_try_format
    if (param.dst_fmt.fmt.pix.width != param.user_width ||
        param.dst_fmt.fmt.pix.height != param.user_height ||
        param.dst_fmt.fmt.pix.pixelformat != param.pixelType) {
        yCWarning(USBCAMERA) << "Conversion from HW supported configuration into user requested format will require addictional step.\n"
                   << "Performance issue may arise.";

        param.addictionalResize = true;

        // Compute offsets for cropping image in case the source image and the one
        // required by the user have different form factors, i.e 16/9 vs 4/3
        double inputFF = (double)param.dst_fmt.fmt.pix.width / (double)param.dst_fmt.fmt.pix.height;
        double outputFF = (double)param.user_width / (double)param.user_height;

        if (outputFF < inputFF) {
            // Use all vertical pixels, crop lateral pixels to get the central portion of the image
            param.resizeOffset_y = 0;
            param.resizeHeight = param.dst_fmt.fmt.pix.height;

            if (!param.dual) {
                param.resizeOffset_x = (param.dst_fmt.fmt.pix.width - (param.dst_fmt.fmt.pix.height * outputFF)) / 2;
                param.resizeWidth = param.dst_fmt.fmt.pix.width - param.resizeOffset_x * 2;
            } else {
                param.resizeOffset_x = (param.dst_fmt.fmt.pix.width - (param.dst_fmt.fmt.pix.height * outputFF)) / 4; //  "/4" is  "/2"  2 times because there are 2 images
                param.resizeWidth = param.dst_fmt.fmt.pix.width / 2 - param.resizeOffset_x * 2;
            }
        } else {
            // Use all horizontal pixels, crop top/bottom pixels to get the central portion of the image
            param.resizeOffset_x = 0;

            if (!param.dual) {
                param.resizeWidth = param.dst_fmt.fmt.pix.width;
                param.resizeOffset_y = (param.dst_fmt.fmt.pix.height - (param.dst_fmt.fmt.pix.width / outputFF)) / 2;
                param.resizeHeight = param.dst_fmt.fmt.pix.height - param.resizeOffset_y * 2;
            } else {
                param.resizeWidth = param.dst_fmt.fmt.pix.width / 2;
                param.resizeOffset_y = (param.dst_fmt.fmt.pix.height - (param.dst_fmt.fmt.pix.width / outputFF)) / 2;
                param.resizeHeight = param.dst_fmt.fmt.pix.height - param.resizeOffset_y * 2;
            }
        }
    } else {
        param.addictionalResize = false;
        param.resizeOffset_x = 0;
        param.resizeWidth = param.user_width / 2;
        param.resizeOffset_y = 0;
        param.resizeHeight = param.user_height;
    }

    if (-1 == xioctl(param.fd, VIDIOC_S_FMT, &param.src_fmt)) {
        yCError(USBCAMERA) << "xioctl error VIDIOC_S_FMT" << strerror(errno);
        return false;
    }

    /* If the user has set the fps to -1, don't try to set the frame interval */
    if (param.fps != -1) {
        CLEAR(frameint);

        /* Attempt to set the frame interval. */
        frameint.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        frameint.parm.capture.timeperframe.numerator = 1;
        frameint.parm.capture.timeperframe.denominator = param.fps;
        if (-1 == xioctl(param.fd, VIDIOC_S_PARM, &frameint)) {
            yCError(USBCAMERA, "Unable to set frame interval.");
        }
    }

    param.src_image_size = param.src_fmt.fmt.pix.sizeimage;
    param.src_image = new unsigned char[param.src_image_size];

    param.dst_image_size_rgb = param.dst_fmt.fmt.pix.width * param.dst_fmt.fmt.pix.height * 3;
    param.dst_image_rgb = new unsigned char[param.dst_image_size_rgb];

    // raw image is for non-standard type only, for example leopard_python
    if (param.camModel == LEOPARD_PYTHON) {
        /* This camera sends bayer 10bit over 2bytes for each piece of information,
         * therefore the total size of the image is 2 times the number of pixels.
         */
        param.raw_image_size = param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 2;
        param.raw_image = new unsigned char[param.raw_image_size];
        param.read_image = param.raw_image; // store the image read in the raw_image buffer
    } else // This buffer should not be used for STANDARD_UVC cameras
    {
        param.read_image = param.src_image; // store the image read in the src_image buffer
        param.raw_image_size = 0;
        param.raw_image = YARP_NULLPTR;
    }

    switch (param.io) {
    case IO_METHOD_READ:
        readInit(param.src_fmt.fmt.pix.sizeimage);
        break;

    case IO_METHOD_MMAP:
        mmapInit();
        break;

    case IO_METHOD_USERPTR:
        userptrInit(param.src_fmt.fmt.pix.sizeimage);
        break;
    }

    if (verbose) {
        query_current_image_fmt_v4l2(param.fd);
    }
    configured = true;

    return true;
}

bool V4L_camera::deviceUninit()
{
    unsigned int i;
    bool ret = true;
    configured = false;

    switch (param.io) {
    case IO_METHOD_READ:
        free(param.buffers[0].start);
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < param.n_buffers; ++i) {
            if (-1 == v4l2_munmap(param.buffers[i].start, param.buffers[i].length)) {
                ret = false;
            }
        }

        CLEAR(param.req);
        //             memset(param.req, 0, sizeof(struct v4l2_requestbuffers));
        param.req.count = 0;
        param.req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        param.req.memory = V4L2_MEMORY_MMAP;
        if (xioctl(param.fd, VIDIOC_REQBUFS, &param.req) < 0) {
            yCError(USBCAMERA, "VIDIOC_REQBUFS - Failed to delete buffers: %s (errno %d)", strerror(errno), errno);
            return false;
        }

        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < param.n_buffers; ++i) {
            free(param.buffers[i].start);
        }
        break;
    }

    if (param.buffers != nullptr) {
        free(param.buffers);
    }

    if (param.raw_image != YARP_NULLPTR) {
        delete[] param.raw_image;
        param.raw_image = YARP_NULLPTR;
    }

    if (param.src_image != YARP_NULLPTR) {
        delete[] param.src_image;
        param.src_image = YARP_NULLPTR;
    }

    if (param.dst_image_rgb != YARP_NULLPTR) {
        delete[] param.dst_image_rgb;
        param.dst_image_rgb = YARP_NULLPTR;
    }

    if (_v4lconvert_data != YARP_NULLPTR) {
        v4lconvert_destroy(_v4lconvert_data);
        _v4lconvert_data = YARP_NULLPTR;
    }

    return ret;
}

/**
 *    close device
 */
bool V4L_camera::close()
{
    yCTrace(USBCAMERA);

    stop(); // stop yarp thread acquiring images

    if (param.fd != -1) {
        captureStop();
        deviceUninit();

        if (-1 == v4l2_close(param.fd)) {
            yCError(USBCAMERA) << "Error closing V4l2 device";
        }
        return false;
    }
    param.fd = -1;
    return true;
}

bool V4L_camera::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    image.resize(width(), height());

    bool res = false;
    mutex.wait();
    if (configured) {
        imagePreProcess();
        imageProcess();

        if (!param.addictionalResize) {
            memcpy(image.getRawImage(), param.dst_image_rgb, param.dst_image_size_rgb);
        } else {
            memcpy(image.getRawImage(), param.outMat.data, param.outMat.total() * 3);
        }
        mutex.post();
        res = true;
    } else {
        yCError(USBCAMERA) << "unable to get the buffer, device uninitialized";
        mutex.post();
        res = false;
    }
    return res;
}

bool V4L_camera::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    image.resize(width(), height());

    bool res = false;
    mutex.wait();
    if (configured) {
        imagePreProcess();
        memcpy(image.getRawImage(), param.src_image, param.src_image_size);
        res = true;
    } else {
        yCError(USBCAMERA) << "unable to get the buffer, device uninitialized";
        res = false;
    }
    mutex.post();
    return res;
}

/**
 * Return the height of each frame.
 * @return image height
 */
int V4L_camera::height() const
{
    /*
     * return user setting because at the end of the day, this is what
     * the image must look like
     */
    return param.user_height;
}

/**
 * Return the width of each frame.
 * @return image width
 */
int V4L_camera::width() const
{
    /*
     * return user setting because at the end of the day, this is what
     * the image must look like
     */
    return param.user_width;
}

/**
 *    Do ioctl and retry if error was EINTR ("A signal was caught during the ioctl() operation."). Parameters are the same as on ioctl.
 *
 *    \param fd file descriptor
 *    \param request request
 *    \param argp argument
 *    \returns result from ioctl
 */
int V4L_camera::xioctl(int fd, int request, void* argp)
{
    int r;

    do {
        r = v4l2_ioctl(fd, request, argp);
    } while (-1 == r && EINTR == errno);

    return r;
}


////////////////////////////////////////////////////


struct v4l2_queryctrl queryctrl;
struct v4l2_querymenu querymenu;

void V4L_camera::enumerate_menu()
{
    yCInfo(USBCAMERA, "Menu items:");

    memset(&querymenu, 0, sizeof(querymenu));
    querymenu.id = queryctrl.id;

    for (querymenu.index = (__u32)queryctrl.minimum; querymenu.index <= (__u32)queryctrl.maximum; querymenu.index++) {
        if (0 == ioctl(param.fd, VIDIOC_QUERYMENU, &querymenu)) {
            yCInfo(USBCAMERA, " %s", querymenu.name);
        } else {
            yCError(USBCAMERA, "VIDIOC_QUERYMENU: %d, %s", errno, strerror(errno));
            return;
        }
    }
}


bool V4L_camera::enumerate_controls()
{
    memset(&queryctrl, 0, sizeof(queryctrl));

    for (queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_LASTP1; queryctrl.id++) {
        if (0 == ioctl(param.fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
                continue;
            }

            yCInfo(USBCAMERA, "Control %s (id %d)", queryctrl.name, queryctrl.id);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU) {
                enumerate_menu();
            }
        } else {
            if (errno == EINVAL) {
                continue;
            }

            yCError(USBCAMERA, "VIDIOC_QUERYCTRL: %d, %s", errno, strerror(errno));
            return false;
        }
    }

    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;; queryctrl.id++) {
        if (0 == ioctl(param.fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
                continue;
            }

            yCInfo(USBCAMERA, "Control %s", queryctrl.name);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU) {
                enumerate_menu();
            }
        } else {
            if (errno == EINVAL) {
                break;
            }

            yCError(USBCAMERA, "VIDIOC_QUERYCTRL: %d, %s", errno, strerror(errno));
            return false;
        }
    }
    return true;
}

/**
 *   mainloop: read frames and process them
 */
bool V4L_camera::full_FrameRead()
{
    bool got_it = false;
    void* image_ret = nullptr;
    unsigned int count;
    unsigned int numberOfTimeouts;

    fd_set fds;
    struct timeval tv;
    int r;

    numberOfTimeouts = 0;
    count = 10; //trials


    for (unsigned int i = 0; i < count; i++) {
        FD_ZERO(&fds);
        FD_SET(param.fd, &fds);

        /* Timeout. */
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        r = select(param.fd + 1, &fds, nullptr, nullptr, &tv);

        if (r < 0) {
            if (EINTR == errno) {
                continue;
            }

            return image_ret != nullptr;
        }
        if (0 == r) {
            numberOfTimeouts++;
            {
                yCWarning(USBCAMERA, "timeout while reading image [%d/%d]", numberOfTimeouts, count);
                got_it = false;
            }
        } else if ((r > 0) && (FD_ISSET(param.fd, &fds))) {
            if (frameRead()) {
                //yCTrace(USBCAMERA, "got an image");
                got_it = true;
                break;
            }
            yCWarning(USBCAMERA, "trial %d failed", i);

        } else {
            yCWarning(USBCAMERA, "select woke up for something else");
        }

        /* EAGAIN - continue select loop. */
    }
    return got_it;
}

/**
 *    read single frame
 */
bool V4L_camera::frameRead()
{
    unsigned int i;
    struct v4l2_buffer buf;
    mutex.wait();

    switch (param.io) {
    case IO_METHOD_READ:
        if (-1 == v4l2_read(param.fd, param.buffers[0].start, param.buffers[0].length)) {
            mutex.post();
            return false;
        }

        timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec / 1000000.0);
        //             imageProcess(param.buffers[0].start);
        break;


    case IO_METHOD_MMAP:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(param.fd, VIDIOC_DQBUF, &buf)) {
            yCError(USBCAMERA, "usbCamera VIDIOC_DQBUF");
            mutex.post();
            return false;
        }

        if (!(buf.index < param.n_buffers)) {
            mutex.post();
            return false;
        }

        memcpy(param.read_image, param.buffers[buf.index].start, param.buffers[0].length);
        //            imageProcess(param.raw_image);
        timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec / 1000000.0);

        if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf)) {
            yCError(USBCAMERA, "VIDIOC_QBUF");
            mutex.post();
            return false;
        }

        break;

    case IO_METHOD_USERPTR:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(param.fd, VIDIOC_DQBUF, &buf)) {
            yCError(USBCAMERA, "VIDIOC_DQBUF");
            mutex.post();
            return false;
        }

        for (i = 0; i < param.n_buffers; ++i) {
            if (buf.m.userptr == (unsigned long)param.buffers[i].start && buf.length == param.buffers[i].length) {
                break;
            }
        }

        if (!(i < param.n_buffers)) {
            mutex.post();
            return false;
        }

        memcpy(param.read_image, param.buffers[buf.index].start, param.buffers[0].length);
        timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec / 1000000.0);


        if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf)) {
            yCError(USBCAMERA, "VIDIOC_QBUF");
        }
        break;

    default:
        yCError(USBCAMERA, "frameRead no read method configured");
    }
    mutex.post();
    return true;
}

/*
 * This function is intended to perform custom code to adapt
 * non standard pixel types to a standard one, in order to
 * use standard conversion libraries afterward.
 */
void V4L_camera::imagePreProcess()
{
    switch (param.camModel) {
    case LEOPARD_PYTHON:
    {
        // Here we are resizing the byte information from 10 to 8 bits.
        // Width and Height are not modified by this operation.
        const uint _pixelNum = param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height;

        uint16_t* raw_p = (uint16_t*)param.raw_image;
        for (uint i = 0; i < _pixelNum; i++) {
            param.src_image[i] = (unsigned char)(raw_p[i] >> bit_shift);
        }

        // Set the correct pixel type fot the v4l_convert to work on.
        param.src_fmt.fmt.pix.bytesperline = param.src_fmt.fmt.pix.width;
        param.src_fmt.fmt.pix.pixelformat = pixel_fmt_leo;
        break;
    }
    case STANDARD_UVC:
    default:
        // Nothing to do here
        break;
    }
}

/**
 *   process image read
 */
void V4L_camera::imageProcess()
{
    static bool initted = false;
    static int err = 0;

    timeStart = yarp::os::Time::now();

    // imagePreProcess() should already be called before entering here!!
    // src_fmt and dst_fmt must be alredy fixed up if needed!!

    // Convert from src type to RGB
    if (v4lconvert_convert((v4lconvert_data*)_v4lconvert_data,
                           &param.src_fmt,
                           &param.dst_fmt,
                           param.src_image,
                           param.src_image_size,
                           param.dst_image_rgb,
                           param.dst_image_size_rgb)
        < 0) {
        if ((err % 20) == 0) {
            yCError(USBCAMERA, "error converting \n\t Error message is: %s", v4lconvert_get_error_message(_v4lconvert_data));
            err = 0;
        }
        err++;
        return;
    }

    if (param.addictionalResize) {
        if (!param.dual) {
            cv::Mat img(cv::Size(param.dst_fmt.fmt.pix.width, param.dst_fmt.fmt.pix.height), CV_8UC3, param.dst_image_rgb);
            cv::Rect crop(param.resizeOffset_x, param.resizeOffset_y, param.resizeWidth, param.resizeHeight);
            cv::resize(img(crop), param.outMat, cvSize(param.user_width, param.user_height), 0, 0, cv::INTER_CUBIC);
        } else {
            // Load whole image in a cv::Mat
            cv::Mat img(cv::Size(param.dst_fmt.fmt.pix.width, param.dst_fmt.fmt.pix.height), CV_8UC3, param.dst_image_rgb);
            cv::Mat img_right;
            cv::Rect crop(param.resizeOffset_x, param.resizeOffset_y, param.resizeWidth, param.resizeHeight);

            cv::resize(img(crop), param.outMat, cvSize(param.user_width / 2, param.user_height), 0, 0, cv::INTER_CUBIC);
            cv::Rect crop2(param.resizeWidth + param.resizeOffset_x * 2, param.resizeOffset_y, param.resizeWidth, param.resizeHeight);
            cv::resize(img(crop2), img_right, cvSize(param.user_width / 2, param.user_height), 0, 0, cv::INTER_CUBIC);
            cv::hconcat(param.outMat, img_right, param.outMat);
        }
        if (param.flip) {
            cv::flip(param.outMat, param.outMat, 1);
        }
    } else {
        if (param.flip) {
            cv::Mat img(cv::Size(param.dst_fmt.fmt.pix.width, param.dst_fmt.fmt.pix.height), CV_8UC3, param.dst_image_rgb);
            param.outMat = img;
            cv::flip(param.outMat, param.outMat, 1);
        }
    }

    timeElapsed = yarp::os::Time::now() - timeStart;
    myCounter++;
    timeTot += timeElapsed;

    if ((myCounter % 60) == 0) {
        if (!initted) {
            timeTot = 0;
            myCounter = 0;
            initted = true;
        }
    }
}

/**
 *    stop capturing
 */
void V4L_camera::captureStop()
{
    int ret = 0;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    switch (param.io) {
    case IO_METHOD_READ:
        //do nothing
        break;

    case IO_METHOD_MMAP:
    default:
        ret = xioctl(param.fd, VIDIOC_STREAMOFF, &type);
        if (ret < 0) {
            if (errno != 9) { /* errno = 9 means the capture was allready stoped*/
                yCError(USBCAMERA, "VIDIOC_STREAMOFF - Unable to stop capture: %d, %s", errno, strerror(errno));
            }
        }
        break;
    }
}

/**
 *  start capturing
 */
void V4L_camera::captureStart()
{
    unsigned int i;
    enum v4l2_buf_type type;

    switch (param.io) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < param.n_buffers; ++i) {
            struct v4l2_buffer buf;
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf)) {
                yCError(USBCAMERA, "VIDIOC_QBUF");
            }
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl(param.fd, VIDIOC_STREAMON, &type)) {
            yCError(USBCAMERA, "VIDIOC_STREAMON");
        }

        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < param.n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long)param.buffers[i].start;
            buf.length = param.buffers[i].length;

            if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf)) {
                yCError(USBCAMERA, "VIDIOC_QBUF");
            }
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl(param.fd, VIDIOC_STREAMON, &type)) {
            yCError(USBCAMERA, "VIDIOC_STREAMON");
        }

        break;
    }
}


bool V4L_camera::readInit(unsigned int buffer_size)
{
    param.buffers = (struct buffer*)calloc(1, sizeof(*(param.buffers)));

    if (param.buffers == nullptr) {
        yCError(USBCAMERA, "cannot allocate buffer, out of memory");
        return false;
    }

    param.buffers[0].length = buffer_size;
    param.buffers[0].start = malloc(buffer_size);

    if (param.buffers[0].start == nullptr) {
        yCError(USBCAMERA, "cannot allocate buffer, out of memory");
        return false;
    }
    return true;
}

bool V4L_camera::mmapInit()
{
    CLEAR(param.req);

    param.n_buffers = VIDIOC_REQBUFS_COUNT;
    param.req.count = param.n_buffers;
    param.req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(param.fd, VIDIOC_REQBUFS, &param.req)) {
        if (EINVAL == errno) {
            yCError(USBCAMERA, "%s does not support memory mapping", param.deviceId.c_str());
            return false;
        }
        yCError(USBCAMERA, "Error on device %s requesting memory mapping (VIDIOC_REQBUFS)", param.deviceId.c_str());
        return false;
    }

    if (param.req.count < 1) {
        yCError(USBCAMERA, "Insufficient buffer memory on %s", param.deviceId.c_str());
        return false;
    }

    if (param.req.count == 1) {
        yCError(USBCAMERA, "Only 1 buffer was available, you may encounter performance issue acquiring images from device %s", param.deviceId.c_str());
    }

    param.buffers = (struct buffer*)calloc(param.req.count, sizeof(*(param.buffers)));

    if (param.buffers == nullptr) {
        yCError(USBCAMERA, "Out of memory");
        return false;
    }

    struct v4l2_buffer buf;

    for (param.n_buffers = 0; param.n_buffers < param.req.count; param.n_buffers++) {
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = param.n_buffers;

        if (-1 == xioctl(param.fd, VIDIOC_QUERYBUF, &buf)) {
            yCError(USBCAMERA, "VIDIOC_QUERYBUF");
        }

        param.buffers[param.n_buffers].length = buf.length;
        param.buffers[param.n_buffers].start = v4l2_mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, param.fd, buf.m.offset);

        if (MAP_FAILED == param.buffers[param.n_buffers].start) {
            yCError(USBCAMERA, "mmap");
        }
    }
    return true;
}

bool V4L_camera::userptrInit(unsigned int buffer_size)
{
    unsigned int page_size;

    page_size = getpagesize();
    buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

    CLEAR(param.req);

    param.req.count = VIDIOC_REQBUFS_COUNT;
    param.req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(param.fd, VIDIOC_REQBUFS, &param.req)) {
        if (EINVAL == errno) {
            yCError(USBCAMERA, "%s does not support user pointer i/o", param.deviceId.c_str());
            return false;
        }
        yCError(USBCAMERA, "Error requesting VIDIOC_REQBUFS for device %s", param.deviceId.c_str());
        return false;
    }

    param.buffers = (struct buffer*)calloc(4, sizeof(*(param.buffers)));

    if (param.buffers == nullptr) {
        yCError(USBCAMERA, "cannot allocate buffer, out of memory");
        return false;
    }

    for (param.n_buffers = 0; param.n_buffers < 4; ++param.n_buffers) {
        param.buffers[param.n_buffers].length = buffer_size;
        param.buffers[param.n_buffers].start = memalign(/* boundary */ page_size, buffer_size);

        if (param.buffers[param.n_buffers].start == nullptr) {
            yCError(USBCAMERA, "cannot allocate buffer, out of memory");
            return false;
        }
    }
    return true;
}

bool V4L_camera::set_V4L2_control(uint32_t id, double value, bool verbatim)
{
    if (value < 0) {
        return false;
    }

    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = id;

    if (-1 == ioctl(param.fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            yCError(USBCAMERA, "VIDIOC_QUERYCTRL: %d, %s", errno, strerror(errno));
        } else {
            yCError(USBCAMERA, "Cannot set control <%s> (id 0x%0X) is not supported", queryctrl.name, queryctrl.id);
        }
        return false;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        yCError(USBCAMERA, "Control %s is disabled", queryctrl.name);
        return false;
    }
    memset(&control, 0, sizeof(control));
    control.id = id;
    if (verbatim) {
        control.value = value;
    } else {
        if (param.camModel == LEOPARD_PYTHON) {
            if ((V4L2_CID_EXPOSURE == id) || (V4L2_CID_EXPOSURE_ABSOLUTE == id) || (V4L2_CID_EXPOSURE_AUTO == id)) {
                queryctrl.maximum = 8000;
                queryctrl.minimum = 0;
            }
        }
        control.value = (int32_t)(value * (queryctrl.maximum - queryctrl.minimum) + queryctrl.minimum);
    }
    if (-1 == ioctl(param.fd, VIDIOC_S_CTRL, &control)) {
        yCError(USBCAMERA, "VIDIOC_S_CTRL: %d, %s", errno, strerror(errno));
        if (errno == ERANGE) {
            yCError(USBCAMERA, "Normalized input value %f ( equivalent to raw value of %d) was out of range for control %s: Min and Max are: %d - %d", value, control.value, queryctrl.name, queryctrl.minimum, queryctrl.maximum);
        }
        return false;
    }
    if (verbose) {
        yCInfo(USBCAMERA, "set control %s to %d done!", queryctrl.name, control.value);
    }

    return true;
}

bool V4L_camera::check_V4L2_control(uint32_t id)
{
    //     yCTrace(USBCAMERA);
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&control, 0, sizeof(control));
    memset(&queryctrl, 0, sizeof(queryctrl));

    control.id = id;
    queryctrl.id = id;

    if (-1 == ioctl(param.fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            yCError(USBCAMERA, "VIDIOC_QUERYCTRL: %d, %s", errno, strerror(errno));
        }
        return false;
    }
    return true;
}

double V4L_camera::get_V4L2_control(uint32_t id, bool verbatim)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&control, 0, sizeof(control));
    memset(&queryctrl, 0, sizeof(queryctrl));

    control.id = id;
    queryctrl.id = id;

    if (-1 == ioctl(param.fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            yCError(USBCAMERA, "VIDIOC_QUERYCTRL: %d, %s", errno, strerror(errno));
        }

        return -1.0;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        yCError(USBCAMERA, "Control %s is disabled", queryctrl.name);
    } else {
        if (-1 == ioctl(param.fd, VIDIOC_G_CTRL, &control)) {
            yCError(USBCAMERA, "VIDIOC_G_CTRL: %d, %s", errno, strerror(errno));
            return -1.0;
        }
    }
    if (verbatim) {
        return control.value;
    }

    if (param.camModel == LEOPARD_PYTHON) {
        if ((V4L2_CID_EXPOSURE == id) || (V4L2_CID_EXPOSURE_ABSOLUTE == id) || (V4L2_CID_EXPOSURE_AUTO == id)) {
            queryctrl.maximum = 8000;
            queryctrl.minimum = 0;
        }
    }
    return (double)(control.value - queryctrl.minimum) / (queryctrl.maximum - queryctrl.minimum);
}

bool V4L_camera::getCameraDescription(CameraDescriptor* camera)
{
    camera->busType = BUS_USB;
    camera->deviceDescription = "USB3 camera";
    return true;
}

bool V4L_camera::hasFeature(int feature, bool* _hasFeature)
{
    bool tmpMan(false);
    bool tmpAuto(false);
    bool tmpOnce(false);

    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
        tmpMan = check_V4L2_control(V4L2_CID_RED_BALANCE) && check_V4L2_control(V4L2_CID_BLUE_BALANCE);
        tmpOnce = check_V4L2_control(V4L2_CID_DO_WHITE_BALANCE);
        tmpAuto = check_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        break;

    case YARP_FEATURE_EXPOSURE:
        tmpMan = check_V4L2_control(V4L2_CID_EXPOSURE) || check_V4L2_control(V4L2_CID_EXPOSURE_ABSOLUTE);
        tmpAuto = check_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
        break;

    default:
        tmpMan = check_V4L2_control(convertYARP_to_V4L(feature));
        break;
    }

    *_hasFeature = tmpMan || tmpOnce || tmpAuto;
    return true;
}

bool V4L_camera::setFeature(int feature, double value)
{
    bool ret = false;
    switch (feature) {
    case YARP_FEATURE_EXPOSURE:
        if (use_exposure_absolute) {
            ret = set_V4L2_control(V4L2_CID_EXPOSURE_ABSOLUTE, value);
        } else {
            ret = set_V4L2_control(V4L2_CID_EXPOSURE, value);
        }
        break;

    default:
        ret = set_V4L2_control(convertYARP_to_V4L(feature), value);
        break;
    }
    return ret;
}

bool V4L_camera::getFeature(int feature, double* value)
{
    double tmp = 0.0;
    switch (feature) {
    case YARP_FEATURE_EXPOSURE:
        if (use_exposure_absolute) {
            tmp = get_V4L2_control(V4L2_CID_EXPOSURE_ABSOLUTE);
        } else {
            tmp = get_V4L2_control(V4L2_CID_EXPOSURE);
        }
        break;

    default:
        tmp = get_V4L2_control(convertYARP_to_V4L(feature));
        break;
    }

    if (tmp == -1) {
        return false;
    }

    *value = tmp;
    return true;
}

bool V4L_camera::setFeature(int feature, double value1, double value2)
{
    if (feature == YARP_FEATURE_WHITE_BALANCE) {
        bool ret = true;
        ret &= set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, false);
        ret &= set_V4L2_control(V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, V4L2_WHITE_BALANCE_MANUAL);
        ret &= set_V4L2_control(V4L2_CID_RED_BALANCE, value1);
        ret &= set_V4L2_control(V4L2_CID_BLUE_BALANCE, value2);
        return ret;
    }
    return false;
}

bool V4L_camera::getFeature(int feature, double* value1, double* value2)
{
    if (feature == YARP_FEATURE_WHITE_BALANCE) {
        *value1 = get_V4L2_control(V4L2_CID_RED_BALANCE);
        *value2 = get_V4L2_control(V4L2_CID_BLUE_BALANCE);
        return !((*value1 == -1) || (*value2 == -1));
    }
    return false;
}

bool V4L_camera::hasOnOff(int feature, bool* _hasOnOff)
{
    bool _hasAuto;
    // I can't find any meaning of setting a feature to off on V4l ... what it is supposed to do????
    switch (feature) {
    // The following do have a way to set them auto/manual
    case YARP_FEATURE_WHITE_BALANCE:
    case YARP_FEATURE_EXPOSURE:
        if (hasAuto(feature, &_hasAuto)) {
            *_hasOnOff = true;
        } else {
            *_hasOnOff = false;
        }
        break;

    // try it out
    default:
        hasAuto(feature, &_hasAuto);
        if (_hasAuto) {
            *_hasOnOff = true;
        } else {
            *_hasOnOff = false;
        }
        break;
    }
    return true;
}

bool V4L_camera::setActive(int feature, bool onoff)
{
    // I can't find any meaning of setting a feature to off on V4l ... what it is supposed to do????
    bool tmp;
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
        tmp = set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, onoff);
        if (tmp) {
            isActive_vector[feature] = onoff;
        }
        break;

    case YARP_FEATURE_EXPOSURE:
        if (onoff) {
            set_V4L2_control(V4L2_LOCK_EXPOSURE, false);

            hasAuto(feature, &tmp);
            if (tmp) {
                tmp = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO);
            } else {
                tmp = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
            }

            if (tmp) {
                isActive_vector[feature] = onoff;
            }
        } else {
            bool man = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
            if (!man) {
                man = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_SHUTTER_PRIORITY, true);
                if (!man) {
                    yCError(USBCAMERA) << "Cannot set manual exposure";
                }
            }
            set_V4L2_control(V4L2_LOCK_EXPOSURE, true);
            isActive_vector[feature] = onoff;
        }
        break;

    default: // what to do in each case?
        if (onoff) {
            isActive_vector[feature] = true;
            return true;
        }
        isActive_vector[feature] = false;
        return false;
    }
    return true;
}

bool V4L_camera::getActive(int feature, bool* _isActive)
{
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
    {
        double tmp = get_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        if (tmp == 1) {
            *_isActive = true;
        } else {
            *_isActive = false;
        }
        break;
    }

    case YARP_FEATURE_EXPOSURE:
    {
        bool _hasMan(false);
        bool _hasMan2(false);
        hasFeature(V4L2_CID_EXPOSURE, &_hasMan) || hasFeature(V4L2_CID_EXPOSURE_ABSOLUTE, &_hasMan2); // check manual version (normal and asbolute)
        double _hasAuto = get_V4L2_control(V4L2_CID_EXPOSURE_AUTO, true); // check auto version

        *_isActive = (_hasAuto == V4L2_EXPOSURE_AUTO) || _hasMan || _hasMan2;
        break;
    }

    default:
        *_isActive = true;
        break;
    }

    return true;
}

bool V4L_camera::hasAuto(int feature, bool* _hasAuto)
{
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
        *_hasAuto = check_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        break;

    case YARP_FEATURE_BRIGHTNESS:
        *_hasAuto = check_V4L2_control(V4L2_CID_AUTOBRIGHTNESS);
        break;

    case YARP_FEATURE_GAIN:
        *_hasAuto = check_V4L2_control(V4L2_CID_AUTOGAIN);
        break;

    case YARP_FEATURE_EXPOSURE:
        *_hasAuto = check_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
        break;

    case YARP_FEATURE_HUE:
        *_hasAuto = check_V4L2_control(V4L2_CID_HUE_AUTO);
        break;

    default:
        *_hasAuto = false;
        break;
    }
    return true;
}

bool V4L_camera::hasManual(int feature, bool* _hasManual)
{
    if (feature == YARP_FEATURE_WHITE_BALANCE) {
        *_hasManual = check_V4L2_control(V4L2_CID_RED_BALANCE) && check_V4L2_control(V4L2_CID_BLUE_BALANCE);
        return true;
    }

    if (feature == YARP_FEATURE_EXPOSURE) {
        *_hasManual = check_V4L2_control(V4L2_CID_EXPOSURE) || check_V4L2_control(V4L2_CID_EXPOSURE_ABSOLUTE);
        return true;
    }
    return hasFeature(feature, _hasManual);
}

bool V4L_camera::hasOnePush(int feature, bool* _hasOnePush)
{
    // I'm not able to map a 'onePush' request on V4L api
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
        *_hasOnePush = check_V4L2_control(V4L2_CID_DO_WHITE_BALANCE);
        return true;

    default:
        *_hasOnePush = false;
        break;
    }
    return true;
}

bool V4L_camera::setMode(int feature, FeatureMode mode)
{
    bool ret = false;
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
        if (mode == MODE_AUTO) {
            ret = set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, true);
        } else {
            ret = set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, false);
        }
        break;

    case YARP_FEATURE_EXPOSURE:
        bool _tmpAuto;
        hasAuto(V4L2_CID_EXPOSURE_AUTO, &_tmpAuto);

        if (_tmpAuto) {
            if (mode == MODE_AUTO) {
                ret = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, true);
            } else {
                ret = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, false);
            }
        } else {
            ret = mode != MODE_AUTO;
        }
        break;

    case YARP_FEATURE_GAIN:
        if (mode == MODE_AUTO) {
            yCInfo(USBCAMERA) << "GAIN: set mode auto";
            ret = set_V4L2_control(V4L2_CID_AUTOGAIN, true);
        } else {
            yCInfo(USBCAMERA) << "GAIN: set mode manual";
            ret = set_V4L2_control(V4L2_CID_AUTOGAIN, false);
        }
        break;

    case YARP_FEATURE_BRIGHTNESS:
    {
        bool _tmpAuto;
        hasAuto(YARP_FEATURE_BRIGHTNESS, &_tmpAuto);

        if (_tmpAuto) {
            if (mode == MODE_AUTO) {
                ret = set_V4L2_control(V4L2_CID_AUTOBRIGHTNESS, true);
            } else {
                ret = set_V4L2_control(V4L2_CID_AUTOBRIGHTNESS, false);
            }
        } else {
            ret = mode != MODE_AUTO;
        }
        break;
    }

    case YARP_FEATURE_HUE:
        if (mode == MODE_AUTO) {
            ret = set_V4L2_control(V4L2_CID_HUE_AUTO, true);
        } else {
            ret = set_V4L2_control(V4L2_CID_HUE_AUTO, false);
        }
        break;

    default:
        yCError(USBCAMERA) << "Feature " << feature << " does not support auto mode";
        break;
    }
    return ret;
}

bool V4L_camera::getMode(int feature, FeatureMode* mode)
{
    bool _tmpAuto;
    switch (feature) {
    case YARP_FEATURE_WHITE_BALANCE:
    {
        double ret = get_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        *mode = toFeatureMode(ret != 0.0);
        break;
    }

    case YARP_FEATURE_EXPOSURE:
    {
        double ret = get_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
        if (ret == -1.0) {
            *mode = MODE_MANUAL;
            break;
        }

        if (ret == V4L2_EXPOSURE_MANUAL) {
            *mode = MODE_MANUAL;
        } else {
            *mode = MODE_AUTO;
        }
        break;
    }

    case YARP_FEATURE_BRIGHTNESS:
        hasAuto(YARP_FEATURE_BRIGHTNESS, &_tmpAuto);
        *mode = toFeatureMode(_tmpAuto);
        if (!_tmpAuto) {
            *mode = MODE_MANUAL;
        } else {
            double ret = get_V4L2_control(V4L2_CID_AUTOBRIGHTNESS);
            *mode = toFeatureMode(ret != 0.0);
        }
        break;

    case YARP_FEATURE_GAIN:
        hasAuto(YARP_FEATURE_GAIN, &_tmpAuto);
        *mode = toFeatureMode(_tmpAuto);
        if (!_tmpAuto) {
            *mode = MODE_MANUAL;
        } else {
            double ret = get_V4L2_control(V4L2_CID_AUTOGAIN);
            *mode = toFeatureMode(ret != 0.0);
        }
        break;

    case YARP_FEATURE_HUE:
        hasAuto(YARP_FEATURE_HUE, &_tmpAuto);
        *mode = toFeatureMode(_tmpAuto);
        if (!_tmpAuto) {
            *mode = MODE_MANUAL;
        } else {
            double ret = get_V4L2_control(V4L2_CID_HUE_AUTO);
            *mode = toFeatureMode(ret != 0.0);
        }
        break;

    default:
        *mode = MODE_MANUAL;
        break;
    }
    return true;
}

bool V4L_camera::setOnePush(int feature)
{
    // I'm not able to map a 'onePush' request on each V4L api
    if (feature == YARP_FEATURE_WHITE_BALANCE) {
        return set_V4L2_control(V4L2_CID_DO_WHITE_BALANCE, true);
    }
    return false;
}
