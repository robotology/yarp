/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/sig/Image.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::stats_monitorTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("image_compression_ffmpeg", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    auto tc = GENERATE(
       "fast_tcp",
       "fast_tcp+send.portmonitor+file.image_compression_ffmpeg+type.dll+recv.portmonitor+file.image_compression_ffmpeg+type.dll",
       "fast_tcp+send.portmonitor+file.image_compression_ffmpeg+codec.mpeg2video+type.dll+recv.portmonitor+file.image_compression_ffmpeg+codec.mpeg2video+type.dll",
 //      "fast_tcp+send.portmonitor+file.image_compression_ffmpeg+codec.h264+type.dll+recv.portmonitor+file.image_compression_ffmpeg+codec.h264+type.dll",
 //      "fast_tcp+send.portmonitor+file.image_compression_ffmpeg+codec.h265+type.dll+recv.portmonitor+file.image_compression_ffmpeg+codec.h265+type.dll",
       "fast_tcp+send.portmonitor+file.image_compression_ffmpeg+custom_enc.mjpeg+type.dll+recv.portmonitor+file.image_compression_ffmpeg+custom_dec.mjpeg+type.dll"
    );

    SECTION("Test that normally the portmonitor is not used")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc);
        REQUIRE(b);

        yarp::sig::ImageOf<yarp::sig::PixelRgb> imgsend;
        imgsend.resize(640, 480);
        sender.write(imgsend);

        yarp::os::Time::delay(0.5);

        yarp::sig::ImageOf<yarp::sig::PixelRgb> imgrecv;

        receiver.read(imgrecv);
        CHECK(imgrecv.width()== 640);
        CHECK(imgrecv.height()== 480);

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
