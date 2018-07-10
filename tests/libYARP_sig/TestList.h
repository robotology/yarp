/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_TESTS_SIG_TESTLIST_H
#define YARP_TESTS_SIG_TESTLIST_H

#include <yarp/os/impl/UnitTest.h>

namespace yarp {
    namespace sig {
        namespace impl {
            class TestList;
        }
    }
}

// need to made one function for each new test, and add to collectTests()
// method.
extern yarp::os::impl::UnitTest& getImageTest();
extern yarp::os::impl::UnitTest& getVectorTest();
extern yarp::os::impl::UnitTest& getVectorOfTest();
extern yarp::os::impl::UnitTest& getSoundTest();
extern yarp::os::impl::UnitTest& getMatrixTest();
extern yarp::os::impl::UnitTest& getPointCloudTest();

class yarp::sig::impl::TestList {
public:
    static void collectTests() {
        yarp::os::impl::UnitTest& root = yarp::os::impl::UnitTest::getRoot();
        root.add(getImageTest());
        root.add(getVectorTest());
        root.add(getVectorOfTest());
        root.add(getMatrixTest());
        root.add(getSoundTest());
        root.add(getPointCloudTest());
    }
};

#endif // YARP_TESTS_SIG_TESTLIST_H
