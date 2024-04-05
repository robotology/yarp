/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FfmpegGrabber_INC
#define FfmpegGrabber_INC

#include "avpreamble.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

/*
 * A YARP frame grabber device driver using ffmpeg to implement
 * image capture from AVI files.
 */

#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/DeviceDriver.h>

#include "ffmpeg_api.h"
#include "FfmpegGrabber_ParamsParser.h"

/**
 * @ingroup dev_impl_media
 *
 * \brief `ffmpeg_grabber`: An image frame grabber device using ffmpeg to
 * capture images from AVI files.
 *
 * Parameters required by this device are shown in class: FfmpegGrabber_ParamsParser
 */
class FfmpegGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IAudioGrabberSound,
        public yarp::dev::IAudioVisualGrabber,
        public yarp::dev::IAudioVisualStream,
        public yarp::dev::DeviceDriver,
        public FfmpegGrabber_ParamsParser
{
public:

    FfmpegGrabber() :
        active(false),
        startTime(0),
        needRateControl(false),
        imageSync(false)
    {
        memset(&packet,0,sizeof(packet));
    }

    bool open(yarp::os::Searchable & config) override;

    bool close() override;

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;

    bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;

    int height() const override { return m_h; }

    int width() const override { return m_w; }

    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) override;


    bool hasAudio() override {
        return _hasAudio;
    }

    bool hasVideo() override {
        return _hasVideo;
    }

    bool startRecording() override {
        return true;
    }

    bool stopRecording() override {
        return true;
    }

    bool isRecording(bool& recording_enabled) override {
        return false;
    }

    bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize&) override {
        return false;
    }

    bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize&) override {
        return false;
    }

    bool resetRecordingAudioBuffer() override {
        return false;
    }

    bool setHWGain(double gain) override {
        return false;
    }

    bool setSWGain(double gain) override {
        return false;
    }

protected:
    void *system_resource = nullptr;

    AVDictionary* formatParamsVideo = nullptr;
    AVDictionary* formatParamsAudio = nullptr;
    AVFormatContext *pFormatCtx = nullptr;
    AVFormatContext *pFormatCtx2 = nullptr;
    AVFormatContext *pAudioFormatCtx = nullptr;
    AVPacket packet;
    bool active;
    double startTime;
    bool _hasAudio = false;
    bool _hasVideo = false;
    bool needRateControl;
    bool imageSync;

    /** Uri of the images a grabber produces. */
    std::string m_uri;

    /** Width of the images a grabber produces. */
    int m_w = 0;
    /** Height of the images a grabber produces. */
    int m_h = 0;

    int m_channels = 0;
    int m_rate = 0;

    /** Opaque ffmpeg structure for image capture. */
    void * m_capture = nullptr;

    bool openFirewire(yarp::os::Searchable & config,
                      AVFormatContext **ppFormatCtx);

    bool openV4L(yarp::os::Searchable & config,
                 AVFormatContext **ppFormatCtx,
                 AVFormatContext **ppFormatCtx2);

    bool openFile(AVFormatContext **ppFormatCtx,
                  const char *fname);
};

#endif
