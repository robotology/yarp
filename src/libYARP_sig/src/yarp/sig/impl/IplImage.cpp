/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <cstring>

#include <yarp/os/Log.h>
#include <yarp/sig/impl/IplImage.h>

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

    char *p = ((char *)ptr) + addbytes;
    *(p - 1) = addbytes;
    return reinterpret_cast<T*>(p);
}

template <class T>
void FreeAligned (T* ptr)
{
    if (ptr == nullptr) {
        return;
    }

    const char addbytes = *(((char *)ptr) - 1);
    delete[] reinterpret_cast<T*>(((char *)ptr) - addbytes);
}

IPLAPIIMPL(void, iplAllocateImage,(IplImage* image))
{
    image->imageData = AllocAligned<char> (image->imageSize);
}

IPLAPIIMPL(void, iplDeallocateImage,(IplImage* image))
{
    if (image->imageData != nullptr) {
        FreeAligned<char>(image->imageData); ///delete[] image->imageData;
    }
    image->imageData = nullptr;
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
            int   width,      int   height))
{
    switch (depth)
        {
        default:
        case IPL_DEPTH_1U:
            return nullptr;

        case IPL_DEPTH_8U:
        case IPL_DEPTH_8S:
        case IPL_DEPTH_32F:
        case IPL_DEPTH_16U:
        case IPL_DEPTH_16S:
        case IPL_DEPTH_32S:
            break;
        }

    IplImage *r = nullptr;
    r = new IplImage;
    yAssert(r != nullptr);

    r->nSize = sizeof(IplImage);
    r->ID = 0xf0f0f0f0; // pasa's ID for IPL under QNX.

    r->nChannels = nChannels;
    r->alphaChannel = alphaChannel;
    r->depth = depth;

    memcpy (r->colorModel, colorModel, 4);
    memcpy (r->channelSeq, channelSeq, 4);

    yAssert(dataOrder == IPL_DATA_ORDER_PIXEL);

    r->dataOrder = dataOrder;
    r->origin = origin;

    r->align = align;
    r->width = width;
    r->height = height;

    const int linew = width * (depth & IPL_DEPTH_MASK) / 8 * nChannels;
    r->widthStep = linew + PAD_BYTES(linew, align);

    r->imageSize = r->widthStep * height;
    r->imageData = nullptr;

    memset (r->BorderMode, 0, 4 * sizeof(int));
    memset (r->BorderConst, 0, 4 * sizeof(int));

    return r;
}

IPLAPIIMPL(void, iplDeallocateHeader,(IplImage* image))
{
    if (image == nullptr) {
        return;
    }

    yAssert(image->nSize == sizeof(IplImage));
    if (image->imageData != nullptr)
        {
            FreeAligned<char> (image->imageData);
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
        }
}

IPLAPIIMPL(void,iplSetBorderMode,(IplImage *src,int mode,int border,int constVal))
{
    for (int i = 0; i < 4; i++) {
        if ((border >> i) & 0x1)
            {
                src->BorderMode[i] = mode;
                src->BorderConst[i] = constVal;
        }
    }
}

// not used outside this file.
#undef IPLAPIIMPL
