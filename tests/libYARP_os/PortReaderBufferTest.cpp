/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;


class PortReaderBufferTestHelper : public BufferedPort<Bottle>
{
public:
    int count;

    PortReaderBufferTestHelper()
    {
        count = 0;
    }

    using BufferedPort<Bottle>::onRead;
    void onRead(Bottle& datum) override
    {
        count += datum.size();
    }
};

TEST_CASE("os::PortReaderBufferTest", "[yarp::os]")
{
#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

    NetworkBase::setLocalMode(true);

    SECTION("checking direct object accept")
    {
        PortReaderBuffer<Bottle> buffer;
        Bottle dummy;
        Bottle data("hello");
        Bottle data2("there");

        buffer.acceptObject(&data, &dummy);

        Bottle *bot = buffer.read();
        REQUIRE(bot!=nullptr); // Inserted message received
        if (bot!=nullptr) {
            CHECK(bot->toString() == "hello"); // value ok
        }

        buffer.acceptObject(&data2, nullptr);

        bot = buffer.read();
        REQUIRE(bot!=nullptr); // Inserted message received
        if (bot!=nullptr) {
            CHECK(bot->toString() == "there"); // value ok
        }

        buffer.read(false);
    }

    SECTION("checking local carrier")
    {
        Port p0;
        BufferedPort<Bottle> p1, p2;
        p0.open("/p0");
        p1.open("/p1");
        p2.open("/p2");

        Network::connect("/p0", "/p2", "local");
        Network::connect("/p1", "/p2", "local");
        Network::sync("/p0");
        Network::sync("/p1");
        Network::sync("/p2");

        Bottle data;
        data.fromString("hello");
        p0.write(data);

        Bottle *bot = p2.read();
        REQUIRE(bot!=nullptr); // Port message received
        if (bot!=nullptr) {
            CHECK(bot->toString() == "hello"); // value ok
        }

        Bottle& data2 = p1.prepare();
        data2.fromString("hello2");
        p1.write();


        bot = p2.read();
        REQUIRE(bot!=nullptr); // BufferedPort message received
        if (bot!=nullptr) {
            CHECK(bot->toString() == "hello2"); // value ok
        }

        //p0.close();
        //Network::disconnect("/p1", "/p2", "local");
        //p1.close();
        //p2.close();
    }

    SECTION("checking callback")
    {
        BufferedPort<Bottle> out;
        PortReaderBufferTestHelper in;
        out.open("/out");
        in.open("/in");
        in.useCallback();
        Network::connect("/out", "/in");
        Network::sync("/out");
        Network::sync("/in");
        out.prepare().fromString("1 2 3");
        out.write();
        int rep = 0;
        while (in.count==0 && rep<50) {
            Time::delay(0.1);
            rep++;
        }
        CHECK(in.count == 3); // got message #1
        in.disableCallback();
        out.prepare().fromString("1 2 3 4");
        out.write(true);
        Bottle *datum = in.read();
        REQUIRE(datum!=nullptr); // got message #2
        CHECK(datum->size() == (size_t) 4); // message is ok
        in.useCallback();
        in.count = 0;
        out.prepare().fromString("1 2 3 4 5");
        out.write(true);
        rep = 0;
        while (in.count==0 && rep<50) {
            Time::delay(0.1);
            rep++;
        }
        CHECK(in.count == 5); // got message #3
    }

    SECTION("checking callback part without open")
    {
        {
            INFO("test 1");
            PortReaderBufferTestHelper in;
            in.useCallback();
            in.disableCallback();
        }
        {
            INFO("test 2");
            PortReaderBufferTestHelper in;
            in.useCallback();
            in.disableCallback();
        }
        {
            INFO("test 3");
            PortReaderBufferTestHelper in;
            in.useCallback();
        }
        {
            INFO("test 4");
            PortReaderBufferTestHelper in;
            in.useCallback();
            in.close();
        }
    }

    NetworkBase::setLocalMode(false);
}
