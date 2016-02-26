/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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

class yarp::os::impl::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getMathTest());
        root.add(getSVDTest());
        root.add(getRandTest());
    }
};

#endif

