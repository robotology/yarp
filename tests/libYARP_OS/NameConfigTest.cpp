// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;

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
