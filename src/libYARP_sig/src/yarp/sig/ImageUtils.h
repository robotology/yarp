/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGEUTILS_H
#define YARP_SIG_IMAGEUTILS_H

#include <utility> // std::pair
#include <yarp/sig/Image.h>

namespace yarp::sig::utils {

/**
 * @brief Split vertically an image in two images of the same size.
 * @param[in] inImg image to be vertically split.
 * @param[out] outImgL left half of inImg.
 * @param[out] outImgR right half of inImg.
 * @note The input image must have same height, double width of the output images and same pixel type.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API vertSplit(const yarp::sig::Image& inImg, yarp::sig::Image& outImgL, yarp::sig::Image& outImgR);

/**
 * @brief Split horizontally an image in two images of the same size.
 * @param[in] inImg image to be horizontally split.
 * @param[out] outImgUp top half of inImg.
 * @param[out] outImgDown bottom half of inImg.
 * @note The input image must have same height, double width of the output images and same pixel type.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API horzSplit(const yarp::sig::Image& inImg, yarp::sig::Image& outImgUp, yarp::sig::Image& outImgDown);

/**
 * @brief Concatenate horizontally two images of the same size in one with double width.
 * @param[in] inImgL input left image.
 * @param[in] inImgR input right image.
 * @param[out] outImg result of the horizontal concatenation.
 * @note The input images must have same dimensions and pixel type, and the output image must have same height and
 * double width.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API horzConcat(const yarp::sig::Image& inImgL, const yarp::sig::Image& inImgR, yarp::sig::Image& outImg);

/**
 * @brief Concatenate vertically two images of the same size in one with double height.
 * @param[in] inImgUp input top image.
 * @param[in] inImgDown input bottom image.
 * @param[out] outImg result of the horizontal concatenation.
 * @note The input images must have same dimensions and pixel type, and the output image must have same width and
 * double height.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API vertConcat(const yarp::sig::Image& inImgUp, const yarp::sig::Image& inImgDown, yarp::sig::Image& outImg);

/**
 * @brief Crop a rectangle area out of an image given two opposite vertices.
 * @param[in] inImg input image.
 * @param[in] vertex1 first vertex of the crop rectangle area.
 * @param[in] vertex2 second vertex of the crop rectangle area.
 * @param[out] outImg result of cropping the input image.
 * @note Input and output images must have same pixel type and the crop area must lay within the input image. Vertices
 * needn't be passed in the usual top-left, bottom-right order. The output image is resized to match the crop dimensions.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API cropRect(const yarp::sig::Image& inImg,
                           const std::pair<unsigned int, unsigned int>& vertex1,
                           const std::pair<unsigned int, unsigned int>& vertex2,
                           yarp::sig::Image& outImg);

/**
 * @brief applies an image on the top over another image. Currently it is implemented only for RGB Images
 * @param[in/out] OutImg the output image. It must be a valid image on the top of which data will be summed. It may contain a background or it can be zero.
 * @param[in] InImg the layer to be applied
 * @param[in] colorkey colorkey for the InImg image. If a pixel is == colorkey, then it will be made transparent and the background will be visible.
 * @param[in] alpha to be applied to InImg.
 * @param[in] off_x horizontal offset applied to InImg. Excess will be cropped.
 * @param[in] off_y vertical offset applied to InImg. Excess will be cropped.
 * @note The two images must have the same pixelCode.
 * @return true on success, false if image cannot be summed because of incompatible format.
 */
bool YARP_sig_API sum(yarp::sig::Image& OutImg,
                      const yarp::sig::Image& InImg,
                      bool colorkey_enable,
                      int colorkey,
                      bool alpha_enable,
                      float alpha,
                      size_t off_x,
                      size_t off_y);

} // namespace yarp::sig::utils


#endif // YARP_SIG_IMAGEUTILS_H
