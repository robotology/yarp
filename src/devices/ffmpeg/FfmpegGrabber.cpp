/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006 Jonas Ruesch
 * SPDX-FileCopyrightText: 2006 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FfmpegGrabber.h"
#include "ffmpeg_api.h"

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace {
YARP_LOG_COMPONENT(FFMPEGGRABBER, "yarp.device.ffmpeg_grabber")
}

class DecoderState
{
public:
    int      bytesRemaining;
    int      bytesDecoded;
    int      frameFinished;
    int      index;

    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    // video buffers
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVFrame         *pAudio;
    uint8_t         *buffer;
    int16_t         *audioBuffer;
    int16_t         *audioBufferAt;
    int audioBufferLen;

    DecoderState() :
        bytesRemaining(0),
        bytesDecoded(0),
        frameFinished(0),
        index(-1),
        pCodecCtx(nullptr),
        pCodec(nullptr),
        pFrame(nullptr),
        pFrameRGB(nullptr),
        pAudio(nullptr),
        buffer(nullptr),
        audioBuffer(nullptr),
        audioBufferAt(nullptr),
        audioBufferLen(0)
    {
    }

    bool isFinished()
    {
        return frameFinished!=0;
    }

    int getIndex()
    {
        return index;
    }

    virtual ~DecoderState()
    {
        if (pCodecCtx!=nullptr) {
            avcodec_close(pCodecCtx);
        }
        if (audioBuffer!=nullptr) {
            delete [] audioBuffer;
        }
        if (buffer!=nullptr) {
            delete [] buffer;
        }
        if (pFrameRGB!=nullptr) {
            av_free(pFrameRGB);
        }
        if (pFrame!=nullptr) {
            av_free(pFrame);
        }
        if (pAudio!=nullptr) {
            av_free(pAudio);
        }
    }

    int getStream(AVFormatContext *pFormatCtx, AVMediaType code, const char *name)
    {
        // Find the first stream
        int videoStream=-1;
        for(int i=0; i<(int)(pFormatCtx->nb_streams); i++) {
            if(pFormatCtx->streams[i]->codecpar->codec_type==code) {
                videoStream=i;
                yCInfo(FFMPEGGRABBER, "First %s stream is stream #%d", name, i);
                break;
            }
        }
        if(videoStream==-1) {
            yCError(FFMPEGGRABBER, "Could not find %s stream", name);
        }
        index = videoStream;

        return index;
    }

    bool getCodec(AVFormatContext *pFormatCtx)
    {
        // Get a pointer to the codec context for the video stream
        pCodecCtx=pFormatCtx->streams[index]->codec;

        // Find the decoder for the video stream
        pCodec = avcodec_find_decoder(pFormatCtx->streams[index]->codecpar->codec_id);
        if(pCodec==nullptr) {
            yCError(FFMPEGGRABBER, "Codec not found");
            return false; // Codec not found
        }

        // Open codec
        if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
            yCError(FFMPEGGRABBER, "Could not open codec");
            return false; // Could not open codec
        }

        return true;
    }


    bool allocateImage()
    {
        // Allocate video frame
        pFrame=av_frame_alloc();

        // Allocate an AVFrame structure
        pFrameRGB=av_frame_alloc();
        if(pFrameRGB==nullptr) {
            yCError(FFMPEGGRABBER, "Could not allocate a frame");
            return false;
        }

        // Determine required buffer size and allocate buffer
        int numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                        pCodecCtx->height);
        buffer=new uint8_t[numBytes];

        // Assign appropriate parts of buffer to image planes in pFrameRGB
        avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
                       pCodecCtx->width, pCodecCtx->height);
        return true;
    }

    bool allocateSound()
    {
        audioBufferLen = MAX_AUDIO_FRAME_SIZE*10;
        audioBuffer = new int16_t[audioBufferLen];
        audioBufferAt = audioBuffer;
        yCInfo(FFMPEGGRABBER,
               "channels %d, sample_rate %d, frame_size %d",
               pCodecCtx->channels,
               pCodecCtx->sample_rate,
               pCodecCtx->frame_size);
        return true;
    }

    int getWidth()
    {
        return pCodecCtx->width;
    }

    int getHeight()
    {
        return pCodecCtx->height;
    }


    int getRate()
    {
        return pCodecCtx->sample_rate;
    }

    int getChannels()
    {
        return pCodecCtx->channels;
    }

    bool getAudio(AVPacket& packet,Sound& sound)
    {
        int ct = 0;
        int bytesRead = 0;
        int bytesWritten = 0;
        int gotFrame = 0;
        while (bytesRead<packet.size) {
            ct = audioBufferLen;
            AVPacket tmp = packet;
            tmp.data += bytesRead;
            tmp.size -= bytesRead;
            if (!pAudio) {
                if (!(pAudio = av_frame_alloc())) {
                    yCFatal(FFMPEGGRABBER, "out of memory");
                }
            } else {
                av_frame_unref(pAudio);
            }
            int r = avcodec_decode_audio4(pCodecCtx, pAudio, &gotFrame, &packet);
            ct = 0;
            if (gotFrame) {
                ct = av_samples_get_buffer_size(nullptr,
                                                pCodecCtx->channels,
                                                pAudio->nb_samples,
                                                pCodecCtx->sample_fmt,
                                                1);
            }
            if (r<0) {
                yCError(FFMPEGGRABBER, "error decoding audio");
                return false;
            }
            int num_channels = getChannels();
            int num_rate = getRate();
            //audioBufferAt += ct;
            //audioBufferLen += ct;
            bytesRead += r;
            bytesWritten += ct;
            if (bytesRead==packet.size) {
                int num_samples = bytesWritten/(sizeof(int16_t)*num_channels);
                sound.resize(num_samples,num_channels);
                sound.setFrequency(num_rate);

                int idx = 0;
                for (int i=0; i<num_samples; i++) {
                    for (int j=0; j<num_channels; j++) {
                        sound.set(audioBuffer[idx],i,j);
                        idx++;
                    }
                }
            }
        }
        return true;
    }

    bool getVideo(AVPacket& packet)
    {
        // Decode video frame
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                              &packet);

        // Did we get a video frame?
        if(frameFinished) {
            // Convert the image from its native format to RGB
            int w = pCodecCtx->width;
            int h = pCodecCtx->height;
            static struct SwsContext *img_convert_ctx = nullptr;
            if (img_convert_ctx==nullptr) {
                img_convert_ctx = sws_getContext(w,h,
                                                 pCodecCtx->pix_fmt,
                                                 w, h, AV_PIX_FMT_RGB24,
                                                 //0,
                                                 //SWS_BILINEAR,
                                                 SWS_BICUBIC,
                                                 nullptr, nullptr, nullptr);
            }
            if (img_convert_ctx!=nullptr) {
                sws_scale(img_convert_ctx, ((AVPicture*)pFrame)->data,
                          ((AVPicture*)pFrame)->linesize, 0,
                          pCodecCtx->height,
                          ((AVPicture*)pFrameRGB)->data,
                          ((AVPicture*)pFrameRGB)->linesize);
            } else {
                yCFatal(FFMPEGGRABBER, "Software scaling not working");
            }
        }
        return frameFinished;
    }


    bool getVideo(ImageOf<PixelRgb>& image)
    {
        if (frameFinished) {
            FlexImage flex;
            flex.setPixelCode(VOCAB_PIXEL_RGB);
            flex.setQuantum((pFrameRGB->linesize[0]));
            flex.setExternal(pFrameRGB->data[0],
                             pCodecCtx->width,
                             pCodecCtx->height);
            image.copy(flex);
        }

        return frameFinished;
    }

    bool haveFrame()
    {
        return frameFinished;
    }
};

class FfmpegHelper
{
public:
    DecoderState videoDecoder;
    DecoderState audioDecoder;
};


#define HELPER(x) (*((FfmpegHelper*)x))


const char *xstrdup(const char *str)
{
    if (str[0]=='-') return nullptr;
    return strdup(str);
}

bool FfmpegGrabber::openV4L(yarp::os::Searchable & config,
                            AVFormatContext **ppFormatCtx,
                            AVFormatContext **ppFormatCtx2)
{
    bool audio = (ppFormatCtx==nullptr);
    AVDictionary*& formatParams =
        *(audio?(&formatParamsAudio):(&formatParamsVideo));

    AVInputFormat *iformat;
    Value v;

    if (!audio) {
        //formatParams.prealloced_context = 1;
        v = config.check("v4ldevice",
                         Value("/dev/video0"),
                         "device name");
    } else {
        v = config.check("audio",
                         Value("/dev/dsp"),
                         "optional audio device name");
    }
    yCInfo(FFMPEGGRABBER, "Device %s",v.asString().c_str());

    m_uri = v.asString();

    if (audio) {
        av_dict_set_int(&formatParams,
                        "sample_rate",
                        config.check("audio_rate",
                                     Value(44100),
                                     "audio sample rate").asInt32(),
                        0);
        av_dict_set_int(&formatParams,
                        "channels",
                        config.check("channels",
                                     Value(1),
                                     "number of channels").asInt32(),
                        0);
    } else {
        if (config.check("time_base_num") && config.check("time_base_den")) {
            char buf[256];
            sprintf(buf, "%d/%d",
                    config.check("time_base_num",
                                 Value(1),
                                 "numerator of basic time unit").asInt32(),
                    config.check("time_base_den",
                                 Value(29),
                                 "denominator of basic time unit").asInt32());
            av_dict_set(&formatParams, "framerate", buf, 0);
        }

        if (config.check("channel")) {
            av_dict_set_int(&formatParams,
                            "channel",
                            config.check("channel",
                                         Value(0),
                                         "channel identifier").asInt32(),
                            0);
        }
        if (config.check("standard")) {
            av_dict_set(&formatParams,
                        "standard",
                        config.check("standard",
                                     Value("-"),
                                     "pal versus ntsc").asString().c_str(),
                        0);
        }
        av_dict_set_int(&formatParams,
                        "width",
                        config.check("width",
                                     Value(640),
                                     "width of image").asInt32(),
                        0);
        av_dict_set_int(&formatParams,
                        "height",
                        config.check("height",
                                     Value(480),
                                     "height of image").asInt32(),
                        0);
    }

    std::string videoDevice = (config.check("v4l1") ? "video4linux" : "video4linux2");
    iformat = av_find_input_format(audio ? "audio_device" : videoDevice.c_str());

    int result = avformat_open_input(audio ? ppFormatCtx2 : ppFormatCtx,
                                     v.asString().c_str(),
                                     iformat,
                                     &formatParams);

    bool ok = (result==0);
    if (!ok) {
        yCError(FFMPEGGRABBER, "%s: ffmpeg error %d", v.asString().c_str(), result);
    }

    if (ok) {
        if (ppFormatCtx!=nullptr) {
            if (config.check("audio",
                             "optional audio device")) {
                ok = openV4L(config,nullptr,ppFormatCtx2);
            }
        }
    }

    return ok;
}



bool FfmpegGrabber::openFirewire(yarp::os::Searchable & config,
                                 AVFormatContext **ppFormatCtx)
{
    AVInputFormat *iformat;
    std::string devname = config.check("devname",
                                       Value("/dev/dv1394"),
                                       "firewire device name").asString();
    iformat = av_find_input_format("dv1394");
    yCInfo(FFMPEGGRABBER, "Checking for digital video in %s", devname.c_str());

    m_uri = devname;

    return avformat_open_input(ppFormatCtx, strdup(devname.c_str()), iformat, nullptr) == 0;
}


bool FfmpegGrabber::openFile(AVFormatContext **ppFormatCtx,
                             const char *fname)
{
    m_uri = fname;
    return avformat_open_input(ppFormatCtx, fname, nullptr, nullptr) == 0;
}


bool FfmpegGrabber::open(yarp::os::Searchable & config)
{
    std::string fname =
        config.check("source",
                     Value("default.avi"),
                     "media file to read from").asString();

    if (config.check("loop","media should loop (default)")) {
        shouldLoop = true;
    }

    if (config.check("noloop","media should not loop")) {
        shouldLoop = false;
    }

    imageSync = false;
    std::string sync =
        config.check("sync",
                     Value("image"),
                     "sync on image or audio (if have to choose)?").asString();
    imageSync = (sync=="image");

    needRateControl = true; // default for recorded media

    if (config.check("nodelay","media will play in simulated realtime unless this is present")) {
        needRateControl = false;
    }

    pace = config.check("pace",Value(1.0),
                        "simulated realtime multiplier factor (must be <1 right now)").asFloat64();

    // Register all formats and codecs
    av_register_all();
    avdevice_register_all();

    // Open video file
    if (config.check("v4l","if present, read from video4linux") || config.check("v4l1","if present, read from video4linux") || config.check("v4l2","if present, read from video4linux2")) {
        needRateControl = false; // reading from live media
        if (!openV4L(config,&pFormatCtx,&pFormatCtx2)) {
            yCError(FFMPEGGRABBER, "Could not open Video4Linux input");
            return false;
        }
    } else if (config.check("ieee1394","if present, read from firewire")) {
        needRateControl = false; // reading from live media
        if (!openFirewire(config,&pFormatCtx)) {
            yCError(FFMPEGGRABBER, "Could not open ieee1394 input");
            return false;
        }
    } else {
        if (!openFile(&pFormatCtx,fname.c_str())) {
            yCError(FFMPEGGRABBER, "Could not open media file %s", fname.c_str());
            return false; // Couldn't open file
        }
    }


    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, nullptr)<0) {
        yCError(FFMPEGGRABBER, "Could not find stream information in %s", m_uri.c_str());
        return false; // Couldn't find stream information
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, m_uri.c_str(), false);

    if (pFormatCtx2!=nullptr) {

        if(avformat_find_stream_info(pFormatCtx2, nullptr)<0) {
            yCError(FFMPEGGRABBER, "Could not find stream information in %s", m_uri.c_str());
            return false; // Couldn't find stream information
        }

        // Dump information about file onto standard error
        av_dump_format(pFormatCtx2, 0, m_uri.c_str(), false);
    }


    if (pFormatCtx2!=nullptr) {
        pAudioFormatCtx = pFormatCtx2;
    } else {
        pAudioFormatCtx = pFormatCtx;
    }

    yCAssert(FFMPEGGRABBER, system_resource == nullptr);
    system_resource = new FfmpegHelper;
    yCAssert(FFMPEGGRABBER, system_resource != nullptr);

    FfmpegHelper& helper = HELPER(system_resource);
    DecoderState& videoDecoder = helper.videoDecoder;
    DecoderState& audioDecoder = helper.audioDecoder;


    // Find the first video stream
    int videoStream = videoDecoder.getStream(pFormatCtx,
                                             AVMEDIA_TYPE_VIDEO,
                                             "video");
    // Find the first audio stream
    int audioStream = audioDecoder.getStream(pAudioFormatCtx,
                                             AVMEDIA_TYPE_AUDIO,
                                             "audio");

    if (videoStream==-1&&audioStream==-1) {
        return false;
    }

    _hasVideo = (videoStream!=-1);
    _hasAudio = (audioStream!=-1);

    bool ok = true;
    if (_hasVideo) {
        ok = ok && videoDecoder.getCodec(pFormatCtx);
    }
    if (_hasAudio) {
        ok = ok && audioDecoder.getCodec(pAudioFormatCtx);
    }
    if (!ok) return false;

    if (_hasVideo) {
        ok = ok && videoDecoder.allocateImage();
    }
    if (_hasAudio) {
        ok = ok && audioDecoder.allocateSound();
    }
    if (!ok) return false;

    if (_hasVideo) {
        m_w = videoDecoder.getWidth();
        m_h = videoDecoder.getHeight();
    }
    if (_hasAudio) {
        m_channels = audioDecoder.getChannels();
        m_rate = audioDecoder.getRate();
    }
    yCInfo(FFMPEGGRABBER,
           "  video size %dx%d, audio %dHz with %d channels, %s sync",
           m_w,
           m_h,
           m_rate,
           m_channels,
           imageSync ? "image" : "audio");

    if (!(_hasVideo||_hasAudio)) {
        return false;
    }
    active = true;
    return true;
}

bool FfmpegGrabber::close()
{
    if (formatParamsVideo) {
        av_dict_free(&formatParamsVideo);
        formatParamsVideo = nullptr;
    }
    if (formatParamsAudio) {
        av_dict_free(&formatParamsAudio);
        formatParamsAudio = nullptr;
    }

    if (!active) {
        return false;
    }

    // Close the video file
    if (pFormatCtx!=nullptr) {
        avformat_close_input(&pFormatCtx);
    }
    if (pFormatCtx2!=nullptr) {
        avformat_close_input(&pFormatCtx2);
    }
    if (system_resource!=nullptr) {
        delete &HELPER(system_resource);
        system_resource = nullptr;
    }

    active = false;
    return true;
}

bool FfmpegGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image)
{
    if (!_hasVideo) {
        return false;
    }
    Sound sound;
    return getAudioVisual(image,sound);
}

bool FfmpegGrabber::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    if (!_hasAudio) {
        return false;
    }
    ImageOf<PixelRgb> image;
    return getAudioVisual(image, sound);
}


bool FfmpegGrabber::getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                   yarp::sig::Sound& sound)
{

    FfmpegHelper& helper = HELPER(system_resource);
    DecoderState& videoDecoder = helper.videoDecoder;
    DecoderState& audioDecoder = helper.audioDecoder;

    bool tryAgain = false;
    bool triedAgain = false;

    do {

        bool gotAudio = false;
        bool gotVideo = false;
        if (startTime<0.5) {
            startTime = SystemClock::nowSystem();
        }
        double time_target = 0;
        while(av_read_frame(pFormatCtx, &packet)>=0) {
            // Is this a packet from the video stream?
            bool done = false;
            if (packet.stream_index==videoDecoder.getIndex()) {
                done = videoDecoder.getVideo(packet);
                image.resize(1,1);
                if (done) {
                    yCTrace(FFMPEGGRABBER, "got a video frame");
                    gotVideo = true;
                }
            } if (packet.stream_index==audioDecoder.getIndex()) {
                done = audioDecoder.getAudio(packet,sound);
                if (done) {
                    yCTrace(FFMPEGGRABBER, "got an audio frame");
                    gotAudio = true;
                }
            }
            AVRational& time_base = pFormatCtx->streams[packet.stream_index]->time_base;
            double rbase = av_q2d(time_base);

            time_target = packet.pts*rbase;

            av_free_packet(&packet);
            if (((imageSync?gotVideo:videoDecoder.haveFrame())||!_hasVideo)&&
                ((imageSync?1:gotAudio)||!_hasAudio)) {
                if (_hasVideo) {
                    videoDecoder.getVideo(image);
                } else {
                    image.resize(0,0);
                }
                if (needRateControl) {
                    double now = (SystemClock::nowSystem()-startTime)*pace;
                    double delay = time_target-now;
                    if (delay>0) {
                        SystemClock::delaySystem(delay);
                    }
                }

                if (!_hasAudio) {
                    sound.resize(0,0);
                }
                return true;
            }
        }

        tryAgain = !triedAgain;

        if (tryAgain) {
            if (!shouldLoop) {
                return false;
            }
            av_seek_frame(pFormatCtx,-1,0,AVSEEK_FLAG_BACKWARD);
            startTime = SystemClock::nowSystem();
            triedAgain = true;
        }
    } while (tryAgain);

    return false;
}
