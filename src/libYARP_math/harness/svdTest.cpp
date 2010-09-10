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
#include <math.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;

class SVDTest : public UnitTest {
public:
    virtual String getName() { return "SVDTest"; }

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

    virtual void runTests() 
    {
        svd();
    }
};

static SVDTest theSVDTest;

UnitTest& getSVDTest() {
    return theSVDTest;
}

