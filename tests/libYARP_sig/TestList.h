/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
extern yarp::os::impl::UnitTest& getSoundTest();
extern yarp::os::impl::UnitTest& getMatrixTest();

class yarp::sig::impl::TestList {
public:
    static void collectTests() {
        yarp::os::impl::UnitTest& root = yarp::os::impl::UnitTest::getRoot();
        root.add(getImageTest());
        root.add(getVectorTest());
        root.add(getMatrixTest());
        root.add(getSoundTest());
    }
};

#endif // YARP_TESTS_SIG_TESTLIST_H
