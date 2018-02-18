/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef _YARP2_MATHTESTLIST_
#define _YARP2_MATHTESTLIST_

#include <yarp/os/impl/UnitTest.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TestList;
        }
    }
}

//
// need to made one function for each new test, and add to collectTests()
// method.
extern yarp::os::impl::UnitTest& getMathTest();
extern yarp::os::impl::UnitTest& getSVDTest();
extern yarp::os::impl::UnitTest& getRandTest();
extern yarp::os::impl::UnitTest& getVec2DTest();

class yarp::os::impl::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getMathTest());
        root.add(getSVDTest());
        root.add(getRandTest());
        root.add(getVec2DTest());
    }
};

#endif

