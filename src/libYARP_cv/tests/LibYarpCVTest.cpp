/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/cv/Cv.h>
#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::cv;
using namespace yarp::os;
using namespace yarp::sig;

TEST_CASE("dev::LibYarpCvTest", "[yarp::dev]")
{
    Network::setLocalMode(true);

    SECTION("Test yarp::sig::ImageOf<PixelRgb> LibYarpCvTest")
    {
        yarp::sig::ImageOf<PixelRgb> img_in_rgb;
        img_in_rgb.resize(10,10);
        img_in_rgb.zero();
        PixelRgb pixa;
        pixa.r = 30;
        pixa.g = 30;
        pixa.b = 30;
        yarp::sig::draw::addRectangle(img_in_rgb, pixa, 0,0,9,9);
        cv::Mat cvImage = yarp::cv::toCvMat(img_in_rgb);

        yarp::sig::ImageOf<PixelRgb> img_out_rgb;
        img_out_rgb.resize(10, 10);
        img_out_rgb.zero();
        img_out_rgb = yarp::cv::fromCvMat<yarp::sig::PixelRgb>(cvImage);
        bool b = true;
        for (size_t x=0; x<10; x++)
            for (size_t y = 0; y < 10; y++)
                   {PixelRgb pixb = img_out_rgb.pixel(x ,y);
                    b &= (pixb.r == 30);
                    b &= (pixb.g == 30);
                    b &= (pixb.b == 30);}
        CHECK(b);
    }

    SECTION("Test yarp::sig::ImageOf<PixelFloat> LibYarpCvTest")
    {
        yarp::sig::ImageOf<PixelFloat> img_in_float;
        img_in_float.resize(10, 10);
        img_in_float.zero();
        PixelFloat pixa;
        pixa = 30.3;
        yarp::sig::draw::addRectangle(img_in_float, pixa, 0, 0, 9, 9);
        cv::Mat cvImage = yarp::cv::toCvMat(img_in_float);

        yarp::sig::ImageOf<PixelFloat> img_out_float;
        img_out_float.resize(10, 10);
        img_out_float.zero();
        img_out_float = yarp::cv::fromCvMat<yarp::sig::PixelFloat>(cvImage);

        bool b = true;
        for (size_t x = 0; x < 10; x++)
            for (size_t y = 0; y < 10; y++)
            {
                PixelFloat pixb = img_out_float.pixel(x, y);
                b &= (fabs(pixb-30.3) <= 0.0001);
            }
        CHECK(b);
    }

    SECTION("Test FlexImage (RGB) LibYarpCvTest")
    {
        yarp::sig::FlexImage img_in;
        img_in.setPixelCode(VOCAB_PIXEL_RGB);
        img_in.resize(10, 10);
        img_in.zero();
        PixelRgb* pointerin = (PixelRgb*)img_in.getRawImage();
        for (size_t i = 0; i < 100; i++, pointerin++)
        {
            (*pointerin).r = 1;
            (*pointerin).g = 2;
            (*pointerin).b = 3;
        }
        cv::Mat cvImage = yarp::cv::toCvMat(img_in);

        yarp::sig::ImageOf<PixelRgb> img_out_rgb;
        img_out_rgb.resize(10, 10);
        img_out_rgb.zero();
        img_out_rgb = yarp::cv::fromCvMat<yarp::sig::PixelRgb>(cvImage);

        bool b = true;
        PixelRgb* pointerout = (PixelRgb*)img_out_rgb.getRawImage();
        for (size_t i = 0; i < 100; i++, pointerout++)
        {
            b &= ((*pointerout).r == 1 &&
                  (*pointerout).g == 2 &&
                  (*pointerout).b == 3);
        }
        CHECK(b);
    }

    SECTION("Test FlexImage (float) LibYarpCvTest")
    {
        yarp::sig::FlexImage img_in;
        img_in.setPixelCode(VOCAB_PIXEL_MONO_FLOAT);
        img_in.resize(10, 10);
        img_in.zero();
        PixelFloat* pointerin= (PixelFloat*)img_in.getRawImage();
        for (size_t i = 0; i < 100; i++, pointerin++)
        {
            *pointerin = 30.3;
        }
        cv::Mat cvImage = yarp::cv::toCvMat(img_in);

        yarp::sig::ImageOf<PixelFloat> img_out_float;
        img_out_float.resize(10, 10);
        img_out_float.zero();
        img_out_float = yarp::cv::fromCvMat<yarp::sig::PixelFloat>(cvImage);

        bool b = true;
        float* pointerout = (float*)img_out_float.getRawImage();
        for (size_t i = 0; i < 100; i++, pointerout++)
        {
            b &= (fabs(*pointerout - 30.3) <= 0.0001);
        }
        CHECK(b);
    }

    /* This test will intentionally fail because toCvMat is not yet implemented
    for VOCAB_PIXEL_HSV type. It was put here as a reminder.
    SECTION("Test FlexImage (HSV) LibYarpCvTest")
    {
        yarp::sig::FlexImage img_in;
        img_in.setPixelCode(VOCAB_PIXEL_HSV);
        img_in.resize(10, 10);
        img_in.zero();
        cv::Mat cvImage = yarp::cv::toCvMat(img_in);
    }
    */

    Network::setLocalMode(false);
}
