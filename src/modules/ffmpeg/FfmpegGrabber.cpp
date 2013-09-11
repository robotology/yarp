// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium, Jonas Ruesch, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Jonas Ruesch, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "FfmpegGrabber.h"
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "ffmpeg_api.h"

#include <stdio.h>

#define ERROR_PROBLEM

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

#define DBG if (0)



static void print_error(const char *filename, int err) {
#ifdef ERROR_PROBLEM
    fprintf(stderr, "%s: ffmpeg error %d\n", filename,err);
#else
    switch(err) {
    case AVERROR_NUMEXPECTED:
        fprintf(stderr, "%s: Incorrect image filename syntax.\n"
                "Use '%%d' to specify the image number:\n"
                "  for img1.jpg, img2.jpg, ..., use 'img%%d.jpg';\n"
                "  for img001.jpg, img002.jpg, ..., use 'img%%03d.jpg'.\n",
                filename);
        break;
    case AVERROR_INVALIDDATA:
        fprintf(stderr, "%s: Error while parsing header\n", filename);
        break;
    case AVERROR_NOFMT:
        fprintf(stderr, "%s: Unknown format\n", filename);
        break;
    case AVERROR(EIO):
        fprintf(stderr, "%s: I/O error occured\n",
                filename);
        break;
    case AVERROR(ENOMEM):
        fprintf(stderr, "%s: memory allocation error occured\n", filename);
        break;
    case AVERROR(ENOENT):
        fprintf(stderr, "%s: no such file or directory\n", filename);
        break;
    default:
        fprintf(stderr, "%s: Error while opening file (%d)\n", 
                filename, err);
        break;
    }
#endif
}

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
        for(int i=0; i<(int)(pFormatCtx->nb_streams); i++)
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
        if(YARP_avcodec_open(pCodecCtx, pCodec)<0) {
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
        audioBufferLen = AVCODEC_MAX_AUDIO_FRAME_SIZE*10;
        audioBuffer = new int16_t[audioBufferLen];
        audioBufferAt = audioBuffer;
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
            ct = audioBufferLen;
#if LIBAVCODEC_BUILD < 65536
            int r = avcodec_decode_audio(pCodecCtx, 
                                         audioBuffer+bytesWritten, 
                                         &ct,
                                         packet.data+bytesRead, 
                                         packet.size-bytesRead);
#else
#  ifdef FFEPOCH3            
            AVPacket tmp = packet;
            tmp.data += bytesRead;
            tmp.size -= bytesRead;
            int r = avcodec_decode_audio3(pCodecCtx, 
                                          audioBuffer+bytesWritten, 
                                          &ct,
                                          &packet);
#  else 
            int r = avcodec_decode_audio2(pCodecCtx, 
                                          audioBuffer+bytesWritten, 
                                          &ct,
                                          packet.data+bytesRead, 
                                          packet.size-bytesRead);
#  endif
#endif
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
#ifdef FFEPOCH3
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, 
                              &packet);
#else
        avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                             packet.data, packet.size);
#endif
        
        // Did we get a video frame?
        if(frameFinished) {
            // Convert the image from its native format to RGB
#ifdef OLD_FFMPEG
            img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                        (AVPicture*)pFrame, pCodecCtx->pix_fmt, 
                        pCodecCtx->width, 
                        pCodecCtx->height);
#else
            int w = pCodecCtx->width;
            int h = pCodecCtx->height;
            static struct SwsContext *img_convert_ctx = NULL;
            if (img_convert_ctx==NULL) {
                img_convert_ctx = sws_getContext(w,h,
                                                 pCodecCtx->pix_fmt, 
                                                 w, h, PIX_FMT_RGB24, 
                                                 //0,
                                                 //SWS_BILINEAR,
                                                 SWS_BICUBIC, 
                                                 NULL, NULL, NULL);
            }
            if (img_convert_ctx!=NULL) {
                sws_scale(img_convert_ctx, ((AVPicture*)pFrame)->data, 
                          ((AVPicture*)pFrame)->linesize, 0, 
                          pCodecCtx->height, 
                          ((AVPicture*)pFrameRGB)->data, 
                          ((AVPicture*)pFrameRGB)->linesize);
            } else {
                printf("Software scaling not working\n");
                ::exit(1);
            }

#endif
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


const char *xstrdup(const char *str) {
    if (str[0]=='-') return NULL;
    return strdup(str);
}

bool FfmpegGrabber::openV4L(yarp::os::Searchable & config, 
                            AVFormatContext **ppFormatCtx,
                            AVFormatContext **ppFormatCtx2) {

#ifdef NO_DEVICE
    printf("FFMPEG interface changed, YARP driver not updated yet - sorry\n");
    return false;

#else
	bool audio = (ppFormatCtx==NULL);
    YARP_AVDICT& formatParams = 
        *(audio?(&formatParamsAudio):(&formatParamsVideo));

    AVInputFormat *iformat;
    Value v;

    YARP_AVDICT_CLEAN(formatParams);
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
    printf("Device %s\n",v.asString().c_str());
#ifndef FACTORED_DEVICE
    YARP_AVDICT_SET_STR(formatParams,device,v.asString().c_str());
#endif

    if (audio) {
        YARP_AVDICT_SET_INT(formatParams,sample_rate,
                            config.check("audio_rate",
                                         Value(44100),
                                         "audio sample rate").asInt());
        YARP_AVDICT_SET_INT(formatParams,channels,
                            config.check("channels",Value(1),
                                         "number of channels").asInt());
    } else {
        if (config.check("time_base_num") && config.check("time_base_den")) {
            YARP_AVDICT_SET_FRAC(formatParams,time_base,framerate,
                                 config.check("time_base_num",
                                              Value(1),
                                              "numerator of basic time unit").asInt(),
                                 config.check("time_base_den",
                                              Value(29),
                                              "denominator of basic time unit").asInt());
        }
        if (config.check("channel")) {
            YARP_AVDICT_SET_INT(formatParams,channel,
                                config.check("channel",Value(0),
                                             "channel identifier").asInt());
        }
        if (config.check("standard")) {
            YARP_AVDICT_SET_STR(formatParams,standard,
                                config.check("standard",
                                             Value("-"),
                                             "pal versus ntsc").asString().c_str());
        }
        YARP_AVDICT_SET_INT(formatParams,width,
                            config.check("width",Value(640),"width of image").asInt());
        YARP_AVDICT_SET_INT(formatParams,height,
                            config.check("height",Value(480),"height of image").asInt());
    }

    ConstString videoDevice = (config.check("v4l1")?"video4linux":"video4linux2");
    iformat = av_find_input_format(audio?"audio_device":videoDevice.c_str());

    int result = YARP_AV_OPEN_INPUT_FILE(audio?ppFormatCtx2:ppFormatCtx,
                                         v.asString().c_str(),
                                         iformat, 
                                         &formatParams);

    bool ok = (result==0);
    if (!ok) {
        print_error(v.asString().c_str(),result);
    }

    if (ok) {
        if (ppFormatCtx!=NULL) {
            if (config.check("audio",
                             "optional audio device")) {
                ok = openV4L(config,NULL,ppFormatCtx2);
            }
        }
    }

    return ok;

#endif

}



bool FfmpegGrabber::openFirewire(yarp::os::Searchable & config, 
                                 AVFormatContext **ppFormatCtx) {

#ifdef NO_DEVICE
    printf("FFMPEG interface changed, YARP driver not updated yet - sorry\n");
    return false;

#else
    AVFormatParameters formatParams;
    AVInputFormat *iformat;
    ConstString devname = config.check("devname",
                                       Value("/dev/dv1394"),
                                       "firewire device name").asString();
#ifndef FACTORED_DEVICE
    formatParams.device = devname.c_str();
#endif
    iformat = av_find_input_format("dv1394");
    printf("Checking for digital video in %s\n", devname.c_str());
    return av_open_input_file(ppFormatCtx,
#ifndef FACTORED_DEVCE
                              "",
#else
                              strdup(devname.asString().c_str()), 
#endif
                              iformat, 0, &formatParams)==0;
#endif
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
#ifdef FACTORED_DEVICE
    avdevice_register_all();
#endif

    // Open video file
    if (config.check("v4l","if present, read from video4linux") || config.check("v4l1","if present, read from video4linux") || config.check("v4l2","if present, read from video4linux2")) {
        needRateControl = false; // reading from live media
        if (!openV4L(config,&pFormatCtx,&pFormatCtx2)) {
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

    if (pFormatCtx2!=NULL) {
        
        if(av_find_stream_info(pFormatCtx2)<0) {
            printf("Could not find stream information in %s\n", fname.c_str());
            return false; // Couldn't find stream information
        }
        
        // Dump information about file onto standard error
        dump_format(pFormatCtx2, 0, fname.c_str(), false);
    }


    if (pFormatCtx2!=NULL) {
        pAudioFormatCtx = pFormatCtx2;
    } else {
        pAudioFormatCtx = pFormatCtx;
    }
        
    YARP_ASSERT(system_resource==NULL);
    system_resource = new FfmpegHelper;
    YARP_ASSERT(system_resource!=NULL);
    FfmpegHelper& helper = HELPER(system_resource);
    DecoderState& videoDecoder = helper.videoDecoder;
    DecoderState& audioDecoder = helper.audioDecoder;


    // Find the first video stream
    int videoStream = videoDecoder.getStream(pFormatCtx,
                                             CODEC_TYPE_VIDEO,
                                             "video");
    // Find the first audio stream
    int audioStream = audioDecoder.getStream(pAudioFormatCtx,
                                             CODEC_TYPE_AUDIO,
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
    YARP_AVDICT_DESTROY(formatParamsVideo);
    YARP_AVDICT_DESTROY(formatParamsAudio);

    if (!active) {
        return false;
    }
    
    // Close the video file
    if (pFormatCtx!=NULL) {
        av_close_input_file(pFormatCtx);
    }
    if (pFormatCtx2!=NULL) {
        av_close_input_file(pFormatCtx2);
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


