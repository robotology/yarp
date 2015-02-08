// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
} PACKED_FOR_NET;
YARP_END_PACK


class BinPortableTest : public UnitTest {
public:
    virtual String getName() { return "BinPortableTest"; }

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
        output.addOutput(Contact::byName("/in").addCarrier("tcp"));
        report(0,"writing...");
        output.write(i);
        report(0,"reading...");
        BinPortable<int> *result = buf.read();
        checkTrue(result!=NULL,"got something check");
        if (result!=NULL) {
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
        
        t1.write(con.getWriter());
        t2.read(con.getReader());

        checkEqual(t2.content().x, 10, "x value");
        checkEqual(t2.content().y, 20, "y value");
    }

    virtual void runTests() {
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
