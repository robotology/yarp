/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class NameConfigTest : public UnitTest {
public:
    virtual std::string getName() override { return "NameConfigTest"; }

    void testRead() {
        report(0,"no tests yet");
        //report(0,"checking writing the config file");
        NameConfig nc;
        //bool result = nc.createPath("/tmp/work/bozo/foo/namer.conf");
        std::string fname = nc.getConfigFileName();
        //std::string txt = nc.readConfig(fname);
        report(0,fname);
        //report(0,txt);
    }

    virtual void runTests() override {
        testRead();
    }
};

static NameConfigTest theNameConfigTest;

UnitTest& getNameConfigTest() {
    return theNameConfigTest;
}
