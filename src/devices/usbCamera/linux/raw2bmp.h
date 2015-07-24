#ifndef __RAW2BMP__

#define __RAW2BMP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define TRUE -1
#define FALSE 0

typedef unsigned char uint8_t;

// extern "C"
// {
void convert_border_bayer_line_to_bgr24( uint8_t* bayer, uint8_t* adjacent_bayer, uint8_t *bgr, int width, int start_with_green, int blue_line);
void bayer_to_rgbbgr24(uint8_t *bayer, uint8_t *bgr, int width, int height, int start_with_green, int blue_line);
void bayer_to_rgb24(uint8_t *pBay, uint8_t *pRGB24, int width, int height, int pix_order);
void flip_bgr_image(uint8_t *bgr_buff, int width, int height);

// }

#endif // __RAW2BMP__


