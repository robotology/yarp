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

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>

using namespace yarp;
using namespace yarp::sig;
using namespace yarp::math;

class MathTest : public UnitTest {
public:
    virtual String getName() { return "MathTest"; }

    void vectorOp()
    {
        report(0,"checking vector operators...");
    
        Vector a(3);
        Vector b(3);
        Vector c;
        a=1;
        b=1;
    
        //test 
        c=a+b;

        double acc=c[0]+c[1]+c[2];
        checkEqual(acc,6, "operator+ on vectors works");
        
        c=a-b;
        acc=c[0]+c[1]+c[2];
        checkEqual(acc,0, "operator- on vectors works");

        double dot=a*b;
        checkEqual(dot, 3, "operator* (dot product) on vectors works");
    }
    virtual void runTests() 
    {
        vectorOp();
    }
};

static MathTest theMathTest;

UnitTest& getMathTest() {
    return theMathTest;
}

