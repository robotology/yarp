// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/NameConfig.h>

#include <yarp/UnitTest.h>
//#include "TestList.h"

using namespace yarp;

class NameConfigTest : public UnitTest {
public:
    virtual String getName() { return "NameConfigTest"; }

    void testRead() {
        report(0,"no tests yet");
        //report(0,"checking writing the config file");
        NameConfig nc;
        //bool result = nc.createPath("/tmp/work/bozo/foo/namer.conf");
        String fname = nc.getConfigFileName();
        //String txt = nc.readConfig(fname);
        report(0,fname);
        //report(0,txt);
    }

    virtual void runTests() {
        testRead();
    }
};

static NameConfigTest theNameConfigTest;

UnitTest& getNameConfigTest() {
    return theNameConfigTest;
}
