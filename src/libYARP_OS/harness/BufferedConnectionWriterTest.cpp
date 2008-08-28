// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StringOutputStream.h>

#include <yarp/UnitTest.h>
//#include "TestList.h"

using namespace yarp;

class BufferedConnectionWriterTest : public UnitTest {
public:
    virtual String getName() { return "BufferedConnectionWriterTest"; }

    void testWrite() {
        report(0,"testing writing...");
        StringOutputStream sos;    
        BufferedConnectionWriter bbr;
        bbr.reset(true);
        bbr.appendLine("Hello");
        bbr.appendLine("Greetings");
        bbr.write(sos);
        checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes");
    }

    virtual void runTests() {
        testWrite();
    }
};

static BufferedConnectionWriterTest theBufferedConnectionWriterTest;

UnitTest& getBufferedConnectionWriterTest() {
    return theBufferedConnectionWriterTest;
}

