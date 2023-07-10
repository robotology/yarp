/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAudioGrabberSound.h>

#include <yarp/os/Network.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/Time.h>

#include <string>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::audioRecorder_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMicrophone", "device");
    YARP_REQUIRE_PLUGIN("audioRecorder_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("audioRecorder_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the audioRecorder_nwc_yarp device with a audioRecorder_nws_yarp device")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        PolyDriver dd_nwc;
        Property p_fake;
        Property p_nws;
        Property p_nwc;

        p_nws.put("device","audioRecorder_nws_yarp");
        p_nws.put("name", "/audioRecorder_nws");
        p_fake.put("device","fakeMicrophone");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        p_nwc.put("device", "audioRecorder_nwc_yarp");
        p_nwc.put("remote", "/audioRecorder_nws");
        p_nwc.put("local", "/audioRecorder_nwc");
        REQUIRE(dd_nwc.open(p_nwc));

        IAudioGrabberSound* igrab = nullptr;
        REQUIRE(dd_nwc.view(igrab));

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK (igrab->stopRecording());
        bool isrec = false;
        CHECK (igrab->isRecording(isrec)); CHECK(isrec == false);
        CHECK (igrab->startRecording());
        CHECK (igrab->isRecording(isrec)); CHECK(isrec == true);
        CHECK (igrab->stopRecording());
        CHECK (igrab->isRecording(isrec)); CHECK(isrec == false);
        CHECK (igrab->setHWGain(0.5));
        CHECK (igrab->setSWGain(0.5));

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nwc.close());
        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
