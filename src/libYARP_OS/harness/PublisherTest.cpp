// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::os::impl;

static bool waitForOutput(Contactable& c,double timeout) {
    double start = Time::now();
    while (Time::now()-start<timeout) {
        if (c.getOutputCount()>0) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}


class PublisherTest : public UnitTest {
public:
    virtual ConstString getName() { return "PublisherTest"; }

    void testPublisherToBufferedPort() {
        report(0,"Publisher to BufferedPort test");

        Node n("/node");
        Publisher<Bottle> p("/very_interesting_topic");

        {
            Node n2("/node2");
            BufferedPort<Bottle> pin;
            pin.setReadOnly();
            pin.setStrict();
            pin.open("very_interesting_topic");

            waitForOutput(p,10);
            
            Bottle& b = p.prepare();
            b.clear();
            b.addInt(42);
            p.write();
            p.waitForWrite();

            Bottle *bin = pin.read();
            checkTrue(bin!=NULL,"message arrived");
            if (!bin) return;
            checkEqual(bin->get(0).asInt(),42,"message is correct");
        }
    }

    void testBufferedPortToSubscriber() {
        report(0,"BufferedPort to Subscriber test");

        Node n("/node");
        BufferedPort<Bottle> pout;
        pout.setWriteOnly();
        pout.open("very_interesting_topic");

        {           
            Node n2("/node2");
            Subscriber<Bottle> pin("/very_interesting_topic");
            pin.read(false); // make sure we are in buffered mode

            waitForOutput(pout,10);
            
            Bottle& b = pout.prepare();
            b.clear();
            b.addInt(42);
            pout.write();
            pout.waitForWrite();

            Bottle *bin = pin.read();
            checkTrue(bin!=NULL,"message arrived");
            if (!bin) return;
            checkEqual(bin->get(0).asInt(),42,"message is correct");
        }
    }

    void testPublisherToSubscriber() {
        report(0,"Publisher to Subscriber test");

        Node n("/node");
        Publisher<Bottle> pout;
        pout.topic("/very_interesting_topic");

        {           
            Node n2("/node2");
            Subscriber<Bottle> pin("/very_interesting_topic");
            pin.read(false); // make sure we are in buffered mode

            waitForOutput(pout,10);
            
            Bottle& b = pout.prepare();
            b.clear();
            b.addInt(42);
            pout.write();
            pout.waitForWrite();

            Bottle *bin = pin.read();
            checkTrue(bin!=NULL,"message arrived");
            if (!bin) return;
            checkEqual(bin->get(0).asInt(),42,"message is correct");
        }
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testPublisherToBufferedPort();
        testBufferedPortToSubscriber();
        testPublisherToSubscriber();
        Network::setLocalMode(false);
    }
};


static PublisherTest thePublisherTest;

UnitTest& getPublisherTest() {
    return thePublisherTest;
}

