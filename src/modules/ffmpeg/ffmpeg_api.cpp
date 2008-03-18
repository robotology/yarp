#include "ffmpeg_api.h"


int stable_img_convert (AVPicture *dst, int dst_pix_fmt, 
			const AVPicture *src, int src_pix_fmt, 
			int src_width, int src_height) {
#ifdef OLD_FFMPEG
  return img_convert(dst,dst_pix_fmt,src,src_pix_fmt,src_width,src_height);
#else
  int w = src_width;
  int h = src_height;
  static struct SwsContext *img_convert_ctx = NULL;
  if (img_convert_ctx==NULL) {
    img_convert_ctx = sws_getContext(src_width, src_height,
				     dst_pix_fmt, 
				     src_width, src_height,
				     src_pix_fmt, 
				     0, NULL, NULL, NULL);
  }
  if (img_convert_ctx!=NULL) {
    sws_scale(img_convert_ctx, ((AVPicture*)src)->data, 
	      ((AVPicture*)src)->linesize, 0, src_height,
	      ((AVPicture*)dst)->data, ((AVPicture*)dst)->linesize);
  } else {
    fprintf(stderr,"image conversion failed\n");
    return -1;
  }
  return 0;
#endif
}


