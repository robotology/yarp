// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_MATH_SVD__
#define __YARP_MATH_SVD__

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>

namespace yarp
{
    namespace math 
    {
        /**
        * Factorize the M-by-N matrix 'in' into the singular value decomposition in = U S V^T for M >= N.
        * The diagonal elements of the singular value matrix S are stored in the vector S.
        * The singular values are non-negative and form a non-increasing sequence from S_1 to S_N. 
        * The matrix V contains the elements of V in untransposed form. To form the product U S V^T it 
        * is necessary to take the transpose of V. This routine uses the Golub-Reinsch SVD algorithm. 
        */
        void YARP_math_API SVD(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a MxN matrix (for M >= N).
        * Modified Golub Reinsch method. Fast for M>>N.
        */
        void YARP_math_API SVDMod(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a matrix using the Jacobi method. The Jacobi method
        * can compute singular values to higher relative accuracy than Golub-Reinsch algorithms.
        */
        void SVDJacobi(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform the moore-penrose pseudo-inverse on a MxN matrix for M >= N.
        * @param in input matrix 
        * @param tol singular values less than tol are set to zero
        * @return pseudo-inverse of the matrix 'in'
        */
        yarp::sig::Matrix YARP_math_API pinv(const yarp::sig::Matrix &in, double tol=0);
    }
}

#endif
