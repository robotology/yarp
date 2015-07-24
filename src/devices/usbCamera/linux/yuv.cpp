/***************************************************************************
 *   Copyright (C) 2012 by Tobias Müller                                   *
 *   Tobias_Mueller@twam.info                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


unsigned char clamp(float val)
{
    if (val > 255)
        return 255;
    if(val <0)
        return 0;

    return (unsigned char) val;
}

/**
	Convert from YUV420 format to YUV444.

	\param width width of image
	\param height height of image
	\param src source
	\param dst destination
*/
void YUV420toYUV444(int width, int height, unsigned char* src, unsigned char* dst) {
	int line, column;
	unsigned char *py, *pu, *pv;
	unsigned char *tmp = dst;

	// In this format each four bytes is two pixels. Each four bytes is two Y's, a Cb and a Cr.
	// Each Y goes to one of the pixels, and the Cb and Cr belong to both pixels.
	unsigned char *base_py = src;
	unsigned char *base_pu = src+(height*width);
	unsigned char *base_pv = src+(height*width)+(height*width)/4;

	for (line = 0; line < height; ++line) {
		for (column = 0; column < width; ++column) {
			py = base_py+(line*width)+column;
			pu = base_pu+(line/2*width/2)+column/2;
			pv = base_pv+(line/2*width/2)+column/2;

			*tmp++ = *py;
			*tmp++ = *pu;
			*tmp++ = *pv;
		}
	}
}

void YUYV422toRGB(unsigned char* src, unsigned char* rgb, const int width, const int height)
{
    //yuv to rgb
    float Y1, Y2, U, V;
    float R1,G1,B1;
    float R2,G2,B2;

    int pixel = 0;
    for(pixel=0; pixel < width * height; pixel+=2)
    {
        Y1 = src[pixel*2 + 0];
        U  = src[pixel*2 + 1];
        Y2 = src[pixel*2 + 2];
        V  = src[pixel*2 + 3];

// //         first version
        R1 = Y1 + 1.402 *(V-128);
        G1 = Y1 - 0.34414 *(U-128) - 0.71414 *(V-128);
        B1 = Y1 + 1.772 *(U-128);

        R2 = Y2 + 1.402 *(V-128);
        G2 = Y2 - 0.34414 *(U-128) - 0.71414 *(V-128);
        B2 = Y2 + 1.772 *(U-128);


//         second version
//         R1 = 1.164 * (Y1 - 16) + 1.596 * (V - 128);
//         B1 = 1.164 * (Y1 - 16) + 2.018 * (U - 128);
//         G1 = 1.164 * (Y1 - 16) - 0.813 * (V - 128) - 0.391 * (U - 128);
//
//         R2 = 1.164 * (Y2 - 16) + 1.596 * (V - 128);
//         B2 = 1.164 * (Y2 - 16) + 2.018 * (U - 128);
//         G2 = 1.164 * (Y2 - 16) - 0.813 * (V - 128) - 0.391 * (U - 128);

//         assign to destination
        rgb[pixel*3 + 0] = clamp(R1);
        rgb[pixel*3 + 1] = clamp(G1);
        rgb[pixel*3 + 2] = clamp(B1);
        rgb[pixel*3 + 3] = clamp(R2);
        rgb[pixel*3 + 4] = clamp(G2);
        rgb[pixel*3 + 5] = clamp(B2);
    }
}


