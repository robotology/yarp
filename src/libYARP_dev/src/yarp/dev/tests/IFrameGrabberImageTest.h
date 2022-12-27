/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IFRAMEGRABBERIMAGETEST_H
#define IFRAMEGRABBERIMAGETEST_H

#include <array>
#include <yarp/dev/IFrameGrabberImage.h>
#include <catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    constexpr int default_height = 240;
    constexpr int default_width = 320;
    constexpr double default_hfov = 1.0;
    constexpr double default_vfov = 2.0;
    constexpr double default_physFocalLength = 3.0;
    constexpr double default_focalLengthX = 4.0;
    constexpr double default_focalLengthY = 5.0;
    constexpr double default_principalPointX = 6.0;
    constexpr double default_principalPointY = 7.0;
    constexpr double default_k1 = 8.0;
    constexpr double default_k2 = 9.0;
    constexpr double default_k3 = 10.0;
    constexpr double default_t1 = 11.0;
    constexpr double default_t2 = 12.0;
    const std::string default_distortionModel = "FishEye";
    constexpr std::array<double, 9> default_rectificationMatrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

    constexpr size_t crop_h = 24;
    constexpr size_t crop_w = 32;

    const yarp::sig::VectorOf<std::pair<int, int>> vertices{ {0, 0}, {crop_w - 1, crop_h - 1} };

    inline void exec_IFrameGrabberImage_test_1(IFrameGrabberImage* iframe)
    {
        // Check width()
        CHECK(iframe->width() == default_width);

        // Check height()
        CHECK(iframe->height() == default_height);

        // Check getImage()
        ImageOf<PixelRgb> img;
        iframe->getImage(img);
        CHECK(img.width() == default_width);
        CHECK(img.height() == default_height);

        // Check getImageCrop()
        ImageOf<PixelRgb> crop;
        CHECK(iframe->getImageCrop(YARP_CROP_RECT, vertices, crop));
        CHECK(crop.width() == crop_w);
        CHECK(crop.height() == crop_h);
    }
}

#endif
