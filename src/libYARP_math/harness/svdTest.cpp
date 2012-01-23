// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale
* Copyright (C) 2010 The Robotcub consortium.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


/**
 * \infile Tests for SVD.
 */

#include <yarp/os/impl/UnitTest.h>

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/SVD.h>
#include <yarp/math/Rand.h>
#include <math.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

const double TOL = 1e-8;

class SVDTest : public UnitTest {
public:
    virtual String getName() { return "SVDTest"; }

    // Assert that 2 matrices are equal
    void assertEqual(const Matrix &A, const Matrix &B, string testName, bool verbose=false)
    {
        if(A.cols() != B.cols() || A.rows()!=B.rows()){
            if(verbose) printf("A != B: %s != %s\n", A.toString(3).c_str(), B.toString(3).c_str());
            checkTrue(false, testName.c_str());
        }
        for(int r=0; r<A.rows(); r++){
            for(int c=0; c<A.cols(); c++){
                if(fabs(A(r,c)-B(r,c))>TOL){
                    if(verbose) printf("A != B: %s != %s\n", A.toString(3).c_str(), B.toString(3).c_str());
                    checkTrue(false, testName.c_str());
                }
            }
        }
        checkTrue(true, testName.c_str());
    }

    void svd()
    {
        report(0,"checking SVD");

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

        bool svdOk=true;
        for(int r=0;r<M.rows();r++)
            for(int c=0;c<M.cols(); c++)
            {
                if (fabs(T[r][c]-M[r][c])>0.01)
                    svdOk=false;
            }

        checkTrue(svdOk, "SVD decomposition");

        //printf("%s\n", M.toString().c_str());
        //printf("%s\n", T.toString().c_str());
    }

    void pInv()
    {
        report(0, "checking pInv");

        int m=6, n=5, nTest=1;
        Matrix M, Minv;
        for(int i=0; i<nTest; i++)
        {
            do{
                M = Rand::matrix(m,m);  // create a random nonsingular square matrix
            }while(fabs(det(M))<TOL);
            Minv = pinv(M, TOL);
            assertEqual(M*Minv, eye(m), "pinv of square nonsingular matrix");
        }

        Matrix U(m,n), V(n,n);
        Vector s(n);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // skinny full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < TOL);
            Minv = pinv(M, TOL);
            assertEqual(Minv*M, eye(n), "pinv of full-rank skinny matrix");
        }
    }

    virtual void runTests() 
    {
        svd();
        pInv();
    }
};

static SVDTest theSVDTest;

UnitTest& getSVDTest() {
    return theSVDTest;
}

