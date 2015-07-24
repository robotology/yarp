/*                 Raw to bmp converter
#
#              Paulo Assis <pj.assis@gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define TRUE -1
#define FALSE 0

// extern "C"
// {

typedef unsigned char uint8_t;

void convert_border_bayer_line_to_bgr24( uint8_t* bayer, uint8_t* adjacent_bayer,
    uint8_t *bgr, int width, int start_with_green, int blue_line)
{
    int t0, t1;

    if (start_with_green)
    {
    /* First pixel */
        if (blue_line)
        {
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
            *bgr++ = adjacent_bayer[0];
        }
        else
        {
            *bgr++ = adjacent_bayer[0];
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
        }
        /* Second pixel */
        t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
        t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
        }
        else
        {
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
        }
        bayer++;
        adjacent_bayer++;
        width -= 2;
    }
    else
    {
        /* First pixel */
        t0 = (bayer[1] + adjacent_bayer[0] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[0];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[0];
        }
        width--;
    }

    if (blue_line)
    {
        for ( ; width > 2; width -= 2)
        {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
            bayer++;
            adjacent_bayer++;
        }
    }
    else
    {
        for ( ; width > 2; width -= 2)
        {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            bayer++;
            adjacent_bayer++;
        }
    }

    if (width == 2)
    {
        /* Second to last pixel */
        t0 = (bayer[0] + bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
        }
        /* Last pixel */
        t0 = (bayer[1] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[2];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[2];
        }
    }
    else
    {
        /* Last pixel */
        if (blue_line)
        {
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
        }
    }
}

static void bayer_to_rgbbgr24(uint8_t *bayer,
    uint8_t *bgr, int width, int height,
    int start_with_green, int blue_line)
{
    /* render the first line */
    convert_border_bayer_line_to_bgr24(bayer, bayer + width, bgr, width,
        start_with_green, blue_line);
    bgr += width * 3;

    /* reduce height by 2 because of the special case top/bottom line */
    for (height -= 2; height; height--)
    {
        int t0, t1;
        /* (width - 2) because of the border */
        uint8_t *bayerEnd = bayer + (width - 2);

        if (start_with_green)
        {
            /* OpenCV has a bug in the next line, which was
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1; */
            t0 = (bayer[1] + bayer[width * 2 + 1] + 1) >> 1;
            /* Write first pixel */
            t1 = (bayer[0] + bayer[width * 2] + bayer[width + 1] + 1) / 3;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width];
            }
            else
            {
                *bgr++ = bayer[width];
                *bgr++ = t1;
                *bgr++ = t0;
            }

            /* Write second pixel */
            t1 = (bayer[width] + bayer[width + 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
            }
            else
            {
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
                *bgr++ = t0;
            }
            bayer++;
        }
        else
        {
            /* Write first pixel */
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width];
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = bayer[width];
                *bgr++ = t0;
            }
        }

        if (blue_line)
        {
            for (; bayer <= bayerEnd - 2; bayer += 2)
            {
                t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                    bayer[width * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[width] +
                    bayer[width + 2] + bayer[width * 2 + 1] +
                    2) >> 2;
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];

                t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
                t1 = (bayer[width + 1] + bayer[width + 3] +
                    1) >> 1;
                *bgr++ = t0;
                *bgr++ = bayer[width + 2];
                *bgr++ = t1;
            }
        }
        else
        {
            for (; bayer <= bayerEnd - 2; bayer += 2)
            {
                t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                    bayer[width * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[width] +
                    bayer[width + 2] + bayer[width * 2 + 1] +
                    2) >> 2;
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;

                t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
                t1 = (bayer[width + 1] + bayer[width + 3] +
                    1) >> 1;
                *bgr++ = t1;
                *bgr++ = bayer[width + 2];
                *bgr++ = t0;
            }
        }

        if (bayer < bayerEnd)
        {
            /* write second to last pixel */
            t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                bayer[width * 2 + 2] + 2) >> 2;
            t1 = (bayer[1] + bayer[width] +
                bayer[width + 2] + bayer[width * 2 + 1] +
                2) >> 2;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }
            /* write last pixel */
            t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width + 2];
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = bayer[width + 2];
                *bgr++ = t0;
            }
            bayer++;
        }
        else
        {
            /* write last pixel */
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1;
            t1 = (bayer[1] + bayer[width * 2 + 1] + bayer[width] + 1) / 3;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }
        }

        /* skip 2 border pixels */
        bayer += 2;

        blue_line = !blue_line;
        start_with_green = !start_with_green;
    }

    /* render the last line */
    convert_border_bayer_line_to_bgr24(bayer + width, bayer, bgr, width,
        !start_with_green, !blue_line);
}

/*convert bayer raw data to rgb24
* args:
*      pBay: pointer to buffer containing Raw bayer data data
*      pRGB24: pointer to buffer containing rgb24 data
*      width: picture width
*      height: picture height
*      pix_order: bayer pixel order (0=gb/rg   1=gr/bg  2=bg/gr  3=rg/bg)
*/
void bayer_to_rgb24(uint8_t *pBay, uint8_t *pRGB24, int width, int height, int pix_order)
{
    switch (pix_order)
    {
        /*conversion functions are build for bgr, by switching b and r lines we get rgb*/
        case 0: /* gbgbgb... | rgrgrg... (V4L2_PIX_FMT_SGBRG8)*/
            bayer_to_rgbbgr24(pBay, pRGB24, width, height, TRUE, FALSE);
            break;

        case 1: /* grgrgr... | bgbgbg... (V4L2_PIX_FMT_SGRBG8)*/
            bayer_to_rgbbgr24(pBay, pRGB24, width, height, TRUE, TRUE);
            break;

        case 2: /* bgbgbg... | grgrgr... (V4L2_PIX_FMT_SBGGR8)*/
            bayer_to_rgbbgr24(pBay, pRGB24, width, height, FALSE, FALSE);
            break;

        case 3: /* rgrgrg... ! gbgbgb... (V4L2_PIX_FMT_SRGGB8)*/
            bayer_to_rgbbgr24(pBay, pRGB24, width, height, FALSE, TRUE);
            break;

        default: /* default is 0*/
            bayer_to_rgbbgr24(pBay, pRGB24, width, height, TRUE, FALSE);
            break;
    }
}

void flip_bgr_image(uint8_t *bgr_buff, int width, int height)
{
    int i =0;
    /*alloc a temp buffer*/
    uint8_t *tmp_buff = (uint8_t *)calloc(width*height*3, 1);
    uint8_t *t1 = bgr_buff + (width*height*3); /*point to the end of buffer*/

    for(i=0;i<height;i++)
    {
        t1 -= width * 3; /*decrement a line*/
        memcpy(tmp_buff+(i*width*3), t1, width*3); /* copy the line to temp buffer*/
    }
    /*get the fliped buffer*/
    memcpy(bgr_buff, tmp_buff, height*width*3);
    free(tmp_buff);
}

// }
