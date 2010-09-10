// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


/**
 * \infile Tests for math.
 */

// Added tests for random generator.

#include <yarp/os/impl/UnitTest.h>

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Rand.h>
#include <yarp/math/SVD.h>

#include <math.h>

using namespace yarp::os::impl;
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
        checkTrue(acc==6, "operator+ on vectors");
        
        c=a-b;
        acc=c[0]+c[1]+c[2];
        checkTrue(acc==0,  "operator- on vectors");

        a=2;
        b=2;
        acc=dot(a,b);
        checkTrue(acc==12,  "dot product on vectors");

        //scalar mult
        a=1;
        Vector m1=a*3;
        Vector m2=3*a;

        acc=m1[0]+m1[1]+m1[2];
        checkTrue(acc==9, "operator*");
        acc=m2[0]+m2[1]+m2[2];
        checkTrue(acc==9, "operator*");
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
        checkTrue((ret1==a1), "Vector by Matrix multiplication");
        a2=m*v2;
        checkTrue((ret2==a2), "Matrix by Vector multiplication");
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
        checkTrue(ret, "Matrix::operator*");
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
    
    void matrixInv() {
        report(0,"checking matrix inversions...");
        Matrix A(4,4);
        int counter = 1;
        for(int r = 0; r < A.rows(); r++) {
            for(int c = 0; c < A.cols(); c++) {
                A(r, c) = counter++ + (r == c ? 1 : 0);
            }
        }
        Matrix Ainv = luinv(A);
        Matrix I = A * Ainv;

        bool invGood=true;
        Matrix ref=eye(4,4);
        for(int r=0;r<I.rows(); r++)
            for(int c=0;c<I.cols(); c++)
            {
                if (fabs(I[r][c]-ref[r][c])>0.0001)
                    invGood=false;
            
            }
            
        checkTrue(invGood, "luinv");

        //printf("luinv: %s\n", I.toString().c_str());
        
        /*  [ 2 1 0 0 ]^-1   [ 1 -1  1 -1 ]
         *  [ 1 2 1 0 ]      [-1  2 -2  2 ]
         *  [ 0 1 2 1 ]    = [ 1 -2  3 -3 ]
         *  [ 0 0 1 1 ]      [-1  2 -3  4 ]
         */
        /* See note in math.cpp implementation of chinv
        Matrix B = zeros(4,4);
        B(0,0) = B(1,1) = B(2,2) = 2;
        B(0,1) = B(1,0) = B(1,2) = B(2,1) = B(2,3) = B(3,2) = B(3,3) = 1;
        Matrix Binv = chinv(B);
        I = B * Binv;
        printf("chinv: %s\n", I.toString().c_str());
        */
        
    }
    
    void matrixDet() {
        report(0,"checking matrix determinant...");
        Matrix A(4,4);
        A(0,0) = 2;
        A(0,1) = 3;
        A(0,2) = 5;
        A(0,3) = 7;

        A(1,0) = 4;
        A(1,1) = 7;
        A(1,2) = 2;
        A(1,3) = 9;

        A(2,0) = 5;
        A(2,1) = 5;
        A(2,2) = 6;
        A(2,3) = 8;

        A(3,0) = 1;
        A(3,1) = 1;
        A(3,2) = 4;
        A(3,3) = 7;
        
        double val = det(A);
        bool ok = ((val - -163) < 1e-10 && (-163 - val) < 1e-10);
        checkTrue(ok, "Matrix determinant");
        printf("det: %g\n", val);
    }

    virtual void runTests() 
    {
        checkMiscOperations();
        vectorOps();
        matrixOps();
        vectMatrix();
        matrixInv();
        matrixDet();
    }
};

static MathTest theMathTest;

UnitTest& getMathTest() {
    return theMathTest;
}

