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
#include <yarp/math/Rand.h>
#include <yarp/math/SVD.h>

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
        m=eye(5,2);

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

    void vectMatrix()
    {
        
        Matrix m(3,2);
        m=eye(3,2);
        Vector v1(3);
        v1=1;
        Vector v2(2);
        v2=1;
        Vector a1;
        Vector a2;
        Vector ret1(2);
        Vector ret2(3);
        ret1(0)=1;
        ret1(1)=1;

        ret2(0)=1;
        ret2(1)=1;
        ret2(2)=0;

        a1=v1*m;
        checkTrue((ret1==a1), "Vector by Matrix mult works");
        a2=m*v2;
        checkTrue((ret2==a2), "Matrix by Vector mult works");
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

    void svd()
    {
        Matrix M(6,5);
        M=1;
        Matrix U,V;
        Vector s;
        Matrix S;
        S.resize(5,5);

        U.resize(6,5);
        s.resize(5);
        V.resize(5,5);

        SVD(M, U, s, V);

        S.diagonal(s);

        Matrix T(6,5);
        T=U*S*V.transposed();

        printf("%s\n", M.toString().c_str());
        printf("%s\n", T.toString().c_str());
    }

    virtual void runTests() 
    {
        checkMiscOperations();
        vectorOps();
        matrixOps();
        vectMatrix();
        svd();
    }
};

static MathTest theMathTest;

UnitTest& getMathTest() {
    return theMathTest;
}

