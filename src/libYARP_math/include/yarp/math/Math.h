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
#include <math.h>

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
        * Scalar product between vectors, returns a*b.
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
         * Matrix-Matrix concatenation by column.
         * @param m1 a matrix nXm
         * @param m2 a matrix rXm
         * @return a matrix (n+r)Xm
         */
        yarp::sig::Matrix YARP_math_API pile(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2);

        /**
         * Vector-Matrix concatenation. Add a row at the beginning of a matrix.
         * @param m a matrix nXm
         * @param v a vector m
         * @return a matrix (n+1)Xm
         */
        yarp::sig::Matrix YARP_math_API pile(const yarp::sig::Vector &v, const yarp::sig::Matrix &m);        

        /**
         * Matrix-Vector concatenation. Add a row at the end of a matrix.
         * @param m a matrix nXm
         * @param v a vector m
         * @return a matrix (n+1)Xm
         */
        yarp::sig::Matrix YARP_math_API pile(const yarp::sig::Matrix &m, const yarp::sig::Vector &v);

        /**
         * Vector-Vector concatenation. Create a two row matrix by stacking two vectors.
         * @param v1 an n-vector
         * @param v2 an n-vector
         * @return a matrix 2Xn
         */
        yarp::sig::Matrix YARP_math_API pile(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2);

        /**
         * Matrix-Matrix concatenation by row.
         * @param m1 a matrix nXm
         * @param m2 a matrix nXr
         * @return a matrix nX(m+r)
         */
        yarp::sig::Matrix YARP_math_API cat(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2);

        /**
         * Matrix-Vector concatenation. Add a column at the end of a matrix.
         * @param m a matrix nXm
         * @param v a vector n
         * @return a matrix nX(m+1)
         */
        yarp::sig::Matrix YARP_math_API cat(const yarp::sig::Matrix &m, const yarp::sig::Vector &v);        

        /**
         * Vector-Matrix concatenation. Add a column at the beginning of a matrix.
         * @param m a matrix nXm
         * @param v a vector n
         * @return a matrix nX(m+1)
         */
        yarp::sig::Matrix YARP_math_API cat(const yarp::sig::Vector &v, const yarp::sig::Matrix &m);

        /**
         * Vector-Vector concatenation. Create a vector by putting two vectors side by side.
         * @param v1 an n-vector
         * @param v2 an m-vector
         * @return a (n+m)-vector
         */
        yarp::sig::Vector YARP_math_API cat(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2);

        /**
         * Vector-scalar concatenation. Create a vector by putting the scalar at the end of the vector.
         * @param v an n-vector
         * @param s a scalar
         * @return a (n+1)-vector
         */
        yarp::sig::Vector YARP_math_API cat(const yarp::sig::Vector &v, double s);

        /**
         * Scalar-vector concatenation. Create a vector by putting the scalar at the beginning of the vector.
         * @param s a scalar
         * @param v an n-vector
         * @return a (n+1)-vector
         */
        yarp::sig::Vector YARP_math_API cat(double s, const yarp::sig::Vector &v);

        /**
         * Scalar-scalar concatenation. Create a vector containing the two specified scalar values.
         * @param s1 a scalar
         * @param s2 a scalar
         * @return a 2-vector
         */
        yarp::sig::Vector YARP_math_API cat(double s1, double s2);
        

        /**
        * Returns the the cross product between two vectors. 
        * @param a is the first input vector. 
        * @param b is the second input vector. 
        * @param verbose sets some verbosity. 
        * @return axb.
        */
        yarp::sig::Vector YARP_math_API cross(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Returns the Euclidean norm of the vector. 
        * @param v is the input vector. 
        * @return ||v||. 
        */
        inline double YARP_math_API norm(const yarp::sig::Vector &v)
        {
            return sqrt(dot(v,v));
        }

        /**
        * Returns the Euclidean squared norm of the vector. 
        * @param v is the input vector. 
        * @return ||v||^2. 
        */
        inline double YARP_math_API norm2(const yarp::sig::Vector &v)
        {
            return dot(v,v);
        }

        /**
        * Returns the maximum of the elements of a real vector.
        * @param v is the input vector. 
        * @return max(v). 
        */
        double YARP_math_API max(const yarp::sig::Vector &v);


        /**
        * Returns the minimum of the elements of a real vector.
        * @param v is the input vector. 
        * @return min(v). 
        */
        double YARP_math_API min(const yarp::sig::Vector &v);

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
