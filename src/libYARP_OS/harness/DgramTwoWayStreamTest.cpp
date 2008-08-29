// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/DgramTwoWayStream.h>
#include <yarp/String.h>
#include <yarp/UnitTest.h>

using namespace yarp;
using namespace yarp::os;

class DgramTwoWayStreamTest : public yarp::UnitTest {
public:
    virtual yarp::String getName() { return "DgramTwoWayStreamTest"; }

    virtual void runTests() {
    }
};

static DgramTwoWayStreamTest theDgramTwoWayStreamTest;

yarp::UnitTest& getDgramTwoWayStreamTest() {
    return theDgramTwoWayStreamTest;
}

