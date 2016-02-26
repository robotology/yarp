/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef FFMPEG_API_YARP
#define FFMPEG_API_YARP

// ffmpeg changes all the time.
// time to start factoring out some of the things that change.

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C" {
#include <avcodec.h>
#include <avformat.h>
#include <avdevice.h>
#include <swscale.h>
#include <mathematics.h>
}


#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(53, 0, 0)
#  error "ffmpeg version is too old, sorry - please download and compile newer version"
#endif


#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(54, 4, 100)
#  define av_dict_set_int(x, k, v, f) { char buf[256]; sprintf(buf,"%d",v); av_dict_set(x, k ,buf, 0); }
#endif

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 0, 0)
#  define USE_AVFORMAT_OPEN_INPUT
#endif

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 0, 0)
#  define USE_AV_FRAME_ALLOC
#  define USE_AUDIO4
#endif



int stable_img_convert (AVPicture *dst, int dst_pix_fmt,
                        const AVPicture *src, int src_pix_fmt,
                        int src_width, int src_height);

#ifndef CODEC_TYPE_AUDIO
#  define CodecType AVMediaType
#  define CODEC_TYPE_UNKNOWN    AVMEDIA_TYPE_UNKNOWN
#  define CODEC_TYPE_VIDEO      AVMEDIA_TYPE_VIDEO
#  define CODEC_TYPE_AUDIO      AVMEDIA_TYPE_AUDIO
#  define CODEC_TYPE_DATA       AVMEDIA_TYPE_DATA
#  define CODEC_TYPE_SUBTITLE   AVMEDIA_TYPE_SUBTITLE
#  define CODEC_TYPE_ATTACHMENT AVMEDIA_TYPE_ATTACHMENT
#  define CODEC_TYPE_NB         AVMEDIA_TYPE_NB
#  define PKT_FLAG_KEY          AV_PKT_FLAG_KEY
#  define guess_format          av_guess_format
#  define av_alloc_format_context avformat_alloc_context
#  define FFEPOCH3
#endif



#ifdef USE_AV_FRAME_ALLOC
#  define YARP_avcodec_alloc_frame av_frame_alloc
#else
#  define YARP_avcodec_alloc_frame avcodec_alloc_frame
#endif

#ifdef USE_AVFORMAT_OPEN_INPUT
#  define YARP_av_find_stream_info(a) avformat_find_stream_info(a,NULL)
#  define YARP_dump_format av_dump_format
#  define YARP_av_close_input_file(x) avformat_close_input(&(x))
#  define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#  define CodecID AVCodecID
#  define CODEC_ID_NONE AV_CODEC_ID_NONE
#  define CODEC_ID_PCM_S16LE AV_CODEC_ID_PCM_S16LE
#  define CODEC_ID_PCM_S16BE AV_CODEC_ID_PCM_S16BE
#  define CODEC_ID_PCM_U16LE AV_CODEC_ID_PCM_U16LE
#  define CODEC_ID_PCM_U16BE AV_CODEC_ID_PCM_U16BE
#  define CODEC_ID_MPEG1VIDEO AV_CODEC_ID_MPEG1VIDEO
#  define CODEC_ID_MPEG2VIDEO AV_CODEC_ID_MPEG2VIDEO
#  define url_fopen avio_open
#  define url_fclose avio_close
#  define URL_WRONLY AVIO_FLAG_WRITE
#  define AV_NO_SET_PARAMETERS
#  define av_write_header(x) avformat_write_header(x,NULL)
#  define av_new_stream(x,v) avformat_new_stream(x,NULL)
#else
#  define YARP_av_find_stream_info av_find_stream_info
#  define YARP_dump_format dump_format
#  define YARP_av_close_input_file av_close_input_file
#endif

#endif
