/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ffmpeg_api.h"


int stable_img_convert (AVFrame *dst, int dst_pix_fmt,
                        const AVFrame *src, int src_pix_fmt,
                        int src_width, int src_height) {
  static struct SwsContext *img_convert_ctx = nullptr;
  if (img_convert_ctx==nullptr) {
      //printf("Looking for a context\n");
      img_convert_ctx = sws_getContext(src_width, src_height,
                                       (AVPixelFormat)src_pix_fmt,
                                       src_width, src_height,
                                       (AVPixelFormat)dst_pix_fmt,
                                       SWS_BILINEAR,
                                       nullptr, nullptr, nullptr);

      //printf("Done looking for a context\n");
  }
  if (img_convert_ctx!=nullptr) {
      /*
      printf("software scale: %ld %ld/%ld %d/%d %d\n",
             (long int)img_convert_ctx,
             (long int)(src->data),
             (long int)(dst->data),
             src->linesize[0],
             dst->linesize[0],
             src_height);
      */

      sws_scale(img_convert_ctx, src->data,
                src->linesize, 0, src_height,
                dst->data, dst->linesize);
      //printf("software scale done\n");
  } else {
    fprintf(stderr,"image conversion failed\n");
    return -1;
  }
  return 0;
}
