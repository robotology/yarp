// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP_MATH_SVD__
#define __YARP_MATH_SVD__

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

namespace yarp
{
    namespace math 
    {
        /**
        * Perform SVD decomposition on a NxM matrix. Golub Reinsch method.
        */
        void SVD(const yarp::sig::Matrix &in, 
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a NxM matrix. 
        * Modified Golub Reinsch method, fast for M>>N.
        */
        void SVDMod(const yarp::sig::Matrix &in, 
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

    }
}

#endif
