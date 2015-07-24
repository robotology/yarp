#ifndef _YUV_H_
#define _YUV_H_

unsigned char clamp(float val);
void YUV420toYUV444(int width, int height, unsigned char* src, unsigned char* dst);
void YUYV422toRGB(unsigned char* src, unsigned char* rgb, const int width, const int height);

#endif
