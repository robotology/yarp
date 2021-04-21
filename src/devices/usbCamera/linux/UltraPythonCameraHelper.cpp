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

#include "UltraPythonCameraHelper.h"

#include <fcntl.h>
#include <libudev.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <sstream>

#include "xilinx-v4l2-controls.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

UltraPythonCameraHelper::UltraPythonCameraHelper(InterfaceForCApi *interfaceC)
{
	Log(*this, Severity::debug) << "::UltraPythonCameraHelper";
	if (interfaceC == nullptr)
	{
		ownerCApi_ = true;
		interfaceC = new InterfaceForCApi();
	}
	interfaceCApi_ = interfaceC;
}

UltraPythonCameraHelper::~UltraPythonCameraHelper()
{
	Log(*this, Severity::debug) << "::~UltraPythonCameraHelper";
	if (interfaceCApi_ != nullptr && ownerCApi_)
	{
		delete interfaceCApi_;
	}
}

bool UltraPythonCameraHelper::openPipeline()
{
	Log(*this, Severity::debug) << "::openPipeline";

	// Open main device
	int fd = interfaceCApi_->open_c(mediaName_, O_RDWR);
	if (fd == -1)
	{
		Log(*this, Severity::error) << "ERROR-cannot open media dev";
		return false;
	}

	struct udev *udev;
	udev = interfaceCApi_->udev_new_c();
	if (udev == nullptr)
	{
		Log(*this, Severity::error) << "ERROR-cannot open udev";
		return false;
	}

	// find subdevice
	struct media_entity_desc info;
	int subdeviceIndex = 0;
	for (int id = 0;; id = info.id)
	{
		memset(&info, 0, sizeof(info));
		info.id = id | MEDIA_ENT_ID_FLAG_NEXT;

		int ret = interfaceCApi_->ioctl_media_c(fd, MEDIA_IOC_ENUM_ENTITIES, info);
		if (ret < 0)
		{
			ret = errno != EINVAL ? -errno : 0;
			Log(*this, Severity::warning) << "WARNING-cannot open device not media";
			break;
		}
		Log(*this, Severity::debug) << "found entity name:" << std::string(info.name);

		dev_t devnum = interfaceCApi_->makedev_c(info.v4l.major, info.v4l.minor);
		struct udev_device *device;
		device = interfaceCApi_->udev_device_new_from_devnum_c(udev, 'c', devnum);
		if (device == nullptr)
		{
			udev_device_unref(device);
			continue;
		}

		const char *deviceName;
		deviceName = interfaceCApi_->udev_device_get_devnode_c(device);

		// Open main subdevice
		if ((std::strcmp(info.name, pipelineVideoName) == 0))
		{
			mainSubdeviceFd_ = interfaceCApi_->open_c(deviceName, O_RDWR | O_NONBLOCK, 0);
			if (mainSubdeviceFd_ == -1)
			{
				Log(*this, Severity::error) << "ERROR-cannot open device:" << std::string(deviceName);
				return false;
			}
			Log(*this, Severity::debug) << "open no pipeline:" << std::string(deviceName);
		}
		else
		{
			// Open other subdevice
			/*
			 * If a python camera is found in pipeline, then that's the
			 * source. If only a TPG is present, then it's the source.
			 * In case both are found, stick to camera
			 */
			if (std::strcmp(info.name, pipelinePythonName) == 0)
			{
				if (sourceSubDeviceIndex1_ == -1)
				{
					sourceSubDeviceIndex1_ = subdeviceIndex;
				}
				else
				{
					sourceSubDeviceIndex2_ = subdeviceIndex;
				}
			}
			else if (std::strstr(info.name, pipelineTpgName))
			{
				tpgIndex_ = subdeviceIndex;
			}
			else if (std::strstr(info.name, pipelineCscName))
			{
				cscIndex_ = subdeviceIndex;
			}
			else if (std::strstr(info.name, pipelineImgfusionName))
			{
				imgfusionIndex_ = subdeviceIndex;
			}
			else if (std::strstr(info.name, pipelinePacket32Name))
			{
				packet32Index_ = subdeviceIndex;
			}
			else if (std::strcmp(info.name, pipelineRxifName) == 0)
			{
				if (rxif1Index_ == -1)
				{
					rxif1Index_ = subdeviceIndex;
				}
				else
				{
					rxif2Index_ = subdeviceIndex;
				}
			}
			pipelineSubdeviceFd_[subdeviceIndex] = interfaceCApi_->open_c(deviceName, O_RDWR | O_NONBLOCK, 0);
			if (pipelineSubdeviceFd_[subdeviceIndex] == -1)
			{
				Log(*this, Severity::error) << "ERROR-cannot open device:" << std::string(deviceName);
				return false;
			}
			Log(*this, Severity::debug) << "Open pipeline devicename:" << std::string(deviceName) << " info name:" << info.name << " fd:" << pipelineSubdeviceFd_[subdeviceIndex]
										<< " index:" << subdeviceIndex;
			subdeviceIndex++;
		}
		interfaceCApi_->udev_device_unref_c(device);
	}
	Log(*this, Severity::debug) << "open:" << mediaName_;
	return checkIndex();
}

bool UltraPythonCameraHelper::checkIndex()
{
	if (mainSubdeviceFd_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find main pipe V4L2 device";
		return false;
	}
	if (sourceSubDeviceIndex1_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find source subdev1";
		return false;
	}
	if (sourceSubDeviceIndex2_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find source subdev2";
		return false;
	}
	if (rxif1Index_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find rxif1Index";
		return false;
	}
	if (rxif2Index_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find rxif2Index";
		return false;
	}
	if (cscIndex_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find cscIndex";
		return false;
	}
	if (tpgIndex_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find tpgIndex";
		return false;
	}
	if (imgfusionIndex_ == -1)
	{
		Log(*this, Severity::error) << "Cannot find imgfusionIndex";
		return false;
	}
	return true;
}

bool UltraPythonCameraHelper::setSubDevFormat(int width, int height)
{
	int i;
	int j;
	int n;
	struct v4l2_subdev_format fmt;

	for (i = 0; pipelineSubdeviceFd_[i] != -1; i++)
	{
		if (i == imgfusionIndex_)
		{
			n = 3;
		}
		else
		{
			n = 2;
		}
		for (j = 0; j < n; j++)
		{
			CLEAR(fmt);
			fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
			fmt.pad = j;
			if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_G_FMT, &fmt))
			{
				Log(*this, Severity::error) << "VIDIOC_SUBDEV_G_FMT. subdev" << i << "pad" << j;
				return false;
			}

			fmt.format.width = width;
			fmt.format.height = height;

			/* if yuv is required, then set that on the source PAD of VPSS */
			if ((i == cscIndex_) && (j == 1) && spaceColor_ == SpaceColor::yuv)
			{
				fmt.format.code = MEDIA_BUS_FMT_UYVY8_1X16;
			}

			/* csc, when there is an imgfusion IP receives 2x width frames */
			if ((imgfusionIndex_ != -1) && (i == cscIndex_))
			{
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
				Log(*this, Severity::debug) << "subdev idx:" << i << " pad" << j << " setting format:" << fmt.format.width << ":" << fmt.format.height;
			}
			if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[i], VIDIOC_SUBDEV_S_FMT, &fmt))
			{
				Log(*this, Severity::error) << "VIDIOC_SUBDEV_S_FMT. subdev" << i << "pad" << j;

				return false;
			}
			if ((i == sourceSubDeviceIndex1_) || (i == sourceSubDeviceIndex2_))
			{
				break; /* only one pad */
			}
		}
	}
	return true;
}

bool UltraPythonCameraHelper::setFormat()
{
	struct v4l2_format fmt;
	// todo check dimensions correctness
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (forceFormatProperty_ || cropEnabledProperty_)
	{
		fmt.fmt.pix.width = cropEnabledProperty_ ? cropWidth_ : nativeWidth_;
		fmt.fmt.pix.height = cropEnabledProperty_ ? cropHeight_ : nativeHeight_;

		if (subsamplingEnabledProperty_)
		{
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

		if (!setSubDevFormat(fmt.fmt.pix.width, fmt.fmt.pix.height))
		{
			return false;
		}

		if (imgfusionIndex_ != -1)
		{
			fmt.fmt.pix.width *= 2;
		}

		if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_S_FMT, &fmt))
			return false;

		/* Note VIDIOC_S_FMT may change width and height. */
		return true;
	}
	/* Preserve original settings as set by v4l2-ctl for example */
	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_G_FMT, &fmt))
		return false;
	return true;
}

/*Unused for now*/
bool UltraPythonCameraHelper::crop(int top, int left, int w, int h, int mytry)
{
	cropCheck();

	Log(*this, Severity::debug) << "crop is" << std::string(cropEnabledProperty_ ? "ENABLED" : "DISABLED");
	if (!cropEnabledProperty_)
		return true;

	struct v4l2_subdev_crop _crop;

	_crop.rect.left = left;
	_crop.rect.top = top;
	_crop.rect.width = w;
	_crop.rect.height = h;

	_crop.which = mytry ? V4L2_SUBDEV_FORMAT_TRY : V4L2_SUBDEV_FORMAT_ACTIVE;
	_crop.pad = 0;

	if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_SUBDEV_S_CROP, &_crop))
	{
		return false;
	}
	if (sourceSubDeviceIndex2_ != -1)
	{
		if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_], VIDIOC_SUBDEV_S_CROP, &_crop))
		{
			return false;
		}
	}
	return true;
}

bool UltraPythonCameraHelper::setSubsampling()
{
	Log(*this, Severity::debug) << "subsampling is" << std::string(subsamplingEnabledProperty_ ? "ENABLED" : "DISABLED");
	int subSamplingValue = 0;
	if (subsamplingEnabledProperty_)
	{
		subSamplingValue = 1;
	}

	Log(*this, Severity::debug) << "setSubsampling";
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_XILINX_PYTHON1300_SUBSAMPLING;
	ctrl.value = subSamplingValue;
	if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_S_CTRL, &ctrl))
	{
		Log(*this, Severity::error) << "setSubsampling";
		return false;
	}

	if (sourceSubDeviceIndex2_ != -1)
	{
		if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[sourceSubDeviceIndex2_], VIDIOC_S_CTRL, &ctrl))
		{
			Log(*this, Severity::error) << "setSubsampling";
			return false;
		}
	}

	ctrl.id = V4L2_CID_XILINX_PYTHON1300_RXIF_REMAPPER_MODE;
	ctrl.value = subSamplingValue;
	if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[rxif1Index_], VIDIOC_S_CTRL, &ctrl))
	{
		Log(*this, Severity::error) << "setSubsampling remapper";
		return false;
	}

	if (rxif2Index_ != -1)
	{
		if (-1 == interfaceCApi_->xioctl(pipelineSubdeviceFd_[rxif2Index_], VIDIOC_S_CTRL, &ctrl))
		{
			Log(*this, Severity::error) << "setSubsampling remapper2";
			return false;
		}
	}
	return true;
}

bool UltraPythonCameraHelper::checkDevice(int mainSubdeviceFd)
{
	struct v4l2_capability cap;
	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd, VIDIOC_QUERYCAP, &cap))
	{
		if (EINVAL == errno)
		{
			Log(*this, Severity::error) << "checkDevice:device is no V4L2 device";
		}
		return false;
	}
	return true;
}

bool UltraPythonCameraHelper::initDevice()
{
	Log(*this, Severity::debug) << "initDevice";

	if (!checkDevice(mainSubdeviceFd_))
	{
		return false;
	}
	if (!setSubsampling())
	{
		return false;
	}
	if (!setFormat())
	{
		return false;
	}
	if (!crop(cropTop_, cropLeft_, cropWidth_, cropHeight_, 0))
	{
		return false;
	}
	if (!initMmap())
	{
		return false;
	}

	return true;
}

bool UltraPythonCameraHelper::cropCheck()
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop tmpCrop;
	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (0 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_CROPCAP, &cropcap))
	{
		tmpCrop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tmpCrop.c = cropcap.defrect; /* reset to default */

		if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_S_CROP, &tmpCrop))
		{
			switch (errno)
			{
				case EINVAL:
					Log(*this, Severity::error) << "cropping not supported";
					break;
				default:
					Log(*this, Severity::error) << "cropping";
					break;
			}
		}
	}
	else
	{
		Log(*this, Severity::warning) << "cropping-2";
	}
	return true;
}

bool UltraPythonCameraHelper::initMmap()
{
	Log(*this, Severity::debug) << "initMmap";
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = requestBufferNumber_;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno)
		{
			Log(*this, Severity::error) << "device does not support memmap";
			return false;
		}
		Log(*this, Severity::error) << "device does not support memmap";
		return false;
	}

	if (req.count < 1)
	{
		Log(*this, Severity::error) << "Insufficient buffer memory on";
		return false;
	}

	for (unsigned int currentUsedBufferIndex = 0; currentUsedBufferIndex < req.count; ++currentUsedBufferIndex)
	{
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = currentUsedBufferIndex;

		if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_QUERYBUF, &buf))
		{
			return false;
		}

		mMapBuffers_[currentUsedBufferIndex].length = buf.length;
		mMapBuffers_[currentUsedBufferIndex].start =
			interfaceCApi_->mmap_c(nullptr /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, mainSubdeviceFd_, buf.m.offset);

		if (MAP_FAILED == mMapBuffers_[currentUsedBufferIndex].start)
		{
			return false;
		}
	}
	return true;
}

bool UltraPythonCameraHelper::startCapturing()
{
	Log(*this, Severity::debug) << "startCapturing";
	enum v4l2_buf_type type;

	for (size_t i = 0; i < requestBufferNumber_; ++i)
	{
		struct v4l2_buffer buf;

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf))
		{
			Log(*this, Severity::error) << "VIDIOC_QBUF";
			return false;
		}
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_STREAMON, &type))
	{
		Log(*this, Severity::error) << "VIDIOC_STREAMON";
		return false;
	}
	return true;
}

bool UltraPythonCameraHelper::step()
{
	static int seq = 0;
	static int sequence = 0;

	fd_set fds;
	struct timeval tv
	{
		80, 0
	};	// Timeout

	FD_ZERO(&fds);
	FD_SET(mainSubdeviceFd_, &fds);

	int ret = interfaceCApi_->select_c(mainSubdeviceFd_ + 1, &fds, nullptr, nullptr, &tv);
	if (ret == -1)
	{
		if (EINTR == errno)
		{
			return false;
		}
		Log(*this, Severity::error) << "select";
		return false;
	}
	else if (ret == 0)
	{
		Log(*this, Severity::error) << "-select timeout";
		return false;
	}

	seq = readFrame();
	if (seq == -1)
	{
		return false;
	}
	if (seq != sequence++)
	{
		Log(*this, Severity::warning) << "dropped frame..";
		sequence = seq + 1;
	}
	return true;
}

int UltraPythonCameraHelper::readFrame()
{
	struct v4l2_buffer buf;
	int seq = 1;
	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_DQBUF, &buf))
	{
		switch (errno)
		{
			case EAGAIN:

				Log(*this, Severity::error) << "VIDIOC_DQBUF eagain";
				return -1;

			case EIO:
			default:
				Log(*this, Severity::error) << "-VIDIOC_DQBUF";
				return -1;
		}
	}

	if (buf.index >= requestBufferNumber_)
	{
		Log(*this, Severity::error) << "readframe";
		return -1;
	}

	if (buf.flags & V4L2_BUF_FLAG_ERROR)
	{
		Log(*this, Severity::error) << "V4L2_BUF_FLAG_ERROR";
		return -1;
	}

	seq = buf.sequence;
	processImage(mMapBuffers_[buf.index].start, buf.bytesused);
	//**Debug start
	// memset(mMapBuffers_[buf.index].start, 255, buf.bytesused);
	//**Debug end
	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_QBUF, &buf))
	{
		Log(*this, Severity::error) << "VIDIOC_QBUF";
		return -1;
	}

	return seq;
}

void UltraPythonCameraHelper::processImage(const void *p, int size)
{
	if (injectedProcessImage_ == nullptr)
		return;
	injectedProcessImage_(p, size);
}

bool UltraPythonCameraHelper::closeAll()
{
	if (!stopCapturing())
		return false;
	if (!unInitDevice())
		return false;
	if (!closePipeline())
		return false;
	return true;
}

bool UltraPythonCameraHelper::unInitDevice()
{
	Log(*this, Severity::debug) << "uninit_device";
	unsigned int i;

	for (i = 0; i < requestBufferNumber_; ++i)
	{
		if (-1 == munmap(mMapBuffers_[i].start, mMapBuffers_[i].length))
		{
			Log(*this, Severity::error) << "munmap";
			return false;
		}
	}
	return true;
}

bool UltraPythonCameraHelper::stopCapturing()
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == interfaceCApi_->xioctl(mainSubdeviceFd_, VIDIOC_STREAMOFF, &type))
	{
		Log(*this, Severity::error) << "VIDIOC_STREAMOFF";
		return false;
	}
	return true;
}

bool UltraPythonCameraHelper::closePipeline()
{
	int i;

	for (i = 0; pipelineSubdeviceFd_[i] != -1; i++)
	{
		if (-1 == close(pipelineSubdeviceFd_[i]))
		{
			Log(*this, Severity::error) << "close pipeline";
			return false;
		}
	}
	return true;
}

void UltraPythonCameraHelper::setInjectedProcess(std::function<void(const void *, int)> toinJect) { injectedProcessImage_ = toinJect; }
void UltraPythonCameraHelper::setInjectedUnlock(std::function<void()> toinJect) { unlock_ = toinJect; }
void UltraPythonCameraHelper::setInjectedLock(std::function<void()> toinJect) { lock_ = toinJect; }

void UltraPythonCameraHelper::setSubsamplingProperty(bool value) { subsamplingEnabledProperty_ = value; }

bool UltraPythonCameraHelper::openAll()
{
	if (!openPipeline())
	{
		return false;
	}

	if (!initDevice())
	{
		return false;
	}

	if (!setDefaultControl())
	{
		return false;
	}

	if (!startCapturing())
	{
		return false;
	}
	return true;
}

void UltraPythonCameraHelper::setStepPeriod(double msec) { stepPeriod_ = msec; }

void UltraPythonCameraHelper::setInjectedLog(std::function<void(const std::string &, Severity)> toinJect) { log_ = toinJect; }

bool UltraPythonCameraHelper::setControl(uint32_t v4lCtrl, double value, bool absolute)
{
	if (!absolute)
	{
		if (value > 1.0 || value < 0.0)
		{
			Log(*this, Severity::error) << "Not absolute settings should be between 0,1:" << v4lCtrl;
			return false;
		}
	}

	v4lCtrl = remapControl(v4lCtrl);

	if (!hasControl(v4lCtrl))
	{
		Log(*this, Severity::error) << "setControl Missing ctr id:" << v4lCtrl;
		return false;
	}

	Log(*this, Severity::debug) << "try setControl for:" << v4lCtrl << " value:" << value;
	switch (v4lCtrl)
	{
		case V4L2_CID_GAIN:
			return setGain(value, absolute);
		case V4L2_ANALOGGAIN_ULTRA_PYTHON:
		case V4L2_CID_BRIGHTNESS:
		case V4L2_EXTTRIGGGER_ULTRA_PYTHON:	 // EXT_TRIGGER
			setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_], value, absolute);
			setControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_], value, absolute);
			return true;
		case V4L2_REDBALANCE_ULTRA_PYTHON:	 // V4L2_CID_RED_BALANCE
		case V4L2_BLUEBALANCE_ULTRA_PYTHON:	 // V4L2_CID_BLUE_BALANCE
			setControl(v4lCtrl, mainSubdeviceFd_, value, absolute);
			return true;
		case V4L2_DEADTIME_ULTRA_PYTHON:  // trg_h
		case V4L2_EXPOSURE_ULTRA_PYTHON:  // EXPOSURE trg_l
			setControl(v4lCtrl, mainSubdeviceFd_, value, absolute);
			return true;
		default:
			return false;
	}
	return false;
}

bool UltraPythonCameraHelper::setControl(uint32_t v4lCtrl, int fd, double value, bool absolute)
{
	if (value < 0)
	{
		Log(*this, Severity::error) << "setControl wrong value control";
		return false;
	}

	v4lCtrl = remapControl(v4lCtrl);

	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = v4lCtrl;

	if (-1 == interfaceCApi_->ioctl_query_c(fd, VIDIOC_QUERYCTRL, queryctrl))
	{
		if (errno != EINVAL)
		{
			Log(*this, Severity::error) << "Cannot setControl1 value:" << value << " fd:" << fd;
		}
		else
		{
			Log(*this, Severity::error) << "Cannot setControl2 value:" << value << " fd:" << fd;
		}
		return false;
	}

	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		Log(*this, Severity::error) << "Cannot setControl is disabled";
		return false;
	}
	memset(&control, 0, sizeof(control));
	control.id = v4lCtrl;

	if (v4lCtrl == V4L2_EXPOSURE_ULTRA_PYTHON)	// trg_l
	{
		queryctrl.maximum = maxPermittedExposition_;
		queryctrl.minimum = minPermittedExposition_;
	}
	if (!absolute)
		control.value = (int32_t)(value * (queryctrl.maximum - queryctrl.minimum) + queryctrl.minimum);
	else
		control.value = (int32_t)value;

	if (v4lCtrl == V4L2_EXPOSURE_ULTRA_PYTHON)
	{
		if (stepPeriod_ <= control.value + deadTime_ + 2)
		{
			Log(*this, Severity::warning) << "Exposition will decrease FPS. Limit:" << control.value + deadTime_ + 5 << " current step:" << stepPeriod_;
		}
		else
		{
			Log(*this, Severity::debug) << "Exposition will mantain current FPS Limit:" << control.value + deadTime_ + 5 << " current step:" << stepPeriod_;
		}
	}

	// Do set
	if (-1 == interfaceCApi_->ioctl_control_c(fd, VIDIOC_S_CTRL, control))
	{
		Log(*this, Severity::error) << "Cannot setControl3";
		return false;
	}

	std::stringstream ss;
	Log(*this, Severity::debug) << "SetControl done --> Ctrl name:" << queryctrl.name << " Ctrl value:" << control.value << " Ctrl id:" << control.id;
	return true;
}

double UltraPythonCameraHelper::getControl(uint32_t v4lCtrl)
{
	v4lCtrl = remapControl(v4lCtrl);

	if (!hasControl(v4lCtrl))
	{
		Log(*this, Severity::error) << "getControl Missing ctr id:" << v4lCtrl;
		return false;
	}

	switch (v4lCtrl)
	{
		case V4L2_CID_GAIN:
		case V4L2_ANALOGGAIN_ULTRA_PYTHON:
		case V4L2_CID_BRIGHTNESS:
		case V4L2_EXTTRIGGGER_ULTRA_PYTHON:	 // EXT_TRIGGER
		{
			double left = getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex1_]);
			double right = getControl(v4lCtrl, pipelineSubdeviceFd_[sourceSubDeviceIndex2_]);
			if (left != right)
			{
				Log(*this, Severity::error) << "getControl left and right different";
			}
			return left;
		}
		case V4L2_REDBALANCE_ULTRA_PYTHON:	 // V4L2_CID_RED_BALANCE
		case V4L2_BLUEBALANCE_ULTRA_PYTHON:	 // V4L2_CID_BLUE_BALANCE
			return getControl(v4lCtrl, mainSubdeviceFd_);
		case V4L2_EXPOSURE_ULTRA_PYTHON:  // EXPOSURE trg_l
		case V4L2_DEADTIME_ULTRA_PYTHON:  // trg_h
			return getControl(v4lCtrl, 4);
		default:
			return -1.0;
	}
}

double UltraPythonCameraHelper::getControl(uint32_t v4lCtrl, int fd)
{
	if (v4lCtrl == V4L2_CID_GAIN)
	{
		double min = gainMap_.begin()->first;
		double max = gainMap_.end()->first;
		return (double)(currentGainValue_ - min) / (max - min);
	}

	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	memset(&control, 0, sizeof(control));
	memset(&queryctrl, 0, sizeof(queryctrl));

	control.id = v4lCtrl;
	queryctrl.id = v4lCtrl;

	if (interfaceCApi_->ioctl_query_c(fd, VIDIOC_QUERYCTRL, queryctrl) == -1)
	{
		if (errno != EINVAL)
		{
			Log(*this, Severity::error) << "getControl VIDIOC_QUERYCTRL:" << v4lCtrl;
		}
		return -1.0;
	}

	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		Log(*this, Severity::error) << "Control is disabled:" << v4lCtrl;
	}
	else
	{
		if (interfaceCApi_->ioctl_control_c(fd, VIDIOC_G_CTRL, control) == -1)
		{
			Log(*this, Severity::error) << "getControl VIDIOC_G_CTRL:" << v4lCtrl;
			return -1.0;
		}
	}

	if (v4lCtrl == V4L2_EXPOSURE_ULTRA_PYTHON)	// trg_l
	{
		queryctrl.maximum = maxPermittedExposition_;
		queryctrl.minimum = minPermittedExposition_;
	}

	return (double)(control.value - queryctrl.minimum) / (queryctrl.maximum - queryctrl.minimum);
}

bool UltraPythonCameraHelper::hasControl(uint32_t v4lCtrl) const
{
	v4lCtrl = remapControl(v4lCtrl);

	Log(*this, Severity::debug) << "hascontrol for:" << v4lCtrl;
	switch (v4lCtrl)
	{
		case V4L2_CID_GAIN:
		case V4L2_ANALOGGAIN_ULTRA_PYTHON:
		case V4L2_CID_BRIGHTNESS:
		case V4L2_REDBALANCE_ULTRA_PYTHON:	 // V4L2_CID_RED_BALANCE
		case V4L2_BLUEBALANCE_ULTRA_PYTHON:	 // V4L2_CID_BLUE_BALANCE
		case V4L2_EXTTRIGGGER_ULTRA_PYTHON:	 // EXT_TRIGGER
		case V4L2_EXPOSURE_ULTRA_PYTHON:	 // EXPOSURE  trg_l
		case V4L2_DEADTIME_ULTRA_PYTHON:	 // trg_h
			return true;
		default:
			return false;
	}
	return false;
}

bool UltraPythonCameraHelper::hasAutoControl(uint32_t v4lCtrl) const
{
	v4lCtrl = remapControl(v4lCtrl);
	// std::stringstream ss;
	// ss << "hasauto for:" << v4lCtrl;
	// log(ss.str(), Severity::debug);
	Log(*this, Severity::debug) << "hasauto for:" << v4lCtrl;
	return false;
}

bool UltraPythonCameraHelper::checkControl(uint32_t v4lCtrl)
{
	v4lCtrl = remapControl(v4lCtrl);

	Log(*this, Severity::debug) << "checkCcontrol for:" << v4lCtrl;

	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	memset(&control, 0, sizeof(control));
	memset(&queryctrl, 0, sizeof(queryctrl));

	control.id = v4lCtrl;
	queryctrl.id = v4lCtrl;

	if (-1 == interfaceCApi_->ioctl_query_c(pipelineSubdeviceFd_[sourceSubDeviceIndex1_], VIDIOC_QUERYCTRL, queryctrl))
	{
		if (errno != EINVAL)
		{
			Log(*this, Severity::error) << "checkControl VIDIOC_QUERYCTRL";
		}
		return false;
	}
	return true;
}

bool UltraPythonCameraHelper::setDefaultControl()
{
	setControl(V4L2_EXTTRIGGGER_ULTRA_PYTHON, 1, true);		  // ext_trigger
	setControl(V4L2_EXPOSURE_ULTRA_PYTHON, 20, true);		  // trg_l
	setControl(V4L2_DEADTIME_ULTRA_PYTHON, deadTime_, true);  // trg_h
	setControl(V4L2_CID_BRIGHTNESS, 200, true);
	setControl(V4L2_CID_GAIN, 1, true);
	return true;
}

uint32_t UltraPythonCameraHelper::remapControl(uint32_t v4lCtr) const
{
	uint32_t out = v4lCtr;
	switch (v4lCtr)
	{
		case V4L2_CID_RED_BALANCE:
			out = V4L2_REDBALANCE_ULTRA_PYTHON;
			Log(*this, Severity::debug) << "remap RED BALANCE";
			break;
		case V4L2_CID_BLUE_BALANCE:
			out = V4L2_BLUEBALANCE_ULTRA_PYTHON;
			Log(*this, Severity::debug) << "remap BLUE BALANCE";
			break;
		case V4L2_CID_EXPOSURE:
			out = V4L2_EXPOSURE_ULTRA_PYTHON;  // trg_l
			Log(*this, Severity::debug) << "remap EXPOSURE in:" << v4lCtr << " out:" << out;
			break;
		default:
			break;
	}
	return out;
}

bool UltraPythonCameraHelper::setGain(double value, bool absolute)
{
	double min = gainMap_.begin()->first;
	double max = gainMap_.end()->first;
	int absoluteValue = (int32_t)value;
	if (!absolute)
		absoluteValue = (int32_t)(value * (max - min) + min);

	auto it = gainMap_.find(absoluteValue);
	if (it == gainMap_.end())
	{
		Log(*this, Severity::error) << "wrong gain value";
		return false;
	}

	auto current = gainMap_.at(absoluteValue);
	currentGainValue_ = absoluteValue;
	Log(*this, Severity::debug) << "gain:" << absoluteValue << " digital:" << current.first << " analog:" << current.second;

	setControl(V4L2_CID_GAIN, pipelineSubdeviceFd_[sourceSubDeviceIndex1_], current.first, true);
	setControl(V4L2_CID_GAIN, pipelineSubdeviceFd_[sourceSubDeviceIndex2_], current.first, true);
	setControl(V4L2_ANALOGGAIN_ULTRA_PYTHON, pipelineSubdeviceFd_[sourceSubDeviceIndex1_], current.second, true);
	setControl(V4L2_ANALOGGAIN_ULTRA_PYTHON, pipelineSubdeviceFd_[sourceSubDeviceIndex2_], current.second, true);
	return true;
}
