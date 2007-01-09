// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef FfmpegGrabber_INC
#define FfmpegGrabber_INC

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

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

    FfmpegGrabber() {
        m_h = m_w = 0;
        m_channels = m_rate = 0;
        pFormatCtx = NULL;
        packet.data = NULL;
        active = false;
        startTime = 0;
        _hasAudio = _hasVideo = false;
        system_resource = NULL;
        needRateControl = false;
        shouldLoop = true;
    }
  
    virtual bool open(yarp::os::Searchable & config);
  
    virtual bool close();
  
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);

    virtual bool getSound(yarp::sig::Sound& sound);

    virtual int height() const { return m_h; }
  
    virtual int width() const { return m_w; }
  
    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound);
  

    virtual bool hasAudio() {
        return _hasAudio;
    }

    virtual bool hasVideo() {
        return _hasVideo;
    }

protected:
    void *system_resource;

    AVFormatContext *pFormatCtx;
    AVPacket packet;
    bool active;
    double startTime;
    bool _hasAudio, _hasVideo;
    bool needRateControl;
    bool shouldLoop;
  
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
                 AVFormatContext **ppFormatCtx);

    bool openFile(AVFormatContext **ppFormatCtx,
                  const char *fname);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_ffmpeg ffmpeg_grabber

 A wrapper for the ffmpeg library's image sources, see yarp::dev::FfmpegGrabber.

*/


#endif

