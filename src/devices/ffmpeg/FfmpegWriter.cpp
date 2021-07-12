/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Most of this file is from the output_example.c of ffmpeg -
 * copyright/copypolicy statement follows --
 */

/*
 * Libavformat API example: Output a media file in any supported
 * libavformat format. The default codecs are used.
 *
 * SPDX-FileCopyrightText: 2003 Fabrice Bellard
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
#include "ffmpeg_api.h"

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>

#include <cstdlib>
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926535897931
#endif

#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P /* default pix_fmt */
#define STREAM_PIX_WORK AV_PIX_FMT_RGB24

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace {
YARP_LOG_COMPONENT(FFMPEGWRITER, "yarp.device.ffmpeg_writer")
}


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
static AVStream *add_audio_stream(AVFormatContext *oc, AVCodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = avformat_new_stream(oc, NULL);
    if (!st) {
        yCFatal(FFMPEGWRITER, "Could not alloc stream");
    }

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_AUDIO;

    /* put sample parameters */
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;
    return st;
}

static void open_audio(AVFormatContext *oc, AVStream *st)
{
    yCInfo(FFMPEGWRITER, "Opening audio stream");
    AVCodecContext *c;
    AVCodec *codec;

    c = st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        yCFatal(FFMPEGWRITER, "Audio codec not found");
    }

    /* open it */
    if (avcodec_open2(c, codec, nullptr) < 0) {
        yCFatal(FFMPEGWRITER, "Could not open codec");
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
        case AV_CODEC_ID_PCM_S16LE:
        case AV_CODEC_ID_PCM_S16BE:
        case AV_CODEC_ID_PCM_U16LE:
        case AV_CODEC_ID_PCM_U16BE:
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


    yCFatal(FFMPEGWRITER,
            "FRAME SIZE is %d / samples size is %d\n",
            c->frame_size,
            samples_size);
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

static void make_audio_frame(AVCodecContext *c, AVFrame * &frame,
                             void *&samples) {
    frame = av_frame_alloc();
    if (!frame) {
        yCFatal(FFMPEGWRITER, "Could not allocate audio frame");
    }
    frame->nb_samples     = c->frame_size;
    frame->format         = c->sample_fmt;
    frame->channel_layout = c->channel_layout;
    int buffer_size = av_samples_get_buffer_size(nullptr, c->channels,
                                                 c->frame_size,
                                                 c->sample_fmt, 0);
    if (buffer_size < 0) {
        yCError(FFMPEGWRITER, "Could not get sample buffer size");
    }
    samples = av_malloc(buffer_size);
    if (!samples) {
        yCFatal(FFMPEGWRITER,
                "Could not allocate %d bytes for samples buffer",
                buffer_size);
    }
    /* setup the data pointers in the AVFrame */
    int ret = avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
                                       (const uint8_t*)samples, buffer_size, 0);
    if (ret < 0) {
        yCFatal(FFMPEGWRITER, "Could not setup audio frame");
    }
}

static void write_audio_frame(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;

    get_audio_frame(samples, audio_input_frame_size, c->channels);

    AVFrame *frame;
    void *samples;
    make_audio_frame(c,frame,samples);
    AVPacket tmp;
    int got_packet = 0;
    av_init_packet(&tmp);
    tmp.data = audio_outbuf;
    tmp.size = audio_outbuf_size;
    pkt.size = avcodec_encode_audio2(c, &tmp, frame, &got_packet);
    if (tmp.side_data_elems > 0) {
        for (int i = 0; i < tmp.side_data_elems; i++) {
            av_free(tmp.side_data[i].data);
        }
        av_freep(&tmp.side_data);
        tmp.side_data_elems = 0;
    }
    av_freep(&samples);
    av_frame_free(&frame);

    pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_write_frame(oc, &pkt) != 0) {
        yCFatal(FFMPEGWRITER, "Error while writing audio frame");
    } else {
        yCInfo(FFMPEGWRITER, "Wrote some audio");
    }
}

static void write_audio_frame(AVFormatContext *oc, AVStream *st, Sound& snd)
{
    yCInfo(FFMPEGWRITER, "Preparing to write audio (%d left over)", samples_at);
    AVCodecContext *c;
    int key = 1;

    c = st->codec;

    size_t at = 0;
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


            AVFrame *frame;
            void *samples;
            make_audio_frame(c,frame,samples);
            AVPacket tmp;
            int got_packet = 0;
            av_init_packet(&tmp);
            tmp.data = audio_outbuf;
            tmp.size = audio_outbuf_size;
            pkt.size = avcodec_encode_audio2(c, &tmp, frame, &got_packet);
            if (tmp.side_data_elems > 0) {
                for (int i = 0; i < tmp.side_data_elems; i++) {
                    av_free(tmp.side_data[i].data);
                }
                av_freep(&tmp.side_data);
                tmp.side_data_elems = 0;
            }
            av_freep(&samples);
            av_frame_free(&frame);

            pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base,
                                  st->time_base);
            pkt.dts = pkt.pts;
            yCTrace(FFMPEGWRITER, "(%d)", pkt.size);
            if (key) {
                pkt.flags |= AV_PKT_FLAG_KEY;
                key = 0;
            }
            pkt.stream_index= st->index;
            pkt.data = audio_outbuf;
            pkt.duration = 0;


            /* write the compressed frame in the media file */
            if (av_write_frame(oc, &pkt) != 0) {
                yCFatal(FFMPEGWRITER, "Error while writing audio frame");
            }
            samples_at = 0;
        }
    }
    yCInfo(FFMPEGWRITER, " wrote audio\n");
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
static AVStream *add_video_stream(AVFormatContext *oc, AVCodecID codec_id,
                                  int w, int h, int framerate)
{
    AVCodecContext *c;
    AVStream *st;

    st = avformat_new_stream(oc, NULL);
    if (!st) {
        yCFatal(FFMPEGWRITER, "Could not alloc stream");
    }

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

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
    if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        c->mb_decision=2;
    }
    // some formats want stream headers to be separate
    if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name, "3gp"))
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


    return st;
}

static AVFrame *alloc_picture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = av_frame_alloc();
    if (!picture)
        return nullptr;
    size = avpicture_get_size((AVPixelFormat)pix_fmt, width, height);
    picture_buf = (uint8_t*)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return nullptr;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   (AVPixelFormat)pix_fmt, width, height);
    return picture;
}

void FfmpegWriter::open_video(AVFormatContext *oc, AVStream *st)
{
    yCInfo(FFMPEGWRITER, "Opening video stream");
    AVCodec *codec;
    AVCodecContext *c;

    c = st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        yCFatal(FFMPEGWRITER, "Video codec not found");
    }

    /* open the codec */
    if (avcodec_open2(c, codec, nullptr) < 0) {
        yCFatal(FFMPEGWRITER, "Could not open codec");
    }

    video_outbuf = nullptr;
    /* allocate output buffer */
    /* XXX: API change will be done */
    /* buffers passed into lav* can be allocated any way you prefer,
       as long as they're aligned enough for the architecture, and
       they're freed appropriately (such as using av_free for buffers
       allocated with av_malloc) */
    video_outbuf_size = 200000;
    video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        yCFatal(FFMPEGWRITER, "Could not allocate picture");
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = nullptr;
    if (c->pix_fmt != AV_PIX_FMT_RGB24) {
        tmp_picture = alloc_picture(AV_PIX_FMT_RGB24, c->width, c->height);
        if (!tmp_picture) {
            yCFatal(FFMPEGWRITER, "Could not allocate temporary picture");
        }
    }
}

static void fill_rgb_image(AVFrame *pict, int frame_index, int width,
                           int height, ImageOf<PixelRgb>& img)
{
    int x, y;

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
    int out_size, ret;
    AVCodecContext *c;

    c = st->codec;

    if (c->pix_fmt != AV_PIX_FMT_RGB24) {
        fill_rgb_image(tmp_picture, frame_count, c->width, c->height, img);
        stable_img_convert((AVPicture *)picture, c->pix_fmt,
                           (AVPicture *)tmp_picture, AV_PIX_FMT_RGB24,
                           c->width, c->height);
    } else {
        fill_rgb_image(picture, frame_count, c->width, c->height, img);
    }

    /* encode the image */
    AVPacket tmp;
    int got_packet = 0;
    av_init_packet(&tmp);
    tmp.data = video_outbuf;
    tmp.size = video_outbuf_size;
    out_size = avcodec_encode_video2(c, &tmp, picture, &got_packet);
    if (tmp.side_data_elems > 0) {
        for (int i = 0; i < tmp.side_data_elems; i++) {
                av_free(tmp.side_data[i].data);
        }
        av_freep(&tmp.side_data);
        tmp.side_data_elems = 0;
    }
    /* if zero size, it means the image was buffered */
    if (out_size > 0) {
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
        if(c->coded_frame->key_frame)
            pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= video_outbuf;
        pkt.size= out_size;

        /*
        static int x = 0;
        yCInfo(FFMPEGWRITER,
               "%ld / %ld  :  %ld / %ld  --> %d\n",
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

    if (ret != 0) {
        yCFatal(FFMPEGWRITER, "Error while writing video frame");
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
    yCTrace(FFMPEGWRITER,
            "ffmpeg libavcodec version number %d.%d.%d",
            LIBAVCODEC_VERSION_MAJOR,
            LIBAVCODEC_VERSION_MINOR,
            LIBAVCODEC_VERSION_MICRO);

    ready = false;
    savedConfig.fromString(config.toString());

    // open if possible, if not will do it later
    return delayedOpen(config);
}


bool FfmpegWriter::delayedOpen(yarp::os::Searchable & config) {
    yCTrace(FFMPEGWRITER, "DELAYED OPEN %s", config.toString().c_str());

    int w = config.check("width",Value(0),
                         "width of image (must be even)").asInt32();
    int h = config.check("height",Value(0),
                         "height of image (must be even)").asInt32();
    int framerate = config.check("framerate",Value(30),
                                 "baseline images per second").asInt32();

    int sample_rate = 0;
    int channels = 0;
    bool audio = config.check("audio","should audio be included");
    if (audio) {
        sample_rate = config.check("sample_rate",Value(44100),
                                   "audio samples per second").asInt32();
        channels = config.check("channels",Value(1),
                                "audio samples per second").asInt32();
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
    fmt = av_guess_format(nullptr, filename.c_str(), nullptr);
    if (!fmt) {
        yCInfo(FFMPEGWRITER, "Could not deduce output format from file extension: using MPEG.");
        fmt = av_guess_format("mpeg", nullptr, nullptr);
    }
    if (!fmt) {
        yCFatal(FFMPEGWRITER, "Could not find suitable output format");
    }

    /* allocate the output media context */
    oc = avformat_alloc_context();
    if (!oc) {
        yCFatal(FFMPEGWRITER, "Memory error");
    }
    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", filename.c_str());

    /* add the audio and video streams using the default format codecs
       and initialize the codecs */
    video_st = nullptr;
    audio_st = nullptr;
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        video_st = add_video_stream(oc, fmt->video_codec, w, h, framerate);
    }

    if (audio) {
        yCInfo(FFMPEGWRITER, "Adding audio %dx%d", sample_rate, channels);
        if (fmt->audio_codec != AV_CODEC_ID_NONE) {
            audio_st = add_audio_stream(oc, fmt->audio_codec);
            if (audio_st!=nullptr) {
                AVCodecContext *c = audio_st->codec;
                c->sample_rate = sample_rate;
                c->channels = channels;
            } else {
                yCError(FFMPEGWRITER, "Failed to add audio");
            }
        } else {
            yCWarning(FFMPEGWRITER, "No audio codec available");
        }
    } else {
        yCInfo(FFMPEGWRITER, "Skipping audio");
    }

    av_dump_format(oc, 0, filename.c_str(), 1);

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
        if (avio_open(&oc->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            yCFatal(FFMPEGWRITER, "Could not open '%s'", filename.c_str());
        }
    }

    /* write the stream header, if any */
    avformat_write_header(oc, NULL);

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
    for(unsigned int i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }

    if (!(fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
        avio_close(oc->pb);
    }

    /* free the stream */
    av_free(oc);

    yCInfo(FFMPEGWRITER, "Closed media file %s", filename.c_str());

    return true;
}

bool FfmpegWriter::putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {
    if (delayed) {
        savedConfig.put("width",Value((int)image.width()));
        savedConfig.put("height",Value((int)image.height()));
    }
    if (!isOk()) { return false; }

    /* compute current audio and video time */
    if (audio_st)
        audio_pts = (double)av_stream_get_end_pts(audio_st) * audio_st->time_base.num / audio_st->time_base.den;
    else
        audio_pts = 0.0;

    if (video_st)
        video_pts = (double)av_stream_get_end_pts(video_st) * video_st->time_base.num / video_st->time_base.den;
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
        savedConfig.put("width",Value((int)image.width()));
        savedConfig.put("height",Value((int)image.height()));
        savedConfig.put("sample_rate",Value((int)sound.getFrequency()));
        savedConfig.put("channels",Value((int)sound.getChannels()));
        savedConfig.put("audio",Value(1));
    }
    if (!isOk()) { return false; }

    /* write interleaved audio and video frames */
    write_video_frame(oc, video_st, image);
    write_audio_frame(oc, audio_st, sound);
    return true;
}
