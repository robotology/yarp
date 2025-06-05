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

char* AllocAligned (int size)
{
    char* ptr = new char[size + YARP_IMAGE_ALIGN];
    const int rem = (((size_t)ptr) % YARP_IMAGE_ALIGN);
    const char addbytes = YARP_IMAGE_ALIGN - rem;

    char *p = ((char *)ptr) + addbytes;
    *(p - 1) = addbytes;
    return p;
}

void FreeAligned (char* ptr)
{
    if (ptr == nullptr)
    {
        return;
    }

    const char addbytes = *(ptr - 1);
    delete[] (ptr - addbytes);
}

IPLAPIIMPL(void, iplAllocateImage, (MiniIplImage* image))
{
    image->imageData = AllocAligned (image->imageSize);
}

IPLAPIIMPL(void, iplDeallocateImage, (MiniIplImage* image))
{
    if (image->imageData != nullptr)
    {
        FreeAligned (image->imageData); ///delete[] image->imageData;
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
//  depth         - Bit depth of  pixels. Can be one of
//                      IPL_DEPTH_1U,
//                      IPL_DEPTH_8U,
//                      IPL_DEPTH_8S,
//                      IPL_DEPTH_16U,
//                      IPL_DEPTH_16S,
//                      IPL_DEPTH_32S.
//                      IPL_DEPTH_32F.
//  align         - Alignment of image data.
//                  Can be IPL_ALIGN_4BYTES (IPL_ALIGN_DWORD) or
//                  IPL_ALIGN_8BYTES (IPL_ALIGN_QWORD) or
//                  IPL_ALIGN_16BYTES IPL_ALIGN_32BYTES.
//  width         - Width of  the image in pixels.
//  height        - Height of  the image in pixels.
*/

IPLAPIIMPL(MiniIplImage*, iplCreateImageHeader,
           (int   nChannels,  int     depth,
            int   align,
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

    MiniIplImage* r = new MiniIplImage;
    yAssert(r != nullptr);

    r->nChannels = nChannels;
    r->depth = depth;

    r->align = align;
    r->width = width;
    r->height = height;

    const int linew = width * (depth & IPL_DEPTH_MASK) / 8 * nChannels;
    r->widthStep = linew + PAD_BYTES(linew, align);

    r->imageSize = r->widthStep * height;
    r->imageData = nullptr;

    return r;
}

IPLAPIIMPL(void, iplDeallocateHeader, (MiniIplImage * image))
{
    if (image == nullptr)
    {
        return;
    }

    if (image->imageData != nullptr)
    {
        FreeAligned (image->imageData);
    }

    delete image;
}

// not used outside this file.
#undef IPLAPIIMPL
