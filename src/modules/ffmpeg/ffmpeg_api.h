// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
#include <mathematics.h>
}

#if LIBAVCODEC_VERSION_INT < (51<<16)
#define OLD_FFMPEG
#else
// device support got factored out
#define FACTORED_DEVICE
#endif

#if LIBAVCODEC_VERSION_INT < (53<<16)
// stick with AVFormatParameters
#else
#define GENERALIZED_PARAMETERS
#endif 

#if LIBAVCODEC_VERSION_INT >= (55<<16)
#define USE_AV_FRAME_ALLOC
#define USE_AUDIO4
#define USE_AVFORMAT_OPEN_INPUT
#endif


#ifndef OLD_FFMPEG
extern "C" {
#include <swscale.h>
}
#endif

#ifdef FACTORED_DEVICE

extern "C" {
#include <avdevice.h>
}
#endif

#if LIBAVCODEC_BUILD < 4754
#error "ffmpeg version is too old, sorry - please download and compile newer version"
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


#ifdef GENERALIZED_PARAMETERS
typedef AVDictionary *YARP_AVDICT;
#define YARP_AVDICT_INIT(x) x = NULL
#define YARP_AVDICT_QUOTE(x) #x
#define YARP_AVDICT_SET_STR(x,k,v) av_dict_set(&x,YARP_AVDICT_QUOTE(k),v,0)
#define YARP_AVDICT_SET_INT(x,k,v) { char buf[256]; sprintf(buf,"%d",v); av_dict_set(&x,YARP_AVDICT_QUOTE(k),buf,0); }
#define YARP_AVDICT_SET_FRAC(x,k1,k2,v1,v2) { char buf[256]; sprintf(buf,"%d/%d",v1,v2); av_dict_set(&x,YARP_AVDICT_QUOTE(k2),buf,0); }
#define YARP_AVDICT_DESTROY(x) if (x) { av_dict_free(&x); x = NULL; }
#define YARP_AVDICT_CLEAN(x)
#define YARP_AV_OPEN_INPUT_FILE(a,b,c,d,e) avformat_open_input(a,b,c,d)
#include <mathematics.h>
#define YARP_avcodec_open(x,y) avcodec_open2(x,y,NULL)
#else
typedef AVFormatParameters YARP_AVDICT;
#define YARP_AVDICT_DEFINE(x) AVFormatParameters x
#define YARP_AVDICT_INIT(x)
#define YARP_AVDICT_SET_STR(x,k,v) x.k = strdup(v)
#define YARP_AVDICT_SET_INT(x,k,v) x.k = v
#define YARP_AVDICT_SET_FRAC(x,k1,k2,v1,v2) { x.k1.num = v1; x.k1.den = v2; }
#define YARP_AVDICT_DESTROY(x) 
#define YARP_AVDICT_CLEAN(x) memset(&x, 0, sizeof(x))
#define YARP_AV_OPEN_INPUT_FILE(a,b,c,d) av_open_input_file(a,strdup(b),c,0,d)
#define YARP_avcodec_open(x,y) avcodec_open(x,y)
#endif


#ifdef USE_AV_FRAME_ALLOC
#define YARP_avcodec_alloc_frame av_frame_alloc
#else
#define YARP_avcodec_alloc_frame avcodec_alloc_frame
#endif

#define YARP_av_find_stream_info avformat_find_stream_info
#define YARP_dump_format av_dump_format
#define YARP_av_close_input_file av_close_input_file

#ifdef USE_AVFORMAT_OPEN_INPUT
#undef YARP_AV_OPEN_INPUT_FILE
#define YARP_AV_OPEN_INPUT_FILE(a,b,c,d,e) avformat_open_input(a,b,c,d)
#undef YARP_av_find_stream_info
#define YARP_av_find_stream_info(a) avformat_find_stream_info(a,NULL)
#undef YARP_dump_format
#define YARP_dump_format av_dump_format
#undef YARP_av_close_input_file
#define YARP_av_close_input_file(x) avformat_close_input(&(x))
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#define CodecID AVCodecID
#define CODEC_ID_NONE AV_CODEC_ID_NONE
#define CODEC_ID_PCM_S16LE AV_CODEC_ID_PCM_S16LE
#define CODEC_ID_PCM_S16BE AV_CODEC_ID_PCM_S16BE
#define CODEC_ID_PCM_U16LE AV_CODEC_ID_PCM_U16LE
#define CODEC_ID_PCM_U16BE AV_CODEC_ID_PCM_U16BE
#define CODEC_ID_MPEG1VIDEO AV_CODEC_ID_MPEG1VIDEO
#define CODEC_ID_MPEG2VIDEO AV_CODEC_ID_MPEG2VIDEO
#define url_fopen avio_open
#define url_fclose avio_close
#define URL_WRONLY AVIO_FLAG_WRITE
#define AV_NO_SET_PARAMETERS
#define av_write_header(x) avformat_write_header(x,NULL)
#define av_new_stream(x,v) avformat_new_stream(x,NULL)
#endif

#endif
