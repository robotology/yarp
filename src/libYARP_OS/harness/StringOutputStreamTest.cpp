// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/StringOutputStream.h>

#include "TestList.h"

using namespace yarp;

class StringOutputStreamTest : public UnitTest {
public:
    virtual String getName() { return "StringOutputStreamTest"; }

    void testWrite() {
        report(0,"testing writing...");
        StringOutputStream sos;
        char txt[] = "Hello my friend";
        Bytes b(txt,ACE_OS::strlen(txt));
        sos.write(b);
        checkEqual(txt,sos.toString(),"single write");
        StringOutputStream sos2;
        sos2.write('y');
        sos2.write('o');
        checkEqual("yo",sos2.toString(),"multiple writes");
    }

    virtual void runTests() {
        testWrite();
    }
};

static StringOutputStreamTest theStringOutputStreamTest;

UnitTest& getStringOutputStreamTest() {
    return theStringOutputStreamTest;
}

