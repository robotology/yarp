/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP2_ADAPT_LIBDC1294
#define YARP2_ADAPT_LIBDC1294

/*
  libdc1294_bayer.c is taken verbatim from:
  http://libdc1394.svn.sourceforge.net/viewvc/libdc1394/trunk/libdc1394/dc1394/bayer.c?revision=535

  This header file adapts it for use with YARP.
  We use some tricks for Windows compatibility
  (UPDATE: you'll need to change the second "calloc" to "calloc2" in that file for MSVC support)

*/


#if defined _MSC_VER && _MSC_VER < 1900
// Since visual studio 2015 stdint.h has fixed-width integral types
# define uint8_t unsigned __int8
# define uint16_t unsigned __int16
# define uint32_t unsigned __int32
# define uint64_t unsigned __int64
#else
# include <stdint.h>
#endif

#include <assert.h>

#define restrict
#define dc1394error_t int
#define dc1394bool_t int
#define dc1394bayer_method_t int
#define dc1394color_filter_t int
#define dc1394color_coding_t int
#define dc1394video_mode_t int
#define dc1394camera_t int
#define DC1394_SUCCESS 0
#define DC1394_TRUE 1
#define DC1394_FALSE 0

enum {
  DC1394_COLOR_CODING_MONO16 = 1,
  DC1394_COLOR_CODING_MONO8,
  DC1394_COLOR_CODING_RAW16,
  DC1394_COLOR_CODING_RAW8,
  DC1394_COLOR_CODING_RGB16,
  DC1394_COLOR_CODING_RGB8,

  DC1394_BAYER_METHOD_AHD,
  DC1394_BAYER_METHOD_BILINEAR,
  DC1394_BAYER_METHOD_DOWNSAMPLE,
  DC1394_BAYER_METHOD_EDGESENSE,
  DC1394_BAYER_METHOD_HQLINEAR,
  DC1394_BAYER_METHOD_NEAREST,
  DC1394_BAYER_METHOD_SIMPLE,
  DC1394_BAYER_METHOD_VNG,

  DC1394_COLOR_FILTER_BGGR,
  DC1394_COLOR_FILTER_GBRG,
  DC1394_COLOR_FILTER_GRBG,
  DC1394_COLOR_FILTER_RGGB,

  DC1394_VIDEO_MODE_640x480_RGB8
};

#define DC1394_COLOR_FILTER_MIN (DC1394_COLOR_FILTER_BGGR)
#define DC1394_COLOR_FILTER_MAX (DC1394_COLOR_FILTER_RGGB)

#define DC1394_BAYER_METHOD_MIN (DC1394_BAYER_METHOD_AHD)
#define DC1394_BAYER_METHOD_MAX (DC1394_BAYER_METHOD_VNG)

#define DC1394_FUNCTION_NOT_SUPPORTED -1
#define DC1394_INVALID_BAYER_METHOD -2
#define DC1394_INVALID_COLOR_FILTER -3
#define DC1394_MEMORY_ALLOCATION_FAILURE -4

/* YARP will in fact use only one color coding type, so we take a shortcut */
#define dc1394_get_color_coding_bit_size(x,y) (*y) = (3*8)

/*
 * Structure definition from libdc1394 video.h
 *
 * 1394-Based Digital Camera Control Library
 *
 * Video format headers
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
typedef struct __dc1394_video_frame
{
  unsigned char          * image;                 /* the image. May contain padding data too (vendor specific). Read/write allowed. Free NOT allowed if
                                                     returned by dc1394_capture_dequeue() */
  uint32_t                 size[2];               /* the image size [width, height] */
  uint32_t                 position[2];           /* the WOI/ROI position [horizontal, vertical] == [0,0] for full frame */
  dc1394color_coding_t     color_coding;          /* the color coding used. This field is valid for all video modes. */
  dc1394color_filter_t     color_filter;          /* the color filter used. This field is valid only for RAW modes and IIDC 1.31 */
  uint32_t                 yuv_byte_order;        /* the order of the fields for 422 formats: YUYV or UYVY */
  uint32_t                 data_depth;            /* the number of bits per pixel. The number of grayscale levels is 2^(this_number).
                                                     This is independent from the colour coding */
  uint32_t                 stride;                /* the number of bytes per image line */
  dc1394video_mode_t       video_mode;            /* the video mode used for capturing this frame */
  uint64_t                 total_bytes;           /* the total size of the frame buffer in bytes. May include packet-
                                                     multiple padding and intentional padding (vendor specific) */
  uint32_t                 image_bytes;           /* the number of bytes used for the image (image data only, no padding) */
  uint32_t                 padding_bytes;         /* the number of extra bytes, i.e. total_bytes-image_bytes.  */
  uint32_t                 packet_size;           /* the size of a packet in bytes. (IIDC data) */
  uint32_t                 packets_per_frame;     /* the number of packets per frame. (IIDC data) */
  uint64_t                 timestamp;             /* the unix time [microseconds] at which the frame was captured in
                                                     the video1394 ringbuffer */
  uint32_t                 frames_behind;         /* the number of frames in the ring buffer that are yet to be accessed by the user */
  dc1394camera_t           *camera;               /* the parent camera of this frame */
  uint32_t                 id;                    /* the frame position in the ring buffer */
  uint64_t                 allocated_image_bytes; /* amount of memory allocated in for the *image field. */
  dc1394bool_t             little_endian;         /* DC1394_TRUE if little endian (16bpp modes only),
                                                     DC1394_FALSE otherwise */
  dc1394bool_t             data_in_padding;       /* DC1394_TRUE if data is present in the padding bytes in IIDC 1.32 format,
                                                     DC1394_FALSE otherwise */
} dc1394video_frame_t;


#if defined _MSC_VER && _MSC_VER < 1900
// Since visual studio 2015 libdc1394_bayer.c no longer needs to be compiled
// as C++ and this is no longer needed.
extern "C"
#endif
dc1394error_t dc1394_debayer_frames(dc1394video_frame_t *in, dc1394video_frame_t *out, dc1394bayer_method_t method);

#ifdef _MSC_VER
#define calloc (unsigned char (*)[3])calloc
#define calloc2 (unsigned short (*)[3])calloc
#define pow(x,y) pow((float)(x),(float)(y))
#else
#define calloc2 calloc
#endif

#endif
