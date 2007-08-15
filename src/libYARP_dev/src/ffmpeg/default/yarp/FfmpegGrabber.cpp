// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Jonas Ruesch
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/FfmpegGrabber.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/Logger.h>

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

#include <stdio.h>

#if LIBAVCODEC_BUILD < 4754
#error "ffmpeg version is too old, sorry - please download and compile newer version"
#endif

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

#define DBG if (0)


class DecoderState {
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
    uint8_t         *buffer;
    int16_t         *audioBuffer;
    int16_t         *audioBufferAt;
    int audioBufferLen;

    DecoderState() {
        index = -1;
        pCodec = NULL;
        pCodecCtx = NULL;
        pFrame = NULL;
        pFrameRGB = NULL;
        buffer = NULL;
        audioBuffer = NULL;
        audioBufferAt = NULL;
        audioBufferLen = 0;
        frameFinished = 0;
    }

    bool isFinished() {
        return frameFinished!=0;
    }

    int getIndex() {
        return index;
    }

    virtual ~DecoderState() {
        if (pCodecCtx!=NULL) {
            avcodec_close(pCodecCtx);
        }
        if (audioBuffer!=NULL) {
            delete [] audioBuffer;
        }
        if (buffer!=NULL) {
            delete [] buffer;
        }
        if (pFrameRGB!=NULL) {
            av_free(pFrameRGB);
        }
        if (pFrame!=NULL) {
            av_free(pFrame);
        }
    }

    int getStream(AVFormatContext *pFormatCtx, CodecType code, 
                  const char *name) {
        // Find the first stream
        int videoStream=-1;
        for(int i=0; i<pFormatCtx->nb_streams; i++)
            if(pFormatCtx->streams[i]->codec->codec_type==code)
                {
                    videoStream=i;
                    printf("First %s stream is stream #%d\n", name, i);
                    break;
                }
        if(videoStream==-1) {
            printf("Could not find %s stream\n", name);
        }
        index = videoStream;

        return index;
    }

    bool getCodec(AVFormatContext *pFormatCtx) {
        // Get a pointer to the codec context for the video stream
        pCodecCtx=pFormatCtx->streams[index]->codec;

        // Find the decoder for the video stream
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec==NULL) {
            printf("Codec not found\n");
            return false; // Codec not found
        }
        
        // Open codec
        if(avcodec_open(pCodecCtx, pCodec)<0) {
            printf("Could not open codec\n");
            return false; // Could not open codec
        }

        return true;
    }


    bool allocateImage() {
        // Allocate video frame
        pFrame=avcodec_alloc_frame();
        
        // Allocate an AVFrame structure
        pFrameRGB=avcodec_alloc_frame();
        if(pFrameRGB==NULL) {
            printf("Could not allocate a frame\n");
            return false;
        }
        
        // Determine required buffer size and allocate buffer
        int numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                        pCodecCtx->height);
        buffer=new uint8_t[numBytes];
        
        // Assign appropriate parts of buffer to image planes in pFrameRGB
        avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                       pCodecCtx->width, pCodecCtx->height);
        return true;
    }

    bool allocateSound() {
        audioBuffer = new int16_t[AVCODEC_MAX_AUDIO_FRAME_SIZE];
        audioBufferAt = audioBuffer;
        audioBufferLen = 0;
        printf("channels %d, sample_rate %d, frame_size %d\n",
               pCodecCtx->channels,
               pCodecCtx->sample_rate,
               pCodecCtx->frame_size);
        return true;
    }

    int getWidth() {
        return pCodecCtx->width;
    }

    int getHeight() {
        return pCodecCtx->height;
    }


    int getRate() {
        return pCodecCtx->sample_rate;
    }

    int getChannels() {
        return pCodecCtx->channels;
    }

    bool getAudio(AVPacket& packet,Sound& sound) {
        int ct = 0;
        int bytesRead = 0;
        int bytesWritten = 0;
        while (bytesRead<packet.size) {
            int r = avcodec_decode_audio(pCodecCtx, 
                                         audioBuffer+bytesWritten, 
                                         &ct,
                                         packet.data+bytesRead, 
                                         packet.size-bytesRead);
            if (r<0) {
                printf("error decoding audio\n");
                return false;
            }
            DBG printf("audio bytes %d return %d\n", ct, r);
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

    bool getVideo(AVPacket& packet) {
        // Decode video frame
        avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                             packet.data, packet.size);
        
        // Did we get a video frame?
        if(frameFinished) {
            // Convert the image from its native format to RGB
            img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                        (AVPicture*)pFrame, pCodecCtx->pix_fmt, 
                        pCodecCtx->width, 
                        pCodecCtx->height);
        }
        return frameFinished;
    }


    bool getVideo(ImageOf<PixelRgb>& image) {
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

    bool haveFrame() {
        return frameFinished;
    }
};

class FfmpegHelper {
public:
    DecoderState videoDecoder;
    DecoderState audioDecoder;
};


#define HELPER(x) (*((FfmpegHelper*)x))



bool FfmpegGrabber::openV4L(yarp::os::Searchable & config, 
                            AVFormatContext **ppFormatCtx) {

    AVFormatParameters formatParams;
    AVInputFormat *iformat;
	
	formatParams.device = strdup(config.check("v4ldevice",
                                              Value("/dev/video0"),
                                              "device name").asString().c_str());
    formatParams.channel = config.check("channel",Value(0),
                                        "channel identifier").asInt();
    formatParams.standard = strdup(config.check("standard",
                                                Value("ntsc"),
                                                "pal versus ntsc").asString().c_str());
    formatParams.width = config.check("width",Value(640),"width of image").asInt();
    formatParams.height = config.check("height",Value(480),"height of image").asInt();
    formatParams.time_base.den = config.check("time_base_den",
                                              Value(29),
                                              "denominator of basic time unit").asInt();
    formatParams.time_base.num = config.check("time_base_num",
                                              Value(1),
                                              "numerator of basic time unit").asInt();

    iformat = av_find_input_format("video4linux");

    return (av_open_input_file(ppFormatCtx,
                               formatParams.device, iformat, 0, 
                               &formatParams)==0);

}



bool FfmpegGrabber::openFirewire(yarp::os::Searchable & config, 
                                 AVFormatContext **ppFormatCtx) {
    AVFormatParameters formatParams;
    AVInputFormat *iformat;
    ConstString devname = config.check("devname",
                                       Value("/dev/dv1394"),
                                       "firewire device name").asString();
    formatParams.device = devname.c_str();
    iformat = av_find_input_format("dv1394");
    printf("Checking for digital video in %s\n", devname.c_str());
    return av_open_input_file(ppFormatCtx,
                              "", iformat, 0, &formatParams)==0;
}


bool FfmpegGrabber::openFile(AVFormatContext **ppFormatCtx,
                             const char *fname) {
    return av_open_input_file(ppFormatCtx, fname, NULL, 0, NULL)==0;
}


bool FfmpegGrabber::open(yarp::os::Searchable & config) {

    ConstString fname = 
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
    ConstString sync = 
        config.check("sync",
                     Value("image"),
                     "sync on image or audio (if have to choose)?").asString();
    imageSync = (sync=="image");

    needRateControl = true; // default for recorded media

    if (config.check("nodelay","media will play in simulated realtime unless this is present")) {
        needRateControl = false;
    }

    pace = config.check("pace",Value(1.0),
                        "simulated realtime multiplier factor (must be <1 right now)").asDouble();
    
    // Register all formats and codecs
    av_register_all();

    // Open video file
    if (config.check("v4l","if present, read from video4linux")) {
        needRateControl = false; // reading from live media
        if (!openV4L(config,&pFormatCtx)) {
            printf("Could not open Video4Linux input\n");
            return false;
        }
    } else if (config.check("ieee1394","if present, read from firewire")) {
        needRateControl = false; // reading from live media
        if (!openFirewire(config,&pFormatCtx)) {
            printf("Could not open ieee1394 input\n");
            return false;
        }
    } else {
        if (!openFile(&pFormatCtx,fname.c_str())) {
            printf("Could not open media file %s\n", fname.c_str());
            return false; // Couldn't open file
        }
    }
        
    // Retrieve stream information
    if(av_find_stream_info(pFormatCtx)<0) {
        printf("Could not find stream information in %s\n", fname.c_str());
        return false; // Couldn't find stream information
    }

    // Dump information about file onto standard error
    dump_format(pFormatCtx, 0, fname.c_str(), false);

    YARP_ASSERT(system_resource==NULL);
    system_resource = new FfmpegHelper;
    YARP_ASSERT(system_resource!=NULL);
    FfmpegHelper& helper = HELPER(system_resource);
    DecoderState& videoDecoder = helper.videoDecoder;
    DecoderState& audioDecoder = helper.audioDecoder;


    // Find the first video stream
    int videoStream = videoDecoder.getStream(pFormatCtx,CODEC_TYPE_VIDEO,
                                             "video");
    // Find the first audio stream
    int audioStream = audioDecoder.getStream(pFormatCtx,CODEC_TYPE_AUDIO,
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
        ok = ok && audioDecoder.getCodec(pFormatCtx);
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
    printf("  video size %dx%d, audio %dHz with %d channels, %s sync\n", 
           m_w, m_h,
           m_rate, m_channels,
           imageSync?"image":"audio");

    if (!(_hasVideo||_hasAudio)) {
        return false;
    }
    active = true;
    return true;
}
  
bool FfmpegGrabber::close() {
    if (!active) {
        return false;
    }
    
    // Close the video file
    if (pFormatCtx!=NULL) {
        av_close_input_file(pFormatCtx);
    }
    if (system_resource!=NULL) {
        delete &HELPER(system_resource);
        system_resource = NULL;
    }

    active = false;
    return true;
}
  
bool FfmpegGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {
    if (!_hasVideo) {
        return false;
    }
    Sound sound;
    return getAudioVisual(image,sound);
}


bool FfmpegGrabber::getSound(yarp::sig::Sound& sound) {
    if (!_hasAudio) {
        return false;
    }
    ImageOf<PixelRgb> image;
    return getAudioVisual(image,sound);
}

  
bool FfmpegGrabber::getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                   yarp::sig::Sound& sound) {

    FfmpegHelper& helper = HELPER(system_resource);
    DecoderState& videoDecoder = helper.videoDecoder;
    DecoderState& audioDecoder = helper.audioDecoder;

    bool tryAgain = false;
    bool triedAgain = false;
    
    do {
    
        bool gotAudio = false;
        bool gotVideo = false;
        if (startTime<0.5) {
            startTime = Time::now();
        }
        double time_target = 0;
        while(av_read_frame(pFormatCtx, &packet)>=0) {
            // Is this a packet from the video stream?
            DBG printf("frame ");
            bool done = false;
            if (packet.stream_index==videoDecoder.getIndex()) {
                DBG printf("video ");
                done = videoDecoder.getVideo(packet);
                image.resize(1,1);
                if (done) {
                    //printf("got a video frame\n");
                    gotVideo = true;
                }
            } if (packet.stream_index==audioDecoder.getIndex()) {
                DBG printf("audio ");
                done = audioDecoder.getAudio(packet,sound);
                if (done) {
                    //printf("got an audio frame\n");
                    gotAudio = true;
                }
            } else {
                DBG printf("other ");
            }
            AVRational& time_base = pFormatCtx->streams[packet.stream_index]->time_base;
            double rbase = av_q2d(time_base);
            
            DBG printf(" time=%g ", packet.pts*rbase);
            time_target = packet.pts*rbase;
            
            av_free_packet(&packet);
            DBG printf(" %d\n", done);
            if (((imageSync?gotVideo:videoDecoder.haveFrame())||!_hasVideo)&&
                ((imageSync?1:gotAudio)||!_hasAudio)) {
                if (_hasVideo) {
                    videoDecoder.getVideo(image);
                } else {
                    image.resize(0,0);
                }
                if (needRateControl) {
                    double now = (Time::now()-startTime)*pace;
                    double delay = time_target-now;
                    if (delay>0) {
                        DBG printf("DELAY %g ", delay);
                        Time::delay(delay);
                    } else {
                        DBG printf("NODELAY %g ", delay);
                    }
                }
                DBG printf("IMAGE size %dx%d  ", image.width(), image.height());
                DBG printf("SOUND size %d\n", sound.getSamples());
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
#if LIBAVFORMAT_BUILD > 4616
            av_seek_frame(pFormatCtx,-1,0,AVSEEK_FLAG_BACKWARD);
#else
            av_seek_frame(pFormatCtx,-1,0);
#endif
            startTime = Time::now();
        }
    } while (tryAgain);

    return false;
}


