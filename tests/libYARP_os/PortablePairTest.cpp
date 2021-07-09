/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/PortablePair.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;

TEST_CASE("os::PortablePairTest", "[yarp::os]")
{

    Network::setLocalMode(true);

    SECTION("Test standard PortablePair")
    {
        PortablePair<Bottle, Bottle> pp;
        pp.head.fromString("1 2 3");
        pp.body.fromString("yes no");
        BufferedConnectionWriter writer;
        pp.write(writer);
        std::string s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(), s.length());
        CHECK(bot.size() == (size_t) 2); // it is a pair
        CHECK(bot.get(0).asList()->size() == (size_t) 3); // head len is right
        CHECK(bot.get(1).asList()->size() == (size_t) 2); // body len is right
    }

    SECTION("testing PortablePair transmission")
    {
        PortablePair<Bottle, Bottle> pp;
        pp.head.fromString("1 2 3");
        pp.body.fromString("yes no");

        PortReaderBuffer< PortablePair<Bottle, Bottle> > buf;

        Port input, output;
        input.open("/in");
        output.open("/out");
        buf.setStrict();
        buf.attach(input);
        Network::connect("/out", "/in");

        INFO("Writing...");
        output.write(pp);
        INFO("Reading...");
        PortablePair<Bottle, Bottle> *result = buf.read();

        REQUIRE(result!=nullptr); // got something check
        CHECK(result->head.size() == (size_t) 3); // head len is right
        CHECK(result->body.size() == (size_t) 2); // body len is right

        output.close();
        input.close();
    }

    Network::setLocalMode(false);
}
