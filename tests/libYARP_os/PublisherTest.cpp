/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Publisher.h>
#include <yarp/os/PublisherLatched.h>
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

    SECTION("Unbuffered Subscriber test")
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

    SECTION("Latched Publisher test")
    {
        Node nout("/node_o");
        PublisherLatched<Bottle> pout("/latched_topic");
        Bottle& b = pout.prepare();
        b.clear();
        b.addInt32(42);
        pout.write();
        {
            Node nin1("/node_i1");
            Subscriber<Bottle> sub_in1("/latched_topic");

            Bottle* bin = sub_in1.read();

            REQUIRE(bin != nullptr);
            CHECK(bin->get(0).asInt32() == 42);
        }
        {
            Node nin2("/node_i2");
            Subscriber<Bottle> sub_in2("/latched_topic");

            Bottle* bin = sub_in2.read();

            REQUIRE(bin != nullptr);
            CHECK(bin->get(0).asInt32() == 42);
        }
        pout.close();
    }

    SECTION("Latched Publisher test (no writes called, uninitialized publisher)")
    {
        Node nout("/node_o");
        PublisherLatched<Bottle> pout("/latched_topic");
        {
            Node nin1("/node_i1");
            Subscriber<Bottle> sub_in1("/latched_topic");
            waitForOutput(pout, 10);

            Bottle* bin = sub_in1.read();

            REQUIRE(bin != nullptr);
            CHECK(bin->size() == 0);
        }
        {
            Node nin2("/node_i2");
            Subscriber<Bottle> sub_in2("/latched_topic");
            waitForOutput(pout, 10);

            Bottle* bin = sub_in2.read();

            REQUIRE(bin != nullptr);
            CHECK(bin->size() == 0);
        }
        pout.close();
    }

    Network::setLocalMode(false);
};
