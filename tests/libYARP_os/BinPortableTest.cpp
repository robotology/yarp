/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BinPortable.h>

#include <yarp/os/DummyConnector.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;


YARP_BEGIN_PACK
class BinPortableTarget {
public:
    BinPortableTarget() {
        tag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT32;
        len = 2;
    }

    NetInt32 tag;
    NetInt32 len;
    NetInt32 x;
    NetInt32 y;
};
YARP_END_PACK


TEST_CASE("os::BinPortableTest", "[yarp::os]")
{

    NetworkBase::setLocalMode(true);

    SECTION("checking binary read/write of native int")
    {
        BinPortable<int> i;
        i.content() = 5;

        PortReaderBuffer<BinPortable<int>> buf;
        Port input, output;
        bool ok1 = input.open("/in");
        bool ok2 = output.open("/out");
        REQUIRE(ok1);
        REQUIRE(ok2); // "ports opened ok"

        buf.attach(input);
        output.addOutput(Contact("/in", "tcp"));
        INFO("writing...");
        output.write(i);
        INFO("reading...");
        BinPortable<int> *result = buf.read();
        REQUIRE(result != nullptr); // "got something check"
        CHECK(result->content() == 5); // "value preserved"
        output.close();
        input.close();
    }

    SECTION("checking text mode")
    {
        DummyConnector con;

        BinPortable<BinPortableTarget> t1, t2;
        t1.content().x = 10;
        t1.content().y = 20;

        t2.content().x = 0;
        t2.content().y = 0;

        t1.write(con.getWriter());
        t2.read(con.getReader());

        CHECK(t2.content().x == 10); // "x value"
        CHECK(t2.content().y == 20); // "y value"
    }

    NetworkBase::setLocalMode(false);

}
