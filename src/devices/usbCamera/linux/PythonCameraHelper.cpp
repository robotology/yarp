/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
//# @author Luca Tricerri <luca.tricerri@iit.it>

#include "PythonCameraHelper.h"

#include "xilinx-v4l2-controls.h"
#include <chrono>
#include <fcntl.h>
#include <libudev.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>
#include <linux/videodev2.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

void PythonCameraHelper::openPipeline(void)
{
    log("openPipeline", Severity::info);

    // Open main device
    int fd = interface_.open_c(mediaName_.c_str(), O_RDWR);
    if (fd == -1) {
        log("ERROR-cannot open media dev");
        exit(EXIT_FAILURE);
    }
    log("open:" + mediaName_, Severity::info);


    struct udev* udev;
    udev = udev_new();
    if (udev == NULL) {
        log("ERROR-cannot open udev", Severity::error);
        exit(EXIT_FAILURE);
    }

    // find subdevice
    struct media_entity_desc info;
    int subdeviceIndex = 0;
    for (int id = 0;; id = info.id) {
        memset(&info, 0, sizeof(info));
        info.id = id | MEDIA_ENT_ID_FLAG_NEXT;

        int ret = ioctl(fd, MEDIA_IOC_ENUM_ENTITIES, &info);
        if (ret < 0) {
            ret = errno != EINVAL ? -errno : 0;
            log("WARNING-cannot open device not media");
            break;
        }
        log("found entity name:" + std::string(info.name));

        dev_t devnum = makedev(info.v4l.major, info.v4l.minor);
        struct udev_device* device;
        device = udev_device_new_from_devnum(udev, 'c', devnum);
        if (device == nullptr) {
            udev_device_unref(device);
            continue;
        }

        const char* deviceName;
        deviceName = udev_device_get_devnode(device);

        // Open main subdevice
        if ((std::strcmp(info.name, pipelineVideoName) == 0) || (std::strcmp(info.name, pipelineDummyName) == 0)) {
            mainSubdeviceFd_ = open(deviceName, O_RDWR /* required */ | O_NONBLOCK, 0);
            if (mainSubdeviceFd_ == -1) {
                log("ERROR-cannot open device:" + std::string(deviceName), Severity::error);
                exit(EXIT_FAILURE);
            }
            log("open no pipeline:" + std::string(deviceName));
        } else {
            // Open other subdevice
            /*
       * If a python camera is found in pipeline, then that's the
       * source. If only a TPG is present, then it's the source.
       * In case both are found, stick to camera
       */
            if (std::strcmp(info.name, pipelinePythonName) == 0) {
                if (sourceSubDeviceIndex1_ == -1)
                    sourceSubDeviceIndex1_ = subdeviceIndex;
                else
                    sourceSubDeviceIndex2_ = subdeviceIndex;
            } else if (std::strstr(info.name, pipelineTpgName)) {
                tpgIndex_ = subdeviceIndex;
            } else if (std::strstr(info.name, pipelineCscName)) {
                cscIndex_ = subdeviceIndex;
            } else if (std::strstr(info.name, pipelineImgfusionName)) {
                imgfusionIndex_ = subdeviceIndex;
            } else if (std::strstr(info.name, pipelinePacket32Name)) {
                packet32Index_ = subdeviceIndex;
            } else if (std::strcmp(info.name, pipelineRxifName) == 0) {
                if (rxif1Index_ == -1)
                    rxif1Index_ = subdeviceIndex;
                else
                    rxif2Index_ = subdeviceIndex;
            }
            pipelineSubdeviceFd_[subdeviceIndex] = open(deviceName, O_RDWR /* required */ | O_NONBLOCK, 0);
            if (pipelineSubdeviceFd_[subdeviceIndex] == -1) {
                log("ERROR-cannot open device:" + std::string(deviceName), Severity::error);
                exit(EXIT_FAILURE);
            }
            std::stringstream ss;
            ss << "Open pipeline devicename:" << std::string(deviceName) << " info name:" << info.name << " fd:" << pipelineSubdeviceFd_[subdeviceIndex] << std::endl;
            log(ss.str(), Severity::debug);
            subdeviceIndex++;
        }

        udev_device_unref(device);
    }
    if (mainSubdeviceFd_ == -1) {
        log("ERROR-Cannot find main pipe V4L2 device", Severity::error);
        exit(EXIT_FAILURE);
    }
    if (sourceSubDeviceIndex1_ == -1) {
        log("ERROR-Cannot find source subdev", Severity::error);
        exit(EXIT_FAILURE);
    }
}

void PythonCameraHelper::setSubDevFormat(int width, int height)
{
    int i, j, n;
    struct v4l2_subdev_format fmt;

    for (i = 0; pipelineSubdeviceFd_[i] != -1; i++) {
        if (i == imgfusionIndex_)
            n = 3;
        else
            n = 2;
        for (j = 0; j < n; j++) {
            CLEAR(fmt);
            fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
            fmt.pad = j;
            if (-1 == xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_G_FMT, &fmt)) {
                std::stringstream ss;
                ss << "VIDIOC_SUBDEV_G_FMT. subdev" << i << "pad" << j;
                log(ss.str(), Severity::error);
                exit(EXIT_FAILURE);
            }

            fmt.format.width = width;
            fmt.format.height = height;

            /* if yuv is required, then set that on the source PAD of VPSS */
            if ((i == cscIndex_) && (j == 1) && spaceColor_ == SpaceColor::yuv) {
                fmt.format.code = MEDIA_BUS_FMT_UYVY8_1X16;
            }

            /* csc, when there is an imgfusion IP receives 2x width frames */
            if ((imgfusionIndex_ != -1) && (i == cscIndex_))
                fmt.format.width *= 2;
            /* packet32, when there is an imgfusion IP receives 2x width frames */
            if ((imgfusionIndex_ != -1) && (i == packet32Index_))
                fmt.format.width *= 2;
            /* tpg when there is an imgfusion IP receives 2x width frames */
            if ((imgfusionIndex_ != -1) && (i == tpgIndex_))
                fmt.format.width *= 2;

            /* imgfusion source pad has 2* width */
            if (j == 2)
                fmt.format.width *= 2;

            {
                std::stringstream ss;
                ss << "subdev idx:" << i << " pad" << j << " setting format:" << fmt.format.width << ":" << fmt.format.height;
                log(ss.str(), Severity::debug);
            }
            if (-1 == xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_S_FMT, &fmt)) {
                std::stringstream ss;
                ss << "VIDIOC_SUBDEV_S_FMT. subdev" << i << "pad" << j;
                log(ss.str(), Severity::error);
                exit(EXIT_FAILURE);
            }
            if ((i == sourceSubDeviceIndex1_) || (i == sourceSubDeviceIndex2_))
                break; /* only one pad */
        }
    }
}

void PythonCameraHelper::setFormat()
{
    struct v4l2_format fmt;
    // todo check dimensions correctness
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (forceFormatProperty_ || cropEnabledProperty_) {
        fmt.fmt.pix.width = cropEnabledProperty_ ? cropWidth_ : nativeWidth_;
        fmt.fmt.pix.height = cropEnabledProperty_ ? cropHeight_ : nativeHeight_;

        if (subsamplingEnabledProperty_) {
            fmt.fmt.pix.width /= 2;
            fmt.fmt.pix.height /= 2;
        }

        if (spaceColor_ == SpaceColor::grgb)
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB8;
        else if (spaceColor_ == SpaceColor::yuv)
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        else if (spaceColor_ == SpaceColor::rgb)
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

        fmt.fmt.pix.field = 1;
        fmt.fmt.pix.colorspace = 8;

        setSubDevFormat(fmt.fmt.pix.width, fmt.fmt.pix.height);

        if (imgfusionIndex_ != -1)
            fmt.fmt.pix.width *= 2;

        if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_S_FMT, &fmt))
            exit(EXIT_FAILURE);

        /* Note VIDIOC_S_FMT may change width and height. */
        return;
    }
    /* Preserve original settings as set by v4l2-ctl for example */
    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_G_FMT, &fmt))
        exit(EXIT_FAILURE);
}

void PythonCameraHelper::crop(int top, int left, int w, int h, int mytry)
{

    cropCheck();

    log("crop is" + std::string(cropEnabledProperty_ ? "ENABLED" : "DISABLED"));
    if (!cropEnabledProperty_)
        return;

    struct v4l2_subdev_crop _crop;

    _crop.rect.left = left;
    _crop.rect.top = top;
    _crop.rect.width = w;
    _crop.rect.height = h;

    _crop.which = mytry ? V4L2_SUBDEV_FORMAT_TRY : V4L2_SUBDEV_FORMAT_ACTIVE;
    _crop.pad = 0;

    if (-1 == xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_SUBDEV_S_CROP, &_crop))
        exit(EXIT_FAILURE);
    if (sourceSubDeviceIndex2_ != -1) {
        if (-1 == xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_], VIDIOC_SUBDEV_S_CROP, &_crop))
            exit(EXIT_FAILURE);
    }
}

void PythonCameraHelper::setSubsampling(void)
{
    log("subsampling is" + std::string(subsamplingEnabledProperty_ ? "ENABLED" : "DISABLED"));
    int subSamplingValue = 0;
    if (subsamplingEnabledProperty_)
        subSamplingValue = 1;

    log("setSubsampling");
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_XILINX_PYTHON1300_SUBSAMPLING;
    ctrl.value = subSamplingValue;
    if (-1 == xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_S_CTRL, &ctrl)) {
        log("ERROR-setSubsampling", Severity::error);
        exit(EXIT_FAILURE);
    }

    if (sourceSubDeviceIndex2_ != -1) {
        if (-1 == xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_], VIDIOC_S_CTRL, &ctrl)) {
            log("ERROR-setSubsampling", Severity::error);
            exit(EXIT_FAILURE);
        }
    }

    ctrl.id = V4L2_CID_XILINX_PYTHON1300_RXIF_REMAPPER_MODE;
    ctrl.value = subSamplingValue;
    if (-1 == xioctl(pipelineSubdeviceFd_[rxif1Index_], VIDIOC_S_CTRL, &ctrl)) {
        log("ERROR-setSubsampling remapper", Severity::error);
        exit(EXIT_FAILURE);
    }

    if (rxif2Index_ != -1) {
        if (-1 == xioctl(pipelineSubdeviceFd_[rxif2Index_], VIDIOC_S_CTRL, &ctrl)) {
            log("ERROR-setSubsampling remapper2", Severity::error);
            exit(EXIT_FAILURE);
        }
    }
}

bool PythonCameraHelper::checkDevice(int mainSubdeviceFd)
{
    struct v4l2_capability cap;
    if (-1 == xioctl(mainSubdeviceFd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            log("ERROR-initDevice:device is no V4L2 device", Severity::error);
            exit(EXIT_FAILURE);
        } else {
            exit(EXIT_FAILURE);
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        log("ERROR-initDevice:device is no video capture device", Severity::error);
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        log("ERROR-device does not support streaming i/o", Severity::error);
        exit(EXIT_FAILURE);
    }

    return true;
}

void PythonCameraHelper::initDevice(void)
{
    log("initDevice");

    checkDevice(mainSubdeviceFd_);
    setSubsampling();
    setFormat();
    crop(cropTop_, cropLeft_, cropWidth_, cropHeight_, 0);
    initMmap();
}

bool PythonCameraHelper::cropCheck()
{
    struct v4l2_cropcap cropcap;
    struct v4l2_crop tmpCrop;
    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 == xioctl(mainSubdeviceFd_, VIDIOC_CROPCAP, &cropcap)) {
        tmpCrop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        tmpCrop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_S_CROP, &tmpCrop)) {
            switch (errno) {
            case EINVAL:
                log("ERROR-cropping not supported", Severity::error);
                break;
            default:
                log("ERROR-cropping", Severity::error);
                break;
            }
        }
    } else {
        log("WARNING-cropping-2");
    }
    return true;
}

int PythonCameraHelper::xioctl(int fh, int request, void* arg)
{
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void PythonCameraHelper::initMmap(void)
{
    log("initMmap");
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = requestBufferNumber_;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            log("ERROR-device does not support memmap", Severity::error);
            exit(EXIT_FAILURE);
        } else {
            log("ERROR-device does not support memmap", Severity::error);
            exit(EXIT_FAILURE);
        }
    }

    if (req.count < 1) {
        log("ERROR-Insufficient buffer memory on", Severity::error);
        exit(EXIT_FAILURE);
    }

    for (unsigned int currentUsedBufferIndex = 0;
         currentUsedBufferIndex < req.count;
         ++currentUsedBufferIndex) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = currentUsedBufferIndex;

        if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_QUERYBUF, &buf))
            exit(EXIT_FAILURE);

        mMapBuffers_[currentUsedBufferIndex].length = buf.length;
        mMapBuffers_[currentUsedBufferIndex].start = mmap(NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, mainSubdeviceFd_, buf.m.offset);

        if (MAP_FAILED == mMapBuffers_[currentUsedBufferIndex].start)
            exit(EXIT_FAILURE);
    }
}

void PythonCameraHelper::startCapturing()
{
    log("startCapturing");
    enum v4l2_buf_type type;

    for (size_t i = 0; i < requestBufferNumber_; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf)) {
            log("ERROR-VIDIOC_QBUF", Severity::error);
            exit(EXIT_FAILURE);
        }
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_STREAMON, &type)) {
        log("ERROR-VIDIOC_STREAMON", Severity::error);
        exit(EXIT_FAILURE);
    }
}

void PythonCameraHelper::step()
{

    static int seq = 0;
    static int sequence = 0;

    fd_set fds;
    struct timeval tv
    {
        80, 0
    }; //Timeout
    int ret;

    FD_ZERO(&fds);
    FD_SET(mainSubdeviceFd_, &fds);

    ret = select(mainSubdeviceFd_ + 1, &fds, NULL, NULL, &tv);

    if (-1 == ret) {
        if (EINTR == errno)
            return;
        log("ERROR-select", Severity::error);
        exit(EXIT_FAILURE);
    }

    if (0 == ret) {
        log("ERROR-select timeout", Severity::error);
        exit(EXIT_FAILURE);
    }

    seq = readFrame();
    if (seq != sequence++) {
        log("WANNING-dropped frame..", Severity::warning);
        sequence = seq + 1;
    }
    if (seq) {
        fpsCalculus();
    }
}

int PythonCameraHelper::readFrame(void)
{
    struct v4l2_buffer buf;
    int seq = 1;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    Locker locker(*this);
    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
        case EAGAIN:

            log("ERROR-VIDIOC_DQBUF eagain", Severity::error);
            exit(EXIT_FAILURE);

        case EIO:
        default:
            log("ERROR-VIDIOC_DQBUF", Severity::error);
            exit(EXIT_FAILURE);
        }
    }

    if (buf.index >= requestBufferNumber_) {
        log("ERROR-readframe", Severity::error);
        exit(EXIT_FAILURE);
    }

    if (buf.flags & V4L2_BUF_FLAG_ERROR) {
        log("ERROR-V4L2_BUF_FLAG_ERROR", Severity::error);
        exit(EXIT_FAILURE);
    }

    seq = buf.sequence;
    processImage(mMapBuffers_[buf.index].start, buf.bytesused);

    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf)) {
        log("ERROR-VIDIOC_QBUF", Severity::error);
        exit(EXIT_FAILURE);
    }

    return seq;
}

void PythonCameraHelper::processImage(const void* p, int size)
{
    if (injectedProcessImage_ == nullptr)
        return;
    injectedProcessImage_(p, size);
}

void PythonCameraHelper::log(const std::string& toBeLogged, Severity severity)
{
    if (logOnFile_)
        fs << toBeLogged;

    if (log_ == nullptr)
        return;
    log_(toBeLogged, severity);
}

void PythonCameraHelper::closeAll()
{
    stopCapturing();
    unInitDevice();
    closePipeline();
}

void PythonCameraHelper::unInitDevice()
{
    log("uninit_device");
    unsigned int i;

    for (i = 0; i < requestBufferNumber_; ++i)
        if (-1 == munmap(mMapBuffers_[i].start, mMapBuffers_[i].length)) {
            log("ERROR-munmap", Severity::error);
            exit(EXIT_FAILURE);
        }
}

void PythonCameraHelper::stopCapturing()
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(mainSubdeviceFd_, VIDIOC_STREAMOFF, &type)) {
        log("ERROR-VIDIOC_STREAMOFF", Severity::error);
        exit(EXIT_FAILURE);
    }
}

void PythonCameraHelper::closePipeline()
{
    int i;

    for (i = 0; pipelineSubdeviceFd_[i] != -1; i++)
        if (-1 == close(pipelineSubdeviceFd_[i])) {
            log("ERROR-close pipeline", Severity::error);
            exit(EXIT_FAILURE);
        }
}

void PythonCameraHelper::fpsCalculus()
{
    static unsigned int frames = 0;
    static std::chrono::steady_clock::time_point current;
    static std::chrono::steady_clock::time_point prev = std::chrono::steady_clock::now();

    current = std::chrono::steady_clock::now();
    frames++;
    unsigned int time_delta = std::chrono::duration_cast<std::chrono::milliseconds>(current - prev)
                                  .count();

    if (time_delta >= 1000) {
        fps_ = (((double)frames / (double)time_delta) * 1000.0);
        //log("FPS:" << fps_);
        prev = current;
        frames = 0;
    }
}

double PythonCameraHelper::getCurrentFps() const
{
    return fps_;
}

void PythonCameraHelper::setInjectedProcess(std::function<void(const void*, int)> toinJect)
{
    injectedProcessImage_ = toinJect;
}
void PythonCameraHelper::setInjectedUnlock(std::function<void()> toinJect)
{
    unlock_ = toinJect;
}
void PythonCameraHelper::setInjectedLock(std::function<void()> toinJect)
{
    lock_ = toinJect;
}

void PythonCameraHelper::setSubsamplingProperty(bool value)
{
    subsamplingEnabledProperty_ = value;
}

void PythonCameraHelper::openAll()
{
    openPipeline();
    initDevice();
    setDefaultControl();
    startCapturing();
}

void PythonCameraHelper::setInjectedLog(std::function<void(const std::string&, Severity)> toinJect)
{
    log_ = toinJect;
}

void PythonCameraHelper::setFileLog(bool value)
{
    logOnFile_ = value;
}

bool PythonCameraHelper::setControl(uint32_t v4lCtrl, double value)
{
    v4lCtrl = remapControl(v4lCtrl);

    if (!hasControl(v4lCtrl)) {
        std::stringstream ss;
        ss << "setControl Missing ctr id:" << v4lCtrl << std::endl;
        log(ss.str(), Severity::error);
        return false;
    }

    std::stringstream ss;
    ss << "setControl main for:" << v4lCtrl;
    log(ss.str(), Severity::debug);
    switch (v4lCtrl) {
    case V4L2_CID_GAIN:
    case V4L2_CID_BRIGHTNESS:
    case 0x0098cc03: //EXT_TRIGGER
        setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_], value);
        setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_], value);
        return true;

    case 0x0098c9a3: //V4L2_CID_RED_BALANCE
    case 0x0098c9a5: //V4L2_CID_BLUE_BALANCE
        setControl(v4lCtrl, mainSubdeviceFd_, value);
        return true;
    default:
        return false;
    }
    return false;
}

bool PythonCameraHelper::setControl(uint32_t v4lCtrl, int fd, double value)
{
    if (value < 0) {
        log("setControl wrong value control", Severity::error);
        return false;
    }

    v4lCtrl = remapControl(v4lCtrl);


    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = v4lCtrl;

    if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            std::stringstream ss;
            ss << "Cannot setControl1 value:" << value << " fd:" << fd << std::endl;
        } else {

            std::stringstream ss;
            ss << "Cannot setControl2 value:" << value << " fd:" << fd << std::endl;
            log(ss.str(), Severity::error);
        }
        return false;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        log("Cannot setControl is disabled", Severity::error);
        return false;
    }
    memset(&control, 0, sizeof(control));
    control.id = v4lCtrl;
    //control.value = value;
    control.value = (int32_t)(value * (queryctrl.maximum - queryctrl.minimum) + queryctrl.minimum);

    //Do set
    if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
        log("Cannot setControl3", Severity::error);
        return false;
    }

    std::stringstream ss;
    ss << "SetControl done --> Ctrl name:" << queryctrl.name << " Ctrl value:" << control.value << std::endl;
    log(ss.str(), Severity::debug);
    return true;
}

double PythonCameraHelper::getControl(uint32_t v4lCtrl)
{
    v4lCtrl = remapControl(v4lCtrl);

    if (!hasControl(v4lCtrl)) {
        std::stringstream ss;
        ss << "getControl Missing ctr id:" << v4lCtrl << std::endl;
        log(ss.str(), Severity::error);
        return false;
    }

    switch (v4lCtrl) {
    case V4L2_CID_GAIN:
    case V4L2_CID_BRIGHTNESS:
    case 0x0098cc03: //EXT_TRIGGER
    {
        double left = getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_]);
        double right = getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_]);
        if (left != right) {
            log("getControl left and right different", Severity::error);
        }
        return left;
    }
    case 0x0098c9a3: //V4L2_CID_RED_BALANCE
    case 0x0098c9a5: //V4L2_CID_BLUE_BALANCE
        return getControl(v4lCtrl, mainSubdeviceFd_);
    default:
        return -1.0;
    }
}


double PythonCameraHelper::getControl(uint32_t v4lCtrl, int fd)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&control, 0, sizeof(control));
    memset(&queryctrl, 0, sizeof(queryctrl));

    control.id = v4lCtrl;
    queryctrl.id = v4lCtrl;

    if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            log("getControl VIDIOC_QUERYCTRL", Severity::error);
        }
        return -1.0;
    }

    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        log("Control is disabled", Severity::error);
    } else {
        if (-1 == ioctl(fd, VIDIOC_G_CTRL, &control)) {
            log("getControl VIDIOC_G_CTRL", Severity::error);
            return -1.0;
        }
    }
    return (double)(control.value - queryctrl.minimum) / (queryctrl.maximum - queryctrl.minimum);
}

bool PythonCameraHelper::hasControl(uint32_t v4lCtrl)
{
    v4lCtrl = remapControl(v4lCtrl);

    std::stringstream ss;
    ss << "hascontrol for:" << v4lCtrl;
    log(ss.str(), Severity::debug);
    switch (v4lCtrl) {
    case V4L2_CID_GAIN:
    case V4L2_CID_BRIGHTNESS:
    case 0x0098c9a3: //V4L2_CID_RED_BALANCE
    case 0x0098c9a5: //V4L2_CID_BLUE_BALANCE
    case 0x0098cc03: //EXT_TRIGGER
        return true;
    default:
        return false;
    }
    return false;
}

bool PythonCameraHelper::hasAutoControl(uint32_t v4lCtrl)
{
    v4lCtrl = remapControl(v4lCtrl);
    std::stringstream ss;
    ss << "hasauto for:" << v4lCtrl;
    log(ss.str(), Severity::debug);
    return false;
}

bool PythonCameraHelper::checkControl(uint32_t v4lCtrl)
{
    v4lCtrl = remapControl(v4lCtrl);

    std::stringstream ss;
    ss << "checkCcontrol for:" << v4lCtrl;
    log(ss.str(), Severity::debug);

    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&control, 0, sizeof(control));
    memset(&queryctrl, 0, sizeof(queryctrl));

    control.id = v4lCtrl;
    queryctrl.id = v4lCtrl;

    if (-1 == ioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            log("checkControl VIDIOC_QUERYCTRL", Severity::error);
        }
        return false;
    }
    return true;
}

bool PythonCameraHelper::setDefaultControl()
{
    setControl(0x0098cc03, 1); //ext_trigger
    return true;
}

uint32_t PythonCameraHelper::remapControl(uint32_t v4lCtr)
{
    uint32_t out = v4lCtr;
    switch (v4lCtr) {
    case V4L2_CID_RED_BALANCE:
        out = 0x0098c9a3;
        log("remap RED BALANCE", Severity::debug);
        break;
    case V4L2_CID_BLUE_BALANCE:
        out = 0x0098c9a5;
        log("remap BLUE BALANCE", Severity::debug);
        break;
    default:
        break;
    }
    return out;
}