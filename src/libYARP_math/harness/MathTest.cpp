// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


/**
 * \infile Tests for math.
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

    void checkMiscOperations() {
	    report(0,"check matrix misc operations...");
        Matrix m(4,4);

		m.zero();
        m=eye(5,5);
        m=eye(2,5);
        m=eye(5, 2);

		Matrix mt=m.transposed();
	}

    void vectorOps()
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
        checkTrue(acc==6, "operator+ on vectors works");
        
        c=a-b;
        acc=c[0]+c[1]+c[2];
        checkTrue(acc==0,  "operator- on vectors works");

        a=2;
        b=2;
        acc=dot(a,b);
        checkTrue(acc==12,  "dot product on vectors works");

        //scalar mult
        a=1;
        Vector m1=a*3;
        Vector m2=3*a;

        acc=m1[0]+m1[1]+m1[2];
        checkTrue(acc==9, "operator* works");
        acc=m2[0]+m2[1]+m2[2];
        checkTrue(acc==9, "operator* works");
    }

    void matrixOps()
    {
        report(0,"checking matrix operations...");

        Matrix A(3,4);
        Matrix B(4,2);
        A=2;
        B=3;
        Matrix C=A*B;

        Matrix exp(3,2);
        exp=24; //expected result
        
        bool ret=(exp==C);
        checkTrue(ret, "Matrix::operator* works");
    }

    virtual void runTests() 
    {
        checkMiscOperations();
        vectorOps();
        matrixOps();
    }
};

static MathTest theMathTest;

UnitTest& getMathTest() {
    return theMathTest;
}

