/****************************************************************************
While the underlying libraries are covered by LGPL, this sample is released
as public domain.  It is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
// #include <windows.h>
// #include <commdlg.h>
// //#include <streams.h>
// #include <initguid.h>
// #include <Vidcap.h>
// #include <ks.h>
// #include <ksmedia.h>
// #include <ksproxy.h>
#include <math.h>
#include "Leopard_MT9M021C.h"
#include "raw2bmp.h"
#include <unistd.h>

// extern "C"
// {

////////////// GAMMA_CORRECTION ///////////////////////
static WORD linear_to_gamma[65536];
static double gammaValue = -1;
static int gBPP = 0;

// create gamma table
void initGammaTable(double gamma, int bpp)
{

    printf("initGammaTable\n");

    int result;
	double dMax;
	int iMax;
    WORD addr, value;

	if (bpp > 12)
		return;

	dMax = pow(2, (double)bpp);
	iMax = (int)dMax;

    for (int i = 0; i < iMax; i++) {
        result = (int)(pow((double)i/dMax, 1.0/gamma)*dMax);

        linear_to_gamma[i] = result;
    }

	gammaValue = gamma;
	gBPP = bpp;

}

void gammaCorrection(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, double gamma)
{
	int i;
	WORD *srcShort;
	WORD *dstShort;

	if (gamma != gammaValue || gBPP != bpp)
		initGammaTable(gamma, bpp);


	if (bpp > 8)
	{
		srcShort = (WORD *) (in_bytes);
		dstShort = (WORD *) (out_bytes);

        if(dstShort == 0)
        {
            printf("dstShort is null\n");
            return;
        }

        if(srcShort == 0)
        {
            printf("srcShort is null\n");
            return;
        }

		for (i=0; i<width*height; i++)
        {
            *dstShort++ = linear_to_gamma[*srcShort++];
        }
	}
	else
	{
		for (i=0; i<width*height; i++)
			*out_bytes++ = linear_to_gamma[*in_bytes++];
	}
}
////////////// GAMMA_CORRECTION ///////////////////////

// input image: two images interlaced pixel by pixel.
//              In each WORD, the MSB 8bits are for the left image, the LSB 8bits are for the right image
// output image: two image side by side, each pixel is one byte.
 void convDualImage(BYTE* in_bytes, BYTE* out_bytes, int width, int height)
{
	int i, j;
	BYTE *srcShort;
	BYTE *dstShortL;
	BYTE *dstShortR;

	srcShort = (BYTE *) (in_bytes);
	dstShortL = (BYTE *) (out_bytes);
	dstShortR = dstShortL + width;

	for (i=0; i<height; i++)
	{
		for (j=0; j<width; j++)
		{
			*dstShortL++ = *srcShort++;
			*dstShortR++ = *srcShort++;
		}
		dstShortL += width;
		dstShortR += width;
	}
}

void rgb2rgb(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
					int rr,int rg,int rb,int gr,int gg,int gb,int br,int bg,int bb,
					int r_offset, int g_offset, int b_offset)
{
	int i, j;
	int r_in, g_in, b_in, r_out, g_out, b_out;

	for (i=0; i<height; i++)
		for (j=0; j<width; j++)
		{
			r_in = *in_bytes++;
			g_in = *in_bytes++;
			b_in = *in_bytes++;

			r_out = (rr * r_in + rg * g_in + rb * b_in) / 256 + r_offset;
			g_out = (gr * r_in + gg * g_in + gb * b_in) / 256 + g_offset;
			b_out = (br * r_in + bg * g_in + bb * b_in) / 256 + b_offset;

			*out_bytes++ = (r_out > 255) ? 255 : ( (r_out < 0) ? 0 : r_out);
			*out_bytes++ = (g_out > 255) ? 255 : ( (g_out < 0) ? 0 : g_out);
			*out_bytes++ = (b_out > 255) ? 255 : ( (b_out < 0) ? 0 : b_out);
		}
}
/*
 int get_uvc_extension_property_value(IBaseFilter* camera, int property_id)
{
	HRESULT hr;
	IKsTopologyInfo *pKsTopologyInfo;
	hr = camera->QueryInterface(__uuidof(IKsTopologyInfo), (void **) &pKsTopologyInfo);

	DWORD numberOfNodes;
	hr = pKsTopologyInfo->get_NumNodes(&numberOfNodes);

	DWORD i;	GUID nodeGuid;
	for (i = 0; i < numberOfNodes; i++)
	{
		if (SUCCEEDED(pKsTopologyInfo->get_NodeType(i, &nodeGuid)))
		{
			if ( nodeGuid == KSNODETYPE_DEV_SPECIFIC )
			{ // Found the extension node
				DWORD pNodeId = i;
				IKsNodeControl *pUnk;
				IKsControl *pKsControl;
				BYTE buf[100];

				// create node instance
				hr = pKsTopologyInfo->CreateNodeInstance( i, __uuidof(IUnknown), (VOID**) &pUnk );
				hr = pUnk->QueryInterface( __uuidof(IKsControl),  (VOID**)&pKsControl );

				KSP_NODE  s;	ULONG  ulBytesReturned;
				buf[0] = 0x00;	buf[1] = 0x00;

				// this is guid of our device extension unit
				s.Property.Set = GUID_EXTENSION_UNIT_DESCRIPTOR;
				s.Property.Id = property_id;
				s.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
				s.NodeId = i;
				hr = pKsControl->KsProperty( (PKSPROPERTY) &s, sizeof(s),&buf[0], 2, &ulBytesReturned );

				int ret = ((int)buf[1])<<8 | buf[0];
				return ret;
			}
		}
	}

	return -1;
}

 int set_uvc_extension_property_value(IBaseFilter* camera, int property_id, byte byte1, byte byte0)
{
	HRESULT hr;
	IKsTopologyInfo *pKsTopologyInfo;
	hr = camera->QueryInterface(__uuidof(IKsTopologyInfo), (void **) &pKsTopologyInfo);

	DWORD numberOfNodes;	hr = pKsTopologyInfo->get_NumNodes(&numberOfNodes);

	DWORD i;	GUID nodeGuid;
	for (i = 0; i < numberOfNodes; i++)
	{
		if (SUCCEEDED(pKsTopologyInfo->get_NodeType(i, &nodeGuid)))
		{
			if ( nodeGuid == KSNODETYPE_DEV_SPECIFIC )
			{ // Found the extension node
				DWORD pNodeId = i;
				IKsNodeControl *pUnk;
				IKsControl *pKsControl;
				BYTE buf[100];

				// create node instance
				hr = pKsTopologyInfo->CreateNodeInstance( i, __uuidof(IUnknown), (VOID**) &pUnk );
				hr = pUnk->QueryInterface( __uuidof(IKsControl),  (VOID**)&pKsControl );

				KSP_NODE  s;	ULONG  ulBytesReturned;
				buf[1] = byte1; buf[0] = byte0;

				// this is guid of our device extension unit
				s.Property.Set = GUID_EXTENSION_UNIT_DESCRIPTOR;
				s.Property.Id = property_id;
				s.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
				s.NodeId = i;
				hr = pKsControl->KsProperty( (PKSPROPERTY) &s, sizeof(s),&buf[0], 2, &ulBytesReturned );

				if (hr == S_OK)	return 0;
				else return -1;
			}
		}
	}

	return -1;
}

 int read_from_uvc_extension(IBaseFilter* camera, int property_id, BYTE* bytes, int length, ULONG* ulBytesReturned)
{
	HRESULT hr;
	IKsTopologyInfo *pKsTopologyInfo;
	hr = camera->QueryInterface(__uuidof(IKsTopologyInfo), (void **) &pKsTopologyInfo);

	DWORD numberOfNodes;
	hr = pKsTopologyInfo->get_NumNodes(&numberOfNodes);

	DWORD i;	GUID nodeGuid;
	for (i = 0; i < numberOfNodes; i++)
	{
		if (SUCCEEDED(pKsTopologyInfo->get_NodeType(i, &nodeGuid)))
		{
			if ( nodeGuid == KSNODETYPE_DEV_SPECIFIC )
			{ // Found the extension node
				DWORD pNodeId = i;
				IKsNodeControl *pUnk;
				IKsControl *pKsControl;

				// create node instance
				hr = pKsTopologyInfo->CreateNodeInstance( i, __uuidof(IUnknown), (VOID**) &pUnk );
				hr = pUnk->QueryInterface( __uuidof(IKsControl),  (VOID**)&pKsControl );

				KSP_NODE  s;

				// this is guid of our device extension unit
				s.Property.Set = GUID_EXTENSION_UNIT_DESCRIPTOR;
				s.Property.Id = property_id;
				s.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
				s.NodeId = i;
				hr = pKsControl->KsProperty( (PKSPROPERTY) &s, sizeof(s),bytes, length, ulBytesReturned );

				return 0;
			}
		}
	}

	return -1;
}

 int write_to_uvc_extension(IBaseFilter* camera, int property_id, BYTE* bytes, int length, ULONG* ulBytesReturned)
{
	HRESULT hr;
	IKsTopologyInfo *pKsTopologyInfo;
	hr = camera->QueryInterface(__uuidof(IKsTopologyInfo), (void **) &pKsTopologyInfo);

	DWORD numberOfNodes;	hr = pKsTopologyInfo->get_NumNodes(&numberOfNodes);

	DWORD i;	GUID nodeGuid;
	for (i = 0; i < numberOfNodes; i++)
	{
		if (SUCCEEDED(pKsTopologyInfo->get_NodeType(i, &nodeGuid)))
		{
			if ( nodeGuid == KSNODETYPE_DEV_SPECIFIC )
			{ // Found the extension node
				DWORD pNodeId = i;
				IKsNodeControl *pUnk;
				IKsControl *pKsControl;

				// create node instance
				hr = pKsTopologyInfo->CreateNodeInstance( i, __uuidof(IUnknown), (VOID**) &pUnk );
				hr = pUnk->QueryInterface( __uuidof(IKsControl),  (VOID**)&pKsControl );

				KSP_NODE  s;	//ULONG  ulBytesReturned;

				// this is guid of our device extension unit
				s.Property.Set = GUID_EXTENSION_UNIT_DESCRIPTOR;
				s.Property.Id = property_id;
				s.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
				s.NodeId = i;
				hr = pKsControl->KsProperty( (PKSPROPERTY) &s, sizeof(s), bytes, length, ulBytesReturned );

				if (hr == S_OK)	return 0;
				else return -1;
			}
		}
	}

	return -1;
}*/

 int raw_to_bmp(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, int pixel_order,
									 bool GammaEna, double gamma,
									 int rr,int rg,int rb,int gr,int gg,int gb,int br,int bg,int bb,
									 int r_offset, int g_offset, int b_offset)
{
	int i,j;
	int shift = bpp - 8;
	unsigned short tmp;
	BYTE* ptr = in_bytes;
	BYTE* src = in_bytes;
	WORD* srcWord = (WORD *) in_bytes;
    unsigned char *middle;

	if (GammaEna)
	{
		src = in_bytes;
		gammaCorrection(src, src, width, height, bpp, gamma);
	}

	if (bpp > 8)
	{
		 srcWord = (WORD *) in_bytes;
		// convert 16bit bayer to 8bit bayer
		for (i=0; i<height; i++)
			for (j=0; j<width; j++)
			{
				tmp = (*srcWord++) >> shift;
				*ptr++ = (BYTE) tmp;
			}
	}

	src = in_bytes;
	bayer_to_rgb24(src, out_bytes, width, height, pixel_order);

	src = out_bytes;
	rgb2rgb(src, src, width, height, bpp, rr, rg, rb, gr, gg, gb, br, bg, bb, r_offset, g_offset, b_offset);
	return 0;
}

 int raw_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
									 bool GammaEna, double gamma)
{
	int i,j;
	int shift = bpp - 8;
	unsigned short tmp;
	BYTE* dst = out_bytes;
	BYTE* src = in_bytes;
	WORD* srcWord = (WORD *) in_bytes;

	if (GammaEna)
	{
		src = in_bytes;
		gammaCorrection(src, src, width, height, bpp, gamma);
	}

	if (bpp > 8)
	{
		 srcWord = (WORD *) in_bytes;
		// convert 16bit bayer to 8bit bayer
		for (i=0; i<height; i++)
			for (j=0; j<width; j++)
			{
				tmp = (*srcWord++) >> shift;
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
			}
	}
	else
	{
		for (i=0; i<height; i++)
			for (j=0; j<width; j++)
			{
				tmp = (*src++);
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
			}
	}
	return 0;
}

 double calc_mean(BYTE* in_bytes, int width, int height, int bpp, int startX, int startY, int iSize)
{
	WORD *srcWord;
	BYTE *srcByte;
	int i, j;
	double dSum=0.0;

	if (bpp > 8)
	{
		srcWord = (WORD *) in_bytes;
		srcWord += startY * width + startX;
		for (i=0; i<iSize; i++)
		{
			for (j=0; j<iSize; j++)
			{
				dSum += (double) (*srcWord++);
			}
			srcWord += width;
		}
	}
	else
	{
		srcByte = (BYTE *) in_bytes;
		srcByte += startY * width + startX;
		for (i=0; i<iSize; i++)
		{
			for (j=0; j<iSize; j++)
			{
				dSum += (double) (*srcByte++);
			}
			srcByte += width;
		}
	}

	return dSum / iSize / iSize;
}

 int yuv422_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height)
{
	int i,j;
	unsigned short tmp;
	BYTE* dst = out_bytes;
	BYTE* src = in_bytes;

	{
		for (i=0; i<height; i++)
			for (j=0; j<width; j++)
			{
				tmp = (*src++);
				*src++;
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
				*dst++ = (BYTE) tmp;
			}

	}
	return 0;
}

// remove all Cb & Cr, only leave y in the data
 int yuv422_to_y(BYTE* in_bytes, BYTE* out_bytes, int width, int height)
{
	int i,j;
	BYTE* dst = out_bytes;
	BYTE* src = in_bytes;

	{
		for (i=0; i<height; i++)
			for (j=0; j<width; j++)
			{
				*dst++ = (*src++);
				*src++;
			}

	}
	return 0;
}
 int y_SobleOperator(BYTE *in_buf, int iWidth, int iHeight, int startX, int startY, int iSize)
        {
	    int i,j;
            int HFFactor = 0;
            int w = iSize;
            int h = iSize;
            int x, y;
            unsigned char x1, x2, x3, x4, x5, x6;
            unsigned char y1, y2, y3, y4, y5, y6;

            unsigned char *inImage = in_buf + startY * iWidth + startX;

            for (i = 1; i < w - 1; i++)
                for (j = 1; j < h - 1; j++)
                {
                    x1 = *(inImage + (i - 1)*iWidth + j - 1);
                    x2 = *(inImage + (i - 1)*iWidth + j + 1);
                    x3 = *(inImage + (i)*iWidth + j - 1);
                    x4 = *(inImage + (i)*iWidth + j + 1);
                    x5 = *(inImage + (i + 1)*iWidth + j - 1);
                    x6 = *(inImage + (i + 1)*iWidth + j + 1);


                    y1 = *(inImage + (i - 1)*iWidth + j - 1);
                    y2 = *(inImage + (i - 1)*iWidth + j);
                    y3 = *(inImage + (i - 1)*iWidth + j + 1);
                    y4 = *(inImage + (i + 1)*iWidth + j - 1);
                    y5 = *(inImage + (i + 1)*iWidth + j);
                    y6 = *(inImage + (i + 1)*iWidth + j + 1);

                    x = -x1 + x2 - 2 * x3 + 2 * x4 - x5 + x6;
                    y = -y1 - 2 * y2 - y3 + y4 + 2 * y5 + y6;

                    HFFactor += x * x + y * y;
                }
            return HFFactor/iSize/iSize;
}

 int y_CalcMean(BYTE *in_buf, int iWidth, int iHeight, int startX, int startY, int iSize)
        {
            int i, j;
            int dTotal = 0, dMean = 0;

            int startPos = startY * iWidth + startX;

            {
                for (i = 0; i < iSize; i++)
                {
                    for (j = 0; j < iSize; j++)
                    {
                        dTotal += in_buf[startPos + (i * iWidth + j)];
                    }
                }
            }

            dMean = dTotal / iSize / iSize;

            return (dMean);
        }

// }
