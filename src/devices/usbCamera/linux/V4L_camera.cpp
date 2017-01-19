/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <linux/V4L_camera.hpp>
#include <yarp/os/LogStream.h>
#include <list.hpp>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>
#include "libv4lconvert.h"


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <time.h>

#define errno_exit printf

//#include <Leopard_MT9M021C.h>

struct v4lconvert_data *_v4lconvert_data;

using namespace yarp::os;
using namespace yarp::dev;


static double getEpochTimeShift()
{
    struct timeval epochtime;
    struct timespec  vsTime;

    gettimeofday(&epochtime, NULL);
    clock_gettime(CLOCK_MONOTONIC, &vsTime);

    double uptime = vsTime.tv_sec + vsTime.tv_nsec/1000000000.0;
    double epoch =  epochtime.tv_sec + epochtime.tv_usec/1000000.0;
    return epoch - uptime;
}


#define NOT_PRESENT -1
int V4L_camera::convertYARP_to_V4L(int feature)
{
    switch (feature)
    {
        case YARP_FEATURE_BRIGHTNESS:     return V4L2_CID_BRIGHTNESS;
        case YARP_FEATURE_SHUTTER:        // this maps also on exposure
        case YARP_FEATURE_EXPOSURE:       return V4L2_CID_EXPOSURE;
        case YARP_FEATURE_SHARPNESS:      return V4L2_CID_SHARPNESS;
        case YARP_FEATURE_HUE:            return V4L2_CID_HUE;
        case YARP_FEATURE_SATURATION:     return V4L2_CID_SATURATION;
        case YARP_FEATURE_GAMMA:          return V4L2_CID_GAMMA;
        case YARP_FEATURE_GAIN:           return V4L2_CID_GAIN;
        case YARP_FEATURE_IRIS:           return V4L2_CID_IRIS_ABSOLUTE;

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

V4L_camera::V4L_camera() : RateThread(1000/DEFAULT_FRAMERATE), doCropping(false), dual(false), toEpochOffset(getEpochTimeShift())
{
    param.width  = DEFAULT_WIDTH;
    param.height = DEFAULT_HEIGHT;
    param.fps = DEFAULT_FRAMERATE;
    param.io = IO_METHOD_MMAP;
    param.deviceId = "/dev/video0";
    param.fd  = -1;
    param.image_size = 0;
    param.dst_image = NULL;
    param.n_buffers = 0;
    param.buffers = NULL;
    param.camModel = SEE3CAMCU50;
    myCounter = 0;
    timeTot = 0;
    configFx = false;
    configFy = false;
    configPPx = false;
    configPPy =false;
    configRet = false;
    configDistM = false;
    configIntrins = false;
}

yarp::os::Stamp V4L_camera::getLastInputStamp()
{
    return timeStamp;
}

/**
 *    open device
 */
bool V4L_camera::open(yarp::os::Searchable& config)
{
    struct stat st;
    yTrace() << "input params are " << config.toString();

    if(!fromConfig(config))
        return false;

    // stat file
    if (-1 == stat(param.deviceId.c_str(), &st))
    {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }

    // check if it is a device
    if (!S_ISCHR(st.st_mode))
    {
        fprintf(stderr, "%s is no device\n", param.deviceId.c_str());
        return false;
    }

    // open device
    param.fd = v4l2_open(param.deviceId.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    // check if opening was successfull
    if (-1 == param.fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }


    // if previous instance crashed, maybe will help (?)
    captureStop();
    deviceUninit();
    v4l2_close(param.fd);

    yarp::os::Time::delay(0.5);
    // re-open device
    param.fd = v4l2_open(param.deviceId.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    // check if opening was successfull
    if (-1 == param.fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", param.deviceId.c_str(), errno, strerror(errno));
        return false;
    }


    // Initting video device
    deviceInit();
    yInfo() << "START enumerating controls";
    enumerate_controls();
    yInfo() << "DONE enumerating controls\n\n";
    captureStart();
    yarp::os::Time::delay(0.5);
    start();

    return true;
}


int V4L_camera::getRgbHeight(){
    return height();
}

int V4L_camera::getRgbWidth(){
    return width();
}

bool V4L_camera::setRgbResolution(int width, int height){
    mutex.wait();
    captureStop();
    deviceUninit();
    param.width=width;
    param.height=height;
    bool res=deviceInit();
    captureStart();
    mutex.post();
    return res;
//    yWarning("usbCamera: setRgbResolution not implemented yet");
//    return false;
}

bool V4L_camera::getRgbFOV(double &horizontalFov, double &verticalFov){
    horizontalFov=param.horizontalFov;
    verticalFov=param.verticalFov;
    return configFx && configFy;
}

bool V4L_camera::setRgbFOV(double horizontalFov, double verticalFov){
    param.horizontalFov=horizontalFov;
    param.verticalFov=verticalFov;
    return true;
}

bool V4L_camera::getRgbIntrinsicParam(yarp::os::Property &intrinsic){
    intrinsic=param.intrinsic;
    return configIntrins;
}

bool V4L_camera::getRgbMirroring(bool &mirror){

    mirror=ioctl(param.fd,V4L2_CID_HFLIP);
    return true;
}

bool V4L_camera::setRgbMirroring(bool mirror){
    int ret=ioctl(param.fd,V4L2_CID_HFLIP,&mirror);
    if (ret < 0)
    {
        yError()<<"V4L2_CID_HFLIP - Unable to mirror image-"<<strerror(errno);
        return false;
    }
    return true;
}

bool V4L_camera::fromConfig(yarp::os::Searchable& config)
{
    if(!config.check("width") )
    {
        yDebug() << "width parameter not found, using default value of " << DEFAULT_WIDTH;
        param.width = DEFAULT_WIDTH;
    }
    else
        param.width = config.find("width").asInt();

    if(!config.check("height") )
    {
        yDebug() << "height parameter not found, using default value of " << DEFAULT_HEIGHT;
        param.height = DEFAULT_HEIGHT;
    }
    else
        param.height = config.find("height").asInt();

    if(!config.check("framerate") )
    {
        yDebug() << "framerate parameter not found, using default value of " << DEFAULT_FRAMERATE;
        param.fps = DEFAULT_FRAMERATE;
    }
    else
        param.fps = config.find("framerate").asInt();

    if(!config.check("d") )
    {
        yError() << "No camera identifier was specified! (e.g. '--d /dev/video0' on Linux OS)";
        return false;
    }
    else
        param.deviceId = config.find("d").asString();


    if(!config.check("camModel") )
    {
        yWarning() << "No 'camModel' was specified, working with default SEE3CAMCU50 (camModel 1)";
    }
    else
        param.camModel = (supported_cams) config.find("camModel").asInt();

    if(config.check("crop") )
    {
        doCropping = true;
        yInfo("Cropping enabled.");
    }
    else
        doCropping = false;

    if(config.check("dual") )
    {
        dual = true;
        yInfo("Using dual input camera.");
    }
    else
        dual = false;

    int type = 0;
    if(!config.check("pixelType") )
    {
        yError() << "No 'pixelType' was specified!";
        return false;
    }
    else
        type = config.find("pixelType").asInt();

    switch(type)
    {
        case VOCAB_PIXEL_MONO:
            param.pixelType = V4L2_PIX_FMT_GREY;
            param.dst_image_size = param.width * param.height;
            break;

        case VOCAB_PIXEL_RGB:
            param.pixelType = V4L2_PIX_FMT_RGB24;
            param.dst_image_size = param.width * param.height * 3;
            break;

        default:
            printf("Error, no valid pixel format found!! This should not happen!!\n");
            return false;
            break;
    }
    yarp::os::Value *val;
    Value* retM;
    retM=Value::makeList("1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0");
    configFx=config.check("horizontalFov");
    configFy=config.check("verticalFov");
    configPPx=config.check("principalPointX");
    configPPy=config.check("principalPointY");
    configRet=config.check("retificationMatrix");
    configDistM=config.check("distortionModel");
    Bottle bt;
    bt=config.findGroup("cameraDistortionModelGroup");
    if(!bt.isNull())
    {
        if(bt.find("name").isNull() || bt.find("k1").isNull()
                    || bt.find("k2").isNull() || bt.find("k3").isNull()
                    || bt.find("t1").isNull() || bt.find("t2").isNull())
        {
            yError()<<"usbCamera: group cameraDistortionModelGroup incomplete, "
                      "fields k1, k2, k3, t1, t2, name are required when using cameraDistortionModelGroup";
            configIntrins=false;
            return false;
        }
        else
            configIntrins=true;
    }
    else
        configIntrins=false;
    param.horizontalFov=config.check("horizontalFov",Value(0.0),
                               "desired horizontal fov of test image").asDouble();
    param.verticalFov=config.check("verticalFov",Value(0.0),
                               "desired vertical fov of test image").asDouble();
    if(config.check("mirror"))
    {
        if(!setRgbMirroring(config.check("mirror",
                                        Value(false),
                                        "mirroring disabled by default").asBool())){
            yError("usbCamera: cannot set mirroring option");
            return false;
        }
    }

    param.intrinsic.put("focalLengthX",config.check("focalLengthX",Value(0.0),"").asDouble());
    param.intrinsic.put("focalLengthY",config.check("focalLengthY",Value(0.0),"").asDouble());
    param.intrinsic.put("principalPointX",config.check("principalPointX",Value(0.0),"").asDouble());
    param.intrinsic.put("principalPointY",config.check("principalPointY",Value(0.0),"").asDouble());
    param.intrinsic.put("retificationMatrix",config.check("retificationMatrix",*retM,""));
    param.intrinsic.put("distortionModel",config.check("distortionModel",Value(""),"").asString());
    if(bt.isNull())
    {
        param.intrinsic.put("name","");
        param.intrinsic.put("k1",0.0);
        param.intrinsic.put("k2",0.0);
        param.intrinsic.put("k3",0.0);
        param.intrinsic.put("t1",0.0);
        param.intrinsic.put("t2",0.0);
    }
    else{
        param.intrinsic.put("name",bt.check("name",Value(""),"").asString());
        param.intrinsic.put("k1",bt.check("k1",Value(0.0),"").asDouble());
        param.intrinsic.put("k2",bt.check("k2",Value(0.0),"").asDouble());
        param.intrinsic.put("k3",bt.check("k3",Value(0.0),"").asDouble());
        param.intrinsic.put("t1",bt.check("t1",Value(0.0),"").asDouble());
        param.intrinsic.put("t2",bt.check("t2",Value(0.0),"").asDouble());
    }
    delete retM;

    yDebug() << "using following device " << param.deviceId << "with the configuration: " << param.width << "x" << param.height << "; camModel is " << param.camModel;
    return true;
}

int V4L_camera::getfd()
{
    return param.fd;
}

bool V4L_camera::threadInit()
{
    yTrace();

    timeStart = timeNow = timeElapsed = yarp::os::Time::now();

    frameCounter = 0;
    return true;
}

void V4L_camera::run()
{
    if(full_FrameRead())
        frameCounter++;
    else
        yError() << "Failed acquiring new frame";

    timeNow = yarp::os::Time::now();
    if( (timeElapsed = timeNow - timeStart) > 1.0f)
    {
        printf("frames acquired %d in %f sec\n", frameCounter, timeElapsed);
        frameCounter = 0;
        timeStart = timeNow;
    }
}

void V4L_camera::threadRelease()
{
    yTrace();
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
    unsigned int min;

    if (-1 == xioctl(param.fd, VIDIOC_QUERYCAP, &cap))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s is no V4L2 device\n", param.deviceId.c_str());
        }
        return false;
    }

    list_cap_v4l2(param.fd);

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "%s is no video capture device\n", param.deviceId.c_str());
        return false;
    }
    else
        fprintf(stderr, "%s is good V4L2_CAP_VIDEO_CAPTURE\n", param.deviceId.c_str());

    switch (param.io)
    {

        case IO_METHOD_READ:
        {
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                fprintf(stderr, "%s does not support read i/o\n", param.deviceId.c_str());
                return false;
            }
        } break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
        {
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n", param.deviceId.c_str());
                return false;
            }
        } break;

        default:
            fprintf(stderr, "Unknown io method for device %s\n", param.deviceId.c_str());
            return false;
            break;
    }

    /* Select video input, video standard and tune here. */
    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(param.fd, VIDIOC_CROPCAP, &cropcap))
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(param.fd, VIDIOC_S_CROP, &crop))
        {
            switch (errno)
            {
                case EINVAL:
                    /* Cropping not supported. */
                    break;
                default:
                    /* Errors ignored. */
                    break;
            }
        }
    }
    else
    {
        /* Errors ignored. */
    }

    CLEAR(param.src_fmt);
    CLEAR(param.dst_fmt);

    _v4lconvert_data = v4lconvert_create(param.fd);
    if (_v4lconvert_data == NULL)
        printf("\nERROR: v4lconvert_create\n");
    else
        printf("\nDONE: v4lconvert_create\n");

    /* Here we set the pixel format we want to have to display, for getImage
    // set the desired format after conversion and ask v4l which input format I should
    // ask to the camera
    */

    param.dst_fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.dst_fmt.fmt.pix.width       = param.width;
    param.dst_fmt.fmt.pix.height      = param.height;
    param.dst_fmt.fmt.pix.field       = V4L2_FIELD_NONE;
    param.dst_fmt.fmt.pix.pixelformat = param.pixelType;

    if (v4lconvert_try_format(_v4lconvert_data, &(param.dst_fmt), &(param.src_fmt)) != 0)
    {
        printf("ERROR: v4lconvert_try_format\n\nError is: %s", v4lconvert_get_error_message(_v4lconvert_data));
        return false;
    }
    else
    {
        printf("DONE: v4lconvert_try_format\n\t");
        printf("Message is: %s", v4lconvert_get_error_message(_v4lconvert_data));
    }

    printf("param.width = %d; src.width = %d\n", param.width, param.src_fmt.fmt.pix.width);

    // dst is tmp, just to convert camera pixel type (YUYV) into user pixel type (RGB)
    param.dst_fmt = param.src_fmt;
    param.dst_fmt.fmt.pix.pixelformat = param.pixelType;

    if (-1 == xioctl(param.fd, VIDIOC_S_FMT, &param.src_fmt))
        std::cout << "xioctl error VIDIOC_S_FMT" << std::endl;


    /* Note VIDIOC_S_FMT may change width and height. */
//     if (param.width != param.src_fmt.fmt.pix.width)
//     {
//         param.width = param.src_fmt.fmt.pix.width;
//         std::cout << "Image width set to " << param.width << " by device " << param.deviceId << std::endl;
//     }
//
//     if (param.height != param.src_fmt.fmt.pix.height)
//     {
//         param.height = param.src_fmt.fmt.pix.height;
//         std::cout << "Image height set to " << param.height << " by device " << param.deviceId << std::endl;
//     }

//     if(param.width/param.height  != param.src_fmt.fmt.pix.width/param.src_fmt.fmt.pix.height)
//         doCropping == true;

    /* If the user has set the fps to -1, don't try to set the frame interval */
    if (param.fps != -1)
    {
        CLEAR(frameint);

        /* Attempt to set the frame interval. */
        frameint.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        frameint.parm.capture.timeperframe.numerator = 1;
        frameint.parm.capture.timeperframe.denominator = param.fps;
        if (-1 == xioctl(param.fd, VIDIOC_S_PARM, &frameint))
            fprintf(stderr,"Unable to set frame interval.\n");
    }

    /* Buggy driver paranoia. */
    min = param.src_fmt.fmt.pix.width * 2;
    if (param.src_fmt.fmt.pix.bytesperline < min)
    {
        printf("bytesperline bugged!!\n");
        param.src_fmt.fmt.pix.bytesperline = min;
    }
    min = param.src_fmt.fmt.pix.bytesperline * param.src_fmt.fmt.pix.height;
    if (param.src_fmt.fmt.pix.sizeimage < min)
    {
        printf("sizeimage bugged!!\n");
        param.src_fmt.fmt.pix.sizeimage = min;
    }

    param.image_size = param.src_fmt.fmt.pix.sizeimage;
    param.rgb_src_image_size = param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 3;
    param.dst_image  = (unsigned char*) malloc(param.width * param.height * 3 *100);  // 3 for rgb without gamma  *100 is for debug
    param.tmp_image  = new unsigned char[param.rgb_src_image_size];
    param.tmp_image2 = new unsigned char[param.rgb_src_image_size];

    switch (param.io)
    {
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

    query_current_image_fmt_v4l2(param.fd);

    return true;
}

bool V4L_camera::deviceUninit()
{
    unsigned int i;
    bool ret = true;

    switch (param.io)
    {
        case IO_METHOD_READ:
        {
            free(param.buffers[0].start);
        } break;

        case IO_METHOD_MMAP:
        {
            for (i = 0; i < param.n_buffers; ++i)
            {
                if (-1 == v4l2_munmap(param.buffers[i].start, param.buffers[i].length))
                    ret = false;
            }

            CLEAR(param.req);
//             memset(param.req, 0, sizeof(struct v4l2_requestbuffers));
            param.req.count = 0;
            param.req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            param.req.memory = V4L2_MEMORY_MMAP;
            if(xioctl(param.fd, VIDIOC_REQBUFS, &param.req) < 0)
            {
                printf("VIDIOC_REQBUFS - Failed to delete buffers: %s (errno %d)\n", strerror(errno), errno);
                return false;
            }

        } break;

        case IO_METHOD_USERPTR:
        {
            for (i = 0; i < param.n_buffers; ++i)
                free(param.buffers[i].start);
        } break;
    }

    if(param.buffers != 0)
        free(param.buffers);
    return ret;
}

/**
 *    close device
 */
bool V4L_camera::close()
{
    yTrace();

    stop();   // stop yarp thread acquiring images

    if(param.fd != -1)
    {
        captureStop();
        deviceUninit();

        if (-1 == v4l2_close(param.fd))
            yError() << "Error closing V4l2 device";
        return false;
    }
    param.fd = -1;
    if(param.dst_image != NULL)
    {
        delete[] param.dst_image;
    }

    if(param.tmp_image != NULL)
    {
        delete[] param.tmp_image;
    }

    if(param.tmp_image2 != NULL)
    {
        delete[] param.tmp_image2;
    }

    if(param.raw_image != NULL)
    {
        free (param.raw_image);
    }
    return true;
}


// IFrameGrabberRgb Interface 777
bool V4L_camera::getRgbBuffer(unsigned char *buffer)
{
    mutex.wait();
    imageProcess(param.raw_image);
//     memcpy(buffer, param.dst_image, param.rgb_src_image_size*12/16);
//     memcpy(buffer, param.tmp_image2, param.rgb_src_image_size*12/16);
//     memcpy(buffer, param.outMat.data, param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 3);

//     memcpy(buffer, param.img.data, param.src_fmt.fmt.pix.width*12/16 * param.src_fmt.fmt.pix.height * 3);
//     std::cout << "dst_image_size"<< param.dst_image_size << "; compute " << param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 3 *12 /16  << std::endl;
//     memcpy(buffer, param.outMat.data, param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 3 *12 /16 );
//     int __size = param.outMat.total();
    memcpy(buffer, param.outMat.data, param.outMat.total()*3);
    mutex.post();
    return true;
}

// IFrameGrabber Interface
bool V4L_camera::getRawBuffer(unsigned char *buffer)
{
    mutex.wait();
    imageProcess(param.raw_image);
    memcpy(buffer, param.dst_image, param.dst_image_size);
    mutex.post();
    return true;
}

int V4L_camera::getRawBufferSize()
{
    return 0;
}

/**
 * Return the height of each frame.
 * @return image height
 */
int V4L_camera::height() const
{
//     yTrace() << ": height is " << param.height;
//     return param.src_fmt.fmt.pix.height /** 0.5*/;
//     return param.src_fmt.fmt.pix.height *0.5;
//     return param.outMat.rows;
    return param.height;
}

/**
 * Return the width of each frame.
 * @return image width
 */
int V4L_camera::width() const
{
//     yTrace() << ": width is " << param.width;
//     return param.src_fmt.fmt.pix.width*12/16 *0.5;
//     return param.outMat.cols;
    return param.width;
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

    do r = v4l2_ioctl(fd, request, argp);
    while (-1 == r && EINTR == errno);

    return r;
}


////////////////////////////////////////////////////


struct v4l2_queryctrl queryctrl;
struct v4l2_querymenu querymenu;

void V4L_camera::enumerate_menu (void)
{
    printf ("  Menu items:\n");

    memset (&querymenu, 0, sizeof (querymenu));
    querymenu.id = queryctrl.id;

    for (querymenu.index = (__u32) queryctrl.minimum;  querymenu.index <= (__u32) queryctrl.maximum;  querymenu.index++)
    {
        if (0 == ioctl (param.fd, VIDIOC_QUERYMENU, &querymenu))
        {
            printf ("  %s\n", querymenu.name);
        } else {
            perror ("VIDIOC_QUERYMENU");
            return;
        }
    }
}


bool V4L_camera::enumerate_controls()
{
    memset (&queryctrl, 0, sizeof (queryctrl));

    for (queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_LASTP1; queryctrl.id++)
    {
        if (0 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            printf ("Control %s (id %d)\n", queryctrl.name, queryctrl.id);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        }
        else
        {
            if (errno == EINVAL)
                continue;

            perror ("VIDIOC_QUERYCTRL");
            return false;
        }
    }

    for (queryctrl.id = V4L2_CID_PRIVATE_BASE; ; queryctrl.id++)
    {
        if (0 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            printf ("Control %s\n", queryctrl.name);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        }
        else
        {
            if (errno == EINVAL)
                break;

            perror ("VIDIOC_QUERYCTRL");
            return false;
        }
    }
    return true;
}

/**
 *   mainloop: read frames and process them
 */
void* V4L_camera::full_FrameRead(void)
{
    bool got_it = false;
    void *image_ret = NULL;
    unsigned int count;
    unsigned int numberOfTimeouts;

    fd_set fds;
    struct timeval tv;
    int r;

    numberOfTimeouts = 0;
    count = 10;  //trials


    for (unsigned int i=0; i<count; i++)
    {
        FD_ZERO(&fds);
        FD_SET(param.fd, &fds);

        /* Timeout. */
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        r = select(param.fd + 1, &fds, NULL, NULL, &tv);

        if (r < 0)
        {
            if (EINTR == errno)
                continue;

            return image_ret;
        }
        else if (0 == r)
        {
            numberOfTimeouts++;
            {
                printf("select timeout number %d\n", numberOfTimeouts);
                got_it = false;
            }
        }
        else if ((r > 0) && (FD_ISSET(param.fd, &fds)))
        {
            if( (image_ret=frameRead()) != NULL)
            {
        //             printf("got an image\n");
                got_it = true;
                break;
            }
            else
            {
                printf("trial %d failed\n", i);
            }
        }
        else
            printf("select woke up for something else\n");

        /* EAGAIN - continue select loop. */
    }
    if(!got_it)
        printf("NO GOOD image got \n");

    return image_ret; //param.dst_image;
}

/**
 *    read single frame
 */
void* V4L_camera::frameRead()
{
    struct v4l2_buffer buf;
    unsigned int i;

    switch (param.io)
    {
        case IO_METHOD_READ:
            printf("IO_METHOD_READ\n");
            if (-1 == v4l2_read(param.fd, param.buffers[0].start, param.buffers[0].length))
            {
                switch (errno)
                {
                    case EAGAIN:
                        return NULL;

                    case EIO:
                        // Could ignore EIO, see spec.
                        // fall through

                    default:
                        errno_exit("read");
                        return NULL;
                }
                timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec/1000000.0);
            }

//             memcpy(param.raw_image, param.buffers[0].start, param.image_size);
//             imageProcess(param.buffers[0].start);
            break;


            case IO_METHOD_MMAP:
            {
//                 printf("IO_METHOD_MMAP\n");

                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (-1 == xioctl(param.fd, VIDIOC_DQBUF, &buf))
                {
                    switch (errno)
                    {
                        default:
                            printf("\n ERROR: VIDIOC_DQBUF\n");
                            return NULL;
                    }
                }

                if( !(buf.index < param.n_buffers) )
                {
                    yError() << "at line " << __LINE__;
                }


                mutex.wait();
                memcpy(param.raw_image, param.buffers[buf.index].start, param.buffers[0].length); //param.image_size);
//                 param.raw_image = param.buffers[buf.index].start;
//                 imageProcess(param.raw_image);
                timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec/1000000.0);
                mutex.post();

                if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf))
                {
                    errno_exit("VIDIOC_QBUF");
                    return NULL;
                }

            } break;

            case IO_METHOD_USERPTR:
            {
                printf("IO_METHOD_USERPTR\n");

                CLEAR (buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;

                if (-1 == xioctl(param.fd, VIDIOC_DQBUF, &buf))
                {
                    switch (errno)
                    {
                        case EAGAIN:
                            return 0;

                        case EIO:
                            // Could ignore EIO, see spec.
                            // fall through

                        default:
                            errno_exit("VIDIOC_DQBUF");
                    }
                }

                for (i = 0; i < param.n_buffers; ++i)
                    if (buf.m.userptr == (unsigned long)param.buffers[i].start && buf.length == param.buffers[i].length)
                        break;

                    if(! (i < param.n_buffers) )
                    {
                        yError() << "at line " << __LINE__;
                    }

                mutex.wait();
                memcpy(param.raw_image, param.buffers[buf.index].start, param.image_size);
//                 param.raw_image = (void*) buf.m.userptr;
                timeStamp.update(toEpochOffset + buf.timestamp.tv_sec + buf.timestamp.tv_usec/1000000.0);
                mutex.post();


                if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf))
                    errno_exit("VIDIOC_QBUF");
            }  break;

        default:
        {
            printf("frameRead, default case\n");
//             param.raw_image = NULL;
        }
    }
    return (void*) param.raw_image; //param.dst_image;
}

/**
 *   process image read
 */
void V4L_camera::imageProcess(void* p)
{
    static bool initted = false;
    static int err=0;

    timeStart = yarp::os::Time::now();

    switch(param.camModel)
    {
        case RAW_DATA:
        {
            break;
        }

        case SEE3CAMCU50:
        {
//             std::cout << "dst w " << param.dst_fmt.fmt.pix.width << "; src w " << param.src_fmt.fmt.pix.width << std::endl;
//             std::cout << "dst h " << param.dst_fmt.fmt.pix.height << "; src h " << param.src_fmt.fmt.pix.height << std::endl;
//             std::cout << "param w " << param.width << "; param h " << param.height << std::endl;
//             std::cout << "dst size " << param.dst_image_size << "; src size " << param.image_size << std::endl;

            if( v4lconvert_convert((v4lconvert_data*) _v4lconvert_data,  &param.src_fmt,   &param.dst_fmt,
                                                      (unsigned char *)p, param.image_size, param.tmp_image, param.rgb_src_image_size)  <0 )
            {
                if((err %20) == 0)
                {
                    printf("error converting \n"); fflush(stdout);
                    printf("Message is: %s", v4lconvert_get_error_message(_v4lconvert_data));
                    err=0;
                }
                err++;
                return;
            }

            if(doCropping)
            {
                int tmp1_rowSize = param.dst_fmt.fmt.pix.width * 3;
                int tmp2_rowSize = tmp1_rowSize *12/16;

                if(dual)
                {
                    for(int h=0; h<param.dst_fmt.fmt.pix.height; h++)
                    {
                        memcpy((void *) &param.tmp_image2[h*tmp2_rowSize],                 (void *)&param.tmp_image[h*tmp1_rowSize],                tmp2_rowSize/2);
                        memcpy((void *) &param.tmp_image2[h*tmp2_rowSize+tmp2_rowSize/2],  (void *)&param.tmp_image[h*tmp1_rowSize+tmp1_rowSize/2], tmp2_rowSize/2);
                    }
// 666
//                     cv::Mat img( param.src_fmt.fmt.pix.height, param.src_fmt.fmt.pix.width*12/16, CV_8UC3, param.tmp_image2);
                    cv::Mat img(cv::Size(param.src_fmt.fmt.pix.width*12/16, param.src_fmt.fmt.pix.height), CV_8UC3, param.tmp_image2);
                    param.img=img;
//                     cv::resize(img, param.outMat, cvSize(param.src_fmt.fmt.pix.width*12/16, param.src_fmt.fmt.pix.height));
//                     cv::resize(img, param.outMat, cvSize(0, 0), param.src_fmt.fmt.pix.width*12/16/param.width, param.src_fmt.fmt.pix.height/param.height, cv::INTER_CUBIC);
                    cv::resize(img, param.outMat, cvSize(param.width, param.height), 0, 0, cv::INTER_CUBIC);

//                     memcpy((unsigned char *)param.dst_image, (unsigned char *) param.outMat.data, param.width* param.height*3);


//                     for(int h=0; h<param.src_fmt.fmt.pix.height; h++)
//                     {
//                         memcpy((void *) &param.tmp_image2[h*tmp_rowSize_out],                    (void *)&param.tmp_image[h*tmp_rowSize],               tmp_rowSize_out/2);
//                         memcpy((void *) &param.tmp_image2[h*tmp_rowSize_out+tmp_rowSize_out/2],  (void *)&param.tmp_image[h*tmp_rowSize+tmp_rowSize/2], tmp_rowSize_out/2);
//                     }
                }
                else
                {
                    for(int h=0; h<param.dst_fmt.fmt.pix.height; h++)
                    {
                        memcpy((void *) &param.tmp_image2[h*tmp2_rowSize], (void *)&param.tmp_image[h*tmp1_rowSize], tmp2_rowSize);
                    }
//                     for(int h=0; h<param.src_fmt.fmt.pix.height; h++)
//                     {
//                         memcpy((void *) &param.dst_image[h*rowSize/16*12], (void *)&param.tmp_image[h*rowSize], rowSize/16*12);
//                     }
                }
            }
            else
            {
                cv::Mat img(cv::Size(param.src_fmt.fmt.pix.width, param.src_fmt.fmt.pix.height), CV_8UC3, param.tmp_image);
                param.img=img;
                cv::resize(img, param.outMat, cvSize(param.width, param.height), 0, 0, cv::INTER_CUBIC);
//                 memcpy((unsigned char *)param.dst_image, (unsigned char *)param.tmp_image, param.image_size);
            }

            break;
        }

        case LEOPARD_MT9M021C:
        {
//             raw_to_bmp( (uint8_t*) p, (uint8_t*) param.dst_image, param.width, param.height, 12, 0,
//                         true, 1.6,
//                         600, -92, -70, -97, 389, -36, -130, -304, 690, 0, 0, 0);
            break;
        }

        default:
        {
            yError() << "Unsupported camera, don't know how to do color reconstruction to RGB";
            break;
        }
    }

    timeElapsed = yarp::os::Time::now() - timeStart;
    myCounter++;
    timeTot += timeElapsed;
//     yDebug("Conversion time is %.6f ms", timeElapsed*1000);

    if((myCounter % 60) == 0)
    {
        if(!initted)
        {
            timeTot = 0;
            myCounter = 0;
            initted  = true;
        }
    }

}

/**
 *    stop capturing
 */
void V4L_camera::captureStop()
{
    int ret=0;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    switch(param.io)
    {
        case IO_METHOD_READ:
            //do nothing
            break;

        case IO_METHOD_MMAP:
        default:
            ret = xioctl(param.fd, VIDIOC_STREAMOFF, &type);
            if (ret < 0)
            {
                if(errno != 9)      /* errno = 9 means the capture was allready stoped*/
                    perror("VIDIOC_STREAMOFF - Unable to stop capture");
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

    switch (param.io)
    {
        case IO_METHOD_READ:
            /* Nothing to do. */
            break;

        case IO_METHOD_MMAP:
            for (i = 0; i < param.n_buffers; ++i)
            {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf))
                    errno_exit("VIDIOC_QBUF");
            }

            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (-1 == xioctl(param.fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");

//             param.raw_image = param.buffers[0].start;
            break;

        case IO_METHOD_USERPTR:
            for (i = 0; i < param.n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR (buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;
                buf.index = i;
                buf.m.userptr = (unsigned long) param.buffers[i].start;
                buf.length = param.buffers[i].length;

                if (-1 == xioctl(param.fd, VIDIOC_QBUF, &buf))
                    errno_exit("VIDIOC_QBUF");
            }

            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (-1 == xioctl(param.fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");

            break;
    }
}



bool V4L_camera::readInit(unsigned int buffer_size)
{
    param.buffers = (struct buffer *) calloc(1, sizeof(*(param.buffers)));

    if (!param.buffers)
    {
        fprintf(stderr, "Out of memory\n");
        return false;
    }

    param.buffers[0].length = buffer_size;
    param.buffers[0].start = malloc(buffer_size);

    if (!param.buffers[0].start)
    {
        fprintf (stderr, "Out of memory\n");
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

    if (-1 == xioctl(param.fd, VIDIOC_REQBUFS, &param.req))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s does not support memory mapping\n", param.deviceId.c_str());
            return false;
        }
        else
        {
            fprintf(stderr, "Error on device %s requesting memory mapping (VIDIOC_REQBUFS)\n", param.deviceId.c_str());
            return false;
        }
    }

    if (param.req.count < 1)
    {
        fprintf(stderr, "Insufficient buffer memory on %s\n", param.deviceId.c_str());
        return false;
    }

    if (param.req.count == 1)
    {
        fprintf(stderr, "Only 1 buffer was available, you may encounter performance issue acquiring images from device %s\n", param.deviceId.c_str());
    }

    param.buffers = (struct buffer *) calloc(param.req.count, sizeof(*(param.buffers)));

    if (!param.buffers)
    {
        fprintf(stderr, "Out of memory\n");
        return false;
    }

    struct v4l2_buffer buf;

    printf("n buff is %d\n", param.req.count);

    for (param.n_buffers = 0; param.n_buffers < param.req.count; param.n_buffers++)
    {

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = param.n_buffers;

        if (-1 == xioctl(param.fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        printf("image size is %d - buf.len is %d, offset is %d - new offset is %d\n", param.image_size, buf.length, buf.m.offset, param.image_size*param.n_buffers);
        param.buffers[param.n_buffers].length = buf.length;
        param.buffers[param.n_buffers].start = v4l2_mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, param.fd, buf.m.offset);

        if (MAP_FAILED == param.buffers[param.n_buffers].start)
            errno_exit("mmap");
    }

    param.raw_image = malloc(param.image_size);
    yInfo() << "size is " << buf.length << " or " << param.image_size << param.raw_image;

    return true;
}

bool V4L_camera::userptrInit(unsigned int buffer_size)
{
//     struct v4l2_requestbuffers req;
    unsigned int page_size;

    page_size = getpagesize();
    buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

    CLEAR(param.req);

    param.req.count = VIDIOC_REQBUFS_COUNT;
    param.req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(param.fd, VIDIOC_REQBUFS, &param.req))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s does not support user pointer i/o\n", param.deviceId.c_str());
            return false;
        }
        else
        {
            fprintf(stderr, "Error requesting VIDIOC_REQBUFS for device %s\n", param.deviceId.c_str());
            return false;
        }
    }

    param.buffers = (struct buffer *) calloc(4, sizeof(*(param.buffers)));

    if (!param.buffers)
    {
        fprintf(stderr, "Out of memory\n");
        return false;
    }

    for (param.n_buffers = 0; param.n_buffers < 4; ++param.n_buffers)
    {
        param.buffers[param.n_buffers].length = buffer_size;
        param.buffers[param.n_buffers].start = memalign(/* boundary */ page_size, buffer_size);

        if (!param.buffers[param.n_buffers].start)
        {
            fprintf(stderr, "Out of memory\n");
            return false;
        }
    }
    return true;
}

bool V4L_camera::set_V4L2_control(uint32_t id, double value, bool verbatim)
{
//     yTrace();
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&queryctrl, 0, sizeof (queryctrl));
    queryctrl.id = id;

    if (-1 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
        if (errno != EINVAL)
        {
            perror ("VIDIOC_QUERYCTRL");
        }
        else
        {
            yError("Cannot set control <%s> (id %d) is not supported \n", queryctrl.name, queryctrl.id);
        }
        return false;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        printf ("Control %s is disabled\n", queryctrl.name);
        return false;
    }
    else
    {
        memset (&control, 0, sizeof (control));
        control.id = id;
        if(verbatim)
            control.value = value;
        else
            control.value = (int32_t) (value * (queryctrl.maximum - queryctrl.minimum) + queryctrl.minimum);

        if (-1 == ioctl(param.fd, VIDIOC_S_CTRL, &control))
        {
            perror ("VIDIOC_S_CTRL");
            if(errno == ERANGE)
            {
                printf("Normalized input value %f ( equivalent to raw value of %d) was out of range for control %s: Min and Max are: %d - %d \n", value, control.value, queryctrl.name, queryctrl.minimum, queryctrl.maximum);
            }
            return false;
        }
        printf("set control %s to %d done!\n", queryctrl.name, control.value);
    }
    return true;
}

bool V4L_camera::check_V4L2_control(uint32_t id)
{
//     yTrace();
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&control, 0, sizeof (control));
    memset (&queryctrl, 0, sizeof (queryctrl));

    control.id = id;
    queryctrl.id = id;

    if (-1 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
        if (errno != EINVAL)
        {
            perror ("VIDIOC_QUERYCTRL");
        }
//         else
//         {
//             printf ("Control %s (id %d) is not supported\n", queryctrl.name, id);
//         }
        return false;
    }
    return true;
}

double V4L_camera::get_V4L2_control(uint32_t id, bool verbatim)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&control, 0, sizeof (control));
    memset (&queryctrl, 0, sizeof (queryctrl));

    control.id = id;
    queryctrl.id = id;

    if (-1 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
        if (errno != EINVAL)
        {
            perror ("VIDIOC_QUERYCTRL");
        }
//         else
//         {
//             printf ("Control %s is not supported\n", queryctrl.name);
//         }
        return -1.0;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        printf ("Control %s is disabled\n", queryctrl.name);
    }
    else
    {
        if (-1 == ioctl(param.fd, VIDIOC_G_CTRL, &control))
        {
            perror ("VIDIOC_G_CTRL");
            return -1.0;
        }
//         printf("Control %s got value %d!\n", queryctrl.name, control.value);
    }
    if(verbatim)
        return control.value;

    return (double) (control.value - queryctrl.minimum) /  (queryctrl.maximum - queryctrl.minimum);
}


// GET CONTROLS!!
double V4L_camera::getBrightness()
{
    return get_V4L2_control(V4L2_CID_BRIGHTNESS);
}

double V4L_camera::getExposure()
{
    return get_V4L2_control(V4L2_CID_EXPOSURE);
}

double V4L_camera::getGain()
{
    return get_V4L2_control(V4L2_CID_GAIN);
}

double V4L_camera::getGamma()
{
    return get_V4L2_control(V4L2_CID_GAMMA);
}

double V4L_camera::getHue()
{
    return get_V4L2_control(V4L2_CID_HUE);
}

double V4L_camera::getIris()
{
    return get_V4L2_control(V4L2_CID_IRIS_ABSOLUTE);
}

double V4L_camera::getSaturation()
{
    return get_V4L2_control(V4L2_CID_SATURATION);
}

double V4L_camera::getSharpness()
{
    return get_V4L2_control(V4L2_CID_SHARPNESS);
}

double V4L_camera::getShutter()
{
    printf("Don't know how to map it :-&\n");
    return false;
}

bool V4L_camera::getWhiteBalance(double &blue, double &red)
{
    blue = get_V4L2_control(V4L2_CID_RED_BALANCE);
    red  = get_V4L2_control(V4L2_CID_BLUE_BALANCE);
    if( (red == -1) || (blue == -1) )
        return false;
    else
        return true;
}


    // SET CONTROLS!!
bool V4L_camera::setBrightness(double v)
{
    return set_V4L2_control(V4L2_CID_BRIGHTNESS, v);
}

bool V4L_camera::setExposure(double v)
{
    return set_V4L2_control(V4L2_CID_EXPOSURE, v);
}

bool V4L_camera::setGain(double v)
{
    return set_V4L2_control(V4L2_CID_GAIN, v);
}

bool V4L_camera::setGamma(double v)
{
    return set_V4L2_control(V4L2_CID_GAMMA, v);
}

bool V4L_camera::setHue(double v)
{
    return set_V4L2_control(V4L2_CID_HUE, v);
}

bool V4L_camera::setIris(double v)
{
    return set_V4L2_control(V4L2_CID_IRIS_ABSOLUTE, v);
}

bool V4L_camera::setSaturation(double v)
{
    return set_V4L2_control(V4L2_CID_SATURATION, v);
}

bool V4L_camera::setSharpness(double v)
{
    return set_V4L2_control(V4L2_CID_SHARPNESS, v);
}

bool V4L_camera::setShutter(double v)
{
//     return set_V4L2_control(V4L2_CID_BRIGHTNESS, v);
    printf("No known function on V4L2 for shutter :-&\n");
    return false;
}

bool V4L_camera::setWhiteBalance(double blue, double red)
{
    bool ret = true;
    ret &= set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, false);
    ret &= set_V4L2_control(V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, V4L2_WHITE_BALANCE_MANUAL);
    ret &= set_V4L2_control(V4L2_CID_RED_BALANCE, blue);
    ret &= set_V4L2_control(V4L2_CID_BLUE_BALANCE, red);
    return ret;
}


bool V4L_camera::getCameraDescription(CameraDescriptor* camera)
{
    camera->busType = BUS_USB;
    camera->deviceDescription = "USB3 camera";
    return true;
}

bool V4L_camera::hasFeature(int feature, bool *_hasFeature)
{
    bool tmpMan(false), tmpAuto(false), tmpOnce(false);

    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            tmpMan = check_V4L2_control(V4L2_CID_RED_BALANCE) && check_V4L2_control(V4L2_CID_BLUE_BALANCE);
            tmpOnce = check_V4L2_control(V4L2_CID_DO_WHITE_BALANCE);
            tmpAuto = check_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        } break;

        case YARP_FEATURE_EXPOSURE:
        {
            tmpMan = check_V4L2_control(V4L2_CID_EXPOSURE);
            tmpAuto = check_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
        } break;

        default:
        {
            tmpMan = check_V4L2_control(convertYARP_to_V4L(feature));
        } break;
    }

    *_hasFeature = tmpMan || tmpOnce || tmpAuto;
    return true;
}

bool V4L_camera::setFeature(int feature, double value)
{
    return set_V4L2_control(convertYARP_to_V4L(feature), value);
}

bool V4L_camera::getFeature(int feature, double* value)
{
    double tmp =  get_V4L2_control(convertYARP_to_V4L(feature));
    if( tmp == -1)
        return false;

    *value = tmp;
    return true;
}

bool V4L_camera::setFeature(int feature, double value1, double value2)
{
    if(feature == YARP_FEATURE_WHITE_BALANCE)
    {
        return setWhiteBalance(value1, value2);
    }
    return false;
}

bool V4L_camera::getFeature(int feature, double* value1, double* value2)
{
    if(feature == YARP_FEATURE_WHITE_BALANCE)
    {
        return getWhiteBalance(*value1, *value2);
    }
    return false;
}

bool V4L_camera::hasOnOff(int feature, bool *_hasOnOff)
{
    bool _hasAuto;
    // I can't find any meaning of setting a feature to off on V4l ... what it is supposed to do????
    switch(feature)
    {
        // The following do have a way to set them auto/manual
        case YARP_FEATURE_WHITE_BALANCE:
        case YARP_FEATURE_EXPOSURE:
        {
            if(hasAuto(feature, &_hasAuto) )
                *_hasOnOff = true;
            else
                *_hasOnOff = false;
        } break;

        // try it out
        default:
        {
            hasAuto(feature, &_hasAuto);
            if(_hasAuto)
                *_hasOnOff = true;
            else
                *_hasOnOff = false;
        } break;
    }
    return true;
}

bool V4L_camera::setActive(int feature, bool onoff)
{
    // I can't find any meaning of setting a feature to off on V4l ... what it is supposed to do????
    bool tmp;
    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            tmp = set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, onoff);
            if(tmp)
                isActive_vector[feature] = onoff;
        } break;

        case YARP_FEATURE_EXPOSURE:
        {
            if(onoff)
            {
                set_V4L2_control(V4L2_LOCK_EXPOSURE, false);

                hasAuto(feature, &tmp);
                if(tmp)
                    tmp = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO);
                else
                    tmp = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);

                if(tmp)
                    isActive_vector[feature] = onoff;
            }
            else
            {
                bool man = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
                if(!man)
                {
                    man = set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_SHUTTER_PRIORITY, true);
                    if(!man)
                        yError() << "Cannot set manual exposure";
                }
                set_V4L2_control(V4L2_LOCK_EXPOSURE, true);
                isActive_vector[feature] = onoff;
            }
        } break;

        default:    // what to do in each case?
        {
            if(onoff == true)
            {
                isActive_vector[feature] = true;
                return true;
            }
            else
            {
                isActive_vector[feature] = false;
                return false;
            }
        } break;
    }
}

bool V4L_camera::getActive(int feature, bool *_isActive)
{
    // I can't find any meaning of setting a feature to off on V4l ... what it is supposed to do????

    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            double tmp = get_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
            if(tmp == 1)
            {
                *_isActive = true;
            }
            else
                *_isActive = false;
        } break;


        case YARP_FEATURE_EXPOSURE:
        {
            bool _hasMan;
            hasFeature(V4L2_CID_EXPOSURE, &_hasMan);                // check manual version
            double _hasAuto =  get_V4L2_control(V4L2_CID_EXPOSURE_AUTO, true); // check auto version

            *_isActive = (_hasAuto == V4L2_EXPOSURE_AUTO)|| _hasMan;
        } break;

        default:
        {
            *_isActive = true;
        } break;
    }

    return true;
}

bool V4L_camera::hasAuto(int feature, bool* _hasAuto)
{
    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            *_hasAuto = check_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
        } break;

        case YARP_FEATURE_BRIGHTNESS:
        {
            *_hasAuto = check_V4L2_control(V4L2_CID_AUTOBRIGHTNESS);
        } break;

        case YARP_FEATURE_GAIN:
        {
            *_hasAuto = check_V4L2_control(V4L2_CID_AUTOGAIN);
        } break;

        case YARP_FEATURE_EXPOSURE:
        {
            *_hasAuto = check_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
        } break;

        case YARP_FEATURE_HUE:
        {
            *_hasAuto = check_V4L2_control(V4L2_CID_HUE_AUTO);
        } break;

        default:
        {
            *_hasAuto = false;
        } break;
    }
    return true;
}

bool V4L_camera::hasManual(int feature, bool* _hasManual)
{
    if(feature == YARP_FEATURE_WHITE_BALANCE)
    {
        *_hasManual = check_V4L2_control(V4L2_CID_RED_BALANCE) && check_V4L2_control(V4L2_CID_BLUE_BALANCE);
        return true;
    }

    if(feature == YARP_FEATURE_EXPOSURE)
    {
        *_hasManual = check_V4L2_control(V4L2_CID_EXPOSURE);
        return true;
    }
    return hasFeature(feature, _hasManual);
}

bool V4L_camera::hasOnePush(int feature, bool *_hasOnePush)
{
    // I'm not able to map a 'onePush' request on V4L api
    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            *_hasOnePush = check_V4L2_control(V4L2_CID_DO_WHITE_BALANCE);
            return true;
        } break;

        default:
        {
            *_hasOnePush = false;
        } break;
    }
    return true;
}

bool V4L_camera::setMode(int feature, FeatureMode mode)
{
    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            if(mode == MODE_AUTO)
                set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, true);
            else
                set_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE, false);
        } break;

        case YARP_FEATURE_EXPOSURE:
        {
            if(mode == MODE_AUTO)
                set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, true);
            else
                set_V4L2_control(V4L2_CID_EXPOSURE_AUTO, false);
        } break;

        case YARP_FEATURE_GAIN:
        {
            if(mode == MODE_AUTO)
            {
                yInfo() << "GAIN: set mode auto";
                set_V4L2_control(V4L2_CID_AUTOGAIN, true);
            }
            else
            {
                yInfo() << "GAIN: set mode manual";
                set_V4L2_control(V4L2_CID_AUTOGAIN, false);
            }
        } break;

        case YARP_FEATURE_BRIGHTNESS:
        {
            if(mode == MODE_AUTO)
                set_V4L2_control(V4L2_CID_AUTOBRIGHTNESS, true);
            else
                set_V4L2_control(V4L2_CID_AUTOBRIGHTNESS, false);
        } break;

        case YARP_FEATURE_HUE:
        {
            if(mode == MODE_AUTO)
                set_V4L2_control(V4L2_CID_HUE_AUTO, true);
            else
                set_V4L2_control(V4L2_CID_HUE_AUTO, false);
        } break;

        default:
        {
            yError() << "Feature " << feature << " does not support auto mode";
        } break;
    }
    return true;
}

bool V4L_camera::getMode(int feature, FeatureMode *mode)
{
    bool _tmpAuto;
    switch(feature)
    {
        case YARP_FEATURE_WHITE_BALANCE:
        {
            double ret  = get_V4L2_control(V4L2_CID_AUTO_WHITE_BALANCE);
            *mode = toFeatureMode(ret);
        } break;

        case YARP_FEATURE_EXPOSURE:
        {
            double ret  = get_V4L2_control(V4L2_CID_EXPOSURE_AUTO);
            if( ret == V4L2_EXPOSURE_MANUAL)
                *mode = MODE_MANUAL;
            else
                *mode = MODE_AUTO;
        } break;

        case YARP_FEATURE_BRIGHTNESS:
        {
            hasAuto(YARP_FEATURE_BRIGHTNESS, &_tmpAuto);
            *mode = toFeatureMode(_tmpAuto);
            if(!_tmpAuto)
                *mode = MODE_MANUAL;
            else
            {
                double ret  = get_V4L2_control(V4L2_CID_AUTOBRIGHTNESS);
                *mode = toFeatureMode(ret);
            }
        } break;

        case YARP_FEATURE_GAIN:
        {
            hasAuto(YARP_FEATURE_GAIN, &_tmpAuto);
            *mode = toFeatureMode(_tmpAuto);
            if(!_tmpAuto)
            {
                *mode = MODE_MANUAL;
            }
            else
            {
                double ret  = get_V4L2_control(V4L2_CID_AUTOGAIN);
                *mode = toFeatureMode(ret);
            }
        } break;

        case YARP_FEATURE_HUE:
        {
            hasAuto(YARP_FEATURE_HUE, &_tmpAuto);
            *mode = toFeatureMode(_tmpAuto);
            if(!_tmpAuto)
                *mode = MODE_MANUAL;
            else
            {
                double ret  = get_V4L2_control(V4L2_CID_HUE_AUTO);
                *mode = toFeatureMode(ret);
            }
        } break;

        default:
        {
            *mode = MODE_MANUAL;
        } break;
    }
    return true;
}

bool V4L_camera::setOnePush(int feature)
{
    // I'm not able to map a 'onePush' request on each V4L api
    if(feature == YARP_FEATURE_WHITE_BALANCE)
    {
        return set_V4L2_control(V4L2_CID_DO_WHITE_BALANCE, true);
    }
    return false;
}
