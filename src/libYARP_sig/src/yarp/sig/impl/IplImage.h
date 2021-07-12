/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 1995, 2000 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

// originates from cvtypes.h in intel OpenCV project,
// some local modifications.

// check if CV is present
#ifndef _CV_H_
// check if IPL is present
#ifndef HAVE_IPL
// or asserted to be present
#ifndef HAVE_OPENCV

#ifndef YARP_CVTYPES_H_
#define YARP_CVTYPES_H_

// To simplify interaction with opencv header files, say that we
// have ipl headers
// We may fail anyway, but we will certainly fail otherwise.
#ifndef HAVE_IPL
#define HAVE_IPL
#endif


#include <cassert>
#include <cstdlib>

#if defined _MSC_VER || defined __BORLANDC__
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

#ifndef __IPL_H__
typedef unsigned char uchar;
#endif




/****************************************************************************************\
 *                                  Image type (IplImage)                                 *
\****************************************************************************************/

/*
 * The following definitions (until #endif)
 * is an extract from IPL headers.
 * Copyright (c) 1995 Intel Corporation.
 */

#define IPL_DEPTH_SIGN 0x80000000

#define IPL_DEPTH_1U     1
#define IPL_DEPTH_8U     8
#define IPL_DEPTH_16U   16
#define IPL_DEPTH_32F   32

#define IPL_DEPTH_8S  (int)(IPL_DEPTH_SIGN| 8)
#define IPL_DEPTH_16S (int)(IPL_DEPTH_SIGN|16)
#define IPL_DEPTH_32S (int)(IPL_DEPTH_SIGN|32)

#define IPL_DATA_ORDER_PIXEL  0
#define IPL_DATA_ORDER_PLANE  1

#define IPL_ORIGIN_TL 0
#define IPL_ORIGIN_BL 1

#define IPL_ALIGN_4BYTES   4
#define IPL_ALIGN_8BYTES   8
#define IPL_ALIGN_16BYTES 16
#define IPL_ALIGN_32BYTES 32

#define IPL_ALIGN_DWORD   IPL_ALIGN_4BYTES
#define IPL_ALIGN_QWORD   IPL_ALIGN_8BYTES

typedef struct _IplImage {
    int  nSize;         /**< sizeof(IplImage) */
    int  ID;            /**< version (=0)*/
    int  nChannels;     /**< Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;  /**< ignored by OpenCV */
    int  depth;         /**< pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                          IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4]; /**< ignored by OpenCV */
    char channelSeq[4]; /**< ignored by OpenCV */
    int  dataOrder;     /**< 0 - interleaved color channels, 1 - separate color channels.
                          cvCreateImage can only create interleaved images */
    int  origin;        /**< 0 - top-left origin,
                          1 - bottom-left origin (Windows bitmaps style) */
    int  align;         /**< Alignment of image rows (4 or 8).
                          OpenCV ignores it and uses widthStep instead */
    int  width;         /**< image width in pixels */
    int  height;        /**< image height in pixels */
    struct _IplROI *roi;/**< image ROI. if NULL, the whole image is selected */
    struct _IplImage *maskROI; /**< must be NULL */
    void  *imageId;     /**< must be NULL */
    struct _IplTileInfo *tileInfo; /**< must be null */
    int  imageSize;     /**< image data size in bytes
                          (==image->height*image->widthStep
                          in case of interleaved data)*/
    char *imageData;  /**< pointer to aligned image data */
    int  widthStep;   /**< size of aligned image row in bytes */
    int  BorderMode[4]; /**< ignored by OpenCV */
    int  BorderConst[4]; /**< ignored by OpenCV */
    char *imageDataOrigin; /**< pointer to very origin of image data
                             (not necessarily aligned) -
                             needed for correct deallocation */
}
IplImage;

typedef struct _IplTileInfo IplTileInfo;

typedef struct _IplROI {
    int  coi; /**< 0 - no COI (all channels are selected), 1 - 0th channel is selected ...*/
    int  xOffset;
    int  yOffset;
    int  width;
    int  height;
}
IplROI;

typedef struct _IplConvKernel
{
    int  nCols;
    int  nRows;
    int  anchorX;
    int  anchorY;
    int *values;
    int  nShiftR;
}
IplConvKernel;

typedef struct _IplConvKernelFP
{
    int  nCols;
    int  nRows;
    int  anchorX;
    int  anchorY;
    float *values;
}
IplConvKernelFP;

#define IPL_IMAGE_HEADER 1
#define IPL_IMAGE_DATA   2
#define IPL_IMAGE_ROI    4

#ifndef IPL_IMAGE_MAGIC_VAL
#define IPL_IMAGE_MAGIC_VAL  ((int)sizeof(IplImage))
#endif

/* for file storages make the value independent from arch */
#ifndef IPL_IMAGE_FILE_MAGIC_VAL
#define IPL_IMAGE_FILE_MAGIC_VAL  112
#endif

#ifndef IPL_DEPTH_64F
#define IPL_DEPTH_64F  64 /* for storing double-precision
                             floating point data in IplImage's */
#endif


// to fool a few Windows declarations.
//typedef int HDC;
//typedef char _TCHAR;

/**
 * Computes the ipl image padding.
 * @param lineSize the total length in bytes of the row of the image.
 * @param align is the alignment bytes (e.g. typically 8).
 * @return the number of bytes to add to the line to round with modulo 8.
 */
int _iplCalcPadding (int lineSize, int align);

/**
 * Definition for functions implemented within YARP_sig.
 */
#define IPLAPIIMPL(type,name,arg) extern type name arg


IPLAPIIMPL(IplConvKernel*,
           iplCreateConvKernel,(int nCols, int nRows,
                                int anchorX, int anchorY,
                                int* values, int nShiftR));

IPLAPIIMPL(IplConvKernelFP*,
           iplCreateConvKernelFP,(int nCols, int nRows,
                                  int anchorX, int anchorY, float* values));

IPLAPIIMPL(void,iplGetConvKernel,
           (IplConvKernel* kernel, int* nCols, int* nRows,
            int* anchorX, int* anchorY, int** values, int *nShiftR));


IPLAPIIMPL(void,iplGetConvKernelFP,
           (IplConvKernelFP* kernel,int* nCols, int* nRows,
            int* anchorX, int* anchorY, float** values));

IPLAPIIMPL(void, iplDeleteConvKernel,(IplConvKernel* kernel));

IPLAPIIMPL(void, iplDeleteConvKernelFP,(IplConvKernelFP* kernel));

IPLAPIIMPL(void, iplConvolve2D,
           (IplImage* srcImage, IplImage* dstImage,
            IplConvKernel** kernel, int nKernels, int combineMethod));

IPLAPIIMPL(void, iplConvolve2DFP,
           (IplImage* srcImage, IplImage* dstImage,
            IplConvKernelFP** kernel, int nKernels, int combineMethod));

IPLAPIIMPL(void, iplConvolveSep2DFP,(IplImage* srcImage,
                                     IplImage* dstImage,
                                     IplConvKernelFP* xKernel,
                                     IplConvKernelFP* yKernel));

//IPLAPIIMPL(IPLStatus, iplFixedFilter,(IplImage* srcImage, IplImage* dstImage,
//           IplFilter filter));

IPLAPIIMPL(void, iplConvolveSep2D,(IplImage* srcImage, IplImage* dstImage,
                                   IplConvKernel* xKernel, IplConvKernel* yKernel));

IPLAPIIMPL(void, iplAllocateImage,(IplImage* image, int doFill, int fillValue));

IPLAPIIMPL(void, iplAllocateImageFP,(IplImage* image, int doFill, float fillValue));


IPLAPIIMPL(void, iplDeallocateImage,(IplImage* image));

IPLAPIIMPL(IplImage*, iplCreateImageHeader,
           (int   nChannels,  int     alphaChannel, int     depth,
            char* colorModel, char*   channelSeq,   int     dataOrder,
            int   origin,     int     align,
            int   width,      int   height, IplROI* roi, IplImage* maskROI,
            void* imageId,    IplTileInfo* tileInfo));

IPLAPIIMPL(IplImage*, iplCloneImage, ( const IplImage* img ) );

IPLAPIIMPL(void, iplCopy, (IplImage* srcImage, IplImage* dstImage));

IPLAPIIMPL(void, iplDeallocateHeader,(IplImage* image));

IPLAPIIMPL(void, iplDeallocate,(IplImage* image, int flag));

IPLAPIIMPL(void,iplSetBorderMode,(IplImage *src,int mode,int border,int constVal));

IPLAPIIMPL(void, iplSet, (IplImage* image, int fillValue));

IPLAPIIMPL(void, iplSetFP, (IplImage* image, float fillValue));

IPLAPIIMPL(void, iplAddS,(IplImage* srcImage, IplImage* dstImage, int value));

IPLAPIIMPL(void, iplAdd,(IplImage* srcImageA, IplImage* srcImageB,
                         IplImage* dstImage));

IPLAPIIMPL(void, iplSubtract,(IplImage* srcImageA, IplImage* srcImageB,
                              IplImage* dstImage));

IPLAPIIMPL(void, iplSubtractS,(IplImage* srcImage, IplImage* dstImage, int value,
                               bool flip));

IPLAPIIMPL(void, iplMultiplySFP,(IplImage* srcImage, IplImage* dstImage,
                                 float value));

IPLAPIIMPL(void, iplAbs,(IplImage* srcImage, IplImage* dstImage));

IPLAPIIMPL(void, iplThreshold, (IplImage* srcImage, IplImage* dstImage, int threshold));

IPLAPIIMPL(void, iplColorToGray,(IplImage* srcImage, IplImage* dstImage));

IPLAPIIMPL(IplROI *,iplCreateROI,(int coi,    int xOffset, int   yOffset,
                                  int width, int height ));


IPLAPIIMPL(void, iplRGB2HSV,(IplImage* rgbImage, IplImage* hsvImage));

IPLAPIIMPL(void, iplHSV2RGB,(IplImage* hsvImage, IplImage* rgbImage));


IPLAPIIMPL(void, iplXorS,(IplImage* srcImage, IplImage* dstImage, unsigned int value));

#define IPL_BORDER_CONSTANT 0

#define IPL_SIDE_TOP_INDEX 0
#define IPL_SIDE_BOTTOM_INDEX 1
#define IPL_SIDE_LEFT_INDEX 2
#define IPL_SIDE_RIGHT_INDEX 3
#define IPL_SIDE_TOP (1<<IPL_SIDE_TOP_INDEX)
#define IPL_SIDE_BOTTOM (1<<IPL_SIDE_BOTTOM_INDEX)
#define IPL_SIDE_LEFT (1<<IPL_SIDE_LEFT_INDEX)
#define IPL_SIDE_RIGHT (1<<IPL_SIDE_RIGHT_INDEX)
#define IPL_SIDE_ALL (IPL_SIDE_RIGHT|IPL_SIDE_TOP|IPL_SIDE_LEFT|IPL_SIDE_BOTTOM)

#define IPL_DEPTH_MASK 0x7FFFFFFF

#define IPL_IMAGE_HEADER 1
#define IPL_IMAGE_DATA   2
#define IPL_IMAGE_ROI    4
#define IPL_IMAGE_TILE   8
#define IPL_IMAGE_MASK  16
#define IPL_IMAGE_ALL (IPL_IMAGE_HEADER|IPL_IMAGE_DATA|\
                       IPL_IMAGE_TILE|IPL_IMAGE_ROI|IPL_IMAGE_MASK)
#define IPL_IMAGE_ALL_WITHOUT_MASK (IPL_IMAGE_HEADER|IPL_IMAGE_DATA|\
                       IPL_IMAGE_TILE|IPL_IMAGE_ROI)

#define IPL_INTER_NN          0
#define IPL_INTER_LINEAR      1
#define IPL_INTER_CUBIC       2
#define IPL_INTER_SUPER       3
#define IPL_SMOOTH_EDGE      16

#define YARP_IMAGE_ALIGN     8

#endif /*YARP_CVTYPES_H_*/
#endif /*HAVE_OPENCV*/
#endif /*HAVE_IPL*/
#endif /*_CV_H_*/

/* End of file. */
