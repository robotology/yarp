/*
* Author: Lorenzo Natale
* Copyright (C) 2010 The Robotcub consortium.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


/**
 * \infile Tests for SVD.
 */

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/UnitTest.h>

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/SVD.h>
#include <yarp/math/Rand.h>
#include <math.h>
#include <string>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

const double TOL = 1e-8;

class SVDTest : public UnitTest {
public:
    virtual ConstString getName() { return "SVDTest"; }

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

    // Assert that 2 matrices are not equal
    void assertNotEqual(const Matrix &A, const Matrix &B, string testName, bool verbose=false)
    {
        if(A.cols() != B.cols() || A.rows()!=B.rows()){
            checkTrue(true, testName.c_str());
            return;
        }
        for(int r=0; r<A.rows(); r++)
            for(int c=0; c<A.cols(); c++)
                if(fabs(A(r,c)-B(r,c))>TOL){
                    checkTrue(true, testName.c_str());
                    return;
                }
        checkTrue(false, testName.c_str());
    }

    void svd()
    {
        report(0,"checking SVD of skinny matrix");

        int m=6, n=5, nTest=1;
        Matrix U(m,n), V(n,n);
        Vector s(n);
        Matrix S(n,n);

        for(int i=0;i<nTest;i++){
            Matrix M = Rand::matrix(m,n)*100;
            SVD(M, U, s, V);
            S.diagonal(s);
            Matrix T = U*S*V.transposed();
            assertEqual(T, M, "SVD decomposition of skinny matrix");
        }
    }

    void svdFat()
    {
        report(0,"checking SVD of fat matrix");

        int m=5, n=6, nTest=1;
        Matrix U(m,m), V(n,m);
        Vector s(m);
        Matrix S(m,m);

        for(int i=0;i<nTest;i++){
            Matrix M = Rand::matrix(m,n)*100;
            SVD(M, U, s, V);
            S.diagonal(s);
            Matrix T = U*S*V.transposed();
            assertEqual(T, M, "SVD decomposition of fat matrix");
        }
    }

    void pInv()
    {
        report(0, "checking pInv of skinny/square matrix");

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

    void pInvFat()
    {
        report(0, "checking pInv of fat matrix");
        int m=4, n=5, nTest=1;
        Matrix M, Minv;
        Matrix U(m,m), V(m,n);
        Vector s(m);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // fat full rank matrix
                SVD(M, U, s, V);
            }while(s[m-1] < TOL);
            Minv = pinv(M, TOL);
            assertEqual(M*Minv, eye(m), "pinv of full-rank fat matrix");
        }
    }

    void pInvDamp()
    {
        report(0, "checking Damped Pseudo-Inverse");

        int m=6, n=5, nTest=1;
        Matrix M, Minv;
        for(int i=0; i<nTest; i++)
        {
            do{
                M = Rand::matrix(m,m);  // create a random nonsingular square matrix
            }while(fabs(det(M))<10*TOL);
            Minv = pinvDamped(M, TOL);
            assertEqual(M*Minv, eye(m), "pinvDamped of square nonsingular matrix");
        }

        Matrix U(m,n), V(n,n);
        Vector s(n);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // skinny full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < 10*TOL);
            Minv = pinvDamped(M, TOL);
            assertEqual(Minv*M, eye(n), "pinvDamped of full-rank skinny matrix");
        }

        m=5;
        n=6;
        U.resize(m,m); 
        V.resize(m,n);
        s.resize(m);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // fat full rank matrix
                SVD(M, U, s, V);
            }while(s[m-1] < 10*TOL);
            Minv = pinvDamped(M, TOL);
            assertEqual(M*Minv, eye(m), "pinvDamped of full-rank fat matrix");
        }
    }

    void projMat()
    {
        report(0, "checking projection matrix");
        int m=7, n=3, nTest=1;
        Matrix M, Mp;
        Matrix U(m,n), V(n,n);
        Vector s(n);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // skinny full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < TOL);
            Mp = projectionMatrix(M, TOL);
            assertEqual(Mp*M, M, "projection matrix of full-rank skinny matrix");
        }

        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(n,m)*100;  // fat full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < TOL);
            Mp = projectionMatrix(M, TOL);
            assertEqual(Mp, eye(n), "projection matrix of full-rank fat matrix");
        }
    }

     void nullspaceMat()
    {
        report(0, "checking nullspace projection matrix");
        int m=7, n=3, nTest=1;
        Matrix M, N;
        Matrix U(m,n), V(n,n);
        Vector s(n);
        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(m,n)*100;  // skinny full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < TOL);
            N = nullspaceProjection(M, TOL);
            assertEqual(N, zeros(n,n), "nullspace projection matrix of full-rank skinny matrix is zero");
        }

        for(int i=0; i<nTest; i++)
        {
            do
            {
                M = Rand::matrix(n,m)*100;  // fat full rank matrix
                SVD(M, U, s, V);
            }while(s[n-1] < TOL);
            N = nullspaceProjection(M, TOL);
            assertNotEqual(N, zeros(m,m), "nullspace projection matrix of full-rank fat matrix is not zero");
            assertEqual(M*N, zeros(n,m), "nullspace projection matrix of full-rank fat matrix");
        }
    }

    virtual void runTests() 
    {
        svd();
        svdFat();
        pInv();
        pInvFat();
        pInvDamp();
        projMat();
        nullspaceMat();
    }
};

static SVDTest theSVDTest;

UnitTest& getSVDTest() {
    return theSVDTest;
}

