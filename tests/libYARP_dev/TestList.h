/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
extern yarp::os::impl::UnitTest& getMapGrid2DTest();
#endif

#ifdef YARP_MULTIPLEANALOGSENSORSINTERFACES_TESTS
extern yarp::os::impl::UnitTest& getMultipleAnalogSensorsInterfacesTest();
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
        root.add(getMapGrid2DTest());
#endif
#ifdef YARP_MULTIPLEANALOGSENSORSINTERFACES_TESTS
        root.add(getMultipleAnalogSensorsInterfacesTest());
#endif
    }
};

#endif

