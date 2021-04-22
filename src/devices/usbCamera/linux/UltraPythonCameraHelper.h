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
#pragma once

#include <array>
#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>

#include "InterfaceForCApi.h"

struct v4l2_format;

struct MmapBuffer
{
	void *start;
	size_t length;
};
enum class SpaceColor
{
	yuv,
	rgb,
	grgb
};

enum class Severity
{
	debug,
	info,
	warning,
	error
};

class UltraPythonCameraHelper
{
   public:
	static constexpr unsigned int hiresWidth_{2560};										 // HI RES width image
	static constexpr unsigned int hiresHeight_{1024};										 // HI RES height image
	static constexpr unsigned int lowresWidth_{1280};										 // LOW RES width image
	static constexpr unsigned int lowresHeight_{512};										 // LOW RES height image
	static constexpr unsigned int hiresImageBufferSize_{hiresWidth_ * hiresHeight_ * 3};	 // HI RES buffer size
	static constexpr unsigned int lowresImageBufferSize_{lowresWidth_ * lowresHeight_ * 3};	 // LOW RES buffer size
	static constexpr int lowresFrameRate_{35};
	static constexpr int hiresFrameRate_{14};
	static constexpr unsigned int deadTime_{8};
	static constexpr unsigned int minPermittedExposition_{15};
	static constexpr unsigned int maxPermittedExposition_{100};

	// Ctrl for Python
	static constexpr unsigned int V4L2_EXPOSURE_ULTRA_PYTHON{0x0098cb03};	  // trg_l
	static constexpr unsigned int V4L2_DEADTIME_ULTRA_PYTHON{0x0098cb02};	  // trg_h
	static constexpr unsigned int V4L2_EXTTRIGGGER_ULTRA_PYTHON{0x0098cc03};  // ext_trigger
	static constexpr unsigned int V4L2_REDBALANCE_ULTRA_PYTHON{0x0098c9a3};	  // Red Balance
	static constexpr unsigned int V4L2_BLUEBALANCE_ULTRA_PYTHON{0x0098c9a5};  // Blue balance
	static constexpr unsigned int V4L2_ANALOGGAIN_ULTRA_PYTHON{0x009e0903};	  // Analog gain

   private:
	static constexpr const char *mediaName_ = "/dev/media0";

	// Pipeline string
	static constexpr const char *pipelineVideoName = "vcap_python output 0";
	static constexpr const char *pipelinePythonName = "PYTHON1300";
	static constexpr const char *pipelineTpgName = "v_tpg";
	static constexpr const char *pipelineCscName = "v_proc_ss";
	static constexpr const char *pipelinePacket32Name = "Packet32";
	static constexpr const char *pipelineImgfusionName = "imgfusion";
	static constexpr const char *pipelineRxifName = "PYTHON1300_RXIF";

	static constexpr unsigned int requestBufferNumber_ = {8};
	static constexpr unsigned int pipelineMaxLen = {16};

	// Native resolution for cam
	static constexpr unsigned int nativeWidth_{1280};	// Sensor HI RES width
	static constexpr unsigned int nativeHeight_{1024};	// Sensor HI RES height

   public:
	// Main
	bool openAll();
	bool step();
	bool closeAll();

	// Settings
	void setSubsamplingProperty(bool value);
	bool setControl(uint32_t v4lCtrl, double value, bool absolute);	 // if not absolute normalized between 0-1
	double getControl(uint32_t v4lCtrl);							 // Normalize control value
	bool hasControl(uint32_t v4lCtrl) const;
	bool hasAutoControl(uint32_t v4lCtrl) const;
	bool checkControl(uint32_t v4lCtr);
	void setStepPeriod(double msec);
	void setHonorFps(bool value);

	// Inject function from out
	void setInjectedProcess(std::function<void(const void *, int)> toinJect);
	void setInjectedUnlock(std::function<void()> toinJect);
	void setInjectedLock(std::function<void()> toinJect);
	void setInjectedLog(std::function<void(const std::string &, Severity severity)> toinJect);

	explicit UltraPythonCameraHelper(InterfaceForCApi *interfaceC);
	virtual ~UltraPythonCameraHelper();

   private:
	InterfaceForCApi *interfaceCApi_;  // Unittest purpouse c interface
	bool ownerCApi_{false};
	bool openPipeline();
	bool initDevice();
	bool startCapturing();
	bool setDefaultControl();					   // Some important default controls
	uint32_t remapControl(uint32_t v4lCtr) const;  // Different id from the official ones
	bool setControl(uint32_t v4lCtrl, int fd, double value, bool absolute);
	double getControl(uint32_t v4lCtrl, int fd);
	bool setSubDevFormat(int width, int height);
	bool setFormat();
	bool setSubsampling();
	bool crop(int top, int left, int w, int h, int mytry);
	bool checkDevice(int mainSubdeviceFd);
	int readFrame();
	void processImage(const void *p, int size);
	bool unInitDevice();
	bool stopCapturing();
	bool closePipeline();
	bool initMmap();
	bool cropCheck();
	void log(const std::string &toBeLogged, Severity severity = Severity::debug);
	bool checkIndex();
	bool setGain(double value, bool absolute);

	// Property
	bool subsamplingEnabledProperty_{true};
	bool cropEnabledProperty_{false};
	bool forceFormatProperty_{true};  // Overwrite preesistent format

	// Image memory map
	MmapBuffer mMapBuffers_[requestBufferNumber_];

	// File descriptors and indexes
	int mainSubdeviceFd_ = -1;
	std::array<int, pipelineMaxLen> pipelineSubdeviceFd_ = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	};
	int sourceSubDeviceIndex1_ = -1;
	int sourceSubDeviceIndex2_ = -1;
	int rxif1Index_ = -1;
	int rxif2Index_ = -1;
	int cscIndex_ = -1;
	int tpgIndex_ = -1;
	int imgfusionIndex_ = -1;
	int packet32Index_ = -1;

	double stepPeriod_{100};  // Interval for two step() call
	bool honorfps_{false};	  // Keep FPS stable

	const SpaceColor spaceColor_{SpaceColor::rgb};

	// Gain
	double currentGainValue_{1};
	const std::map<unsigned int, std::pair<unsigned int, unsigned int>> gainMap_{{1, {1, 1}}, {2, {1, 2}}, {3, {1, 3}}, {4, {1, 4}},  {5, {1, 5}}, {6, {2, 2}},
																				 {7, {2, 3}}, {8, {2, 4}}, {9, {2, 5}}, {10, {2, 6}}, {11, {2, 7}}};

	// Crop size
	unsigned int cropLeft_{0};
	unsigned int cropTop_{0};
	unsigned int cropHeight_{0};
	unsigned int cropWidth_{0};

	// injected functionality
	std::function<void(const void *, int)> injectedProcessImage_;  // Process image external
	std::function<void()> lock_;								   // Mutex injected method
	std::function<void()> unlock_;								   // Mutex injected method
	std::function<void(const std::string &, Severity)> log_;	   // Logging injected method

	// Injected RAI mutex
	class Locker
	{
	   private:
		const UltraPythonCameraHelper &parent_;

	   public:
		explicit Locker(const UltraPythonCameraHelper &parent) : parent_(parent)
		{
			if (parent_.lock_)
				parent_.lock_();
		};
		~Locker()
		{
			if (parent_.unlock_)
			{
				parent_.unlock_();
			}
		};
	};

	// Injected RAI log
	class Log
	{
	   private:
		const UltraPythonCameraHelper &parent_;
		Severity severity_;
		std::stringstream ss_;

	   public:
		Log(const UltraPythonCameraHelper &parent, Severity severity) : parent_(parent), severity_(severity){};
		~Log()
		{
			if (parent_.log_)
			{
				parent_.log_(ss_.str(), severity_);
			}
		};

		using streamtype = std::ostream &(std::ostream &);
		Log &operator<<(streamtype);  // std::endl only

		template <typename T>
		Log &operator<<(const T &data)	// All other case
		{
			ss_ << data;
			return *this;
		};
	};
};
