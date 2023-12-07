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

void test_fakeMicrophone (size_t channels)
{
    PolyDriver dd_fake;
    PolyDriver dd_nws;
    Property p_fake;
    Property p_nws;

    p_nws.put("device", "audioRecorder_nws_yarp");
    p_nws.put("start", "");
    p_fake.put("device", "fakeMicrophone");

    char buff_channels[10];
    snprintf (buff_channels, sizeof(buff_channels), "%d", (int)channels);
    Property& psub = p_fake.addGroup("AUDIO_BASE");
    psub.put("channels", (int)(channels));

    REQUIRE(dd_fake.open(p_fake));
    REQUIRE(dd_nws.open(p_nws));
    yarp::os::SystemClock::delaySystem(0.5);

    {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
    REQUIRE(ww_nws);
    bool result_att = ww_nws->attach(&dd_fake);
    REQUIRE(result_att); }

    yarp::os::SystemClock::delaySystem(1.0);

    CHECK(dd_nws.close());
    CHECK(dd_fake.close());
}

TEST_CASE("dev::AudioRecorder_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMicrophone", "device");
    YARP_REQUIRE_PLUGIN("audioRecorder_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the audioRecorder_nws_yarp device with no devices attached")
    {
        PolyDriver dd_nws;
        Property p_nws;

        p_nws.put("device", "audioRecorder_nws_yarp");
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        yarp::os::SystemClock::delaySystem(1.0);

        CHECK(dd_nws.close());
    }

    SECTION("Test the audioRecorder_nws_yarp device with a fakeMicrophone device")
    {
        test_fakeMicrophone(1);
        test_fakeMicrophone(2);
    }

    Network::setLocalMode(false);
}
