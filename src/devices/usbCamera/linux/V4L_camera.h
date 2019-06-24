/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef YARP_DEVICE_USBCAMERA_LINUX_V4L_CAMERA_H
#define YARP_DEVICE_USBCAMERA_LINUX_V4L_CAMERA_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/IVisualParams.h>
#include <yarp/dev/PreciselyTimed.h>

#include <asm/types.h>
#include <cv.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <iostream>
#include <jpeglib.h>
#include <libv4l2.h>
#include <libv4lconvert.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

// minimum number of buffers to request in VIDIOC_REQBUFS call
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_FRAMERATE 30
#define VIDIOC_REQBUFS_COUNT 2

typedef enum
{
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
} io_method;

typedef enum
{
    STANDARD_UVC = 0,
    LEOPARD_PYTHON,
} supported_cams;


struct buffer
{
    void* start;
    size_t length;
};


typedef struct
{
    int fd;
    std::string deviceId;

    bool addictionalResize;
    int resizeOffset_x, resizeOffset_y;
    int resizeWidth, resizeHeight;

    __u32 user_width;
    __u32 user_height;

    double horizontalFov;
    double verticalFov;
    yarp::os::Property intrinsic;
    bool dual;

    io_method io;
    int fps;

    // Temporary step required for leopard python camera only
    // The image has to be converted into standard bayer format
    // in order to be correctly converted into rgb
    unsigned char* raw_image;
    unsigned int raw_image_size;

    // this is a helper pointer set either to raw_image or src_image,
    // depending if the source is custom or standard
    unsigned char* read_image;

    // src image: standard image type read from the camera sensor
    // used as input for color conversion
    unsigned char* src_image;
    unsigned int src_image_size;

    // RGB image after color conversion. The size may not be the one
    // requested by the user and a rescaling may be required afterwards
    unsigned char* dst_image_rgb;
    unsigned int dst_image_size_rgb;

    // OpenCV object to perform the final rescaling of the image
    cv::Mat outMat; // OpenCV output

    yarp::sig::VectorOf<yarp::dev::CameraConfig> configurations;
    bool flip;

    unsigned int n_buffers;
    struct buffer* buffers;
    struct v4l2_format src_fmt;
    struct v4l2_format dst_fmt;
    struct v4l2_requestbuffers req;
    size_t pixelType;
    supported_cams camModel; // In case some camera requires custom procedure
} Video_params;


/*
 *  Device handling
 */

class V4L_camera :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameGrabberRgb,
        public yarp::dev::IFrameGrabber,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IPreciselyTimed,
        public yarp::os::PeriodicThread,
        public yarp::dev::IRgbVisualParams
{
public:
    V4L_camera();

    // DeviceDriver Interface
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    yarp::os::Stamp getLastInputStamp() override;

    // IFrameGrabberRgb    Interface
    bool getRgbBuffer(unsigned char* buffer) override;

    // IFrameGrabber Interface
    bool getRawBuffer(unsigned char* buffer) override;
    int getRawBufferSize() override;

    /**
     * Return the height of each frame.
     * @return image height
     */
    int height() const override;

    /**
     * Return the width of each frame.
     * @return image width
     */
    int width() const override;

    /*Implementation of IRgbVisualParams interface*/
    int getRgbHeight() override;
    int getRgbWidth() override;
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations) override;
    bool getRgbResolution(int& width, int& height) override;
    bool setRgbResolution(int width, int height) override;
    bool getRgbFOV(double& horizontalFov, double& verticalFov) override;
    bool setRgbFOV(double horizontalFov, double verticalFov) override;
    bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    bool getRgbMirroring(bool& mirror) override;
    bool setRgbMirroring(bool mirror) override;


    /* Implementation of IFrameGrabberControls interface */
    bool getCameraDescription(CameraDescriptor* camera) override;
    bool hasFeature(int feature, bool* hasFeature) override;
    bool setFeature(int feature, double value) override;
    bool getFeature(int feature, double* value) override;
    bool setFeature(int feature, double value1, double value2) override;
    bool getFeature(int feature, double* value1, double* value2) override;
    bool hasOnOff(int feature, bool* _hasOnOff) override;
    bool setActive(int feature, bool onoff) override;
    bool getActive(int feature, bool* _isActive) override;
    bool hasAuto(int feature, bool* _hasAuto) override;
    bool hasManual(int feature, bool* _hasManual) override;
    bool hasOnePush(int feature, bool* _hasOnePush) override;
    bool setMode(int feature, FeatureMode mode) override;
    bool getMode(int feature, FeatureMode* mode) override;
    bool setOnePush(int feature) override;

private:
    bool verbose;
    v4lconvert_data* _v4lconvert_data;
    bool use_exposure_absolute;

    yarp::os::Stamp timeStamp;
    Video_params param;
    yarp::os::Semaphore mutex;
    bool configFx, configFy;
    bool configPPx, configPPy;
    bool configRet, configDistM;
    bool configIntrins;
    bool configured;
    bool doCropping;
    bool isActive_vector[YARP_FEATURE_NUMBER_OF];
    double timeStart, timeTot, timeNow, timeElapsed;
    int myCounter;
    int frameCounter;

    std::map<std::string, supported_cams> camMap;

    bool fromConfig(yarp::os::Searchable& config);

    void populateConfigurations();

    int convertV4L_to_YARP_format(int format);

    double checkDouble(yarp::os::Searchable& config, const char* key);

    // initialize device
    bool deviceInit();

    // de-initialize device
    bool deviceUninit();

    void captureStart();
    void captureStop();

    bool threadInit() override;
    void run() override;
    void threadRelease() override;


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
    bool frameRead();

    bool full_FrameRead(void);

    /*
    * This function is intended to perform custom code to adapt
    * non standard pixel types to a standard one, in order to
    * use standard conversion libraries afterward.
    */
    void imagePreProcess();

    /*
    * This function is intended to perform all the required conversions
    * from the camera pixel type to the RGB one and eventually rescaling
    * to size requested by the user.
    */
    void imageProcess();

    int getfd();

private:
    // low level stuff - all functions here uses the Linux V4L specific definitions
    /**
     *    Do ioctl and retry if error was EINTR ("A signal was caught during the ioctl() operation."). Parameters are the same as on ioctl.
     *
     *    \param fd file descriptor
     *    \param request request
     *    \param argp argument
     *    \returns result from ioctl
     */
    int xioctl(int fd, int request, void* argp);

    int convertYARP_to_V4L(int feature);
    void enumerate_menu();
    bool enumerate_controls();
    bool check_V4L2_control(uint32_t id);
    bool set_V4L2_control(u_int32_t id, double value, bool verbatim = false);
    double get_V4L2_control(uint32_t id, bool verbatim = false); // verbatim = do not convert value, for enum types

    double toEpochOffset;

    // leopard de-bayer test
    int bit_shift;
    int bit_bayer;
    int pixel_fmt_leo;
};

#endif // YARP_DEVICE_USBCAMERA_LINUX_V4L_CAMERA_H
