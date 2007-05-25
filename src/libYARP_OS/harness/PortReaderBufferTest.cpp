// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class PortReaderBufferTest : public UnitTest {
public:
    virtual String getName() { return "PortReaderBufferTest"; }
  

    void checkAccept() {
        // add tests here

        PortReaderBuffer<Bottle> buffer;
        Bottle dummy;
        Bottle data("hello");
        Bottle data2("there");

        buffer.acceptObject(&data, &dummy);
        
        Bottle *bot = buffer.read();
        if (bot!=NULL) {
            printf("got %s\n", bot->toString().c_str());
        } else {
            printf("doh, no data\n");
        }

        buffer.acceptObject(&data2, NULL);

        bot = buffer.read();
        if (bot!=NULL) {
            printf("got %s\n", bot->toString().c_str());
        } else {
            printf("doh, no data\n");
        }
        
        buffer.read(false);
    }
  

    void checkLocal() {
        report(0, "checking local carrier...");

        Port p0;
        BufferedPort<Bottle> p1, p2;
        p0.open("/p0");
        p1.open("/p1");
        p2.open("/p2");

        Network::connect("/p0","/p2","local");
        Network::connect("/p1","/p2","local");
        Network::sync("/p0");
        Network::sync("/p1");
        Network::sync("/p2");

        Bottle data;
        data.fromString("hello");
        p0.write(data);

        Bottle *bot = p2.read();
        checkTrue(bot!=NULL,"Port message received");
        if (bot!=NULL) {
            checkEqual(bot->toString().c_str(),"hello","value ok");
        }

        Bottle& data2 = p1.prepare();
        data2.fromString("hello2");
        p1.write();
        
        bot = p2.read();
        checkTrue(bot!=NULL,"BufferedPort message received");
        if (bot!=NULL) {
            checkEqual(bot->toString().c_str(),"hello2","value ok");
        }
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        checkLocal();
        //checkAccept();
        Network::setLocalMode(false);
    }
};

static PortReaderBufferTest thePortReaderBufferTest;

UnitTest& getPortReaderBufferTest() {
    return thePortReaderBufferTest;
}
