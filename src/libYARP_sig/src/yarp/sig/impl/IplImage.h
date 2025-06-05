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
 *                                  Image type (MiniIplImage)                                 *
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
    int  nChannels;     /**< Most of OpenCV functions support 1,2,3 or 4 channels */
    int  depth;         /**< pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                          IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    int  align;         /**< Alignment of image rows (4 or 8).
                          OpenCV ignores it and uses widthStep instead */
    int  width;         /**< image width in pixels */
    int  height;        /**< image height in pixels */
    int  imageSize;     /**< image data size in bytes
                          (==image->height*image->widthStep
                          in case of interleaved data)*/
    char *imageData;  /**< pointer to aligned image data */
    int  widthStep;   /**< size of aligned image row in bytes */
}
MiniIplImage;

typedef struct _IplTileInfo IplTileInfo;

#define IPL_IMAGE_HEADER 1
#define IPL_IMAGE_DATA   2
#define IPL_IMAGE_ROI    4


/**
 * Definition for functions implemented within YARP_sig.
 */
#define IPLAPIIMPL(type,name,arg) extern type name arg

IPLAPIIMPL(void, iplAllocateImage, (MiniIplImage * image));

IPLAPIIMPL(void, iplDeallocateImage, (MiniIplImage * image));

IPLAPIIMPL(MiniIplImage*, iplCreateImageHeader,
           (int   nChannels,  int     depth,
            int   align,
            int   width,      int   height));

IPLAPIIMPL(void, iplDeallocateHeader, (MiniIplImage * image));

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

#define YARP_IMAGE_ALIGN     8

#endif /*YARP_CVTYPES_H_*/
#endif /*HAVE_OPENCV*/
#endif /*HAVE_IPL*/
#endif /*_CV_H_*/

/* End of file. */
