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
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class PortReaderBufferTest : public UnitTest {
public:
    virtual String getName() { return "PortReaderBufferTest"; }
  

    void checkAccept() {
        report(0, "checking direct object accept...");

        PortReaderBuffer<Bottle> buffer;
        Bottle dummy;
        Bottle data("hello");
        Bottle data2("there");

        buffer.acceptObject(&data, &dummy);
        
        Bottle *bot = buffer.read();
        checkTrue(bot!=NULL,"Inserted message received");
        if (bot!=NULL) {
            checkEqual(bot->toString().c_str(),"hello","value ok");
        }

        buffer.acceptObject(&data2, NULL);

        bot = buffer.read();
        checkTrue(bot!=NULL,"Inserted message received");
        if (bot!=NULL) {
            checkEqual(bot->toString().c_str(),"there","value ok");
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

        //p0.close();
        //Network::disconnect("/p1","/p2","local");
        //p1.close();
        //p2.close();
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        checkLocal(); // still rather experimental
        checkAccept();
        Network::setLocalMode(false);
    }
};

static PortReaderBufferTest thePortReaderBufferTest;

UnitTest& getPortReaderBufferTest() {
    return thePortReaderBufferTest;
}
