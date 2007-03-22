// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


/**
 *
 * Tests for math. Empty for now
 *
 */

#include <yarp/UnitTest.h>

using namespace yarp;

class MathTest : public UnitTest {
public:
    virtual String getName() { return "MathTest"; }

    virtual void runTests() {
    }
};

static MathTest theMathTest;

UnitTest& getMathTest() {
    return theMathTest;
}

