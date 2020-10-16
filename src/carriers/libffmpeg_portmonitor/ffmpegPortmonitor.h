#ifndef YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_H
#define YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Image.h>
#include <yarp/os/MonitorObject.h>

extern "C" {
    #include <libavcodec/avcodec.h>
}


class FfmpegMonitorObject : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy(void) override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

protected:
    int compress(yarp::sig::Image* img, AVPacket* pkt);
    int decompress(AVPacket* pkt, unsigned char** decompressed, int* sizeDecompressed, int w, int h, int pixelCode);
    void getParamsFromCommandLine(std::string carrierString);
    int setCommandLineParams();

public:
    yarp::os::Things th;
    yarp::os::Bottle data;
    yarp::sig::FlexImage imageOut;
    bool senderSide;
    const char* codecName;
    const AVCodec *codec;
    AVCodecContext *codecContext;
    bool firstTime;
    std::map<std::string, std::string> paramsMap;
    std::map<int, int> pixelMap;
    std::map<int, int> codecPixelMap;
    int counter = 0;
    int counter2 = 0;
    int countertot = 0;
};

#endif
