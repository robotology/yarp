/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 *
 */


#ifndef FfmpegGrabber_INC
#define FfmpegGrabber_INC

#include "avpreamble.h"

extern "C" {
#include <avcodec.h>
#include <avformat.h>
}

/*
 * A Yarp 2 frame grabber device driver using ffmpeg to implement
 * image capture from AVI files.
 *
 */

namespace yarp {
    namespace dev {
        class FfmpegGrabber;
    }
}

#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/DeviceDriver.h>

#include "ffmpeg_api.h"

/**
 * @ingroup dev_impl_media
 *
 * An image frame grabber device using ffmpeg to capture images from
 * AVI files.
 */
class yarp::dev::FfmpegGrabber : public IFrameGrabberImage,
            public IAudioGrabberSound,
            public IAudioVisualGrabber,
            public IAudioVisualStream,
            public DeviceDriver
{
public:

    FfmpegGrabber() :
        system_resource(nullptr),
        formatParamsVideo(nullptr),
        formatParamsAudio(nullptr),
        pFormatCtx(nullptr),
        pFormatCtx2(nullptr),
        pAudioFormatCtx(nullptr),
        active(false),
        startTime(0),
        _hasAudio(false),
        _hasVideo(false),
        needRateControl(false),
        shouldLoop(true),
        pace(1),
        imageSync(false),
        m_w(0),
        m_h(0),
        m_channels(0),
        m_rate(0),
        m_capture(nullptr)
    {
        memset(&packet,0,sizeof(packet));
    }

    virtual bool open(yarp::os::Searchable & config) override;

    virtual bool close() override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;

    virtual bool getSound(yarp::sig::Sound& sound) override;

    virtual int height() const override { return m_h; }

    virtual int width() const override { return m_w; }

    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) override;


    virtual bool hasAudio() override {
        return _hasAudio;
    }

    virtual bool hasVideo() override {
        return _hasVideo;
    }

    virtual bool startRecording() override {
        return true;
    }

    virtual bool stopRecording() override {
        return true;
    }

protected:
    void *system_resource;

    AVDictionary* formatParamsVideo;
    AVDictionary* formatParamsAudio;
    AVFormatContext *pFormatCtx;
    AVFormatContext *pFormatCtx2;
    AVFormatContext *pAudioFormatCtx;
    AVPacket packet;
    bool active;
    double startTime;
    bool _hasAudio, _hasVideo;
    bool needRateControl;
    bool shouldLoop;
    double pace;
    bool imageSync;

    /** Uri of the images a grabber produces. */
    std::string m_uri;

    /** Width of the images a grabber produces. */
    int m_w;
    /** Height of the images a grabber produces. */
    int m_h;

    int m_channels;
    int m_rate;

    /** Opaque ffmpeg structure for image capture. */
    void * m_capture;

    bool openFirewire(yarp::os::Searchable & config,
                      AVFormatContext **ppFormatCtx);

    bool openV4L(yarp::os::Searchable & config,
                 AVFormatContext **ppFormatCtx,
                 AVFormatContext **ppFormatCtx2);

    bool openFile(AVFormatContext **ppFormatCtx,
                  const char *fname);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_ffmpeg_grabber ffmpeg_grabber

 A wrapper for the ffmpeg library's image sources, see yarp::dev::FfmpegGrabber.

*/


#endif
