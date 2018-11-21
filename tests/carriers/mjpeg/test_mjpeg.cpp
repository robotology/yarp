/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <MjpegDecompression.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/all.h>
#include <yarp/os/Network.h>
#include <yarp/sig/all.h>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::mjpeg;

TEST_CASE("carriers::MjpegTest", "[carriers::mjpeg]") {
    Network yarp;
    yarp.setLocalMode(true);
    SECTION("test compression-decompression") {

        std::string inName {"/mjpeg/in"};
        std::string outName {"/mjpeg/out"};

        BufferedPort<ImageOf<PixelRgb>> in;
        BufferedPort<ImageOf<PixelRgb>> out;

        REQUIRE(in.open(inName));
        REQUIRE(out.open(outName));
        REQUIRE(Network::connect(out.getName(), in.getName(), "mjpeg"));

        MjpegDecompression decompression; // just for compile

        size_t width {320};
        size_t height {240};
        ImageOf<PixelRgb>& outImg = out.prepare();
        outImg.resize(width, height);

        out.write();
        yarp::os::Time::delay(0.4);

        ImageOf<PixelRgb>* inImg = in.read();
        REQUIRE(inImg != nullptr);
        CHECK(inImg->width() == width);
        CHECK(inImg->height() == height);

        in.interrupt();
        in.close();
        out.interrupt();
        out.close();
    }
    yarp.setLocalMode(false);
}
