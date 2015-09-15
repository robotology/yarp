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

#define errno_exit printf

//#include <Leopard_MT9M021C.h>

struct v4lconvert_data *_v4lconvert_data;

using namespace yarp::os;
using namespace yarp::dev;

V4L_camera::V4L_camera() : RateThread(1000/DEFAULT_FRAMERATE)
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
//     yTrace();
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
        printf("ERROR: v4lconvert_try_format\n\n");
        return false;
    }
    else
        printf("DONE: v4lconvert_try_format\n\n");


    if (-1 == xioctl(param.fd, VIDIOC_S_FMT, &param.src_fmt))
        std::cout << "xioctl error VIDIOC_S_FMT" << std::endl;


    /* Note VIDIOC_S_FMT may change width and height. */
    if (param.width != param.src_fmt.fmt.pix.width)
    {
        param.width = param.src_fmt.fmt.pix.width;
        std::cout << "Image width set to " << param.width << " by device " << param.deviceId << std::endl;
    }

    if (param.height != param.src_fmt.fmt.pix.height)
    {
        param.height = param.src_fmt.fmt.pix.height;
        std::cout << "Image height set to " << param.height << " by device " << param.deviceId << std::endl;
    }

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
    param.dst_image = (unsigned char*) malloc(param.src_fmt.fmt.pix.width * param.src_fmt.fmt.pix.height * 3);  // 3 for rgb without gamma

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
    return true;
}


// IFrameGrabberRgb Interface
bool V4L_camera::getRgbBuffer(unsigned char *buffer)
{
    mutex.wait();
    imageProcess(param.raw_image);
    memcpy(buffer, param.dst_image, param.dst_image_size);
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
    yTrace();
    return param.height;
}

/**
 * Return the width of each frame.
 * @return image width
 */
int V4L_camera::width() const
{
    yTrace();
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

            printf ("Control %s\n", queryctrl.name);

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
                memcpy(param.raw_image, param.buffers[buf.index].start, param.image_size);
//                 param.raw_image = param.buffers[buf.index].start;
//                 imageProcess(param.raw_image);
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

    timeStart = yarp::os::Time::now();

    switch(param.camModel)
    {
        case RAW_DATA:
        {
            break;
        }

        case SEE3CAMCU50:
        {
            if( v4lconvert_convert((v4lconvert_data*) _v4lconvert_data, &param.src_fmt, &param.dst_fmt,  (unsigned char *)p, param.image_size,  (unsigned char *)param.dst_image, param.dst_image_size)  <0 )
                printf("error converting \n");
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
        else
            yDebug("time mean is %.06f ms\n", timeTot/myCounter*1000);
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

bool V4L_camera::set_V4L2_control(uint32_t id, double value)
{
    yTrace();
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&queryctrl, 0, sizeof (queryctrl));
    queryctrl.id = id;

    if (-1 == ioctl (param.fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
        if (errno != EINVAL)
        {
            perror ("VIDIOC_QUERYCTRL");
            return false;
        }
        else
        {
            printf ("Control %s is not supported\n", queryctrl.name);
        }
    }
    else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        printf ("Control %s is disabled\n", queryctrl.name);
        return false;
    }
    else
    {
        memset (&control, 0, sizeof (control));
        control.id = id;
        control.value = (int32_t) (value * (queryctrl.maximum - queryctrl.minimum) + queryctrl.minimum);

        if (-1 == ioctl(param.fd, VIDIOC_S_CTRL, &control))
        {
            perror ("VIDIOC_S_CTRL");
            return false;
        }
        if(errno == ERANGE)
        {
            printf("Normalized input value %f ( equivalent to raw value of %d) was out of range for control %s: Min and Max are: %d - %d \n", value, control.value, queryctrl.name, queryctrl.minimum, queryctrl.maximum);
        }
        printf("set control %s to %d done!\n", queryctrl.name, control.value);
    }
    return true;
}

double V4L_camera::get_V4L2_control(uint32_t id)
{
    yTrace();
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
            return false;
        }
        else
        {
            printf ("Control %s is not supported\n", queryctrl.name);
        }
    }
    else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        printf ("Control %s is disabled\n", queryctrl.name);
    }
    else
    {
        if (-1 == ioctl(param.fd, VIDIOC_G_CTRL, &control))
        {
            perror ("VIDIOC_G_CTRL");
            return false;
        }
//         printf("Control %s got value %d!\n", queryctrl.name, control.value);
    }
    return (double) (control.value - queryctrl.minimum) /  (queryctrl.maximum - queryctrl.minimum);
}


    // GET CONTROLS!!
double V4L_camera::getBrightness()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_BRIGHTNESS);
}

double V4L_camera::getExposure()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_EXPOSURE);
}

double V4L_camera::getGain()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_GAIN);
}

double V4L_camera::getGamma()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_GAMMA);
}

double V4L_camera::getHue()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_HUE);
}

double V4L_camera::getIris()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_IRIS_ABSOLUTE);
}

double V4L_camera::getSaturation()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_SATURATION);
}

double V4L_camera::getSharpness()
{
    yTrace();
    return get_V4L2_control(V4L2_CID_SHARPNESS);
}

double V4L_camera::getShutter()
{
    yTrace();
    printf("Don't know how to map it :-&\n");
    return false;
}

bool V4L_camera::getWhiteBalance(double& blue, double& red)
{
    yTrace();
    bool ret = true;
    blue = set_V4L2_control(V4L2_CID_RED_BALANCE, blue);
    red  = set_V4L2_control(V4L2_CID_BLUE_BALANCE, red);
    return ret;
}


    // SET CONTROLS!!
bool V4L_camera::setBrightness(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_BRIGHTNESS, v);
}

bool V4L_camera::setExposure(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_EXPOSURE, v);
}

bool V4L_camera::setGain(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_GAIN, v);
}

bool V4L_camera::setGamma(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_GAMMA, v);
}

bool V4L_camera::setHue(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_HUE, v);
}

bool V4L_camera::setIris(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_IRIS_ABSOLUTE, v);
}

bool V4L_camera::setSaturation(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_SATURATION, v);
}

bool V4L_camera::setSharpness(double v)
{
    yTrace();
    return set_V4L2_control(V4L2_CID_SHARPNESS, v);
}

bool V4L_camera::setShutter(double v)
{
    yTrace();
//     return set_V4L2_control(V4L2_CID_BRIGHTNESS, v);
    printf("Don't know how to map it :-&\n");
    return false;
}

bool V4L_camera::setWhiteBalance(double blue, double red)
{
    yTrace();
    bool ret = true;
    ret &= set_V4L2_control(V4L2_CID_RED_BALANCE, blue);
    ret &= set_V4L2_control(V4L2_CID_BLUE_BALANCE, red);
    return ret;
}


bool V4L_camera::getCameraDescription(CameraDescriptor* camera)
{
    yTrace();
    camera->busType = BUS_USB;
    camera->deviceDescription = "USB3 camera";
    return true;
}

bool V4L_camera::hasFeature(int feature, bool* hasFeature)
{

}

bool V4L_camera::setFeature(int feature, double* values)
{

}

bool V4L_camera::getFeature(int feature, double* values)
{

}

bool V4L_camera::hasOnOff(int feature, bool* hasOnOff)
{

}

bool V4L_camera::setActive(int feature, bool onoff)
{

}

bool V4L_camera::getActive(int feature, bool* isActive)
{

}

bool V4L_camera::hasAuto(int feature, bool* hasAuto)
{

}

bool V4L_camera::hasManual(int feature, bool* hasManual)
{

}

bool V4L_camera::hasOnePush(int feature, bool* hasOnePush)
{

}

bool V4L_camera::setMode(int feature, FeatureMode mode)
{

}

bool V4L_camera::getMode(int feature, FeatureMode* mode)
{

}

bool V4L_camera::setOnePush(int feature)
{

}






