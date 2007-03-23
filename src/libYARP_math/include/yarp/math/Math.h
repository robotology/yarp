// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP_MATH__
#define __YARP_MATH__

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

namespace yarp
{
    namespace math 
    {
        yarp::sig::Vector operator+(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        yarp::sig::Vector operator-(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        double dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        yarp::sig::Vector operator*(double k, const yarp::sig::Vector &b);
        yarp::sig::Vector operator*(const yarp::sig::Vector &a, double k);

        /**
        * Creates a vector of zeros.
        * @param s the size of the new vector
        * @return a copy of the new vector
        */
        yarp::sig::Vector zeros(int s);

        /**
        * Build an identity matrix.
        * @param r number of rows
        * @param c number of columns
        * @return the new matrix
        */
        yarp::sig::Matrix eye(int r, int c);

        /**
        * Build a matrix of zeros.
        * @param r number of rows
        * @param c number of columns
        */
        yarp::sig::Matrix zeros(int r, int c);
    }
}

#endif