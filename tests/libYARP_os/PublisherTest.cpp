/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

static bool waitForOutput(Contactable& c, double timeout) {
    double start = Time::now();
    while (Time::now()-start<timeout) {
        if (c.getOutputCount()>0) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}


TEST_CASE("os::PublisherTest", "[yarp::os]")
{
    Network::setLocalMode(true);

    SECTION("Publisher to BufferedPort test")
    {
        Node n("/node");
        Publisher<Bottle> p("/very_interesting_topic");

        {
            Node n2("/node2");
            BufferedPort<Bottle> pin;
            pin.setReadOnly();
            pin.setStrict();
            pin.open("very_interesting_topic");

            waitForOutput(p, 10);

            Bottle& b = p.prepare();
            b.clear();
            b.addInt32(42);
            p.write();
            p.waitForWrite();

            Bottle *bin = pin.read();
            REQUIRE(bin!=nullptr); // "message arrived"

            CHECK(bin->get(0).asInt32() == 42);  // "message is correct"
        }
    }

    SECTION("BufferedPort to Subscriber test")
    {
        Node n("/node");
        BufferedPort<Bottle> pout;
        pout.setWriteOnly();
        pout.open("very_interesting_topic");

        {
            Node n2("/node2");
            Subscriber<Bottle> pin("/very_interesting_topic");
            pin.read(false); // make sure we are in buffered mode

            waitForOutput(pout, 10);

            Bottle& b = pout.prepare();
            b.clear();
            b.addInt32(42);
            pout.write();
            pout.waitForWrite();

            Bottle *bin = pin.read();
            REQUIRE(bin != nullptr);  // "message arrived"

            CHECK(bin->get(0).asInt32() == 42);  // "message is correct"
        }
    }

    SECTION("Publisher to Subscriber test")
    {
        Node n("/node");
        Publisher<Bottle> pout;
        pout.topic("/very_interesting_topic");
        {
            Node n2("/node2");
            Subscriber<Bottle> pin("/very_interesting_topic");
            pin.read(false); // make sure we are in buffered mode

            waitForOutput(pout, 10);

            Bottle& b = pout.prepare();
            b.clear();
            b.addInt32(42);
            pout.write();
            pout.waitForWrite();

            Bottle *bin = pin.read();
            REQUIRE(bin != nullptr);  // "message arrived"

            CHECK(bin->get(0).asInt32() == 42);  // "message is correct"
        }
    }

    SECTION("Unbuffered Publisher test")
    {

        Node n("/node");
        Publisher<Bottle> p("/very_interesting_topic");

        {
            Node n2("/node2");
            BufferedPort<Bottle> pin;
            pin.setReadOnly();
            pin.setStrict();
            pin.open("very_interesting_topic");

            waitForOutput(p, 10);

            Bottle b;
            b.addInt32(42);
            p.write(b);

            Bottle *bin = pin.read();
            REQUIRE(bin != nullptr); // "message arrived"

            CHECK(bin->get(0).asInt32() == 42);  // "message is correct"
        }
    }

    SECTION("Unbuffereded Subscriber test")
    {
        Node n("/node");
        BufferedPort<Bottle> pout;
        pout.setWriteOnly();
        pout.open("very_interesting_topic");

        {
            Node n2("/node2");
            Subscriber<Bottle> pin("/very_interesting_topic");

            waitForOutput(pout, 10);

            Bottle& b = pout.prepare();
            b.clear();
            b.addInt32(42);
            pout.write();

            Bottle bin;
            bin.addInt32(99);
            pin.read(bin);
            pout.waitForWrite();
            CHECK(bin.get(0).asInt32() == 42);  // "message is correct"
        }
    }

    Network::setLocalMode(false);
};
