// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_MATH__
#define __YARP_MATH__

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>

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
        yarp::sig::Vector YARP_math_API operator+(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Addition operator between matrices, returns a+b.
        */
        yarp::sig::Matrix YARP_math_API operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
        
        /**
        * Subtraction operator between vectors, returns a-b.
        */
        yarp::sig::Vector YARP_math_API operator-(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Addition operator between matrices, returns a+b.
        */
        yarp::sig::Matrix YARP_math_API operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
        
        /**
        * Scalar product between vectors, returns a.*b.
        */
        double YARP_math_API dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        
        /**
        * Scalar-vector product operator. 
        * @param k a scalar
        * @param b vector
        * @return k*b
        */
        yarp::sig::Vector YARP_math_API operator*(double k, const yarp::sig::Vector &b);

        /**
        * Vector-scalar product operator. 
        * @param b a vector
        * @param k a scalar
        * @return b*k
        */
        yarp::sig::Vector YARP_math_API operator*(const yarp::sig::Vector &b, double k);

        
        /**
        * Vector-matrix product operator. 
        * @param a is a vector (interpreted as a row)
        * @param m is a matrix
        * @return a*m
        */
         yarp::sig::Vector YARP_math_API operator*(const yarp::sig::Vector &a, 
            const yarp::sig::Matrix &m);
        
        /**
        * Matrix-vector product operator. 
        * @param a is a vector (interpreted as a column)
        * @param m is a matrix
        * @return m*a
        */
         yarp::sig::Vector YARP_math_API operator*(const yarp::sig::Matrix &m, 
            const yarp::sig::Vector &a);
        
         /**
        * Matrix-matrix product operator. 
        * @param a a matrix
        * @param b a matrix
        * @return a*b
        */
        yarp::sig::Matrix YARP_math_API operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

        /**
       * Scalar-matrix product operator. 
       * @param k a scalar
       * @param M a matrix
       * @return k*M
       */
        yarp::sig::Matrix YARP_math_API operator*(const double k, const yarp::sig::Matrix &M);
               
        /**
       * Matrix-scalar product operator. 
       * @param M a matrix
       * @param k a scalar
       * @return M*k
       */
        yarp::sig::Matrix YARP_math_API operator*(const yarp::sig::Matrix &M, const double k);
        
        /**
       * Vector-vector element-wise product operator. 
       * @param a a vector
       * @param b a vector
       * @return a.*b (matlab notation)
       */
        yarp::sig::Vector YARP_math_API operator*(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
                
        /**
       * Vector-vector element-wise division operator. 
       * @param a a vector
       * @param b a vector
       * @return a./b (matlab notation)
       */
        yarp::sig::Vector YARP_math_API operator/(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Vector-scalar division operator. 
        * @param b a vector
        * @param k a scalar
        * @return b/k
        */
        yarp::sig::Vector YARP_math_API operator/(const yarp::sig::Vector &b, double k);

       /**
       * Matrix-scalar division operator. 
       * @param M a matrix
       * @param k a scalar
       * @return M./k (matlab notation)
       */
        yarp::sig::Matrix YARP_math_API operator/(const yarp::sig::Matrix &M, double k);

        /**
        * Creates a vector of zeros.
        * @param s the size of the new vector
        * @return a copy of the new vector
        */
        yarp::sig::Vector YARP_math_API zeros(int s);

        /**
        * Build an identity matrix.
        * @param r number of rows
        * @param c number of columns
        * @return the new matrix
        */
        yarp::sig::Matrix YARP_math_API eye(int r, int c);

        /**
        * Build a matrix of zeros.
        * @param r number of rows
        * @param c number of columns
        */
        yarp::sig::Matrix YARP_math_API zeros(int r, int c);
        
        /**
        * Computes the determinant of a matrix.
        * @param in the matrix
        */
        double YARP_math_API det(const yarp::sig::Matrix& in);

        /**
        * Invert a square matrix using LU-decomposition.
        * @param in square matrix
        * @return the inverse of the matrix
        */
        yarp::sig::Matrix YARP_math_API luinv(const yarp::sig::Matrix& in);

        /**
        * Invert a symmetric and positive definite matrix using Cholesky 
        * decomposition. 
        * @param in symmetric and positive definite matrix
        * @return the inverse of the matrix
        */
        //yarp::sig::Matrix YARP_math_API chinv(const yarp::sig::Matrix& in);
    }
}

#endif
