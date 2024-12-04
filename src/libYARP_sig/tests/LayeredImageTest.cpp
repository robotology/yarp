/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/sig/LayeredImage.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/PeriodicThread.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("sig::LayeredImageTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    SECTION("test serialization of layered images composed by a background + 2 layers")
    {
        FlexImage imageback;
        imageback.setPixelCode(VOCAB_PIXEL_RGB);
        imageback.resize(4, 8);

        FlexImage lay0;
        lay0.setPixelCode(VOCAB_PIXEL_RGB);
        lay0.resize(4, 8);

        FlexImage lay1;
        lay1.setPixelCode(VOCAB_PIXEL_RGB);
        lay1.resize(4, 8);

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = imageback;
        multiLayerImageIn.layers.push_back(lay0);
        multiLayerImageIn.layers.push_back(lay1);

        LayeredImage multiLayerImageOut;

        yarp::os::Bottle output_bot;
        bool b1 = Property::copyPortable(multiLayerImageIn, output_bot);
        CHECK(b1);

        yarp::os::Bottle input_bot = output_bot;
            size_t i1 = output_bot.size();
            size_t i2 = input_bot.size();
            std::string s1 = output_bot.toString();
            std::string s2 = input_bot.toString();
            CHECK((int)i1 == 23);
            CHECK((int)i2 == 23);
            std::cout << "s1: " << s1 << std::endl;
            std::cout << "s2: " << s2 << std::endl;
        bool b2 = Property::copyPortable(input_bot, multiLayerImageOut);
        CHECK(b2);

        bool b3 = (multiLayerImageIn == multiLayerImageOut);
        CHECK(b3);
    }

    SECTION("test serialization of layered images composed by a background image only")
    {
        FlexImage imageback;
        imageback.setPixelCode(VOCAB_PIXEL_RGB);
        imageback.resize(16, 8);

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = imageback;

        LayeredImage multiLayerImageOut;

        yarp::os::Bottle output_bot;
        bool b1 = Property::copyPortable(multiLayerImageIn, output_bot);
        CHECK(b1);

        yarp::os::Bottle input_bot = output_bot;
            size_t i1 = output_bot.size();
            size_t i2 = input_bot.size();
            std::string s1 = output_bot.toString();
            std::string s2 = input_bot.toString();
            CHECK((int)i1 == 3);
            CHECK((int)i2 == 3);
        bool b2 = Property::copyPortable(input_bot, multiLayerImageOut);
        CHECK(b2);

        bool b3 = (multiLayerImageIn == multiLayerImageOut);
        CHECK(b3);
    }

    SECTION("test alpha layered image")
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> imageback;
        imageback.resize(16, 8);
        for (size_t iy = 0; iy < imageback.height(); iy++)
            for (size_t ix = 0; ix < imageback.width(); ix++) {
                imageback.pixel(ix, iy).r = 10;
                imageback.pixel(ix, iy).g = 10;
                imageback.pixel(ix, iy).b = 10;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay0;
        lay0.resize(16, 8);
        for (size_t iy = 0; iy < lay0.height(); iy++)
            for (size_t ix = 0; ix < lay0.width(); ix++) {
                lay0.pixel(ix, iy).r = 8;
                lay0.pixel(ix, iy).g = 10;
                lay0.pixel(ix, iy).b = 12;
            }

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = *(reinterpret_cast<yarp::sig::FlexImage*>(&imageback));
        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay0)));
        multiLayerImageIn.layers[0].alpha.value = 0.5;

        FlexImage flat_img = multiLayerImageIn;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> flat_rgb_img = *(reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(&flat_img));
        for (size_t iy = 0; iy < flat_rgb_img.height() / 2; iy++)
            for (size_t ix = 0; ix < flat_rgb_img.width() / 2; ix++) {
                CHECK(flat_rgb_img.pixel(ix, iy).r == 9);
                CHECK(flat_rgb_img.pixel(ix, iy).g == 10);
                CHECK(flat_rgb_img.pixel(ix, iy).b == 11);
            }
    }

    SECTION("test offset layered image")
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> imageback;
        imageback.resize(4, 4);
        for (size_t iy = 0; iy < imageback.height(); iy++)
            for (size_t ix = 0; ix < imageback.width(); ix++) {
                imageback.pixel(ix, iy).r = 10;
                imageback.pixel(ix, iy).g = 10;
                imageback.pixel(ix, iy).b = 10;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay0;
        lay0.resize(2, 2);
        for (size_t iy = 0; iy < lay0.height(); iy++)
            for (size_t ix = 0; ix < lay0.width(); ix++) {
                lay0.pixel(ix, iy).r = 20;
                lay0.pixel(ix, iy).g = 20;
                lay0.pixel(ix, iy).b = 20;
            }

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = *(reinterpret_cast<yarp::sig::FlexImage*>(&imageback));
        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay0)));
        multiLayerImageIn.layers[0].offset_x = 2;
        multiLayerImageIn.layers[0].offset_y = 1;

        FlexImage flat_img = multiLayerImageIn;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> flat_rgb_img = *(reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(&flat_img));
        CHECK(flat_rgb_img.pixel(0, 0).r == 10);
        CHECK(flat_rgb_img.pixel(1, 0).r == 10);
        CHECK(flat_rgb_img.pixel(2, 0).r == 10);
        CHECK(flat_rgb_img.pixel(3, 0).r == 10);
        CHECK(flat_rgb_img.pixel(0, 1).r == 10);
        CHECK(flat_rgb_img.pixel(1, 1).r == 10);
        CHECK(flat_rgb_img.pixel(2, 1).r == 20);
        CHECK(flat_rgb_img.pixel(3, 1).r == 20);
        CHECK(flat_rgb_img.pixel(0, 2).r == 10);
        CHECK(flat_rgb_img.pixel(1, 2).r == 10);
        CHECK(flat_rgb_img.pixel(2, 2).r == 20);
        CHECK(flat_rgb_img.pixel(3, 2).r == 20);
        CHECK(flat_rgb_img.pixel(0, 3).r == 10);
        CHECK(flat_rgb_img.pixel(1, 3).r == 10);
        CHECK(flat_rgb_img.pixel(2, 3).r == 10);
        CHECK(flat_rgb_img.pixel(3, 3).r == 10);
    }

    SECTION("test colorkey layered image")
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> imageback;
        imageback.resize(4, 4);
        for (size_t iy = 0; iy < imageback.height(); iy++)
            for (size_t ix = 0; ix < imageback.width(); ix++) {
                imageback.pixel(ix, iy).r = 10;
                imageback.pixel(ix, iy).g = 10;
                imageback.pixel(ix, iy).b = 10;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay0;
        lay0.resize(4, 4);
        lay0.zero();
        imageback.pixel(0, 0).r = 20;
        imageback.pixel(1, 0).r = 20;
        imageback.pixel(2, 0).r = 20;
        imageback.pixel(3, 0).r = 20;
        imageback.pixel(0, 1).r = 20;
        imageback.pixel(1, 1).r = 50;
        imageback.pixel(2, 1).r = 50;
        imageback.pixel(3, 1).r = 20;
        imageback.pixel(0, 2).r = 20;
        imageback.pixel(1, 2).r = 50;
        imageback.pixel(2, 2).r = 50;
        imageback.pixel(3, 2).r = 20;
        imageback.pixel(0, 3).r = 20;
        imageback.pixel(1, 3).r = 50;
        imageback.pixel(2, 3).r = 20;
        imageback.pixel(3, 3).r = 20;

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = *(reinterpret_cast<yarp::sig::FlexImage*>(&imageback));
        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay0)));

        FlexImage flat_img = multiLayerImageIn;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> flat_rgb_img = *(reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(&flat_img));

        CHECK(flat_rgb_img.pixel(0, 0).r == 10);
        CHECK(flat_rgb_img.pixel(1, 0).r == 10);
        CHECK(flat_rgb_img.pixel(2, 0).r == 10);
        CHECK(flat_rgb_img.pixel(3, 0).r == 10);
        CHECK(flat_rgb_img.pixel(0, 1).r == 10);
        CHECK(flat_rgb_img.pixel(1, 1).r == 50);
        CHECK(flat_rgb_img.pixel(2, 1).r == 50);
        CHECK(flat_rgb_img.pixel(3, 1).r == 10);
        CHECK(flat_rgb_img.pixel(0, 2).r == 10);
        CHECK(flat_rgb_img.pixel(1, 2).r == 50);
        CHECK(flat_rgb_img.pixel(2, 2).r == 50);
        CHECK(flat_rgb_img.pixel(3, 2).r == 10);
        CHECK(flat_rgb_img.pixel(0, 3).r == 10);
        CHECK(flat_rgb_img.pixel(1, 3).r == 50);
        CHECK(flat_rgb_img.pixel(2, 3).r == 10);
        CHECK(flat_rgb_img.pixel(3, 3).r == 10);
    }

    SECTION("test flattening multi layered image")
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> imageback;
        imageback.resize(8, 8);
        for (size_t iy = 0; iy < imageback.height(); iy++)
            for (size_t ix = 0; ix < imageback.width(); ix++)
            {
                imageback.pixel(ix, iy).r = 10;
                imageback.pixel(ix, iy).g = 10;
                imageback.pixel(ix, iy).b = 10;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay0;
        lay0.resize(8, 8);
        for (size_t iy = 0; iy < lay0.height(); iy++)
            for (size_t ix = 0; ix < lay0.width(); ix++) {
                lay0.pixel(ix, iy).r = 100;
                lay0.pixel(ix, iy).g = 100;
                lay0.pixel(ix, iy).b = 100;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay1;
        lay1.resize(8, 8);
        for (size_t iy = 0; iy < lay1.height(); iy++)
            for (size_t ix = 0; ix < lay1.width(); ix++) {
                lay1.pixel(ix, iy).r = 101;
                lay1.pixel(ix, iy).g = 101;
                lay1.pixel(ix, iy).b = 101;
            }

        yarp::sig::ImageOf<yarp::sig::PixelRgb> lay2;
        lay2.resize(8, 8);
        for (size_t iy = 0; iy < lay2.height(); iy++)
            for (size_t ix = 0; ix < lay2.width(); ix++) {
                lay2.pixel(ix, iy).r = 102;
                lay2.pixel(ix, iy).g = 102;
                lay2.pixel(ix, iy).b = 102;
            }

        LayeredImage multiLayerImageIn;
        multiLayerImageIn.background = *(reinterpret_cast<yarp::sig::FlexImage*>(&imageback));
        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay0)));
        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay1)));
        multiLayerImageIn.layers[1].enable = false;

        FlexImage flat_img = multiLayerImageIn;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> flat_rgb_img = *(reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(&flat_img));
        for (size_t iy = 0; iy < flat_img.height() ; iy++)
            for (size_t ix = 0; ix < flat_img.width() ; ix++)
            {
                CHECK(flat_rgb_img.pixel(ix, iy).r == 100);
            }

        multiLayerImageIn.layers.push_back(*(reinterpret_cast<yarp::sig::FlexImage*>(&lay2)));
        FlexImage flat_img2 = multiLayerImageIn;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> flat_rgb_img2 = *(reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(&flat_img2));
        for (size_t iy = 0; iy < flat_img2.height() ; iy++)
            for (size_t ix = 0; ix < flat_img2.width() ; ix++) {
                CHECK(flat_rgb_img2.pixel(ix, iy).r == 102);
            }
    }

    NetworkBase::setLocalMode(false);
}
