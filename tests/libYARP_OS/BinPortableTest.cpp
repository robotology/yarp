/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>

#include <yarp/os/BinPortable.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt32.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;


YARP_BEGIN_PACK
class BinPortableTarget {
public:
    BinPortableTarget() {
        tag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT;
        len = 2;
    }

    NetInt32 tag;
    NetInt32 len;
    NetInt32 x;
    NetInt32 y;
};
YARP_END_PACK


class BinPortableTest : public UnitTest {
public:
    virtual std::string getName() override { return "BinPortableTest"; }

    void testInt() {
        report(0,"checking binary read/write of native int");
        BinPortable<int> i;
        i.content() = 5;

        PortReaderBuffer<BinPortable<int> > buf;
        Port input, output;
        bool ok1 = input.open("/in");
        bool ok2 = output.open("/out");
        checkTrue(ok1&&ok2,"ports opened ok");
        if (!(ok1&&ok2)) {
            return;
        }
        //input.setReader(buf);
        buf.attach(input);
        output.addOutput(Contact("/in", "tcp"));
        report(0,"writing...");
        output.write(i);
        report(0,"reading...");
        BinPortable<int> *result = buf.read();
        checkTrue(result!=nullptr,"got something check");
        if (result!=nullptr) {
            checkEqual(result->content(),5,"value preserved");
        }
        output.close();
        input.close();
    }

    void testText() {
        report(0,"checking text mode");
        DummyConnector con;
        
        BinPortable<BinPortableTarget> t1, t2;
        t1.content().x = 10;
        t1.content().y = 20;

        t2.content().x = 0;
        t2.content().y = 0;
        
        t1.write(con.getWriter());
        t2.read(con.getReader());

        checkEqual(t2.content().x, 10, "x value");
        checkEqual(t2.content().y, 20, "y value");
    }

    virtual void runTests() override {
        NetworkBase::setLocalMode(true);
        testInt();
        testText();
        NetworkBase::setLocalMode(false);
    }
};

static BinPortableTest theBinPortableTest;

UnitTest& getBinPortableTest() {
    return theBinPortableTest;
}
