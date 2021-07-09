/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FFMPEG_DEVICE_FFMPEG_API_H
#define YARP_FFMPEG_DEVICE_FFMPEG_API_H

// ffmpeg changes all the time.
// time to start factoring out some of the things that change.

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(53, 0, 0)
#  error "ffmpeg version is too old, sorry - please download and compile newer version"
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 0, 0)
#  define av_frame_alloc avcodec_alloc_frame
#  define av_frame_unref avcodec_get_frame_defaults
#  define av_frame_free  avcodec_free_frame
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 0, 0)
#  define AV_CODEC_FLAG_GLOBAL_HEADER CODEC_FLAG_GLOBAL_HEADER
#endif

#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(54, 4, 100)
#  define av_dict_set_int(x, k, v, f) { char buf[256]; sprintf(buf,"%d",v); av_dict_set(x, k ,buf, 0); }
#endif

int stable_img_convert (AVPicture *dst, int dst_pix_fmt,
                        const AVPicture *src, int src_pix_fmt,
                        int src_width, int src_height);


#endif // YARP_FFMPEG_DEVICE_FFMPEG_API_H
