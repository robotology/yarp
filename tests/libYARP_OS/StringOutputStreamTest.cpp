/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
    virtual ConstString getName() override { return "StringOutputStreamTest"; }

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

