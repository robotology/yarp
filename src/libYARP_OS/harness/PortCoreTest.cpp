// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/Readable.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class PortCoreTest : public UnitTest, public PortReader {
public:
    virtual String getName() { return "PortCoreTest"; }

    String expectation;
    int receives;

    bool read(ConnectionReader& reader) {
        if (!reader.isValid()) {
            return false;
        }
        receives++;
        BottleImpl bot;
        bot.read(reader);
        if (expectation==String("")) {
            report(1,"got unexpected input");
            return false;
        }
        checkEqual(bot.toString(),expectation,"received bottle");
        return true;
    }

    void testStartStop() {
        report(0,"checking start/stop works (requires free port 9999)...");
        NameClient& nic = NameClient::getNameClient();
        nic.setFakeMode(true);

        Address address("localhost",9999,"tcp","/port");
        PortCore core;
        core.listen(address);
        core.start();
        report(0,"there will be a small delay, stress-testing port...");
        int tct = 10;
        int ct = 0;
        for (int i=0; i<tct; i++) {
            Time::delay(0.01*(i%4));
            OutputProtocol *op = Carriers::connect(address);
            if (op!=NULL) {
                op->getOutputStream().write('h');
                op->close();
                delete op;
                ct++; // connect is an event
            } else {
                report(1,"a connection failed");
            }
        }

        Time::delay(0.2);  // close will take precedence over pending connections
        core.close();
        ct++; // close is an event

        report(0,"finished stress-testing port...");

        core.join();
        checkEqual(core.getEventCount(),ct,"Got all events");

        nic.setFakeMode(false);
    }


    void testBottle() {
        report(0,"simple bottle transmission check (needs ports 9997, 9998, 9999)...");

        expectation = "";
        receives = 0;

        NameClient& nic = NameClient::getNameClient();
        nic.setFakeMode(true);

        Address write = nic.registerName("/write",Address("localhost",9999,"tcp"));
        Address read = nic.registerName("/read",Address("localhost",9998,"tcp"));
        Address fake = Address("localhost",9997,"tcp");

        checkEqual(nic.queryName("/write").isValid(),true,"name server sanity");
        checkEqual(nic.queryName("/read").isValid(),true,"name server sanity");


        PortCore sender;
        PortCore receiver;
        receiver.setReadHandler(*this);
        sender.listen(write);
        receiver.listen(read);
        sender.start();
        receiver.start();
        //Time::delay(1);
        BottleImpl bot;
        bot.addInt(0);
        bot.addString("Hello world");
        report(0,"sending bottle, should received nothing");
        expectation = "";
        sender.send(bot);
        Time::delay(0.3);
        checkEqual(receives,0,"nothing received");
        Companion::connect("/write", "/read");
        Time::delay(0.3);
        report(0,"sending bottle, should receive it this time");
        expectation = bot.toString();
        sender.send(bot);
        Time::delay(0.3);
        checkEqual(receives,1,"something received");
        sender.close();
        receiver.close();

        nic.setFakeMode(false);
    }


    void testBackground() {
        report(0,"background transmission check (needs ports 9997, 9998, 9999)...");

        expectation = "";
        receives = 0;

        NameClient& nic = NameClient::getNameClient();
        nic.setFakeMode(true);

        Address write = nic.registerName("/write",Address("localhost",9999,"tcp"));
        Address read = nic.registerName("/read",Address("localhost",9998,"tcp"));
        Address fake = Address("localhost",9997,"tcp");

        checkEqual(nic.queryName("/write").isValid(),true,"name server sanity");
        checkEqual(nic.queryName("/read").isValid(),true,"name server sanity");


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
        BottleImpl bot;
        bot.addInt(0);
        bot.addString("Hello world");
        report(0,"sending bottle, should received nothing");
        expectation = "";
        sender.send(bot);
        Time::delay(0.3);
        checkEqual(receives,0,"nothing received");
        Companion::connect("/write", "/read");
        Time::delay(0.3);
        report(0,"sending bottle, should receive it this time");
        expectation = bot.toString();
        sender.send(bot);
        Time::delay(0.3);
        checkEqual(receives,1,"something received");
        sender.close();
        receiver.close();

        nic.setFakeMode(false);
    }


    virtual void runTests() {
        testStartStop();
        testBottle();
        testBackground();
    }
};

static PortCoreTest thePortCoreTest;

UnitTest& getPortCoreTest() {
    return thePortCoreTest;
}
