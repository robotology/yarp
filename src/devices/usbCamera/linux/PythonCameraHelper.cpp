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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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

PythonCameraHelper::PythonCameraHelper(InterfaceForCFunction *interfaceC) {
  Log(*this, Severity::debug) << "::PythonCameraHelper";
  if (interfaceC == nullptr) {
    interfaceC = new InterfaceForCFunction();
  }
  interface_ = interfaceC;
}

PythonCameraHelper::~PythonCameraHelper() {
  Log(*this, Severity::debug) << "::~PythonCameraHelper";
  if (interface_ != nullptr) {
    // delete interface_;
  }
}

bool PythonCameraHelper::openPipeline() {
  Log(*this, Severity::debug) << "::openPipeline";

  // Open main device
  int fd = interface_->open_c(mediaName_, O_RDWR);
  if (fd == -1) {
    Log(*this, Severity::error) << "ERROR-cannot open media dev";
    return false;
  }

  struct udev *udev;
  udev = interface_->udev_new_c();
  if (udev == nullptr) {
    Log(*this, Severity::error) << "ERROR-cannot open udev";
    return false;
  }

  // find subdevice
  struct media_entity_desc info;
  int subdeviceIndex = 0;
  for (int id = 0;; id = info.id) {
    memset(&info, 0, sizeof(info));
    info.id = id | MEDIA_ENT_ID_FLAG_NEXT;

    int ret = interface_->ioctl_c(fd, MEDIA_IOC_ENUM_ENTITIES, &info);
    if (ret < 0) {
      ret = errno != EINVAL ? -errno : 0;
      Log(*this, Severity::warning) << "WARNING-cannot open device not media";
      break;
    }
    Log(*this, Severity::debug)
        << "found entity name:" << std::string(info.name);

    dev_t devnum = interface_->makedev_c(info.v4l.major, info.v4l.minor);
    struct udev_device *device;
    device = interface_->udev_device_new_from_devnum_c(udev, 'c', devnum);
    if (device == nullptr) {
      udev_device_unref(device);
      continue;
    }

    const char *deviceName;
    deviceName = interface_->udev_device_get_devnode_c(device);

    // Open main subdevice
    if ((std::strcmp(info.name, pipelineVideoName) == 0)) {
      mainSubdeviceFd_ = interface_->open_c(deviceName, O_RDWR | O_NONBLOCK, 0);
      if (mainSubdeviceFd_ == -1) {
        Log(*this, Severity::error)
            << "ERROR-cannot open device:" << std::string(deviceName);
        return false;
      }
      Log(*this, Severity::debug)
          << "open no pipeline:" << std::string(deviceName);
    } else {
      // Open other subdevice
      /*
       * If a python camera is found in pipeline, then that's the
       * source. If only a TPG is present, then it's the source.
       * In case both are found, stick to camera
       */
      if (std::strcmp(info.name, pipelinePythonName) == 0) {
        if (sourceSubDeviceIndex1_ == -1) {
          sourceSubDeviceIndex1_ = subdeviceIndex;
        } else {
          sourceSubDeviceIndex2_ = subdeviceIndex;
        }
      } else if (std::strstr(info.name, pipelineTpgName)) {
        tpgIndex_ = subdeviceIndex;
      } else if (std::strstr(info.name, pipelineCscName)) {
        cscIndex_ = subdeviceIndex;
      } else if (std::strstr(info.name, pipelineImgfusionName)) {
        imgfusionIndex_ = subdeviceIndex;
      } else if (std::strstr(info.name, pipelinePacket32Name)) {
        packet32Index_ = subdeviceIndex;
      } else if (std::strcmp(info.name, pipelineRxifName) == 0) {
        if (rxif1Index_ == -1) {
          rxif1Index_ = subdeviceIndex;
        } else {
          rxif2Index_ = subdeviceIndex;
        }
      }
      pipelineSubdeviceFd_[subdeviceIndex] =
          interface_->open_c(deviceName, O_RDWR | O_NONBLOCK, 0);
      if (pipelineSubdeviceFd_[subdeviceIndex] == -1) {
        Log(*this, Severity::error)
            << "ERROR-cannot open device:" << std::string(deviceName);
        return false;
      }
      Log(*this, Severity::debug)
          << "Open pipeline devicename:" << std::string(deviceName)
          << " info name:" << info.name
          << " fd:" << pipelineSubdeviceFd_[subdeviceIndex]
          << " index:" << subdeviceIndex;
      subdeviceIndex++;
    }
    interface_->udev_device_unref_c(device);
  }
  Log(*this, Severity::debug) << "open:" << mediaName_;
  return checkIndex();
}

bool PythonCameraHelper::checkIndex() {
  if (mainSubdeviceFd_ == -1) {
    Log(*this, Severity::error) << "Cannot find main pipe V4L2 device";
    return false;
  }
  if (sourceSubDeviceIndex1_ == -1) {
    Log(*this, Severity::error) << "Cannot find source subdev1";
    return false;
  }
  if (sourceSubDeviceIndex2_ == -1) {
    Log(*this, Severity::error) << "Cannot find source subdev2";
    return false;
  }
  if (rxif1Index_ == -1) {
    Log(*this, Severity::error) << "Cannot find rxif1Index";
    return false;
  }
  if (rxif2Index_ == -1) {
    Log(*this, Severity::error) << "Cannot find rxif2Index";
    return false;
  }
  if (cscIndex_ == -1) {
    Log(*this, Severity::error) << "Cannot find cscIndex";
    return false;
  }
  if (tpgIndex_ == -1) {
    Log(*this, Severity::error) << "Cannot find tpgIndex";
    return false;
  }
  if (imgfusionIndex_ == -1) {
    Log(*this, Severity::error) << "Cannot find imgfusionIndex";
    return false;
  }
  if (packet32Index_ == -1) {
    Log(*this, Severity::error) << "Cannot find packet32Index";
    return false;
  }
  return true;
}

bool PythonCameraHelper::setSubDevFormat(int width, int height) {
  int i;
  int j;
  int n;
  struct v4l2_subdev_format fmt;

  for (i = 0; pipelineSubdeviceFd_[i] != -1; i++) {
    if (i == imgfusionIndex_) {
      n = 3;
    } else {
      n = 2;
    }
    for (j = 0; j < n; j++) {
      CLEAR(fmt);
      fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
      fmt.pad = j;
      if (-1 == interface_->xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_G_FMT,
                                   &fmt)) {
        Log(*this, Severity::error)
            << "VIDIOC_SUBDEV_G_FMT. subdev" << i << "pad" << j;
        return false;
      }

      fmt.format.width = width;
      fmt.format.height = height;

      /* if yuv is required, then set that on the source PAD of VPSS */
      if ((i == cscIndex_) && (j == 1) && spaceColor_ == SpaceColor::yuv) {
        fmt.format.code = MEDIA_BUS_FMT_UYVY8_1X16;
      }

      /* csc, when there is an imgfusion IP receives 2x width frames */
      if ((imgfusionIndex_ != -1) && (i == cscIndex_)) {
        fmt.format.width *= 2;
      }
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
        Log(*this, Severity::debug) << "subdev idx:" << i << " pad" << j
                                    << " setting format:" << fmt.format.width
                                    << ":" << fmt.format.height;
      }
      if (-1 == interface_->xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_S_FMT,
                                   &fmt)) {
        Log(*this, Severity::error)
            << "VIDIOC_SUBDEV_S_FMT. subdev" << i << "pad" << j;

        return false;
      }
      if ((i == sourceSubDeviceIndex1_) || (i == sourceSubDeviceIndex2_)) {
        break; /* only one pad */
      }
    }
  }
  return true;
}

bool PythonCameraHelper::setFormat() {
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

    if (!setSubDevFormat(fmt.fmt.pix.width, fmt.fmt.pix.height)) {
      return false;
    }

    if (imgfusionIndex_ != -1) {
      fmt.fmt.pix.width *= 2;
    }

    if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_S_FMT, &fmt))
      return false;

    /* Note VIDIOC_S_FMT may change width and height. */
    return true;
  }
  /* Preserve original settings as set by v4l2-ctl for example */
  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_G_FMT, &fmt))
    return false;
  return true;
}

bool PythonCameraHelper::crop(int top, int left, int w, int h, int mytry) {

  cropCheck();

  Log(*this, Severity::debug)
      << "crop is"
      << std::string(cropEnabledProperty_ ? "ENABLED" : "DISABLED");
  if (!cropEnabledProperty_)
    return true;

  struct v4l2_subdev_crop _crop;

  _crop.rect.left = left;
  _crop.rect.top = top;
  _crop.rect.width = w;
  _crop.rect.height = h;

  _crop.which = mytry ? V4L2_SUBDEV_FORMAT_TRY : V4L2_SUBDEV_FORMAT_ACTIVE;
  _crop.pad = 0;

  if (-1 == interface_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_],
                               VIDIOC_SUBDEV_S_CROP, &_crop)) {
    return false;
  }
  if (sourceSubDeviceIndex2_ != -1) {
    if (-1 == interface_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_],
                                 VIDIOC_SUBDEV_S_CROP, &_crop)) {
      return false;
    }
  }
  return true;
}

bool PythonCameraHelper::setSubsampling() {
  Log(*this, Severity::debug)
      << "subsampling is"
      << std::string(subsamplingEnabledProperty_ ? "ENABLED" : "DISABLED");
  int subSamplingValue = 0;
  if (subsamplingEnabledProperty_) {
    subSamplingValue = 1;
  }

  Log(*this, Severity::debug) << "setSubsampling";
  struct v4l2_control ctrl;

  ctrl.id = V4L2_CID_XILINX_PYTHON1300_SUBSAMPLING;
  ctrl.value = subSamplingValue;
  if (-1 == interface_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_],
                               VIDIOC_S_CTRL, &ctrl)) {
    Log(*this, Severity::error) << "setSubsampling";
    return false;
  }

  if (sourceSubDeviceIndex2_ != -1) {
    if (-1 == interface_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_],
                                 VIDIOC_S_CTRL, &ctrl)) {
      Log(*this, Severity::error) << "setSubsampling";
      return false;
    }
  }

  ctrl.id = V4L2_CID_XILINX_PYTHON1300_RXIF_REMAPPER_MODE;
  ctrl.value = subSamplingValue;
  if (-1 == interface_->xioctl(pipelineSubdeviceFd_[rxif1Index_], VIDIOC_S_CTRL,
                               &ctrl)) {
    Log(*this, Severity::error) << "setSubsampling remapper";
    return false;
  }

  if (rxif2Index_ != -1) {
    if (-1 == interface_->xioctl(pipelineSubdeviceFd_[rxif2Index_],
                                 VIDIOC_S_CTRL, &ctrl)) {
      Log(*this, Severity::error) << "setSubsampling remapper2";
      return false;
    }
  }
  return true;
}

bool PythonCameraHelper::checkDevice(int mainSubdeviceFd) {
  struct v4l2_capability cap;
  if (-1 == interface_->xioctl(mainSubdeviceFd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      Log(*this, Severity::error) << "checkDevice:device is no V4L2 device";
    }
    return false;
  }
  /*
      if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
          log("ERROR-checkDevice:device is no video capture device",
     Severity::error); return false;
      }

      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
          log("ERROR-checkDevice-device does not support streaming i/o",
     Severity::error); return false;
      }
  */
  return true;
}

bool PythonCameraHelper::initDevice() {
  Log(*this, Severity::debug) << "initDevice";

  if (!checkDevice(mainSubdeviceFd_)) {
    return false;
  }
  if (!setSubsampling()) {
    return false;
  }
  if (!setFormat()) {
    return false;
  }
  if (!crop(cropTop_, cropLeft_, cropWidth_, cropHeight_, 0)) {
    return false;
  }
  if (!initMmap()) {
    return false;
  }

  return true;
}

bool PythonCameraHelper::cropCheck() {
  struct v4l2_cropcap cropcap;
  struct v4l2_crop tmpCrop;
  CLEAR(cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (0 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_CROPCAP, &cropcap)) {
    tmpCrop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    tmpCrop.c = cropcap.defrect; /* reset to default */

    if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_S_CROP, &tmpCrop)) {
      switch (errno) {
      case EINVAL:
        Log(*this, Severity::error) << "cropping not supported";
        break;
      default:
        Log(*this, Severity::error) << "cropping";
        break;
      }
    }
  } else {
    Log(*this, Severity::warning) << "cropping-2";
  }
  return true;
}

bool PythonCameraHelper::initMmap() {
  Log(*this, Severity::debug) << "initMmap";
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = requestBufferNumber_;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      Log(*this, Severity::error) << "device does not support memmap";
      return false;
    }
    Log(*this, Severity::error) << "device does not support memmap";
    return false;
  }

  if (req.count < 1) {
    Log(*this, Severity::error) << "Insufficient buffer memory on";
    return false;
  }

  for (unsigned int currentUsedBufferIndex = 0;
       currentUsedBufferIndex < req.count; ++currentUsedBufferIndex) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = currentUsedBufferIndex;

    if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_QUERYBUF, &buf)) {
      return false;
    }

    mMapBuffers_[currentUsedBufferIndex].length = buf.length;
    mMapBuffers_[currentUsedBufferIndex].start = interface_->mmap_c(
        nullptr /* start anywhere */, buf.length,
        PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */,
        mainSubdeviceFd_, buf.m.offset);

    if (MAP_FAILED == mMapBuffers_[currentUsedBufferIndex].start) {
      return false;
    }
  }
  return true;
}

bool PythonCameraHelper::startCapturing() {
  Log(*this, Severity::debug) << "startCapturing";
  enum v4l2_buf_type type;

  for (size_t i = 0; i < requestBufferNumber_; ++i) {
    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf)) {
      Log(*this, Severity::error) << "VIDIOC_QBUF";
      return false;
    }
  }
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_STREAMON, &type)) {
    Log(*this, Severity::error) << "VIDIOC_STREAMON";
    return false;
  }
  return true;
}

bool PythonCameraHelper::step() {

  static int seq = 0;
  static int sequence = 0;

  fd_set fds;
  struct timeval tv {
    80, 0
  }; // Timeout
  int ret;

  FD_ZERO(&fds);
  FD_SET(mainSubdeviceFd_, &fds);

  ret = select(mainSubdeviceFd_ + 1, &fds, nullptr, nullptr, &tv);

  if (-1 == ret) {
    if (EINTR == errno) {
      return false;
    }
    Log(*this, Severity::error) << "select";
    return false;
  }

  if (0 == ret) {
    Log(*this, Severity::error) << "-select timeout";
    return false;
  }

  seq = readFrame();
  if (seq == -1) {
    return false;
  }
  if (seq != sequence++) {
    Log(*this, Severity::warning) << "dropped frame..";
    sequence = seq + 1;
  }
  if (seq) {
    fpsCalculus();
  }
  return true;
}

int PythonCameraHelper::readFrame() {
  struct v4l2_buffer buf;
  int seq = 1;
  CLEAR(buf);

  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  Locker locker(*this);
  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_DQBUF, &buf)) {
    switch (errno) {
    case EAGAIN:

      Log(*this, Severity::error) << "VIDIOC_DQBUF eagain";
      return -1;

    case EIO:
    default:
      Log(*this, Severity::error) << "-VIDIOC_DQBUF";
      return -1;
    }
  }

  if (buf.index >= requestBufferNumber_) {
    Log(*this, Severity::error) << "readframe";
    return -1;
  }

  if (buf.flags & V4L2_BUF_FLAG_ERROR) {
    Log(*this, Severity::error) << "V4L2_BUF_FLAG_ERROR";
    return -1;
  }

  seq = buf.sequence;
  processImage(mMapBuffers_[buf.index].start, buf.bytesused);

  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf)) {
    Log(*this, Severity::error) << "VIDIOC_QBUF";
    return -1;
  }

  return seq;
}

void PythonCameraHelper::processImage(const void *p, int size) {
  if (injectedProcessImage_ == nullptr)
    return;
  injectedProcessImage_(p, size);
}

bool PythonCameraHelper::closeAll() {
  if (!stopCapturing())
    return false;
  if (!unInitDevice())
    return false;
  if (!closePipeline())
    return false;
  return true;
}

bool PythonCameraHelper::unInitDevice() {
  Log(*this, Severity::debug) << "uninit_device";
  unsigned int i;

  for (i = 0; i < requestBufferNumber_; ++i) {
    if (-1 == munmap(mMapBuffers_[i].start, mMapBuffers_[i].length)) {
      Log(*this, Severity::error) << "munmap";
      return false;
    }
  }
  return true;
}

bool PythonCameraHelper::stopCapturing() {
  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == interface_->xioctl(mainSubdeviceFd_, VIDIOC_STREAMOFF, &type)) {
    Log(*this, Severity::error) << "VIDIOC_STREAMOFF";
    return false;
  }
  return true;
}

bool PythonCameraHelper::closePipeline() {
  int i;

  for (i = 0; pipelineSubdeviceFd_[i] != -1; i++) {
    if (-1 == close(pipelineSubdeviceFd_[i])) {
      Log(*this, Severity::error) << "close pipeline";
      return false;
    }
  }
  return true;
}

void PythonCameraHelper::fpsCalculus() {
  static unsigned int frames = 0;
  static std::chrono::steady_clock::time_point current;
  static std::chrono::steady_clock::time_point prev =
      std::chrono::steady_clock::now();

  current = std::chrono::steady_clock::now();
  frames++;
  unsigned int timeDelta =
      std::chrono::duration_cast<std::chrono::milliseconds>(current - prev)
          .count();

  if (timeDelta >= 1000) {
    fps_ = ((static_cast<double>(frames) / static_cast<double>(timeDelta)) *
            1000.0);
    prev = current;
    frames = 0;
  }
}

double PythonCameraHelper::getCurrentFps() const { return fps_; }

void PythonCameraHelper::setInjectedProcess(
    std::function<void(const void *, int)> toinJect) {
  injectedProcessImage_ = toinJect;
}
void PythonCameraHelper::setInjectedUnlock(std::function<void()> toinJect) {
  unlock_ = toinJect;
}
void PythonCameraHelper::setInjectedLock(std::function<void()> toinJect) {
  lock_ = toinJect;
}

void PythonCameraHelper::setSubsamplingProperty(bool value) {
  subsamplingEnabledProperty_ = value;
}

bool PythonCameraHelper::openAll() {
  if (!openPipeline()) {
    return false;
  }

  if (!initDevice()) {
    return false;
  }

  if (!setDefaultControl()) {
    return false;
  }

  if (!startCapturing()) {
    return false;
  }
  return true;
}

void PythonCameraHelper::setInjectedLog(
    std::function<void(const std::string &, Severity)> toinJect) {
  log_ = toinJect;
}

bool PythonCameraHelper::setControl(uint32_t v4lCtrl, double value,
                                    bool absolute) {
  v4lCtrl = remapControl(v4lCtrl);

  if (!hasControl(v4lCtrl)) {
    Log(*this, Severity::error) << "setControl Missing ctr id:" << v4lCtrl;
    return false;
  }

  std::stringstream ss;
  Log(*this, Severity::debug)
      << "try setControl for:" << v4lCtrl << " value:" << value;
  switch (v4lCtrl) {
  case V4L2_CID_GAIN:
  case V4L2_ANALOGGAIN_ULTRA_PYTON:
  case V4L2_CID_BRIGHTNESS:
  case V4L2_EXTTRIGGGER_ULTRA_PYTON: // EXT_TRIGGER
    setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_], value,
               absolute);
    setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_], value,
               absolute);
    return true;
  case V4L2_REDBALANCE_ULTRA_PYTON:  // V4L2_CID_RED_BALANCE
  case V4L2_BLUEBALANCE_ULTRA_PYTON: // V4L2_CID_BLUE_BALANCE
    setControl(v4lCtrl, mainSubdeviceFd_, value, absolute);
    return true;
  case V4L2_DEADTIME_ULTRA_PYTON: // trg_h
  case V4L2_EXPOSURE_ULTRA_PYTON: // EXPOSURE trg_l
    setControl(v4lCtrl, mainSubdeviceFd_, value, absolute);
    return true;
  default:
    return false;
  }
  return false;
}

bool PythonCameraHelper::setControl(uint32_t v4lCtrl, int fd, double value,
                                    bool absolute) {
  if (value < 0) {
    Log(*this, Severity::error) << "setControl wrong value control";
    return false;
  }

  v4lCtrl = remapControl(v4lCtrl);

  struct v4l2_queryctrl queryctrl;
  struct v4l2_control control;

  memset(&queryctrl, 0, sizeof(queryctrl));
  queryctrl.id = v4lCtrl;

  if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
    if (errno != EINVAL) {
      Log(*this, Severity::error)
          << "Cannot setControl1 value:" << value << " fd:" << fd;
    } else {

      Log(*this, Severity::error)
          << "Cannot setControl2 value:" << value << " fd:" << fd;
    }
    return false;
  }

  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    Log(*this, Severity::error) << "Cannot setControl is disabled";
    return false;
  }
  memset(&control, 0, sizeof(control));
  control.id = v4lCtrl;

  if (v4lCtrl ==
      V4L2_EXPOSURE_ULTRA_PYTON /*&& control.value > maxExposition_*/) // trg_l
  {
    queryctrl.maximum = maxExposition_;
    // control.value = maxExposition_;
  }
  if (!absolute)
    control.value = (int32_t)(value * (queryctrl.maximum - queryctrl.minimum) +
                              queryctrl.minimum);
  else
    control.value = (int32_t)value;

  // Do set
  if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
    Log(*this, Severity::error) << "Cannot setControl3";
    return false;
  }

  std::stringstream ss;
  Log(*this, Severity::debug)
      << "SetControl done --> Ctrl name:" << queryctrl.name
      << " Ctrl value:" << control.value << " Ctrl id:" << control.id;
  return true;
}

double PythonCameraHelper::getControl(uint32_t v4lCtrl) {
  v4lCtrl = remapControl(v4lCtrl);

  if (!hasControl(v4lCtrl)) {
    Log(*this, Severity::error) << "getControl Missing ctr id:" << v4lCtrl;
    return false;
  }

  switch (v4lCtrl) {
  case V4L2_CID_GAIN:
  case V4L2_ANALOGGAIN_ULTRA_PYTON:
  case V4L2_CID_BRIGHTNESS:
  case V4L2_EXTTRIGGGER_ULTRA_PYTON: // EXT_TRIGGER
  {
    double left =
        getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_]);
    double right =
        getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_]);
    if (left != right) {
      Log(*this, Severity::error) << "getControl left and right different";
    }
    return left;
  }
  case V4L2_REDBALANCE_ULTRA_PYTON:  // V4L2_CID_RED_BALANCE
  case V4L2_BLUEBALANCE_ULTRA_PYTON: // V4L2_CID_BLUE_BALANCE
    return getControl(v4lCtrl, mainSubdeviceFd_);
  case V4L2_EXPOSURE_ULTRA_PYTON: // EXPOSURE trg_l
  case V4L2_DEADTIME_ULTRA_PYTON: // trg_h
    return getControl(v4lCtrl, 4);
  default:
    return -1.0;
  }
}

double PythonCameraHelper::getControl(uint32_t v4lCtrl, int fd) {
  struct v4l2_queryctrl queryctrl;
  struct v4l2_control control;

  memset(&control, 0, sizeof(control));
  memset(&queryctrl, 0, sizeof(queryctrl));

  control.id = v4lCtrl;
  queryctrl.id = v4lCtrl;

  if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
    if (errno != EINVAL) {
      Log(*this, Severity::error) << "getControl VIDIOC_QUERYCTRL";
    }
    return -1.0;
  }

  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    Log(*this, Severity::error) << "Control is disabled";
  } else {
    if (-1 == ioctl(fd, VIDIOC_G_CTRL, &control)) {
      Log(*this, Severity::error) << "getControl VIDIOC_G_CTRL";
      return -1.0;
    }
  }

  if (v4lCtrl == V4L2_EXPOSURE_ULTRA_PYTON) // trg_l
  {
    queryctrl.maximum = maxExposition_;
  }

  return (double)(control.value - queryctrl.minimum) /
         (queryctrl.maximum - queryctrl.minimum);
}

bool PythonCameraHelper::hasControl(uint32_t v4lCtrl) {
  v4lCtrl = remapControl(v4lCtrl);

  Log(*this, Severity::debug) << "hascontrol for:" << v4lCtrl;
  switch (v4lCtrl) {
  case V4L2_CID_GAIN:
  case V4L2_ANALOGGAIN_ULTRA_PYTON:
  case V4L2_CID_BRIGHTNESS:
  case V4L2_REDBALANCE_ULTRA_PYTON:  // V4L2_CID_RED_BALANCE
  case V4L2_BLUEBALANCE_ULTRA_PYTON: // V4L2_CID_BLUE_BALANCE
  case V4L2_EXTTRIGGGER_ULTRA_PYTON: // EXT_TRIGGER
  case V4L2_EXPOSURE_ULTRA_PYTON:    // EXPOSURE  trg_l
  case V4L2_DEADTIME_ULTRA_PYTON:    // trg_h
    return true;
  default:
    return false;
  }
  return false;
}

bool PythonCameraHelper::hasAutoControl(uint32_t v4lCtrl) {
  v4lCtrl = remapControl(v4lCtrl);
  // std::stringstream ss;
  // ss << "hasauto for:" << v4lCtrl;
  // log(ss.str(), Severity::debug);
  Log(*this, Severity::debug) << "hasauto for:" << v4lCtrl;
  return false;
}

bool PythonCameraHelper::checkControl(uint32_t v4lCtrl) {
  v4lCtrl = remapControl(v4lCtrl);

  Log(*this, Severity::debug) << "checkCcontrol for:" << v4lCtrl;

  struct v4l2_queryctrl queryctrl;
  struct v4l2_control control;

  memset(&control, 0, sizeof(control));
  memset(&queryctrl, 0, sizeof(queryctrl));

  control.id = v4lCtrl;
  queryctrl.id = v4lCtrl;

  if (-1 == ioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_],
                  VIDIOC_QUERYCTRL, &queryctrl)) {
    if (errno != EINVAL) {
      Log(*this, Severity::error) << "checkControl VIDIOC_QUERYCTRL";
    }
    return false;
  }
  return true;
}

bool PythonCameraHelper::setDefaultControl() {
  setControl(V4L2_EXTTRIGGGER_ULTRA_PYTON, 1, true); // ext_trigger
  setControl(V4L2_EXPOSURE_ULTRA_PYTON, 20, true);   // trg_l
  setControl(V4L2_DEADTIME_ULTRA_PYTON, 10, true);   // trg_h
  setControl(V4L2_CID_BRIGHTNESS, 200, true);
  setControl(V4L2_CID_GAIN, 1, true);
  setControl(V4L2_ANALOGGAIN_ULTRA_PYTON, 2, true);
  return true;
}

uint32_t PythonCameraHelper::remapControl(uint32_t v4lCtr) {
  uint32_t out = v4lCtr;
  switch (v4lCtr) {
  case V4L2_CID_RED_BALANCE:
    out = V4L2_REDBALANCE_ULTRA_PYTON;
    Log(*this, Severity::debug) << "remap RED BALANCE";
    break;
  case V4L2_CID_BLUE_BALANCE:
    out = V4L2_BLUEBALANCE_ULTRA_PYTON;
    Log(*this, Severity::debug) << "remap BLUE BALANCE";
    break;
  case V4L2_CID_EXPOSURE:
    out = V4L2_EXPOSURE_ULTRA_PYTON; // trg_l
    Log(*this, Severity::debug) << "remap EXPOSURE";
    break;
  default:
    break;
  }
  return out;
}
