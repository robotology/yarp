/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_IMAGEUTILS_H
#define YARP_SIG_IMAGEUTILS_H

#include <yarp/sig/Image.h>

namespace yarp {
namespace sig{
/**
 * \ingroup sig_class
 *
 * Image utilities.
 */
namespace utils
{

/**
 * @brief vertSplit, split vertically an image in two images of the same size.
 * @param inImg[in] image to be vertically split.
 * @param outImgL[out] left half of inImg.
 * @param outImgR[out] right half of inImg.
 * @note The input image must have same height, double width of the output images and same pixel type.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API vertSplit(const yarp::sig::Image& inImg, yarp::sig::Image& outImgL, yarp::sig::Image& outImgR);

/**
 * @brief horzSplit, split horizontally an image in two images of the same size.
 * @param inImg[in] image to be horizontally split.
 * @param outImgUp[out] top half of inImg.
 * @param outImgDown[out] bottom half of inImg.
 * @note The input image must have same height, double width of the output images and same pixel type.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API horzSplit(const yarp::sig::Image& inImg, yarp::sig::Image& outImgUp, yarp::sig::Image& outImgDown);

/**
 * @brief horzConcat, concatenate horizontally two images of the same size in one with double width.
 * @param inImgL[in] input left image.
 * @param inImgR[in] input right image.
 * @param outImg[out] result of the horizontal concatenation.
 * @note The input images must have same dimensions and pixel type, and the output image must have same height and
 * double width.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API horzConcat(const yarp::sig::Image& inImgL, const yarp::sig::Image& inImgR, yarp::sig::Image& outImg);

/**
 * @brief vertConcat, concatenate vertically two images of the same size in one with double height.
 * @param inImgUp[in] input top image.
 * @param inImgDown[in] input bottom image.
 * @param outImg[out] result of the horizontal concatenation.
 * @note The input images must have same dimensions and pixel type, and the output image must have same width and
 * double height.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API vertConcat(const yarp::sig::Image& inImgUp, const yarp::sig::Image& inImgDown, yarp::sig::Image& outImg);
} // namespace utils
} // namespace sig
} // namespace yarp

#endif // YARP_SIG_IMAGEUTILS_H
