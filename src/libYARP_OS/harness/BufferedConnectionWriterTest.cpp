// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StringOutputStream.h>

#include "TestList.h"

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
        checkEqual(sos.toString(),"Hello\nGreetings\n","two line writes");
    }

    virtual void runTests() {
        testWrite();
    }
};

static BufferedConnectionWriterTest theBufferedConnectionWriterTest;

UnitTest& getBufferedConnectionWriterTest() {
    return theBufferedConnectionWriterTest;
}

