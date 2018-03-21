/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 *
 */


#ifndef FfmpegWriter_INC
#define FfmpegWriter_INC

#include "avpreamble.h"

extern "C" {
#include <avcodec.h>
#include <avformat.h>
}

/*
 * Uses ffmpeg to write images to movie files.
 *
 */

namespace yarp {
    namespace dev {
        class FfmpegWriter;
    }
}

#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/DeviceDriver.h>

/**
 * @ingroup dev_impl_media
 *
 * Uses ffmpeg to write images/sounds to movie files (AVI, MOV, ...).
 *
 */
class yarp::dev::FfmpegWriter : public IFrameWriterImage,
            public IFrameWriterAudioVisual,
            public DeviceDriver,
            public IAudioVisualStream
{
public:

    FfmpegWriter() :
        fmt(nullptr),
        oc(nullptr),
        audio_st(nullptr),
        video_st(nullptr),
        audio_pts(0.0),
        video_pts(0.0),
        picture(nullptr),
        tmp_picture(nullptr),
        video_outbuf(nullptr),
        frame_count(0),
        video_outbuf_size(0),
        ready(false),
        delayed(false)
    {
        system_resource = NULL;
    }

    virtual bool open(yarp::os::Searchable & config) override;

    virtual bool close() override;

    virtual bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;

    virtual bool putAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) override;


    virtual bool hasAudio() override { return true; }

    virtual bool hasVideo() override { return true; }

private:
    void *system_resource;

    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVStream *audio_st, *video_st;
    double audio_pts, video_pts;
    yarp::os::ConstString filename;
    yarp::os::Property savedConfig;
    AVFrame *picture, *tmp_picture;
    uint8_t *video_outbuf;
    int frame_count, video_outbuf_size;
    bool ready;
    bool delayed;

    virtual bool delayedOpen(yarp::os::Searchable & config);

    bool isOk() {
        if (delayed) {
            delayed = false;
            ready = delayedOpen(savedConfig);
            if (delayed) { ready = false; }
        }
        return ready;
    }

    void open_video(AVFormatContext *oc, AVStream *st);

    void write_video_frame(AVFormatContext *oc, AVStream *st,
                           yarp::sig::ImageOf<yarp::sig::PixelRgb>& img);

    void close_video(AVFormatContext *oc, AVStream *st);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_ffmpeg_writer ffmpeg_writer

 A wrapper for the ffmpeg library's media writing, see yarp::dev::FfmpegWriter.

*/


#endif
