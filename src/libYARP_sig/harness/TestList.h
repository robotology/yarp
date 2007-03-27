// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TESTLIST_
#define _YARP2_TESTLIST_

#include <yarp/UnitTest.h>

namespace yarp {
    class TestList;
}

// need to made one function for each new test, and add to collectTests()
// method.
extern yarp::UnitTest& getImageTest();
extern yarp::UnitTest& getVectorTest();
extern yarp::UnitTest& getSoundTest();
extern yarp::UnitTest& getMatrixTest();

class yarp::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getImageTest());
        root.add(getVectorTest());
		root.add(getMatrixTest());
        root.add(getSoundTest());
    }
};

#endif

