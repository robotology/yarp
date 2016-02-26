/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class StreamConnectionReaderTest : public UnitTest {
public:
    virtual String getName() { return "StreamConnectionReaderTest"; }

    void testRead() {
        report(0,"testing reading...");
        StringInputStream sis;
        StringOutputStream sos;
        sis.add("Hello\ngood evening and welcome");
        StreamConnectionReader sbr;
        Route route;
        sbr.reset(sis,NULL,route,10,true);
        String line = sbr.expectLine();
        checkEqual(line,"Hello","one line");
    }

    virtual void runTests() {
        testRead();
    }
};

static StreamConnectionReaderTest theStreamConnectionReaderTest;

UnitTest& getStreamConnectionReaderTest() {
    return theStreamConnectionReaderTest;
}

