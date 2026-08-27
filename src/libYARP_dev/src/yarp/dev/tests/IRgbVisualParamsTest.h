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
        REQUIRE(irgb);

        // checking getRgbFOV
        {
            double hfov = 0.0;
            double vfov = 0.0;
            CHECK(irgb->getRgbFOV(hfov, vfov));
            CHECK(hfov == 1.0);
            CHECK(vfov == 2.0);
        }

        // checking height
        {
            CHECK(irgb->getRgbHeight() == 240);
        }

        // checking width
        {
            CHECK(irgb->getRgbWidth() == 320);
        }

        // checking intrinsics
        {
            yarp::sig::IntrinsicParams intrinsics;
            CHECK(irgb->getRgbIntrinsicParam(intrinsics));
            CHECK(intrinsics.focalLengthX == 4.0);         // checking focalLength X
            CHECK(intrinsics.focalLengthY == 5.0);         // checking focalLength Y
            CHECK(intrinsics.principalPointX == 6.0);      // checking principalPoint X
            CHECK(intrinsics.principalPointY == 7.0);      // checking principalPoint Y
            CHECK(intrinsics.distortionModel.k1 == 8.0);                   // checking k1
            CHECK(intrinsics.distortionModel.k2 == 9.0);                   // checking k2
            CHECK(intrinsics.distortionModel.k3 == 10.0);                  // checking k3
            CHECK(intrinsics.distortionModel.t1 == 11.0);                  // checking t1
            CHECK(intrinsics.distortionModel.t2 == 12.0);                  // checking t2
            CHECK(intrinsics.distortionModel.type == CameraDistortionType::YARP_FISH_EYE); // checking distorionModel

            double data[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
            REQUIRE(intrinsics.rectificationMatrix3X3.size() == 9);
            for (int i = 0; i < 9; i++)
            {
                CHECK(intrinsics.rectificationMatrix3X3[i] == data[i]);
            }
        }

        // checking getRgbMirroring
        {
            bool rgbMirroring = true;
            CHECK(irgb->getRgbMirroring(rgbMirroring));
            CHECK(!rgbMirroring);
        }

        // checking getRgbResolution
        {
            int height = 0;
            int width = 0;
            CHECK(irgb->getRgbResolution(width, height));
            CHECK(width == 320);
            CHECK(height == 240);
        }

        // checking configurations size
        {
            std::vector<CameraConfig> configurations;
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

        // checking setRgbFOV
        {
            double hfov = 0.0;
            double vfov = 0.0;
            CHECK(irgb->setRgbFOV(3.0, 4.0));
            CHECK(irgb->getRgbFOV(hfov, vfov));
            CHECK(hfov == 3.0);
            CHECK(vfov == 4.0);
        }

        // checking setRgbMirroring
        {
            bool rgbMirroring=true;
            CHECK(irgb->setRgbMirroring(rgbMirroring));
            CHECK(rgbMirroring);
        }

        // checking setRgbResolution
        {
            int height = 0;
            int width = 0;
            CHECK(irgb->setRgbResolution(101, 102));
            CHECK(irgb->getRgbResolution(width, height));
            CHECK(width == 101);
            CHECK(height == 102);
        }
    }
}

#endif
