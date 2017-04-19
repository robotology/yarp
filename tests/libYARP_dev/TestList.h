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
extern yarp::os::impl::UnitTest& getRobotDescriptionTest();

#ifdef YARP_CONTROLBOARDREMAPPER_TESTS
extern yarp::os::impl::UnitTest& getControlBoardRemapperTest();
#endif

#ifdef YARP_ANALOGWRAPPER_TESTS
extern yarp::os::impl::UnitTest& getAnalogWrapperTest();
#endif

#ifdef YARP_TESTFRAMEGRABBER_TESTS
extern yarp::os::impl::UnitTest& getTestFrameGrabberTest();
#endif

#ifdef WITH_YARPMATH
extern yarp::os::impl::UnitTest& getFrameTransformClientTest();
#endif

class yarp::os::impl::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getPolyDriverTest());
        root.add(getRobotDescriptionTest());
#ifdef YARP_CONTROLBOARDREMAPPER_TESTS
        root.add(getControlBoardRemapperTest());
#endif
#ifdef YARP_ANALOGWRAPPER_TESTS
        root.add(getAnalogWrapperTest());
#endif
#ifdef YARP_TESTFRAMEGRABBER_TESTS
        root.add(getTestFrameGrabberTest());
#endif
#ifdef WITH_YARPMATH
        root.add(getFrameTransformClientTest());
#endif	
    }
};

#endif

