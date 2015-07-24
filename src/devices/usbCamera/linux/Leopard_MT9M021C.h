#ifndef __CAPP_LIB__

#define __CAPP_LIB__

#include <stdint.h>


enum MY_COLOR {
    MY_RED = 0,
    MY_GREEN,
    MY_BLUE,
    MY_YELLOW
};

#define BYTE unsigned char
#define WORD uint16_t

extern "C"
{
void initGammaTable(double gamma, int bpp);
void gammaCorrection(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, double gamma);
void convDualImage(BYTE* in_bytes, BYTE* out_bytes, int width, int height);
void rgb2rgb(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
                    int rr,int rg,int rb,int gr,int gg,int gb,int br,int bg,int bb,
                    int r_offset, int g_offset, int b_offset);

int raw_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, bool GammaEna, double gamma);
int raw_to_bmp(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, int pixel_order, bool GammaEna, double gamma,
                int rr,int rg,int rb,int gr,int gg,int gb,int br,int bg,int bb, int r_offset, int g_offset, int b_offset);

double calc_mean(BYTE* in_bytes, int width, int height, int bpp, int startX, int startY, int iSize);
int yuv422_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height);
int yuv422_to_y(BYTE* in_bytes, BYTE* out_bytes, int width, int height);
int y_SobleOperator(BYTE *in_buf, int iWidth, int iHeight, int startX, int startY, int iSize);
int y_CalcMean(BYTE *in_buf, int iWidth, int iHeight, int startX, int startY, int iSize);
}

#endif // __CAPP_LIB__
