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

#pragma once

#include "InterfaceForCFunction.h"
#include <array>
#include <cstring>
#include <fstream>
#include <functional>

struct v4l2_format;

struct MmapBuffer
{
    void* start;
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

class PythonCameraHelper
{
private:
    // Pipeline string
    static constexpr const char* pipelineVideoName = "vcap_python output 0";
    static constexpr const char* pipelineDummyName = "vcap_dummy output 0";
    static constexpr const char* pipelinePythonName = "PYTHON1300";
    static constexpr const char* pipelineTpgName = "v_tpg";
    static constexpr const char* pipelineCscName = "v_proc_ss";
    static constexpr const char* pipelinePacket32Name = "Packet32";
    static constexpr const char* pipelineImgfusionName = "imgfusion";
    static constexpr const char* pipelineRxifName = "PYTHON1300_RXIF";  

    static constexpr unsigned int requestBufferNumber_ = {8};
    static constexpr unsigned int pipelineMaxLen = {16};
    
    // Native resolution for cam
    static constexpr unsigned int nativeWidth_ {1280};
    static constexpr unsigned int nativeHeight_ {1024};

    // Control value max
    static constexpr unsigned int maxExposition_{100};

    // Ctrl for Python
    static constexpr unsigned int V4L2_EXPOSURE_ULTRA_PYTON{0x0098cb03};//trg_l
    static constexpr unsigned int V4L2_DEADTIME_ULTRA_PYTON{0x0098cb02};//trg_h
    static constexpr unsigned int V4L2_EXTTRIGGGER_ULTRA_PYTON{0x0098cc03};//ext_trigger
    static constexpr unsigned int V4L2_REDBALANCE_ULTRA_PYTON{0x0098c9a3};//Red Balance
    static constexpr unsigned int V4L2_BLUEBALANCE_ULTRA_PYTON{0x0098c9a5};//Blue balance
    static constexpr unsigned int V4L2_ANALOGGAIN_ULTRA_PYTON{0x009e0903};//Analog gain
    
public:
    //Main
    void openAll();
    void step();
    void closeAll();

    //Settings
    double getCurrentFps() const;
    void setSubsamplingProperty(bool value);
    bool setControl(uint32_t v4lCtrl, double value,bool absolute);
    double getControl(uint32_t v4lCtrl);
    bool hasControl(uint32_t v4lCtrl);
    bool hasAutoControl(uint32_t v4lCtrl);
    bool checkControl(uint32_t v4lCtr);

    //Inject function from out
    void setInjectedProcess(std::function<void(const void*, int)> toinJect);
    void setInjectedUnlock(std::function<void()> toinJect);
    void setInjectedLock(std::function<void()> toinJect);
    void setInjectedLog(std::function<void(const std::string&, Severity severity)> toinJect);

    //Minor
    void setFileLog(bool value);

private:
    InterfaceForCFunction interface_;
    void openPipeline();
    void initDevice();
    void startCapturing();
    bool setDefaultControl();               //Some important default controls
    uint32_t remapControl(uint32_t v4lCtr); //Different id from the official ones
    bool setControl(uint32_t v4lCtrl, int fd, double value,bool absolute);
    double getControl(uint32_t v4lCtrl, int fd);
    void setSubDevFormat(int width, int height);
    void setFormat();
    void setSubsampling(void);
    void crop(int top, int left, int w, int h, int mytry);
    bool checkDevice(int mainSubdeviceFd);
    int readFrame();
    void processImage(const void* p, int size);
    void unInitDevice(void);
    void stopCapturing();
    void closePipeline();
    int xioctl(int fh, int request, void* arg);
    void initMmap(void);
    bool cropCheck();
    void fpsCalculus();
    void log(const std::string& toBeLogged, Severity severity = Severity::debug);

    //Property
    bool subsamplingEnabledProperty_ {true};
    bool cropEnabledProperty_ {false};
    bool forceFormatProperty_ {true}; // Overwrite preesistent format

    // Image memory map
    MmapBuffer mMapBuffers_[requestBufferNumber_];

    // File descriptors and indexes
    int mainSubdeviceFd_ = -1;
    std::array<int, pipelineMaxLen> pipelineSubdeviceFd_ = {
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
    };
    int sourceSubDeviceIndex1_ = -1;
    int sourceSubDeviceIndex2_ = -1;
    int rxif1Index_ = -1;
    int rxif2Index_ = -1;
    int cscIndex_ = -1;
    int tpgIndex_ = -1;
    int imgfusionIndex_ = -1;
    int packet32Index_ = -1;

    double fps_;

    const SpaceColor spaceColor_ {SpaceColor::rgb};

    const std::string mediaName_ {"/dev/media0"};

    // Crop size
    unsigned int cropLeft_ {0};
    unsigned int cropTop_ {0};
    unsigned int cropHeight_ {0};
    unsigned int cropWidth_ {0};

    // injected functionality
    std::function<void(const void*, int)> injectedProcessImage_; //Process image external
    std::function<void()> lock_;                                 //Mutex injected method
    std::function<void()> unlock_;                               //Mutex injected method
    std::function<void(const std::string&, Severity)> log_;

    // file log
    std::ofstream fs {"./log.log"};
    bool logOnFile_ {false};

    class Locker
    {

    private:
        PythonCameraHelper& parent_;

    public:
        explicit Locker(PythonCameraHelper& parent) :
                parent_(parent)
        {
            if (parent_.lock_)
                parent_.lock_();
        };
        ~Locker()
        {
            if (parent_.unlock_)
                parent_.unlock_();
        };
    };
};