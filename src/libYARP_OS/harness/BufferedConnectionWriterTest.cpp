// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StringOutputStream.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;

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

