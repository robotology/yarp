/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/StringOutputStream.h>

#include <yarp/os/impl/UnitTest.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace yarp::os;
using namespace yarp::os::impl;

class StringOutputStreamTest : public UnitTest {
public:
    virtual std::string getName() override { return "StringOutputStreamTest"; }

    void testWrite() {
        report(0,"testing writing...");
        StringOutputStream sos;
        char txt[] = "Hello my friend";
        Bytes b(txt,strlen(txt));
        sos.write(b);
        checkEqual(txt,sos.toString(),"single write");
        StringOutputStream sos2;
        sos2.write('y');
        sos2.write('o');
        checkEqual("yo",sos2.toString(),"multiple writes");
    }

    virtual void runTests() override {
        testWrite();
    }
};

static StringOutputStreamTest theStringOutputStreamTest;

UnitTest& getStringOutputStreamTest() {
    return theStringOutputStreamTest;
}

