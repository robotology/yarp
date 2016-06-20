/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TESTLIST_
#define _YARP2_TESTLIST_

#include <yarp/os/impl/UnitTest.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TestList;
        }
    }
}

// need to made one function for each new test, and add to collectTests()
// method
extern yarp::os::impl::UnitTest& getPolyDriverTest();

#ifdef YARP_CONTROLBOARDREMAPPER_TESTS
extern yarp::os::impl::UnitTest& getControlBoardRemapperTest();
#endif

#define YARP_TRANSFORM_TESTS
#ifdef YARP_TRANSFORM_TESTS
extern yarp::os::impl::UnitTest& getTransformClientTest();
#endif

class yarp::os::impl::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getPolyDriverTest());
#ifdef YARP_CONTROLBOARDREMAPPER_TESTS
        root.add(getControlBoardRemapperTest());
#endif

#ifdef YARP_TRANSFORM_TESTS
        root.add(getTransformClientTest());
#endif	
    }
};

#endif

