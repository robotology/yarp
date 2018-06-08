/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/StringInputStream.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class StringInputStreamTest : public UnitTest {
public:
    virtual std::string getName() const override { return "StringInputStreamTest"; }

    void testRead() {
        report(0,"test reading...");

        StringInputStream sis;
        sis.add("Hello my friend");
        char buf[256];
        sis.check();
        Bytes b(buf,sizeof(buf));
        int len = sis.read(b,0,5);
        checkEqual(len,5,"len of first read");
        buf[len] = '\0';
        checkEqual("Hello",buf,"first read");
        char ch = sis.read();
        checkEqual(ch,' ',"the space");
        len = sis.read(b,0,2);
        checkEqual(len,2,"len of second read");
        buf[len] = '\0';
        checkEqual("my",buf,"second read");
    }

    virtual void runTests() override {
        testRead();
    }
};

static StringInputStreamTest theStringInputStreamTest;

UnitTest& getStringInputStreamTest() {
    return theStringInputStreamTest;
}

