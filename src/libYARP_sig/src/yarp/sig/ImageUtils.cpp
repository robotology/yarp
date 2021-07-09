/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/ImageUtils.h>
#include <cstring>
#include <algorithm> // std::math

using namespace yarp::sig;

static bool checkImages(const Image& bigImg, const Image& smallImg1, const Image& smallImg2)
{
    return bigImg.getPixelCode() == smallImg1.getPixelCode() &&
           bigImg.getPixelCode() == smallImg2.getPixelCode() &&
           smallImg1.width()  == smallImg2.width() &&
           smallImg1.height() == smallImg2.height() &&
           bigImg.getRawImageSize() == 2*smallImg1.getRawImageSize();

}


bool utils::vertSplit(const Image& inImg, Image& outImgL, Image& outImgR)
{
    outImgL.resize(inImg.width()/2, inImg.height());
    outImgR.resize(inImg.width()/2, inImg.height());

    if (!checkImages(inImg, outImgL, outImgR)) {
        return false;
    }

    size_t inHeight = inImg.height();
    size_t singleImage_rowSizeByte = outImgL.getRowSize();
    unsigned char *pixelLeft = outImgL.getRawImage();
    unsigned char *pixelRight = outImgR.getRawImage();
    unsigned char *pixelInput = inImg.getRawImage();

    for(size_t h=0; h<inHeight; h++)
    {
        // Copy the memory
        memcpy(pixelLeft  + h*singleImage_rowSizeByte, pixelInput, singleImage_rowSizeByte);
        memcpy(pixelRight + h*singleImage_rowSizeByte, pixelInput+=singleImage_rowSizeByte, singleImage_rowSizeByte);

        // Update the pointers
        pixelInput+= singleImage_rowSizeByte;
    }
    return true;
}

bool utils::horzSplit(const Image& inImg, Image& outImgUp, Image& outImgDown)
{
    outImgUp.resize(inImg.width(), inImg.height()/2);
    outImgDown.resize(inImg.width(), inImg.height()/2);

    if (!checkImages(inImg, outImgUp, outImgDown)) {
        return false;
    }
    // Copy the memory
    size_t imgSize = outImgUp.getRawImageSize();
    memcpy(outImgUp.getRawImage(), inImg.getRawImage(), imgSize);
    memcpy(outImgDown.getRawImage(), inImg.getRawImage() + imgSize, imgSize);
    return true;
}



bool utils::horzConcat(const Image& inImgL, const Image& inImgR, Image& outImg)
{
    outImg.resize(inImgL.width()*2, inImgL.height());

    if (!checkImages(outImg, inImgL, inImgR)) {
        return false;
    }

    size_t singleImage_rowSizeByte  = inImgL.getRowSize();
    unsigned char * pixelLeft = inImgL.getRawImage();
    unsigned char * pixelRight = inImgR.getRawImage();
    unsigned char * pixelOutLeft = outImg.getRawImage();
    unsigned char * pixelOutRight = outImg.getRawImage() + singleImage_rowSizeByte;

    size_t height = inImgL.height();

    for(size_t h=0; h<height; h++)
    {
        // Copy the memory
        memcpy(pixelOutLeft, pixelLeft, singleImage_rowSizeByte);
        memcpy(pixelOutRight, pixelRight, singleImage_rowSizeByte);

        // Update the pointers
        pixelOutLeft  += 2*singleImage_rowSizeByte;
        pixelOutRight += 2*singleImage_rowSizeByte;
        pixelLeft     += singleImage_rowSizeByte;
        pixelRight    += singleImage_rowSizeByte;
    }
    return true;
}

bool utils::vertConcat(const Image& inImgUp, const Image& inImgDown, Image& outImg)
{
    outImg.resize(inImgUp.width(), inImgUp.height()*2);

    if (!checkImages(outImg, inImgUp, inImgDown)) {
        return false;
    }

    // Copy the memory
    size_t imgSize = inImgUp.getRawImageSize();
    memcpy(outImg.getRawImage(), inImgUp.getRawImage(), imgSize);
    memcpy(outImg.getRawImage() + imgSize, inImgDown.getRawImage(), imgSize);
    return true;
}

bool utils::cropRect(const yarp::sig::Image& inImg,
                     const std::pair<unsigned int, unsigned int>& vertex1,
                     const std::pair<unsigned int, unsigned int>& vertex2,
                     yarp::sig::Image& outImg)
{
    if (inImg.getPixelCode() != outImg.getPixelCode()) {
        return false;
    }

    // Normalize vertices: upper-left (tlx,tly) and bottom-right (brx,bry) corners
    auto tlx = std::min(vertex1.first, vertex2.first);
    auto tly = std::min(vertex1.second, vertex2.second);
    auto brx = std::max(vertex1.first, vertex2.first);
    auto bry = std::max(vertex1.second, vertex2.second);

    if (!inImg.isPixel(brx, bry)) {
        return false;
    }

    outImg.resize(brx - tlx + 1, bry - tly + 1); // width, height

    auto * pixelOut = outImg.getRawImage();

    for (unsigned int row = 0; row < outImg.height(); row++) {
        const auto * pixelIn = inImg.getPixelAddress(tlx, tly + row);
        memcpy(pixelOut, pixelIn, outImg.getRowSize());
        pixelOut += outImg.getRowSize();
    }

    return true;
}
