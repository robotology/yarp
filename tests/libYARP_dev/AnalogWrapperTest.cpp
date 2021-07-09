/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::AnalogWrapperTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeAnalogSensor", "device");
    YARP_REQUIRE_PLUGIN("analogServer", "device");

    Network::setLocalMode(true);

    SECTION("Analogwrapper Test (test 1)")
    {
        // First test: Try to make it open a single port and I check its
        // existence and of the relative rpc port
        Property p;
        p.put("device","analogServer");
        p.put("name","/testAnalogWrapper");
        p.put("period",20);
        p.put("channels",10);
        p.put("subdevice","fakeAnalogSensor");

        PolyDriver dd;
        REQUIRE(dd.open(p));

        Network::sync("/testAnalogWrapper/rpc:i");
        Network::sync("/testAnalogWrapper");

        CHECK(Network::exists("/testAnalogWrapper/rpc:i")); // /testAnalogWrapper/rpc:i port created succefully
        CHECK(Network::exists("/testAnalogWrapper")); // /testAnalogWrapper port created succefully
        CHECK(dd.close()); // close reported successful
    }

    SECTION("Analogwrapper Test (test 2)")
    {
        // Second test: Try to open a device after opening the same port that it
        // will open
        BufferedPort<Bottle> portrpc;
        CHECK(portrpc.open("/testAnalogWrapper/rpc:i")); // open reported successful
        Network::sync("/testAnalogWrapper/rpc:i");

        BufferedPort<Bottle> port;
        CHECK(port.open("/testAnalogWrapper")); // open reported successful
        Network::sync("/testAnalogWrapper");

        Property p;
        p.put("device","analogServer");
        p.put("name","/testAnalogWrapper");
        p.put("period",20);
        p.put("channels",10);
        p.put("subdevice","fakeAnalogSensor");

        PolyDriver dd;
        CHECK_FALSE(dd.open(p)); // Open of AnalogWrapper2 with one port open failed as expected (address conflict)
        CHECK_FALSE(dd.isValid()); //  isValid() of AnalogWrapper2 with one port failed as expected open (address conflict)

        portrpc.close();
        port.close();
        CHECK(dd.close()); // close reported successful
    }

    SECTION("Analogwrapper Test (test 3)")
    {
        //Third test: The same of before but with multiple ports
        Property p;
        p.put("device","analogServer");
        p.put("name","/testAnalogWrapperMultip");
        p.put("period",20);
        p.put("channels",3);
        p.put("subdevice","fakeAnalogSensor");
        p.fromString("(ports (left_hand left_forearm left_arm)) (left_hand 0 0 0 0) (left_forearm 0 0 0 0) (left_arm 0 0 0 0)", false);

        PolyDriver dd;
        REQUIRE(dd.open(p)); // Open of AnalogWrapper with multiple port open reported successful

        Network::sync("/testAnalogWrapperMultip/left_hand");
        Network::sync("/testAnalogWrapperMultip/left_hand/rpc:i");
        Network::sync("/testAnalogWrapperMultip/left_forearm");
        Network::sync("/testAnalogWrapperMultip/left_forearm/rpc:i");
        Network::sync("/testAnalogWrapperMultip/left_arm");
        Network::sync("/testAnalogWrapperMultip/left_arm/rpc:i");

        CHECK(dd.isValid()); // isValid of AnalogWrapper with multiple port open reported successful
        CHECK_FALSE(Network::exists("/rpc:i")); // /rpc:i has not been opened
        CHECK_FALSE(Network::exists("/rpc:o")); // /rpc:o has not been opened

        CHECK(Network::exists("/testAnalogWrapperMultip/left_hand")); // /testAnalogWrapperMultip/left_hand port opened succesfully
        CHECK(Network::exists("/testAnalogWrapperMultip/left_hand/rpc:i")); // /testAnalogWrapperMultip/left_hand/rpc:i port opened succesfully
        CHECK(Network::exists("/testAnalogWrapperMultip/left_forearm")); // /testAnalogWrapperMultip/left_forearm port opened succesfully
        CHECK(Network::exists("/testAnalogWrapperMultip/left_forearm/rpc:i"));// /testAnalogWrapperMultip/left_forearm/rpc:i port opened succesfully
        CHECK(Network::exists("/testAnalogWrapperMultip/left_arm"));// /testAnalogWrapperMultip/left_arm port opened succesfully
        CHECK(Network::exists("/testAnalogWrapperMultip/left_arm/rpc:i"));// /testAnalogWrapperMultip/left_arm/rpc:i port opened succesfully

        CHECK(dd.close()); // close reported successful
    }


    SECTION("Analogwrapper Test (test 4)")
    {
        //Fourth test: test for address confict with multiple ports
        Property p;
        p.put("device","analogServer");
        p.put("name","/testAnalogWrapperMultip");
        p.put("period",20);
        p.put("channels",3);
        p.put("subdevice","fakeAnalogSensor");
        p.fromString("(ports (left_hand left_forearm left_arm)) (left_hand 0 0 0 0) (left_forearm 0 0 0 0) (left_arm 0 0 0 0)", false);

        BufferedPort<Bottle> port;
        port.open("/testAnalogWrapperMultip/left_hand");
        Network::sync("/testAnalogWrapperMultip/left_hand");

        BufferedPort<Bottle> portrpc;
        portrpc.open("/testAnalogWrapperMultip/left_forearm/rpc:i");
        Network::sync("/testAnalogWrapperMultip/left_forearm/rpc:i");

        PolyDriver dd;
        CHECK_FALSE(dd.open(p)); // Open of AnalogWrapper2 with multiple ports open failed as expected (address conflict)
        CHECK_FALSE(dd.isValid()); // isValid() of AnalogWrapper2 with multiple ports failed as expected open (address conflict)

        portrpc.close();
        port.close();
        CHECK(dd.close()); // close reported successful
    }

    Network::setLocalMode(false);
};
