// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/StreamConnectionReader.h>
#include <yarp/StringInputStream.h>
#include <yarp/StringOutputStream.h>

#include "TestList.h"

using namespace yarp;

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

