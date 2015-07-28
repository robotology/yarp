#ifndef _CAMERA_V4L_HPP_
#define _CAMERA_V4L_HPP_

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

//  L      I  N   N  U   U  X   X
//  L      I  NN  N  U   U   X X
//  L      I  N N N  U   U    X
//  L      I  N  NN  U   U   X X
//  LLLLL  I  N   N   UUU   X   X


#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <jpeglib.h>
#include <libv4l2.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>

// #include "list.h"
// #include "yuv.h"


#define CLEAR(x) memset (&(x), 0, sizeof (x))


// minimum number of buffers to request in VIDIOC_REQBUFS call
#define DEFAULT_WIDTH           640
#define DEFAULT_HEIGHT          480
#define DEFAULT_FRAMERATE       30
#define VIDIOC_REQBUFS_COUNT    2

namespace yarp {
    namespace dev {
        class V4L_camera;
    }
}

typedef enum {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
} io_method;

typedef enum {
    RAW_DATA = 0,
    SEE3CAMCU50,
    LEOPARD_MT9M021C
} supported_cams;

struct buffer {
    void *          start;
    size_t          length;
};


typedef struct
{
    std::string     deviceName;
    int             fd;
    int             width;
    int             height;
    io_method       io;
    int             fps;
    unsigned int    image_size;
    unsigned int    dst_image_size;
    unsigned char   *dst_image;
    void            *raw_image;
    unsigned int    n_buffers;
    struct buffer   *buffers;
    struct v4l2_format src_fmt;
    struct v4l2_format dst_fmt;
    struct v4l2_requestbuffers req;
    int pixelType;
    supported_cams  camModel;    // better is it is pixel format (YUV ...);  it happened that the only 2 cameras I have to test has custom tpyes, not standard ones.
} Video_params;



/*
 *  Device handling
 */

class yarp::dev::V4L_camera :   public yarp::dev::DeviceDriver,
                                public yarp::dev::IFrameGrabberRgb,
                                public yarp::dev::IFrameGrabber,
                                public yarp::dev::IFrameGrabberControls,
                                public yarp::os::RateThread
{
public:
    V4L_camera();

    // DeviceDriver Interface
    bool open(yarp::os::Searchable& config);
    bool close();

    // IFrameGrabberRgb    Interface
    bool getRgbBuffer(unsigned char *buffer);

    // IFrameGrabber Interface
    bool getRawBuffer(unsigned char *buffer);
    int getRawBufferSize();

    /**
     * Return the height of each frame.
     * @return image height
     */
    int height() const;

    /**
     * Return the width of each frame.
     * @return image width
     */
    int width() const;

    // IFrameGrabberControls interface

    bool setBrightness(double v);
    bool setExposure(double v);
    bool setSharpness(double v);
    bool setWhiteBalance(double blue, double red);
    bool setHue(double v);
    bool setSaturation(double v);
    bool setGamma(double v);
    bool setShutter(double v);
    bool setGain(double v);
    bool setIris(double v);
    double getBrightness();
    double getExposure();
    double getSharpness();
    bool getWhiteBalance(double &blue, double &red);
    double getHue();
    double getSaturation();
    double getGamma();
    double getShutter();
    double getGain();
    double getIris();

private:


    yarp::os::Semaphore mutex;
    Video_params param;
    double timeStart, timeTot, timeNow, timeElapsed;
    int myCounter;
    int frameCounter;

    bool fromConfig(yarp::os::Searchable& config);

    // initialize device
    bool deviceInit();

    // de-initialize device
    bool deviceUninit();

    void captureStart();
    void captureStop();

    bool threadInit();
    void run();
    void threadRelease();


    /*
    *  Inintialize different types of reading frame
    */

    // some description
    bool readInit(unsigned int buffer_size);

    // some description
    bool mmapInit();

    // some description
    bool userptrInit(unsigned int buffer_size);


    // use the device for something
    /**
    *    read single frame
    */
    void* frameRead();

    void* full_FrameRead(void);

    void imageProcess(void* p);

    int getfd();

private:

    // low level stuff
    /**
     *    Do ioctl and retry if error was EINTR ("A signal was caught during the ioctl() operation."). Parameters are the same as on ioctl.
     *
     *    \param fd file descriptor
     *    \param request request
     *    \param argp argument
     *    \returns result from ioctl
     */
    int xioctl(int fd, int request, void* argp);

    void enumerate_menu();
    bool enumerate_controls();
};

#endif // _CAMERA_V4L_HPP_
