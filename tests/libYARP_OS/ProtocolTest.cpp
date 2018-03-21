/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/FakeTwoWayStream.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <cstdio>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class ProtocolTest : public UnitTest {
public:
    virtual ConstString getName() override { return "ProtocolTest"; }

    ConstString simplify(ConstString x) {
        return humanize(x);
    }

    void show(FakeTwoWayStream *fake1, FakeTwoWayStream *fake2) {
        printf("fake1  //  in: [%s]  //  out: [%s]\n",
                       simplify(fake1->getInputText()).c_str(),
                       simplify(fake1->getOutputText()).c_str());
        printf("fake2  //  in: [%s]  //  out: [%s]\n\n",
                       simplify(fake2->getInputText()).c_str(),
                       simplify(fake2->getOutputText()).c_str());
    }


    void testBottle() {
        report(0,"trying to send a bottle across a fake stream");

        // set up a fake sender/receiver pair
        FakeTwoWayStream *fake1 = new FakeTwoWayStream();
        FakeTwoWayStream *fake2 = new FakeTwoWayStream();
        fake1->setTarget(fake2->getStringInputStream());
        fake2->setTarget(fake1->getStringInputStream());

        // hand streams over to protocol managers
        Protocol p1(fake1);
        Protocol p2(fake2);

        p1.open(Route("/out","/in","text"));

        checkEqual(fake1->getOutputText(),"CONNECT /out\r\n",
                   "text carrier header");

        p2.open("/in");

        checkEqual(fake2->getOutputText(),"Welcome /out\r\n",
                   "text carrier response");

        BufferedConnectionWriter writer;
        writer.appendLine("d");
        writer.appendLine("0 \"Hello\"");
        p1.write(writer);

        const char *expect = "CONNECT /out\r\nd\r\n0 \"Hello\"\r\n";
        checkEqual(fake1->getOutputText(),expect,
                   "added a bottle");

        ConnectionReader& reader = p2.beginRead();
        ConstString str1 = reader.expectText().c_str();
        ConstString str2 = reader.expectText().c_str();
        p2.endRead();

        checkEqual(str1,ConstString("d"),"data tag");
        const char *expect2 = "0 \"Hello\"";
        checkEqual(str2,ConstString(expect2),"bottle representation");
    }

    virtual void runTests() override {
        testBottle();
    }
};

static ProtocolTest theProtocolTest;

UnitTest& getProtocolTest() {
    return theProtocolTest;
}
