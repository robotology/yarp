#pragma once

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

    static constexpr const char* methodName = "------------";

public:
    void openAll();
    void step();
    void closeAll();
    double getCurrentFps() const;
    void setSubsamplingProperty(bool value);
    void setFileLog(bool value);

    void setInjectedProcess(std::function<void(const void*, int)> toinJect);
    void setUnlock(std::function<void()> toinJect);
    void setLock(std::function<void()> toinJect);
    void setLog(std::function<void(const std::string&, Severity severity)> toinJect);

private:
    void openPipeline();
    void initDevice();
    void startCapturing();

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
    void log(const std::string& toBeLogged,Severity severity=Severity::debug);

    SpaceColor spaceColor_ {SpaceColor::rgb};

    std::string mediaName_ {"/dev/media0"};

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