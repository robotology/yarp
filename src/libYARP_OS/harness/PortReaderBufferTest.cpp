// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;


class PortReaderBufferTestHelper : public BufferedPort<Bottle> {
public:
    int count;

    PortReaderBufferTestHelper() {
        count = 0;
    }

    void onRead(Bottle& datum) {
        count += datum.size();
    }
};

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

    void checkCallback() {
        report(0, "checking callback...");
        BufferedPort<Bottle> out;
        PortReaderBufferTestHelper in;
        out.open("/out");
        in.open("/in");
        in.useCallback();
        Network::connect("/out","/in");
        Network::sync("/out");
        Network::sync("/in");
        out.prepare().fromString("1 2 3");
        out.write();
        int rep = 0;
        while (in.count==0 && rep<50) {
            Time::delay(0.1);
            rep++;
        }
        checkEqual(in.count,3,"got message #1");
        in.disableCallback();
        out.prepare().fromString("1 2 3 4");
        out.write(true);
        Bottle *datum = in.read();
        checkTrue(datum!=NULL, "got message #2");
        checkEqual(datum->size(),4,"message is ok");
        in.useCallback();
        in.count = 0;
        out.prepare().fromString("1 2 3 4 5");
        out.write(true);
        rep = 0;
        while (in.count==0 && rep<50) {
            Time::delay(0.1);
            rep++;
        }
        checkEqual(in.count,5,"got message #3");
    }

    void checkCallbackNoOpen() {
        report(0, "checking callback part without open...");
#if 0
        {
            report(1, "test 1");
            PortReaderBufferTestHelper in;
            in.useCallback();
        }
        {
            report(1, "test 2");
            PortReaderBufferTestHelper in;
            in.useCallback();
            in.close();
        }
#endif
    }

    virtual void runTests() {
        Network::setLocalMode(true);

        //checkLocal(); // still rather experimental
        // in fact too experimental, omit from general builds

        checkAccept();
        checkCallback();
        checkCallbackNoOpen();
        Network::setLocalMode(false);
    }
};

static PortReaderBufferTest thePortReaderBufferTest;

UnitTest& getPortReaderBufferTest() {
    return thePortReaderBufferTest;
}
