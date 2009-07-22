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
     /**
     * Mathematical operations, mostly a GSL wrapper.
     */
    namespace math 
    {
        /**
        * Addition operator between vectors, returns a+b.
        */
        yarp::sig::Vector operator+(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Addition operator between matrices, returns a+b.
        */
        yarp::sig::Matrix operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
        
        /**
        * Subtraction operator between vectors, returns a-b.
        */
        yarp::sig::Vector operator-(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Addition operator between matrices, returns a+b.
        */
        yarp::sig::Matrix operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
        
        /**
        * Scalar product between vectors, returns a.*b.
        */
        double dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        
        /**
        * Scalar-vector product operator. 
        * @param k a scalar
        * @param b vector
        * @return k*b
        */
        yarp::sig::Vector operator*(double k, const yarp::sig::Vector &b);

        /**
        * Vector-scalar product operator. 
        * @param b a vector
        * @param k a scalar
        * @return b*k
        */
        yarp::sig::Vector operator*(const yarp::sig::Vector &b, double k);
        
        /**
        * Vector-matrix product operator. 
        * @param a is a vector (interpreted as a row)
        * @param m is a matrix
        * @return a*m
        */
         yarp::sig::Vector operator*(const yarp::sig::Vector &a, 
            const yarp::sig::Matrix &m);
        
        /**
        * Matrix-vector product operator. 
        * @param a is a vector (interpreted as a column)
        * @param m is a matrix
        * @return m*a
        */
         yarp::sig::Vector operator*(const yarp::sig::Matrix &m, 
            const yarp::sig::Vector &a);
        
         /**
        * Matrix-matrix product operator. 
        * @param a a matrix
        * @param b a matrix
        * @return a*b
        */
        yarp::sig::Matrix operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

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

        /**
        * Invert a square matrix using LU-decomposition.
        * @param in square matrix
        * @return the inverse of the matrix
        */
        yarp::sig::Matrix luinv(const yarp::sig::Matrix& in);

        /**
        * Invert a symmetric and positive definite matrix using Cholesky 
        * decomposition. 
        * @param in symmetric and positive definite matrix
        * @return the inverse of the matrix
        */
        //yarp::sig::Matrix chinv(const yarp::sig::Matrix& in);
    }
}

#endif
