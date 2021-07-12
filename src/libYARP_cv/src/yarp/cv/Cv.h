/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CV_CV_H
#define YARP_CV_CV_H

#include <yarp/sig/Image.h>

#include <type_traits>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

namespace yarp {
namespace cv {

/**
 * Convert a yarp::sig::ImageOf to a cv::Mat object
 * @param[in] yarpImage yarp::sig::ImageOf input.
 * Since the argument is passed as right-reference, the integrity
 * of the data of the input yarp image is not guaranteed.
 * @note The output cv::Mat has the default OpenCV color representation:
 * - 1 channel: GRAY
 * - 3 channels: BGR
 * - 4 channel: BGRA
 * Moreover pay attention on the lifetime of the memory of the input image,
 * since no copy is involved.
 * @return the resulting cv::Mat.
 */
template<typename T>
::cv::Mat toCvMat(yarp::sig::ImageOf<T>& yarpImage);

/**
 * Convert a cv::Mat to a yarp::sig::ImageOf object.
 * @param[in] cvImage cv::Mat input.
 * Since the argument is passed as right-reference, the integrity
 * of the data of the input cv::Mat is not guaranteed.
 * @note The input cv::Mat's type(e.g. CV_8UC1) must be compatible
 * with the output yarp image pixel type requested.
 * E.g.:
 * - CV_8UC3 -> ImageOf<PixelRgb>/<PixelBgr>/<PixelHsv> OK
 * - CV_8UC1 -> ImageOf<PixelMono> OK
 * - CV_8UC3 -> ImageOf<PixelMono> NOT OK
 * - CV_8UC1 -> ImageOf<PixelRgb>/<PixelBgr>/<PixelHsv> NOT OK
 * Please pay attention to the number of channels and pixel depth.
 * Moreover pay attention on the lifetime of the memory of the input image,
 * since no copy is involved.
 * @return the resulting yarp::sig::ImageOf.
 */
template<typename T>
yarp::sig::ImageOf<T> fromCvMat(::cv::Mat& cvImage);

} // namespace cv
} // namespace yarp

#include "Cv-inl.h"

#endif // YARP_CV_CV_H
