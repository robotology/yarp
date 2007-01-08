// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef FfmpegWriter_INC
#define FfmpegWriter_INC

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

/*
 * Uses ffmpeg to write images to movie files.
 *
 * Very rudimentary so far - just writes video not audio.
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
 * Uses ffmpeg to write images to movie files (AVI, MOV, ...).
 *
 */
class yarp::dev::FfmpegWriter : public IFrameWriterImage, 
            public DeviceDriver
{
public:

    FfmpegWriter() {
        system_resource = NULL;
        ready = false;
    }
  
    virtual bool open(yarp::os::Searchable & config);
  
    virtual bool close();
  
    virtual bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);

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

