/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreClient.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Value.h>
#include <yarp/os/YarpPluginSelector.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

TEST_CASE("os::PortCoreClientTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    SECTION("detachPortmonitorIn/detachPortmonitorOut on an existing port")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        CHECK(client.detachPortmonitorIn());
        CHECK(client.detachPortmonitorOut());

        p1.close();
    }

    SECTION("attachPortmonitorIn/attachPortmonitorOut with a portmonitor which cannot be configured")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        CHECK_FALSE(client.attachPortmonitorIn("(type nonexistent_type) (file nonexistent_file)"));
        CHECK_FALSE(client.attachPortmonitorOut("(type nonexistent_type) (file nonexistent_file)"));

        // the port is still able to process the following commands
        CHECK(client.detachPortmonitorIn());
        CHECK(client.detachPortmonitorOut());

        p1.close();
    }

    SECTION("attach/detach of the test_numbers1 portmonitor (adds 1 to each value)")
    {
        YARP_REQUIRE_PLUGIN("test_numbers1", "portmonitor");

        const std::string pm_options = "(type dll) (file test_numbers1)";

        Port sender;
        BufferedPort<Value> receiver;
        receiver.setStrict();
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        auto sendAndReceive = [&](int sent) {
            sender.write(Value(sent));
            Value* received = receiver.read();
            REQUIRE(received != nullptr);
            return received->asInt64();
        };

        CHECK(sendAndReceive(1) == 1);

        // receiver side
        PortCoreClient recvClient("/recv");
        REQUIRE(recvClient.attachPortmonitorIn(pm_options));
        CHECK(sendAndReceive(1) == 2);
        REQUIRE(recvClient.detachPortmonitorIn());
        CHECK(sendAndReceive(1) == 1);

        // sender side
        PortCoreClient sendClient("/send");
        REQUIRE(sendClient.attachPortmonitorOut(pm_options));
        CHECK(sendAndReceive(1) == 2);

        // both sides
        REQUIRE(recvClient.attachPortmonitorIn(pm_options));
        CHECK(sendAndReceive(1) == 3);

        REQUIRE(sendClient.detachPortmonitorOut());
        REQUIRE(recvClient.detachPortmonitorIn());
        CHECK(sendAndReceive(1) == 1);

        receiver.close();
        sender.close();
    }

    SECTION("all the methods fail on a non existing port")
    {
        PortCoreClient client("/nonexistent_port", 0.5);
        CHECK_FALSE(client.attachPortmonitorIn("(file nonexistent_file)"));
        CHECK_FALSE(client.attachPortmonitorOut("(file nonexistent_file)"));
        CHECK_FALSE(client.detachPortmonitorIn());
        CHECK_FALSE(client.detachPortmonitorOut());
    }

    NetworkBase::setLocalMode(false);
}
