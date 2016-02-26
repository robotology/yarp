/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

///
/// $Id: IplImage.cpp,v 1.8 2008-10-14 15:56:49 eshuy Exp $
///
///

#include <stdio.h>
#include <string.h>

#include <yarp/os/Log.h>
#include <yarp/sig/IplImage.h>

static int implemented_yet = 1;

// this was from iplUtil.cpp
bool compareHeader(IplImage* A, IplImage* B)
{
    if( A->nChannels == B->nChannels && !strcmp(A->colorModel, B->colorModel) && !strcmp(A->channelSeq,B->channelSeq) && A->width == B->width
        && A->height == B->height)
        return true;
    else
        return false;
}

///
/// this might turn out to be useful.
inline int PAD_BYTES (int len, int pad)
{
    const int rem = len % pad;
    return (rem != 0) ? (pad - rem) : 0;
}

template <class T>
T* AllocAligned (int size)
{
    T *ptr = new T[size + YARP_IMAGE_ALIGN];
    const int rem = (((size_t)ptr) % YARP_IMAGE_ALIGN);
    const char addbytes = YARP_IMAGE_ALIGN - rem;
    ///((rem != 0) ? (YARP_IMAGE_ALIGN - rem) : 0);

    char *p = ((char *)ptr) + addbytes;
    *(p - 1) = addbytes;
    return (T*)p;
}

template <class T>
void FreeAligned (T* ptr)
{
    if (ptr == NULL) return;

    const char addbytes = *(((char *)ptr) - 1);
    delete[] (T *)(((char *)ptr) - addbytes);
}

///
///
/// WARNING:
///   most of this is implemented for PAD_BYTES == 0.
///
///   fixing the functions for != 0 is trivial though.
///
/// LATER:
///   externd for PAD_BYTES != 0.
///   NOT TESTED YET!
///

/*
  typedef struct _IplConvKernel {
  int           nCols;
  int           nRows;
  int           anchorX;
  int           anchorY;
  int          *values;
  int           nShiftR;
  } IplConvKernel;
*/

IPLAPIIMPL(IplConvKernel*, iplCreateConvKernel,(int nCols, int nRows,
                                                int anchorX, int anchorY, int* values, int nShiftR))
{
    IplConvKernel *ret = new IplConvKernel;
    yAssert(ret != NULL);

    ret->anchorX = anchorX;
    ret->anchorY = anchorY;
    ret->nCols = nCols;
    ret->nRows = nRows;
    ret->nShiftR = nShiftR;
    ret->values = new int[nCols * nRows];
    yAssert(ret->values != NULL);
    memcpy (ret->values, values, sizeof(int) * nCols * nRows);

    return ret;
}

/*
  typedef struct _IplConvKernelFP {
  int           nCols;
  int           nRows;
  int           anchorX;
  int           anchorY;
  float        *values;
  } IplConvKernelFP;
*/

IPLAPIIMPL(IplConvKernelFP*, iplCreateConvKernelFP,(int nCols, int nRows,
                                                    int anchorX, int anchorY, float* values))
{
    IplConvKernelFP *ret = new IplConvKernelFP;
    yAssert(ret != NULL);

    ret->anchorX = anchorX;
    ret->anchorY = anchorY;
    ret->nCols = nCols;
    ret->nRows = nRows;
    ret->values = new float[nCols * nRows];
    yAssert(ret->values != NULL);
    memcpy (ret->values, values, sizeof(float) * nCols * nRows);

    return ret;
}

IPLAPIIMPL(void,iplGetConvKernel,(IplConvKernel* kernel, int* nCols, int* nRows,
                                  int* anchorX, int* anchorY, int** values, int *nShiftR))
{
    yAssert(kernel != NULL);
    yAssert(kernel->values != NULL);

    *nCols = kernel->nCols;
    *nRows = kernel->nRows;
    *anchorX = kernel->anchorX;
    *anchorY = kernel->anchorY;
    memcpy (*values, kernel->values, sizeof(int) * *nCols * *nRows);
    *nShiftR = kernel->nShiftR;
}

IPLAPIIMPL(void,iplGetConvKernelFP,(IplConvKernelFP* kernel,int* nCols, int* nRows,
                                    int* anchorX, int* anchorY, float** values))
{
    yAssert(kernel != NULL);
    yAssert(kernel->values != NULL);

    *nCols = kernel->nCols;
    *nRows = kernel->nRows;
    *anchorX = kernel->anchorX;
    *anchorY = kernel->anchorY;
    memcpy (*values, kernel->values, sizeof(float) * *nCols * *nRows);
}

IPLAPIIMPL(void, iplDeleteConvKernel,(IplConvKernel* kernel))
{
    if (kernel == NULL)
        return;

    delete[] kernel->values;
    delete kernel;
}

IPLAPIIMPL(void, iplDeleteConvKernelFP,(IplConvKernelFP* kernel))
{
    if (kernel == NULL)
        return;

    delete[] kernel->values;
    delete kernel;
}

// implemented for mono img. only.
// TODO: in place stuff.
IPLAPIIMPL(void, iplConvolve2D,(IplImage* srcImage, IplImage* dstImage,
                                IplConvKernel** kernel, int nKernels, int combineMethod))
{
    static char *__tmp_res = NULL;
    static int __tmp_size = -1;

    yAssert(nKernels == 1);
    // implemented only 1 kernel.

    // do not consider anchor, borders are not set, and assumes
    // that the kernel has odd dimensions (both x and y).

    IplConvKernel *ktmp = *kernel;
    int *values = ktmp->values;
    const int ksize = ktmp->nCols * ktmp->nRows;

    yAssert((ktmp->nCols % 2) != 0);
    yAssert((ktmp->nRows % 2) != 0);

    const int krows = ktmp->nRows;
    const int kcols = ktmp->nCols;
    const int borderx = kcols / 2;
    const int bordery = krows / 2;
    const int w = srcImage->width;
    const int h = srcImage->height;

    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->nChannels == 1); // Mono images only.

    if (__tmp_res == NULL)
        {
            __tmp_size = dstImage->imageSize;
            ///__tmp_res = new char[dstImage->imageSize];
            __tmp_res = AllocAligned<char> (dstImage->imageSize);
            yAssert(__tmp_res != NULL);
        }
    else
        {
            if (__tmp_size < dstImage->imageSize)
                {
                    // new size.
                    FreeAligned<char> (__tmp_res);
                    ///delete[] __tmp_res;
                    __tmp_size = dstImage->imageSize;
                    ///__tmp_res = new char[dstImage->imageSize];
                    __tmp_res = AllocAligned<char> (dstImage->imageSize);
                    yAssert(__tmp_res != NULL);
                }
        }

    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                int tmp;
                for (int i = bordery; i <  h - bordery; i++)
                    {
                        for (int j = borderx; j < w - borderx; j++)
                            {
                                tmp = 0;
                                for (int k = 0; k < krows; k++)
                                    for (int l = 0; l < kcols; l++)
                                        {
                                            tmp += srcImage->imageData[(i + k - bordery) * w + j + l - borderx]
                                                * values[ksize - k * kcols - l - 1];
                                        }
                                tmp >>= ktmp->nShiftR;
                                if (tmp > 255)
                                    tmp = 255;
                                else
                                    if (tmp < 0)
                                        tmp = 0;
                                __tmp_res[i * w + j] = char(tmp);
                            }
                    }

                memcpy (dstImage->imageData, __tmp_res, dstImage->imageSize);
            }
            break;

        case IPL_DEPTH_8S:
            {
                int tmp;
                for (int i = bordery; i <  h - bordery; i++)
                    {
                        for (int j = borderx; j < w - borderx; j++)
                            {
                                tmp = 0;
                                for (int k = 0; k < krows; k++)
                                    for (int l = 0; l < kcols; l++)
                                        {
                                            tmp += srcImage->imageData[(i + k - bordery) * w + j + l - borderx]
                                                * values[ksize - k * kcols - l - 1];
                                        }
                                tmp >>= ktmp->nShiftR;
                                if (tmp > 127)
                                    tmp = 127;
                                else
                                    if (tmp < -128)
                                        tmp = -128;
                                __tmp_res[i * w + j] = char(tmp);
                            }
                    }

                memcpy (dstImage->imageData, __tmp_res, dstImage->imageSize);
            }
            break;
        }
}

// Implemented for mono images only.
// LATER: extend to color.
// TODO: allow inplace operation.
// combineMethod is not used because only 1 kernel is allowed.
IPLAPIIMPL(void, iplConvolve2DFP,(IplImage* srcImage, IplImage* dstImage,
                                  IplConvKernelFP** kernel, int nKernels, int combineMethod))
{
    // INPLACE: the first time it need to allocate the wk array.
    // I do not really like this solution, it would be much better to
    // alloc the array together with the Kernel.
    // clearly this is also a memory LEAK!
    static float *__tmp_res = NULL;
    static int __tmp_size = -1;

    yAssert(nKernels == 1);
    // implemented only 1 kernel.

    // do not consider anchor, borders are not set, and assumes
    // that the kernel has odd dimensions (both x and y).

    IplConvKernelFP *ktmp = *kernel;
    float *values = ktmp->values;
    const int ksize = ktmp->nCols * ktmp->nRows;

    yAssert((ktmp->nCols % 2) != 0);
    yAssert((ktmp->nRows % 2) != 0);

    const int kcols = ktmp->nCols;
    const int krows = ktmp->nRows;
    const int borderx = kcols / 2;
    const int bordery = krows / 2;
    const int w = srcImage->width;
    const int h = srcImage->height;

    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->nChannels == 1); // Mono images only.
    yAssert(srcImage->depth == IPL_DEPTH_32F);

    if (__tmp_res == NULL)
        {
            __tmp_size = dstImage->imageSize / sizeof(float);
            ///__tmp_res = new float[dstImage->imageSize / sizeof(float)];
            __tmp_res = AllocAligned<float> (dstImage->imageSize / sizeof(float));
            yAssert(__tmp_res != NULL);
        }
    else
        {
            if (__tmp_size < (int)(dstImage->imageSize / sizeof(float)))
                {
                    // new size.
                    ///delete[] __tmp_res;
                    FreeAligned<float> (__tmp_res);
                    __tmp_size = dstImage->imageSize / sizeof(float);
                    ///__tmp_res = new float[dstImage->imageSize / sizeof(float)];
                    __tmp_res = AllocAligned<float> (dstImage->imageSize / sizeof(float));
                    yAssert(__tmp_res != NULL);
                }
        }

    if (srcImage != dstImage)
        {
            float tmp;
            float *source = (float *)srcImage->imageData;
            float *dest = (float *)dstImage->imageData;
            for (int i = bordery; i <  h - bordery; i++)
                {
                    for (int j = borderx; j < w - borderx; j++)
                        {
                            tmp = 0;
                            for (int k = 0; k < krows; k++)
                                for (int l = 0; l < kcols; l++)
                                    {
                                        tmp += source[(i + k - bordery) * w + j + l - borderx]
                                            * values[ksize - k * kcols - l - 1];
                                    }
                            dest[i * w + j] = tmp;
                        }
                }
        }
    else
        {
            // inplace.
            float tmp;
            float *source = (float *)srcImage->imageData;
            //float *dest = (float *)dstImage->imageData;
            for (int i = bordery; i <  h - bordery; i++)
                {
                    for (int j = borderx; j < w - borderx; j++)
                        {
                            tmp = 0;
                            for (int k = 0; k < krows; k++)
                                for (int l = 0; l < kcols; l++)
                                    {
                                        tmp += source[(i + k - bordery) * w + j + l - borderx]
                                            * values[ksize - k * kcols - l - 1];
                                    }
                            __tmp_res[i * w + j] = tmp;
                        }
                }

            memcpy (dstImage->imageData, __tmp_res, dstImage->imageSize);
        }
}

// TODO: inplace operation.
IPLAPIIMPL(void, iplConvolveSep2DFP,(IplImage* srcImage,
                                     IplImage* dstImage,
                                     IplConvKernelFP* xKernel,
                                     IplConvKernelFP* yKernel))
{
    // here too I need memory to support inplace operations.
    static float *__tmp_res = NULL;
    static int __tmp_size = -1;

    if (xKernel != NULL)
        {
            yAssert(xKernel->nRows == 1);
            yAssert((xKernel->nCols % 2) != 0);
        }
    if (yKernel != NULL)
        {
            yAssert(yKernel->nCols == 1);
            yAssert((yKernel->nRows % 2) != 0);
        }

    // do not consider anchor, borders are not set, and assumes
    // that the kernel has odd dimensions (x only).
    float *xvalues = (xKernel != NULL) ? xKernel->values : NULL;
    const int xksize = (xKernel != NULL) ? xKernel->nCols : 0;
    float *yvalues = (yKernel != NULL) ? yKernel->values : NULL;
    const int yksize = (yKernel != NULL) ? yKernel->nRows : 0;

    const int borderx = (xKernel != NULL) ? xKernel->nCols / 2 : 0;
    const int bordery = (yKernel != NULL) ? yKernel->nRows / 2 : 0;
    const int w = srcImage->width;
    const int h = srcImage->height;

    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->nChannels == 1); // Mono images only.
    yAssert(srcImage->depth == IPL_DEPTH_32F);

    if (__tmp_res == NULL)
        {
            __tmp_size = dstImage->imageSize / sizeof(float);
            ///__tmp_res = new float[dstImage->imageSize / sizeof(float)];
            __tmp_res = AllocAligned<float> (dstImage->imageSize / sizeof(float));
            yAssert(__tmp_res != NULL);
        }
    else
        {
            if (__tmp_size < int(dstImage->imageSize / sizeof(float)))
                {
                    // new size.
                    ///delete[] __tmp_res;
                    FreeAligned<float> (__tmp_res);
                    __tmp_size = dstImage->imageSize / sizeof(float);
                    ///__tmp_res = new float[dstImage->imageSize / sizeof(float)];
                    __tmp_res = AllocAligned<float> (dstImage->imageSize / sizeof(float));
                    yAssert(__tmp_res != NULL);
                }
        }

    // inplace.
    float *src = (float *)srcImage->imageData;
    float *dst = (float *)dstImage->imageData;
    if (xKernel != NULL)
        {
            // apply x kernel.
            float tmp;
            for (int i = 0; i <  h; i++)
                {
                    for (int j = borderx; j < w - borderx; j++)
                        {
                            tmp = 0;
                            for (int k = 0; k < xksize; k++)
                                {
                                    tmp += src[i * w + j + k - borderx]
                                        * xvalues[xksize - k - 1];
                                }
                            __tmp_res[i * w + j] = tmp;
                        }
                }
        }

    if (yKernel != NULL)
        {
            // apply y kernel.
            float tmp;
            for (int i = bordery; i <  h - bordery; i++)
                {
                    // can save borderx (if applied)!
                    for (int j = borderx; j < w - borderx; j++)
                        {
                            tmp = 0;
                            for (int k = 0; k < yksize; k++)
                                {
                                    tmp += __tmp_res[(i + k - bordery) * w + j]
                                        * yvalues[yksize - k - 1];
                                }
                            dst[i * w + j] = tmp;
                        }
                }
        }
}

/*
  IPLAPIIMPL(IPLStatus, iplFixedFilter,(IplImage* srcImage, IplImage* dstImage,
  IplFilter filter))
  {
  // NOT IMPLEMENTED YET.
  yAssert(implemented_yet == 0);
  return -1;
  }
*/

IPLAPIIMPL(void, iplConvolveSep2D,(IplImage* srcImage, IplImage* dstImage,
                                   IplConvKernel* xKernel, IplConvKernel* yKernel))
{
    // in place stuff.
    static char *__tmp_res = NULL;
    static int __tmp_size = -1;

    if (xKernel != NULL)
        {
            yAssert(xKernel->nRows == 1);
            yAssert((xKernel->nCols % 2) != 0);
        }
    if (yKernel != NULL)
        {
            yAssert(yKernel->nCols == 1);
            yAssert((yKernel->nRows % 2) != 0);
        }

    // do not consider anchor, borders are not set, and assumes
    // that the kernel has odd dimensions (x only).
    int *xvalues = (xKernel != NULL) ? xKernel->values : NULL;
    const int xksize = (xKernel != NULL) ? xKernel->nCols : 0;
    int *yvalues = (yKernel != NULL) ? yKernel->values : NULL;
    const int yksize = (yKernel != NULL) ? yKernel->nRows : 0;

    const int borderx = (xKernel != NULL) ? xKernel->nCols / 2 : 0;
    const int bordery = (yKernel != NULL) ? yKernel->nRows / 2 : 0;
    const int w = srcImage->width;
    const int h = srcImage->height;

    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->nChannels == 1); // Mono images only.

    if (__tmp_res == NULL)
        {
            __tmp_size = dstImage->imageSize;
            ///__tmp_res = new char[dstImage->imageSize];
            __tmp_res = AllocAligned<char> (dstImage->imageSize);
            yAssert(__tmp_res != NULL);
        }
    else
        {
            if (__tmp_size < dstImage->imageSize)
                {
                    // new size.
                    FreeAligned<char> (__tmp_res);
                    ///delete[] __tmp_res;
                    __tmp_size = dstImage->imageSize;
                    ///__tmp_res = new char[dstImage->imageSize];
                    __tmp_res = AllocAligned<char> (dstImage->imageSize);
                    yAssert(__tmp_res != NULL);
                }
        }

    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                if (xKernel != NULL)
                    {
                        // apply x kernel.
                        int tmp;
                        for (int i = 0; i <  h; i++)
                            {
                                for (int j = borderx; j < w - borderx; j++)
                                    {
                                        tmp = 0;
                                        for (int k = 0; k < xksize; k++)
                                            {
                                                tmp += srcImage->imageData[i * w + j + k - borderx]
                                                    * xvalues[xksize - k - 1];
                                            }
                                        tmp >>= xKernel->nShiftR;
                                        if (tmp > 255)
                                            tmp = 255;
                                        else
                                            if (tmp < 0)
                                                tmp = 0;
                                        __tmp_res[i * w + j] = char(tmp);
                                    }
                            }
                    }

                if (yKernel != NULL)
                    {
                        // apply y kernel.
                        int tmp;
                        for (int i = bordery; i <  h - bordery; i++)
                            {
                                // can save borderx (if applied)!
                                for (int j = borderx; j < w - borderx; j++)
                                    {
                                        tmp = 0;
                                        for (int k = 0; k < yksize; k++)
                                            {
                                                tmp += __tmp_res[(i + k - bordery) * w + j]
                                                    * yvalues[yksize - k - 1];
                                            }
                                        tmp >>= yKernel->nShiftR;
                                        if (tmp > 255)
                                            tmp = 255;
                                        else
                                            if (tmp < 0)
                                                tmp = 0;
                                        dstImage->imageData[i * w + j] = char(tmp);
                                    }
                            }
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                if (xKernel != NULL)
                    {
                        int tmp;
                        for (int i = 0; i <  h; i++)
                            {
                                for (int j = borderx; j < w - borderx; j++)
                                    {
                                        tmp = 0;
                                        for (int k = 0; k < xksize; k++)
                                            {
                                                tmp += srcImage->imageData[i * w + j + k - borderx]
                                                    * xvalues[xksize - k - 1];
                                            }
                                        tmp >>= xKernel->nShiftR;
                                        if (tmp > 127)
                                            tmp = 127;
                                        else
                                            if (tmp < -128)
                                                tmp = -128;
                                        __tmp_res[i * w + j] = char(tmp);
                                    }
                            }
                    }

                if (yKernel != NULL)
                    {
                        int tmp;
                        for (int i = bordery; i <  h - bordery; i++)
                            {
                                for (int j = borderx; j < w - borderx; j++)
                                    {
                                        tmp = 0;
                                        for (int k = 0; k < yksize; k++)
                                            {
                                                tmp += __tmp_res[(i + k - bordery) * w + j]
                                                    * yvalues[yksize - k - 1];
                                            }
                                        tmp >>= yKernel->nShiftR;
                                        if (tmp > 127)
                                            tmp = 127;
                                        else
                                            if (tmp < -128)
                                                tmp = -128;
                                        dstImage->imageData[i * w + j] = char(tmp);
                                    }
                            }
                    }
            }
        }
}

// TODO: manage IPL ROI and tiling.
IPLAPIIMPL(void, iplAllocateImage,(IplImage* image, int doFill, int fillValue))
{
    // Not implemented depth != 8
    //int depth = (image->depth & IPL_DEPTH_MASK)/8;
    yAssert(image->dataOrder == IPL_DATA_ORDER_PIXEL);
    ///yAssert(image->widthStep == image->width * (image->depth & IPL_DEPTH_MASK) / 8 * image->nChannels);
    yAssert(image->imageSize == image->widthStep * image->height);

    image->imageData = AllocAligned<char> (image->imageSize); // new char[image->imageSize];
    yAssert(image->imageData != NULL);

    if (image->origin == IPL_ORIGIN_TL)
        image->imageDataOrigin = image->imageData + image->imageSize - image->widthStep;
    else
        image->imageDataOrigin = image->imageData;

    if (doFill)
        {
            // this of course is only valid for depth == 8.
            switch (image->depth)
                {
                case IPL_DEPTH_8U:
                case IPL_DEPTH_8S:
                    memset (image->imageData, fillValue, image->imageSize);
                    break;

                default:
                    yAssert(1 == 0);
                    break;
                }
        }
}

IPLAPIIMPL(void, iplAllocateImageFP,(IplImage* image, int doFill, float fillValue))
{
    yAssert(image->depth == IPL_DEPTH_32F);
    yAssert(image->dataOrder == IPL_DATA_ORDER_PIXEL);
    // yAssert(image->widthStep == image->width * (image->depth & IPL_DEPTH_MASK) / 8 * image->nChannels);
    yAssert(image->imageSize == image->widthStep * image->height);

    image->imageData = AllocAligned<char> (image->imageSize);
    yAssert(image->imageData != NULL);

    if (image->origin == IPL_ORIGIN_TL)
        image->imageDataOrigin = image->imageData + image->imageSize - image->widthStep;
    else
        image->imageDataOrigin = image->imageData;

    if (doFill)
        {
            if (fillValue == 0)
                {
                    // assume IEEE float
                    memset (image->imageData, 0, image->imageSize);
                }
            else
                {
                    yAssert(PAD_BYTES (image->widthStep, YARP_IMAGE_ALIGN) == 0);

                    // time consuming
                    float *tmp = (float *)image->imageData;
                    const int limit = image->imageSize / sizeof(float);
                    for (int i = 0; i < limit; i++)
                        {
                            *tmp++ = float(fillValue);
                        }
                }
        }
}

IPLAPIIMPL(void, iplDeallocateImage,(IplImage* image))
{
    if (image->imageData != NULL)
        FreeAligned<char> (image->imageData); ///delete[] image->imageData;
    image->imageData = NULL;

    // Not allocated.
    image->roi = NULL;
}


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateImageHeader
// Purpose:    Creates an IPL image header according to the specified
//             attributes.
// Returns:    The newly constructed IPL image header.
// Parameters:
//  nChannels     - Number of channels in the image.
//  alphaChannel  - Alpha channel number (0 if no alpha channel in image).
//  depth         - Bit depth of  pixels. Can be one of
//                      IPL_DEPTH_1U,
//                      IPL_DEPTH_8U,
//                      IPL_DEPTH_8S,
//                      IPL_DEPTH_16U,
//                      IPL_DEPTH_16S,
//                      IPL_DEPTH_32S.
//                      IPL_DEPTH_32F.
//  colorModel    - A four character array describing the color model,
//                  e.g. "RGB", "GRAY", "MSI" etc.
//  channelSeq    - The sequence of channels in the image,
//                  e.g. "BGR" for an RGB image.
//  dataOrder     - IPL_DATA_ORDER_PIXEL or IPL_DATA_ORDER_PLANE.
//  origin        - The origin of the image.
//                  Can be IPL_ORIGIN_TL or IPL_ORIGIN_BL.
//  align         - Alignment of image data.
//                  Can be IPL_ALIGN_4BYTES (IPL_ALIGN_DWORD) or
//                  IPL_ALIGN_8BYTES (IPL_ALIGN_QWORD) or
//                  IPL_ALIGN_16BYTES IPL_ALIGN_32BYTES.
//  width         - Width of  the image in pixels.
//  height        - Height of  the image in pixels.
//  roi           - Pointer to an ROI (region of interest) structure.
//                  This can be NULL (implying a region of interest comprising
//                  all channels and the entire image area).
//  maskROI       - Pointer on mask image
//  imageId       - use of the application
//  tileInfo      - contains information on tiling
//
// Notes:
*/

IPLAPIIMPL(IplImage*, iplCreateImageHeader,
           (int   nChannels,  int     alphaChannel, int     depth,
            char* colorModel, char*   channelSeq,   int     dataOrder,
            int   origin,     int     align,
            int   width,      int   height, IplROI* roi, IplImage* maskROI,
            void* imageId,    IplTileInfo* tileInfo))
{
    switch (depth)
        {
        default:
        case IPL_DEPTH_1U:
            return NULL;

        case IPL_DEPTH_8U:
        case IPL_DEPTH_8S:
        case IPL_DEPTH_32F:
        case IPL_DEPTH_16U:
        case IPL_DEPTH_16S:
        case IPL_DEPTH_32S:
            break;
        }

    IplImage *r = NULL;
    r = new IplImage;
    yAssert(r != NULL);

    r->nSize = sizeof(IplImage);
    r->ID = 0xf0f0f0f0; // pasa's ID for IPL under QNX.

    r->nChannels = nChannels;
    r->alphaChannel = alphaChannel;
    r->depth = depth;

    memcpy (r->colorModel, colorModel, 4);
    memcpy (r->channelSeq, channelSeq, 4);

    yAssert(dataOrder == IPL_DATA_ORDER_PIXEL);

    r->dataOrder = dataOrder; // IPL_DATA_ORDER_PIXEL, IPL_DATA_ORDER_PLANE
    r->origin = origin;

    //yAssert(align == IPL_ALIGN_QWORD); /// don't want to be bothered w/ alignment beside the
    /// the 8 bytes stuff.
    //yAssert(align == YARP_IMAGE_ALIGN);

    r->align = align;
    r->width = width;
    r->height = height;
    r->roi = NULL;
    r->maskROI = NULL;
    r->imageId = NULL;

    r->tileInfo = NULL;
    const int linew = width * (depth & IPL_DEPTH_MASK) / 8 * nChannels;
    r->widthStep = linew + PAD_BYTES(linew, align);

    r->imageSize = r->widthStep * height;
    r->imageData = NULL;
    r->tileInfo = NULL;

    memset (r->BorderMode, 0, 4);
    memset (r->BorderConst, 0, 4);

    r->imageDataOrigin = NULL;
    return r;
}

IPLAPIIMPL(IplImage*, iplCloneImage, ( const IplImage* img ) )
{
    IplImage *ret = iplCreateImageHeader (
                                          img->nChannels, img->alphaChannel, img->depth, (char *)img->colorModel, (char *)img->channelSeq,
                                          img->dataOrder, img->origin, img->align, img->width, img->height, NULL, NULL,
                                          img->imageId, NULL);

    if (img->imageData != NULL)
        {
            switch (img->depth)
                {
                case IPL_DEPTH_8U:
                case IPL_DEPTH_8S:
                    iplAllocateImage (ret, 0, 0);
                    break;

                case IPL_DEPTH_32F:
                    iplAllocateImageFP (ret, 0, 0.0);
                    break;

                default:
                    yAssert(1 == 0);
                    break;
                }

            memcpy (ret->imageData, img->imageData, img->imageSize);
        }

    return ret;
}

IPLAPIIMPL(void, iplCopy, (IplImage* srcImage, IplImage* dstImage))
{
    yAssert(srcImage->imageData != NULL && dstImage->imageData != NULL);
    memcpy (dstImage->imageData, srcImage->imageData, srcImage->imageSize);
}

IPLAPIIMPL(void, iplDeallocateHeader,(IplImage* image))
{
    if (image == NULL)
        return;

    yAssert(image->nSize == sizeof(IplImage));
    if (image->imageData != NULL)
        {
            FreeAligned<char> (image->imageData);
            ///delete[] image->imageData;
        }

    delete image;
}

IPLAPIIMPL(void, iplDeallocate,(IplImage* image, int flag))
{
    switch (flag)
        {
        case IPL_IMAGE_ALL_WITHOUT_MASK:
        case IPL_IMAGE_ALL:
        case IPL_IMAGE_HEADER:
            iplDeallocateHeader (image);
            break;

        case IPL_IMAGE_DATA:
            iplDeallocateImage (image);
            break;

        case IPL_IMAGE_ROI:
        case IPL_IMAGE_TILE:
        case IPL_IMAGE_MASK:
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void,iplSetBorderMode,(IplImage *src,int mode,int border,int constVal))
{
    for (int i = 0; i < 4; i++)
        if ((border >> i) & 0x1)
            {
                src->BorderMode[i] = mode;
                src->BorderConst[i] = constVal;
            }
}

// this is ok only for 8 bits pixel/planes images. RGB and HSV are ok too.
IPLAPIIMPL(void, iplSet, (IplImage* image, int fillValue))
{
    yAssert(image->imageData != NULL);
    yAssert((image->depth & IPL_DEPTH_MASK) == 8);
    memset (image->imageData, fillValue, image->imageSize);
}

IPLAPIIMPL(void, iplSetFP, (IplImage* image, float fillValue))
{
    yAssert(image->imageData != NULL);
    yAssert(image->depth == IPL_DEPTH_32F);

    const int size = image->imageSize / sizeof(float);
    float *tmp = (float *)image->imageData;
    for (int i = 0; i < size; i++)
        *tmp++ = fillValue;
}

// only 8 bits supported. Clipping is carried out to be ipl compatible.
IPLAPIIMPL(void, iplAddS,(IplImage* srcImage, IplImage* dstImage, int value))
{
    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->depth == dstImage->depth);

    // assume images have the same size and 8 bits/pixel/planes.
    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                const int size = srcImage->imageSize;
                unsigned char * src = (unsigned char *)srcImage->imageData;
                unsigned char * dst = (unsigned char *)dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src++ + value;
                        if (tmp < 0)
                            tmp = 0;
                        else
                            if (tmp > 255)
                                tmp = 255;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImage->imageSize;
                char * src = srcImage->imageData;
                char * dst = dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src++ + value;
                        if (tmp < -128)
                            tmp = -128;
                        else
                            if (tmp > 127)
                                tmp = 127;
                        *dst++ = char(tmp);
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void, iplAdd,(IplImage* srcImageA, IplImage* srcImageB,
                         IplImage* dstImage))
{
    yAssert(compareHeader (srcImageA, srcImageB));
    yAssert(compareHeader (srcImageB, dstImage));

    yAssert(srcImageA->depth == srcImageB->depth);
    yAssert(srcImageA->depth == dstImage->depth);

    // assume images have the same size and 8 bits/pixel/planes.
    switch (srcImageA->depth)
        {
        case IPL_DEPTH_8U:
            {
                const int size = srcImageA->imageSize;
                unsigned char * src1 = (unsigned char *)srcImageA->imageData;
                unsigned char * src2 = (unsigned char *)srcImageB->imageData;
                unsigned char * dst = (unsigned char *)dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src1++ + *src2++;
                        if (tmp > 255)
                            tmp = 255;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImageA->imageSize;
                char * src1 = srcImageA->imageData;
                char * src2 = srcImageB->imageData;
                char * dst = dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src1++ + *src2++;
                        if (tmp < -128)
                            tmp = -128;
                        else
                            if (tmp > 127)
                                tmp = 127;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_32F:
            {
                const int size = srcImageA->imageSize / sizeof(float);
                float * src1 = (float *)srcImageA->imageData;
                float * src2 = (float *)srcImageB->imageData;
                float * dst = (float *)dstImage->imageData;

                for (int i = 0; i < size; i++)
                    {
                        *dst++ = *src1++ + *src2++;
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void, iplSubtract,(IplImage* srcImageA, IplImage* srcImageB,
                              IplImage* dstImage))
{
    yAssert(compareHeader (srcImageA, srcImageB));
    yAssert(compareHeader (srcImageB, dstImage));

    // assume images have the same size and 8 bits/pixel/planes.
    switch (srcImageA->depth)
        {
        case IPL_DEPTH_8U:
            {
                const int size = srcImageA->imageSize;
                unsigned char * src1 = (unsigned char *)srcImageA->imageData;
                unsigned char * src2 = (unsigned char *)srcImageB->imageData;
                unsigned char * dst = (unsigned char *)dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src1++ - *src2++;
                        if (tmp < 0)
                            tmp = 0;
                        if (tmp > 255)
                            tmp = 255;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImageA->imageSize;
                char * src1 = srcImageA->imageData;
                char * src2 = srcImageB->imageData;
                char * dst = dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        tmp = *src1++ - *src2++;
                        if (tmp < -128)
                            tmp = -128;
                        else
                            if (tmp > 127)
                                tmp = 127;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_32F:
            {
                const int size = srcImageA->imageSize / sizeof(float);
                float * src1 = (float *)srcImageA->imageData;
                float * src2 = (float *)srcImageB->imageData;
                float * dst = (float *)dstImage->imageData;

                for (int i = 0; i < size; i++)
                    {
                        *dst++ = *src1++ - *src2++;
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void, iplSubtractS,(IplImage* srcImage, IplImage* dstImage, int value,
                               bool flip))
{
    // assume images have the same size and 8 bits/pixel/planes.
    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                const int size = srcImage->imageSize;
                unsigned char * src = (unsigned char *)srcImage->imageData;
                unsigned char * dst = (unsigned char *)dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        if (flip)
                            tmp = value - *src++;
                        else
                            tmp = *src++ - value;

                        if (tmp < 0)
                            tmp = 0;
                        else
                            if (tmp > 255)
                                tmp = 255;
                        *dst++ = char(tmp);
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImage->imageSize;
                char * src = srcImage->imageData;
                char * dst = dstImage->imageData;

                short tmp;

                for (int i = 0; i < size; i++)
                    {
                        if (flip)
                            tmp = value - *src++;
                        else
                            tmp = *src++ - value;

                        if (tmp < -128)
                            tmp = -128;
                        else
                            if (tmp > 127)
                                tmp = 127;
                        *dst++ = char(tmp);
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void, iplMultiplySFP,(IplImage* srcImage, IplImage* dstImage,
                                 float value))
{
    yAssert(compareHeader (srcImage, dstImage));
    yAssert(srcImage->depth == IPL_DEPTH_32F);

    const int size = srcImage->imageSize / sizeof(float);
    float * src1 = (float *)srcImage->imageData;
    float * dst = (float *)dstImage->imageData;

    for (int i = 0; i < size; i++)
        {
            *dst++ = *src1++ * value;
        }
}

IPLAPIIMPL(void, iplAbs,(IplImage* srcImage, IplImage* dstImage))
{
    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                memcpy (dstImage->imageData, srcImage->imageData, dstImage->imageSize);
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImage->imageSize;
                char * src = srcImage->imageData;
                char * dst = dstImage->imageData;

                for (int i = 0; i < size; i++)
                    {
                        if (*src < 0)
                            *dst++ = -*src++;
                        else
                            *dst++ = *src++;
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

IPLAPIIMPL(void, iplThreshold, (IplImage* srcImage, IplImage* dstImage, int threshold))
{
    switch (srcImage->depth)
        {
        case IPL_DEPTH_8U:
            {
                const int size = srcImage->imageSize;
                unsigned char * src = (unsigned char *)srcImage->imageData;
                unsigned char * dst = (unsigned char *)dstImage->imageData;

                for (int i = 0; i < size; i++)
                    {
                        if (*src++ < threshold)
                            *dst++ = 0;
                        else
                            *dst++ = 255;
                    }
            }
            break;

        case IPL_DEPTH_8S:
            {
                const int size = srcImage->imageSize;
                char * src = srcImage->imageData;
                char * dst = dstImage->imageData;

                for (int i = 0; i < size; i++)
                    {
                        if (*src++ < threshold)
                            *dst++ = -128;
                        else
                            *dst++ = 127;
                    }
            }
            break;

        default:
            yAssert(1 == 0);
            // NOT IMPLEMENTED.
            break;
        }
}

// TODO: HSV to Gray!
IPLAPIIMPL(void, iplColorToGray,(IplImage* srcImage, IplImage* dstImage))
{
    yAssert(srcImage->width == dstImage->width);
    yAssert(srcImage->height == dstImage->height);
    yAssert(srcImage->depth == dstImage->depth);
    yAssert(srcImage->depth != IPL_DEPTH_32F);

    char *sdata = srcImage->imageData; // color
    char *dst = dstImage->imageData; // BW

    const int h = dstImage->height;
    const int w = dstImage->width;
    const int size = w * h;
    for (int i = 0; i < size; i++)
        {
            short tmp = *sdata++;
            tmp += *sdata++;
            tmp += *sdata++;
            tmp /= 3;

            *dst++ = char(tmp);
        }
}

IPLAPIIMPL(IplROI *,iplCreateROI,(int coi,    int xOffset, int   yOffset,
                                  int width, int height ))
{
    // NOT IMPLEMENTED YET.
    yAssert(implemented_yet == 0);
    return NULL;
}

IPLAPIIMPL(void, iplSetROI,(IplROI*   roi,      int coi,
                            int       xOffset,  int yOffset,
                            int width,          int height))
{
    yAssert(implemented_yet == 0);
}

// LATER: image types are not checked.
// It should be RGB not signed.
IPLAPIIMPL(void, iplRGB2HSV,(IplImage* rgbImage, IplImage* hsvImage))
{
    // Image types should be checked.
    //const int planesize = rgbImage->widthStep * rgbImage->height;
    unsigned char *sdata = (unsigned char *)rgbImage->imageData; // bgr
    unsigned char *ddata0 = (unsigned char *)hsvImage->imageData; // Hue.
    unsigned char *ddata1 = ddata0 + 1; // Saturation.
    unsigned char *ddata2 = ddata1 + 1; // Value.

    double max,min;
    double red,green,blue;
    double hue=0,sat;

    const int width = rgbImage->width;
    const int height = rgbImage->height;
    const int size = width * height; // # of pixels.

    for (int i = 0; i < size; i++)
        {
            blue = *sdata++ / 255.0;
            green = *sdata++ / 255.0;
            red = *sdata++ / 255.0;

            if (red > green && red > blue)
                {
                    max = red;
                    if (green > blue)
                        min = blue;
                    else
                        min = green;
                }
            else
                if (green > red && green > blue)
                    {
                        max = green;
                        if (red > blue)
                            min = blue;
                        else
                            min = red;
                    }
                else
                    {
                        max = blue;
                        if (red > green)
                            min = green;
                        else
                            min = red;
                    }

            // value
            *ddata2 = (unsigned char)(255.0 * max);
            ddata2 += 3;

            // saturation
            if (max != 0.0)
                {
                    sat = *ddata1 = (unsigned char)(255 * (max - min) / max);
                }
            else
                sat = *ddata1 = 0;
            ddata1 += 3;

            // hue
            if (sat == 0)
                *ddata0 = 0;
            else
                {
                    double rc = (max - red) / (max - min);
                    double gc = (max - green) / (max - min);
                    double bc = (max - blue) / (max - min);
                    if (red == max)
                        hue = bc - gc;
                    else
                        if (green == max)
                            hue = 2 + rc - bc;
                        else
                            if (blue == max)
                                hue = 4 + gc - rc;

                    hue *= 60.0;
                    if (hue < 0.0)
                        hue += 360.0;

                    yAssert(hue >= 0.0 && hue < 360.0);
                    // IPL 2.5 compatibility. Scaling to 0-255
                    // there's a little chance that the value rounds to 256.0!
                    // need clipping rather than truncation.
                    hue = (hue / 360.0 * 256.0);
                    if (hue == 256.0)
                        hue = 255.0;

                    *ddata0 = (unsigned char)(hue);
                }

            ddata0 += 3;
        }
}

IPLAPIIMPL(void, iplHSV2RGB,(IplImage* hsvImage, IplImage* rgbImage))
{
    // NOT IMPLEMENTED YET.
    yAssert(implemented_yet == 0);
}

IPLAPIIMPL(void, iplXorS,(IplImage* srcImage, IplImage* dstImage,
                          unsigned int value))
{
    // NOT IMPLEMENTED YET.s
    yAssert(1 == 0);
}

// computes the number of pad bytes (end of line) give the line len and
// the requested alignment in byte
inline int _iplCalcPadding (int lineSize, int align)
{
    return PAD_BYTES (lineSize, align);
}

// not used outside this file.
#undef IPLAPIIMPL
