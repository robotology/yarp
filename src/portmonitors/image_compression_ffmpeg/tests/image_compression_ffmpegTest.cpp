/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::image_compression_ffmpegTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("image_compression_ffmpeg", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCarrier
    {
        std::string carrier;
        bool mandatory;
    };

    auto tc = GENERATE(
       TestCarrier("fast_tcp", true),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+type.dll+recv.portmonitor+file.image_compression_ffmpeg+type.dll",true),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+encoder.mpeg2video+qmin.3+type.dll+recv.portmonitor+file.image_compression_ffmpeg+decoder.mpeg2video+qmin.3+type.dll",true),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+encoder.h264+type.dll+recv.portmonitor+file.image_compression_ffmpeg+decoder.h264+type.dll",false),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+encoder.h265+type.dll+recv.portmonitor+file.image_compression_ffmpeg+decoder.h265+type.dll",false),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+encoder.mjpeg+pixel_format.0+color_range.jpeg+type.dll+recv.portmonitor+file.image_compression_ffmpeg+decoder.mjpeg+pixel_format.0+color_range.jpeg+type.dll",false),
       TestCarrier("fast_tcp+send.portmonitor+file.image_compression_ffmpeg+encoder.mjpeg+pixel_format.12+strict.-2+type.dll+recv.portmonitor+file.image_compression_ffmpeg+decoder.mjpeg+pixel_format.12+strict.-2+type.dll",false)
    );

    SECTION("Test that normally the portmonitor is not used")
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> sender;
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        yarp::sig::ImageOf<yarp::sig::PixelRgb>& imgsend = sender.prepare();
        imgsend.resize(640, 480);
        sender.write();

        // Poll for up to 5 seconds with a non-blocking read
        constexpr double maxWaitSeconds = 5.0;
        constexpr double pollIntervalSeconds = 0.1;
        yarp::sig::ImageOf<yarp::sig::PixelRgb>* imgrecv = nullptr;
        double elapsed = 0.0;
        while (elapsed < maxWaitSeconds)
        {
            imgrecv = receiver.read(/*shouldWait=*/false);
            if (imgrecv != nullptr)
            {
                break;
            }
            yarp::os::Time::delay(pollIntervalSeconds);
            elapsed += pollIntervalSeconds;
        }

        if (imgrecv != nullptr)
        {
            CHECK(imgrecv->width() == 640);
            CHECK(imgrecv->height() == 480);
        }
        else
        {
            if (tc.mandatory)
            {
                REQUIRE(imgrecv);
            }
            else
            {
                std::string ss = "Non mandatory test failed with carrier: " + tc.carrier;
                WARN(ss);
            }
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
