/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/Protocol.h>

#include <yarp/os/impl/FakeTwoWayStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <cstdio>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

std::string simplify(std::string x) {
    std::string result("");
    for (unsigned int i=0; i<x.length(); i++) {
        char ch = x[i];
        if (ch == '\n') {
            result += "\\n";
        } else if (ch == '\r') {
            result += "\\r";
        } else if (ch == '\0') {
            result += "\\0";
        } else {
            result += ch;
        }
    }
    return result;
}

void show(FakeTwoWayStream *fake1, FakeTwoWayStream *fake2) {
    printf("fake1  //  in: [%s]  //  out: [%s]\n",
                    simplify(fake1->getInputText()).c_str(),
                    simplify(fake1->getOutputText()).c_str());
    printf("fake2  //  in: [%s]  //  out: [%s]\n\n",
                    simplify(fake2->getInputText()).c_str(),
                    simplify(fake2->getOutputText()).c_str());
}

TEST_CASE("os::impl::ProtocolTest", "[yarp::os][yarp::os::impl]")
{
    SECTION("trying to send a bottle across a fake stream")
    {
        // set up a fake sender/receiver pair
        FakeTwoWayStream *fake1 = new FakeTwoWayStream();
        FakeTwoWayStream *fake2 = new FakeTwoWayStream();
        fake1->setTarget(fake2->getStringInputStream());
        fake2->setTarget(fake1->getStringInputStream());

        // hand streams over to protocol managers
        Protocol p1(fake1);
        Protocol p2(fake2);

        p1.open(Route("/out", "/in", "text"));

        CHECK(fake1->getOutputText() == "CONNECT /out\r\n");    // "text carrier header");

        p2.open("/in");

        CHECK(fake2->getOutputText() == "Welcome /out\r\n");    // "text carrier response");

        BufferedConnectionWriter writer;
        writer.appendLine("d");
        writer.appendLine("0 \"Hello\"");
        p1.write(writer);

        const char *expect = "CONNECT /out\r\nd\r\n0 \"Hello\"\r\n";
        CHECK(fake1->getOutputText() == expect); // added a bottle

        ConnectionReader& reader = p2.beginRead();
        std::string str1 = reader.expectText().c_str();
        std::string str2 = reader.expectText().c_str();
        p2.endRead();

        CHECK(str1 == std::string("d")); // data tag
        const char *expect2 = "0 \"Hello\"";
        CHECK(str2 == std::string(expect2)); // bottle representation
    }
};
