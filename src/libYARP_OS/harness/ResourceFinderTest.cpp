// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Time.h>
#include <yarp/NetType.h>
#include <yarp/String.h>

#include <yarp/UnitTest.h>

using namespace yarp::os;

class ResourceFinderTest : public yarp::UnitTest {
public:
    virtual yarp::String getName() { return "ResourceFinderTest"; }

    virtual void runTests() {
        // none yet
    }
};

static ResourceFinderTest theResourceFinderTest;

yarp::UnitTest& getResourceFinderTest() {
    return theResourceFinderTest;
}

