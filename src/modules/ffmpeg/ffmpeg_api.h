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
}

#if LIBAVCODEC_VERSION_INT < (51<<16)
#define OLD_FFMPEG
#else
// device support got factored out
#define FACTORED_DEVICE
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

#endif
