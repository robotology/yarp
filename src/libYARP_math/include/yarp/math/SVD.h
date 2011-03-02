// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
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
        * Perform SVD decomposition on a NxM matrix. Golub Reinsch method.
        */
        void YARP_math_API SVD(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a NxM matrix. 
        * Modified Golub Reinsch method, fast for M>>N.
        */
        void YARP_math_API SVDMod(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a NxM matrix. 
        * Jacobi method.
        */
        void SVDJacobi(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform moore-penrose pinv on a NxM
        * matrix.
        * @param in the matrix to be processed
        * @param tol tolerance on singular values to be considered as zero
        */
        yarp::sig::Matrix YARP_math_API pinv(const yarp::sig::Matrix &in, double tol=0);
    }
}

#endif
