/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IRGBVISUALPARAMSTEST_H
#define IRGBVISUALPARAMSTEST_H

#include <yarp/dev/IRgbVisualParams.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    const std::vector<yarp::dev::CameraConfig> default_configurations = {
        { 128, 128, 60.0, VOCAB_PIXEL_RGB },
        { 256, 256, 30.0, VOCAB_PIXEL_BGR },
        { 512, 512, 15.0, VOCAB_PIXEL_MONO }
    };

    inline void exec_IRgbVisualParams_test_1(IRgbVisualParams* irgb)
    {
        // check the default parameters

         // checking fov
        double hfov = 0.0;
        double vfov = 0.0;
        irgb->getRgbFOV(hfov, vfov);
        CHECK(hfov == 1.0);
        CHECK(vfov == 2.0);

        // checking height
        CHECK(irgb->getRgbHeight() == 240);

        // checking width
        CHECK(irgb->getRgbWidth() == 320);

        // checking mirroring
        bool rgbMirroring;
        irgb->getRgbMirroring(rgbMirroring);
        CHECK_FALSE(rgbMirroring);

        // checking intrinsics
        yarp::os::Property intrinsics;
        irgb->getRgbIntrinsicParam(intrinsics);
        CHECK(intrinsics.find("focalLengthX").asFloat64() == 4.0); // checking focalLength X
        CHECK(intrinsics.find("focalLengthY").asFloat64() == 5.0); // checking focalLength Y
        CHECK(intrinsics.find("principalPointX").asFloat64() == 6.0); // checking principalPoint X
        CHECK(intrinsics.find("principalPointY").asFloat64() == 7.0); // checking principalPoint Y
        CHECK(intrinsics.find("k1").asFloat64() == 8.0); // checking k1
        CHECK(intrinsics.find("k2").asFloat64() == 9.0); // checking k2
        CHECK(intrinsics.find("k3").asFloat64() == 10.0); // checking k3
        CHECK(intrinsics.find("t1").asFloat64() == 11.0); // checking t1
        CHECK(intrinsics.find("t2").asFloat64() == 12.0);  // checking t2
        CHECK(intrinsics.find("distortionModel").asString() == "FishEye"); // checking distorionModel

        // checking the rectificationMatrix
        yarp::os::Bottle* retM = nullptr;
        retM = intrinsics.find("rectificationMatrix").asList();
        double data[9] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
        Vector v(9, data);
        Vector v2;
        Portable::copyPortable(*retM, v2);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                CHECK(retM->get(i * 3 + j).asFloat64() == v(i * 3 + j));
                CHECK(v2(i * 3 + j) == v(i * 3 + j));
            }
        }

        // checking getRgbResolution
        int height, width;
        irgb->getRgbResolution(width, height);
        CHECK(width == 320);
        CHECK(height == 240);

        // checking configurations size
        VectorOf<CameraConfig> configurations;
        CHECK(irgb->getRgbSupportedConfigurations(configurations));
        CHECK(configurations.size() == 3);

        // checking first supported configuration
        CHECK(configurations[0].height == 128);
        CHECK(configurations[0].width == 128);
        CHECK(configurations[0].framerate == 60.0);
        CHECK(configurations[0].pixelCoding == VOCAB_PIXEL_RGB);

        // checking second supported configuration
        CHECK(configurations[1].height == 256);
        CHECK(configurations[1].width == 256);
        CHECK(configurations[1].framerate == 30.0);
        CHECK(configurations[1].pixelCoding == VOCAB_PIXEL_BGR);

        // checking third supported configuration
        CHECK(configurations[2].height == 512);
        CHECK(configurations[2].width == 512);
        CHECK(configurations[2].framerate == 15.0);
        CHECK(configurations[2].pixelCoding == VOCAB_PIXEL_MONO);
    }
}

#endif
