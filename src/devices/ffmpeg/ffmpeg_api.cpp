/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 *
 */

#include "ffmpeg_api.h"


int stable_img_convert (AVPicture *dst, int dst_pix_fmt,
                        const AVPicture *src, int src_pix_fmt,
                        int src_width, int src_height) {
  static struct SwsContext *img_convert_ctx = nullptr;
  if (img_convert_ctx==nullptr) {
      //printf("Looking for a context\n");
      img_convert_ctx = sws_getContext(src_width, src_height,
                                       (AVPixelFormat)src_pix_fmt,
                                       src_width, src_height,
                                       (AVPixelFormat)dst_pix_fmt,
#ifdef SWS_BILINEAR
                                       SWS_BILINEAR,
#else
                                       0,
#endif
                                       nullptr, nullptr, nullptr);

      //printf("Done looking for a context\n");
  }
  if (img_convert_ctx!=nullptr) {
      /*
      printf("software scale: %ld %ld/%ld %d/%d %d\n",
             (long int)img_convert_ctx,
             (long int)(((AVPicture*)src)->data),
             (long int)(((AVPicture*)dst)->data),
             ((AVPicture*)src)->linesize[0],
             ((AVPicture*)dst)->linesize[0],
             src_height);
      */

      sws_scale(img_convert_ctx, ((AVPicture*)src)->data,
                ((AVPicture*)src)->linesize, 0, src_height,
                ((AVPicture*)dst)->data, ((AVPicture*)dst)->linesize);
      //printf("software scale done\n");
  } else {
    fprintf(stderr,"image conversion failed\n");
    return -1;
  }
  return 0;
}
