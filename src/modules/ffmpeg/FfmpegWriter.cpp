// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 * Most of this file is from the output_example.c of ffmpeg -
 * copyright/copypolicy statement follows --
 *
 */

/*
 * Libavformat API example: Output a media file in any supported
 * libavformat format. The default codecs are used.
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "FfmpegWriter.h"
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "ffmpeg_api.h"

#include <stdio.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

#define DBG if (0)

//#define OMIT_AUDIO

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897931
#endif

//#include <ffmpeg/avformat.h>
//#include <ffmpeg/avcodec.h>

#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */
#define STREAM_PIX_WORK PIX_FMT_RGB24

/**************************************************************/
/* audio output */

float t, tincr, tincr2;

int16_t *samples;
int samples_size;
int samples_at;
int samples_channels;

uint8_t *audio_outbuf;
int audio_outbuf_size;
int audio_input_frame_size;

/*
 * add an audio output stream
 */
static AVStream *add_audio_stream(AVFormatContext *oc, CodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 1);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        ::exit(1);
    }

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_AUDIO;

    /* put sample parameters */
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;
    return st;
}

static void open_audio(AVFormatContext *oc, AVStream *st)
{
    printf("Opening audio stream\n");
    AVCodecContext *c;
    AVCodec *codec;

    c = st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "audio codec not found\n");
        ::exit(1);
    }

    /* open it */
    if (YARP_avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        ::exit(1);
    }

    /* init signal generator */
    t = 0;
    tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    audio_outbuf_size = 10000;
    audio_outbuf = (uint8_t*)av_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1) {
        audio_input_frame_size = audio_outbuf_size / c->channels;
        switch(st->codec->codec_id) {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            audio_input_frame_size >>= 1;
            break;
        default:
            break;
        }
    } else {
        audio_input_frame_size = c->frame_size;
    }
    samples_size = audio_input_frame_size;
    samples_at = 0;
    samples_channels = c->channels;
    samples = (int16_t*)av_malloc(samples_size*2*samples_channels);


    printf("FRAME SIZE is %d / samples size is %d\n", 
           c->frame_size,
           samples_size);
    ::exit(1);
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for(j=0;j<frame_size;j++) {
        v = (int)(sin(t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}

static void write_audio_frame(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;

    get_audio_frame(samples, audio_input_frame_size, c->channels);

    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

    pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_write_frame(oc, &pkt) != 0) {
        fprintf(stderr, "Error while writing audio frame\n");
        ::exit(1);
    } else {
        printf("Wrote some audio\n");
    }
}

static void write_audio_frame(AVFormatContext *oc, AVStream *st, Sound& snd)
{
    printf("Preparing to write audio (%d left over)\n", samples_at);
    AVCodecContext *c;
    int key = 1;

    c = st->codec;

    int at = 0;
    while (at<snd.getSamples()) {
        
        int avail = samples_size - samples_at;
        int remain = snd.getSamples() - at;
        int chan = snd.getChannels();
        if (remain<avail) { avail = remain; }
        for (int i=0; i<avail; i++) {
            int offset = samples_at*samples_channels;
            for (int j=0; j<samples_channels; j++) {
                samples[offset+j] = snd.get(at,j%chan);
            }
            samples_at++;
            at++;
        }
        avail = samples_size - samples_at;
    
        if (avail==0) {
            AVPacket pkt;
            av_init_packet(&pkt);
            pkt.size= avcodec_encode_audio(c, 
                                           audio_outbuf, 
                                           audio_outbuf_size, 
                                           samples);
        
            pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, 
                                  st->time_base);
            pkt.dts = pkt.pts;
            //printf("(%d)", pkt.size);
            if (key) {
                pkt.flags |= PKT_FLAG_KEY;
                key = 0;
            }
            pkt.stream_index= st->index;
            pkt.data = audio_outbuf;
            pkt.duration = 0;
            
            
            /* write the compressed frame in the media file */
            printf("+");
            fflush(stdout);
            if (av_write_frame(oc, &pkt) != 0) {
                fprintf(stderr, "Error while writing audio frame\n");
                ::exit(1);
            } else {
                printf(".");
            }
            samples_at = 0;
        }
    }
    printf(" wrote audio\n");
}

static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);

    av_free(samples);
    av_free(audio_outbuf);
}

/**************************************************************/
/* video output */


/* add a video output stream */
static AVStream *add_video_stream(AVFormatContext *oc, CodecID codec_id,
                                  int w, int h, int framerate)
{
    DBG printf("Video stream %dx%d\n", w, h);

    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        ::exit(1);
    }

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = w;
    c->height = h;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    c->time_base.den = framerate;
    c->time_base.num = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = STREAM_PIX_FMT;
    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        c->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name, "3gp"))
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;


    return st;
}

static AVFrame *alloc_picture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size((PixelFormat)pix_fmt, width, height);
    picture_buf = (uint8_t*)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   (PixelFormat)pix_fmt, width, height);
    return picture;
}

void FfmpegWriter::open_video(AVFormatContext *oc, AVStream *st)
{
    printf("Opening video stream\n");
    AVCodec *codec;
    AVCodecContext *c;

    c = st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "video codec not found\n");
        ::exit(1);
    }

    /* open the codec */
    if (YARP_avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        ::exit(1);
    }

    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 200000;
        video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        fprintf(stderr, "Could not allocate picture\n");
        ::exit(1);
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_RGB24) {
        tmp_picture = alloc_picture(PIX_FMT_RGB24, c->width, c->height);
        if (!tmp_picture) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            ::exit(1);
        } else {
            DBG printf("Allocated PIX_FMT_RGB24 image of dimensions %dx%d\n",
                       c->width, c->height);
        }
    }

    DBG printf("Video stream opened\n");
}

static void fill_rgb_image(AVFrame *pict, int frame_index, int width, 
                           int height, ImageOf<PixelRgb>& img)
{
    int x, y, i;

    i = frame_index;

    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            int base = y*(width*3);
            pict->data[0][base + x*3] = img.safePixel(x,y).r;
            pict->data[0][base +x*3+1] = img.safePixel(x,y).g;
            pict->data[0][base +x*3+2] = img.safePixel(x,y).b;
        }
    }
}


void FfmpegWriter::write_video_frame(AVFormatContext *oc, AVStream *st,
                                     ImageOf<PixelRgb>& img)
{
    DBG printf("Writing video stream\n");

    int out_size, ret;
    AVCodecContext *c;

    c = st->codec;

    if (c->pix_fmt != PIX_FMT_RGB24) {
        DBG printf("Converting to PIX_FMT_RGB24\n");
        fill_rgb_image(tmp_picture, frame_count, c->width, c->height, img);
        DBG printf("Converting to PIX_FMT_RGB24 (stable_img_convert)\n");
        stable_img_convert((AVPicture *)picture, c->pix_fmt,
                           (AVPicture *)tmp_picture, PIX_FMT_RGB24,
                           c->width, c->height);
        DBG printf("Converted to PIX_FMT_RGB24\n");
    } else {
        fill_rgb_image(picture, frame_count, c->width, c->height, img);
    }
        

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data= video_outbuf;
            pkt.size= out_size;

            /*
            static int x = 0;
            printf("%ld / %ld  :  %ld / %ld  --> %d\n", 
                   (long int) c->time_base.num,
                   (long int) c->time_base.den,
                   (long int) st->time_base.num,
                   (long int) st->time_base.den,
                   x);
            pkt.pts = x;
            x++;
            */

            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame\n");
        ::exit(1);
    }
    frame_count++;
}

void FfmpegWriter::close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(picture->data[0]);
    av_free(picture);
    if (tmp_picture) {
        av_free(tmp_picture->data[0]);
        av_free(tmp_picture);
    }
    av_free(video_outbuf);
}




/**************************************************************/
/* YARP adaptation */

bool FfmpegWriter::open(yarp::os::Searchable & config) {

    //printf("ffmeg version number %d\n", LIBAVCODEC_BUILD);


    ready = false;
    savedConfig.fromString(config.toString());

    // open if possible, if not will do it later
    return delayedOpen(config);
}


bool FfmpegWriter::delayedOpen(yarp::os::Searchable & config) {
    //printf("DELAYED OPEN %s\n", config.toString().c_str());

    int w = config.check("width",Value(0),
                         "width of image (must be even)").asInt();
    int h = config.check("height",Value(0),
                         "height of image (must be even)").asInt();
    int framerate = config.check("framerate",Value(30),
                                 "baseline images per second").asInt();
    
    int sample_rate = 0;
    int channels = 0;
    bool audio = config.check("audio","should audio be included");
    if (audio) {
        sample_rate = config.check("sample_rate",Value(44100),
                                   "audio samples per second").asInt();
        channels = config.check("channels",Value(1),
                                "audio samples per second").asInt();
    }

    filename = config.check("out",Value("movie.avi"),
                            "name of movie to write").asString();

    delayed = false;
    if (w<=0||h<=0) {
        delayed = true;
        return true;
    } 
    ready = true;

    /* initialize libavcodec, and register all codecs and formats */
    av_register_all();
    
    /* auto detect the output format from the name. default is
       mpeg. */
    fmt = guess_format(NULL, filename.c_str(), NULL);
    if (!fmt) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        fmt = guess_format("mpeg", NULL, NULL);
    }
    if (!fmt) {
        fprintf(stderr, "Could not find suitable output format\n");
        ::exit(1);
    }

    /* allocate the output media context */
    oc = av_alloc_format_context();
    if (!oc) {
        fprintf(stderr, "Memory error\n");
        ::exit(1);
    }
    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", filename.c_str());

    /* add the audio and video streams using the default format codecs
       and initialize the codecs */
    video_st = NULL;
    audio_st = NULL;
    if (fmt->video_codec != CODEC_ID_NONE) {
        video_st = add_video_stream(oc, fmt->video_codec, w, h, framerate);
    }

#ifndef OMIT_AUDIO
    if (audio) {
        printf("Adding audio %dx%d\n", sample_rate, channels);
        if (fmt->audio_codec != CODEC_ID_NONE) {
            audio_st = add_audio_stream(oc, fmt->audio_codec);
            if (audio_st!=NULL) {
                AVCodecContext *c = audio_st->codec;
                c->sample_rate = sample_rate;
                c->channels = channels;
            } else {
                printf("Failed to add audio\n");
            }
        } else {
            printf("No audio codec available\n");
        }
    } else {
        printf("Skipping audio\n");
    }
#endif

    /* set the output parameters (must be done even if no
       parameters). */
    if (av_set_parameters(oc, NULL) < 0) {
        fprintf(stderr, "Invalid output format parameters\n");
        ::exit(1);
    }

    dump_format(oc, 0, filename.c_str(), 1);

    /* now that all the parameters are set, we can open the audio and
       video codecs and allocate the necessary encode buffers */
    if (video_st) {
        open_video(oc, video_st);
    }
    if (audio_st) {
        open_audio(oc, audio_st);
    }

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&oc->pb, filename.c_str(), URL_WRONLY) < 0) {
            fprintf(stderr, "Could not open '%s'\n", filename.c_str());
            ::exit(1);
        }
    }

    /* write the stream header, if any */
    av_write_header(oc);

    return true;
}
  
bool FfmpegWriter::close() {
    if (!isOk()) { return false; }

    /* close each codec */
    if (video_st)
        close_video(oc, video_st);
    if (audio_st)
        close_audio(oc, audio_st);

    /* write the trailer, if any */
    av_write_trailer(oc);

    /* free the streams */
    for(int i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }

    if (!(fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
#if LIBAVCODEC_BUILD >= 3354624
        url_fclose(oc->pb);
#else
        url_fclose(&oc->pb);
#endif
    }

    /* free the stream */
    av_free(oc);

    printf("Closed media file %s\n", filename.c_str());

    return true;
}
  
bool FfmpegWriter::putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {
    if (delayed) {
        savedConfig.put("width",Value(image.width()));
        savedConfig.put("height",Value(image.height()));
    }
    if (!isOk()) { return false; }

    /* compute current audio and video time */
    if (audio_st)
        audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
    else
        audio_pts = 0.0;
    
    if (video_st)
        video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
    else
        video_pts = 0.0;
    
    if (!(audio_st||video_st))
        return false;
    
    /* write interleaved audio and video frames */
    if (!video_st || (video_st && audio_st && audio_pts < video_pts)) {
        write_audio_frame(oc, audio_st);
    } else {
        write_video_frame(oc, video_st, image);
    }

    return true;
}



bool FfmpegWriter::putAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                  yarp::sig::Sound& sound) {
    if (delayed) {
        savedConfig.put("width",Value(image.width()));
        savedConfig.put("height",Value(image.height()));
        savedConfig.put("sample_rate",Value(sound.getFrequency()));
        savedConfig.put("channels",Value(sound.getChannels()));
        savedConfig.put("audio",Value(1));
    }
    if (!isOk()) { return false; }

    /* write interleaved audio and video frames */
    write_video_frame(oc, video_st, image);
    write_audio_frame(oc, audio_st, sound);
    return true;
}



