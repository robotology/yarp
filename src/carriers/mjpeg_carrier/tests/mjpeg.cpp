/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <yarp/os/Network.h>
#include <yarp/sig/all.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::sig;

TEST_CASE("carriers::mjpeg", "[carriers]")
{
    YARP_REQUIRE_PLUGIN("mjpeg", "carrier");

#if defined(DISABLE_FAILING_VALGRIND_TESTS)
    YARP_SKIP_TEST("Skipping failing tests under valgrind")
#endif

    Network::setLocalMode(true);

    SECTION("test compression-decompression")
    {
        std::string inName {"/mjpeg/in"};
        std::string outName {"/mjpeg/out"};

        BufferedPort<ImageOf<PixelRgb>> in;
        BufferedPort<ImageOf<PixelRgb>> out;

        REQUIRE(in.open(inName));
        REQUIRE(out.open(outName));
        REQUIRE(Network::connect(out.getName(), in.getName(), "mjpeg"));

        size_t width {160};
        size_t height {120};
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

    Network::setLocalMode(false);
}
