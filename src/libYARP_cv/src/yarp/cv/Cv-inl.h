/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CV_CV_INL_H
#define YARP_CV_CV_INL_H

#include<cassert>

namespace yarp {
namespace cv {

template <typename T>
struct type_code;

template<> struct type_code<yarp::sig::PixelMono> : std::integral_constant<int, CV_8UC1> {};
template<> struct type_code<yarp::sig::PixelMono16> : std::integral_constant<int, CV_16UC1> {};
template<> struct type_code<yarp::sig::PixelMonoSigned> : std::integral_constant<int, CV_8SC1> {};
template<> struct type_code<yarp::sig::PixelFloat> : std::integral_constant<int, CV_32FC1> {};
template<> struct type_code<yarp::sig::PixelInt> : std::integral_constant<int, CV_32SC1> {};
template<> struct type_code<yarp::sig::PixelBgr> : std::integral_constant<int, CV_8UC3> {};
template<> struct type_code<yarp::sig::PixelRgb> : std::integral_constant<int, CV_8UC3> {};
template<> struct type_code<yarp::sig::PixelRgbSigned> : std::integral_constant<int, CV_8SC3> {};
template<> struct type_code<yarp::sig::PixelRgbFloat> : std::integral_constant<int, CV_32FC3> {};
template<> struct type_code<yarp::sig::PixelRgbInt> : std::integral_constant<int, CV_32SC3> {};
template<> struct type_code<yarp::sig::PixelHsv> : std::integral_constant<int, CV_8UC3> {};
template<> struct type_code<yarp::sig::PixelHsvFloat> : std::integral_constant<int, CV_32FC3> {};
template<> struct type_code<yarp::sig::PixelRgba> : std::integral_constant<int, CV_8UC4> {};
template<> struct type_code<yarp::sig::PixelBgra> : std::integral_constant<int, CV_8UC4> {};

// -1 : no conversion required.
template <typename T>
struct convert_code_to_cv;

template<> struct convert_code_to_cv<yarp::sig::PixelMono> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelMono16> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelMonoSigned> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelFloat> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelInt> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelBgr> : std::integral_constant<int, -1> {};
template<> struct convert_code_to_cv<yarp::sig::PixelRgb> : std::integral_constant<int, CV_RGB2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelRgbSigned> : std::integral_constant<int, CV_RGB2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelRgbFloat> : std::integral_constant<int, CV_RGB2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelRgbInt> : std::integral_constant<int, CV_RGB2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelHsv> : std::integral_constant<int, CV_HSV2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelHsvFloat> : std::integral_constant<int, CV_HSV2BGR> {};
template<> struct convert_code_to_cv<yarp::sig::PixelRgba> : std::integral_constant<int, CV_RGBA2BGRA> {};
template<> struct convert_code_to_cv<yarp::sig::PixelBgra> : std::integral_constant<int, -1> {};

template <typename T>
struct convert_code_from_cv;

template<> struct convert_code_from_cv<yarp::sig::PixelMono> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelMono16> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelMonoSigned> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelFloat> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelInt> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelBgr> : std::integral_constant<int, -1> {};
template<> struct convert_code_from_cv<yarp::sig::PixelRgb> : std::integral_constant<int, CV_BGR2RGB> {};
template<> struct convert_code_from_cv<yarp::sig::PixelRgbSigned> : std::integral_constant<int, CV_BGR2RGB> {};
template<> struct convert_code_from_cv<yarp::sig::PixelRgbFloat> : std::integral_constant<int, CV_BGR2RGB> {};
template<> struct convert_code_from_cv<yarp::sig::PixelRgbInt> : std::integral_constant<int, CV_BGR2RGB> {};
template<> struct convert_code_from_cv<yarp::sig::PixelHsv> : std::integral_constant<int, CV_BGR2HSV> {};
template<> struct convert_code_from_cv<yarp::sig::PixelHsvFloat> : std::integral_constant<int, CV_BGR2HSV> {};
template<> struct convert_code_from_cv<yarp::sig::PixelRgba> : std::integral_constant<int, CV_BGRA2RGBA> {};
template<> struct convert_code_from_cv<yarp::sig::PixelBgra> : std::integral_constant<int, -1> {};


} // namespace cv
} // namespace yarp


template<typename T>
::cv::Mat yarp::cv::toCvMat(yarp::sig::ImageOf<T>& yarpImage)
{
    ::cv::Mat outMat (yarpImage.height(), yarpImage.width(), yarp::cv::type_code<T>::value,
                      yarpImage.getRawImage(), yarpImage.getRowSize()); // RVO
    if (convert_code_to_cv<T>::value >= 0)
    {
        ::cv::cvtColor(outMat, outMat, convert_code_to_cv<T>::value);
    }
    return outMat;
}


template<typename T>
yarp::sig::ImageOf<T> yarp::cv::fromCvMat(::cv::Mat& cvImage)
{
    constexpr size_t align_8_bytes = 8;
    constexpr size_t align_4_bytes = 4;

    yarp::sig::ImageOf<T> outImg;
    // Checking cv::Mat::type() compatibility with the T PixelType
    assert(yarp::cv::type_code<T>::value == cvImage.type());
    if (convert_code_from_cv<T>::value >= 0)
    {
        ::cv::cvtColor(cvImage, cvImage, convert_code_from_cv<T>::value);
    }
    // Check the cv::Mat alignment
    if (cvImage.step % align_8_bytes == 0) {
        outImg.setQuantum(align_8_bytes);
    }
    else if (cvImage.step % align_4_bytes == 0) {
        outImg.setQuantum(align_4_bytes);
    }
    outImg.setExternal(cvImage.data, cvImage.cols, cvImage.rows);
    return outImg;
}

#endif // YARP_CV_CV_INL_H
