/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/Network.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class PortCoreTest : public PortReader {
public:
    int safePort() { return Network::getDefaultPortRange()+100; }

    int receives;
    std::string expectation;

    bool read(ConnectionReader& reader) override {
        if (!reader.isValid()) {
            return false;
        }
        receives++;
        BottleImpl bot;
        bot.read(reader);
        if (expectation==std::string("")) {
            WARN("got unexpected input");
            return false;
        }
        CHECK(bot.toString() == expectation); // "received bottle"
        return true;
    }

    void testStartStop() {
        Contact address("/port", "tcp", "127.0.0.1", safePort());
        PortCore core;
        core.listen(address);
        core.start();
        INFO("there will be a small delay, stress-testing port");
        int tct = 10;
        int ct = 0;
        for (int i=0; i<tct; i++) {
            Time::delay(0.01*(i%4));
            OutputProtocol *op = Carriers::connect(address);
            if (op!=nullptr) {
                op->getOutputStream().write('h');
                op->close();
                delete op;
                ct++; // connect is an event
            } else {
                INFO("a connection failed");
            }
        }

        while (core.getOutputCount()>0 || core.getEventCount()<ct) {
            // close could abort connections
            Time::delay(0.2);
        }
        core.close();
        ct++; // close is an event

        core.join();
        CHECK(core.getEventCount() == ct); // "Got all events"
    }


    void testBottle() {
        expectation = "";
        receives = 0;

        Contact write = NetworkBase::registerContact(Contact("/write", "tcp", "127.0.0.1", safePort()));
        Contact read = NetworkBase::registerContact(Contact("/read", "tcp", "127.0.0.1", safePort()+1));
        Contact fake("tcp", "127.0.0.1", safePort()+2);

        CHECK(NetworkBase::queryName("/write").isValid() == true); // "name server sanity"
        CHECK(NetworkBase::queryName("/read").isValid()  == true); // "name server sanity"


        PortCore sender;
        PortCore receiver;
        receiver.setReadHandler(*this);
        sender.listen(write);
        receiver.listen(read);
        sender.start();
        receiver.start();
        //Time::delay(1);
        Bottle bot;
        bot.addInt32(0);
        bot.addString("Hello world");
        // INFO("sending bottle, should received nothing");
        expectation = "";
        sender.send(bot);
        Time::delay(0.3);
        CHECK(receives == 0); // "nothing received");
        NetworkBase::connect("/write", "/read");
        Time::delay(0.3);
        // INFO("sending bottle, should receive it this time");
        expectation = bot.toString();
        sender.send(bot);
        for (int i=0; i<1000; i++) {
            if (receives==1) break;
            Time::delay(0.3);
        }
        CHECK(receives == 1); // "something received");
        sender.close();
        receiver.close();
    }


    void testBackground() {
        expectation = "";
        receives = 0;

        Contact write = NetworkBase::registerContact(Contact("/write", "tcp", "127.0.0.1", safePort()));
        Contact read = NetworkBase::registerContact(Contact("/read", "tcp", "127.0.0.1", safePort()+1));
        Contact fake("tcp", "127.0.0.1", safePort()+2);

        CHECK(NetworkBase::queryName("/write").isValid() == true); // "name server sanity");
        CHECK(NetworkBase::queryName("/read"). isValid() == true); // "name server sanity");

        PortCore sender;

        sender.setWaitBeforeSend(false);
        sender.setWaitAfterSend(false);

        PortCore receiver;
        receiver.setReadHandler(*this);
        sender.listen(write);
        receiver.listen(read);
        sender.start();
        receiver.start();
        //Time::delay(1);
        Bottle bot;
        bot.addInt32(0);
        bot.addString("Hello world");
        // report(0, "sending bottle, should received nothing");
        expectation = "";
        sender.send(bot);
        Time::delay(0.3);
        CHECK(receives == 0); // "nothing received");
        NetworkBase::connect("/write", "/read");
        Time::delay(0.3);

        expectation = bot.toString();
        sender.send(bot);
        for (int i=0; i<1000; i++) {
            if (receives==1) break;
            Time::delay(0.3);
        }
        CHECK(receives == 1);  // "something received");
        sender.close();
        receiver.close();
    }
};

TEST_CASE("os::impl::PortCoreTest", "[yarp::os][yarp::os::impl]")
{
    Network::setLocalMode(true);
    PortCoreTest thePortCoreTest;

    SECTION("checking start/stop works")
    {
        thePortCoreTest.testStartStop();
    }

    SECTION("simple bottle transmission check")
    {
        thePortCoreTest.testBottle();
    }

    SECTION("background transmission check")
    {
        thePortCoreTest.testBackground();
    }

    Network::setLocalMode(false);
}
