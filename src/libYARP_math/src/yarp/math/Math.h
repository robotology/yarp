/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_MATH_H
#define YARP_MATH_MATH_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>
#include <yarp/math/Quaternion.h>

/**
* Mathematical operations.
*/

/**
* Addition operator between a scalar and a vector (defined in Math.h).
* Sum the scalar to all the elements of the vector.
*/
YARP_math_API yarp::sig::Vector operator+(const yarp::sig::Vector &a, const double &s);

/**
* Addition operator between a scalar and a vector (defined in Math.h).
* Sum the scalar to all the elements of the vector.
*/
YARP_math_API yarp::sig::Vector operator+(const double &s, const yarp::sig::Vector &a);

/**
* Addition operator between a scalar and a vector (defined in Math.h).
* Sum the scalar to all the elements of the vector.
*/
YARP_math_API yarp::sig::Vector& operator+=(yarp::sig::Vector &a, const double &s);

/**
* Addition operator between vectors, returns a+b (defined in Math.h).
*/
YARP_math_API yarp::sig::Vector operator+(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
 * Addition operator between vectors, returns a+b (defined in Math.h).
 */
YARP_math_API yarp::sig::Vector& operator+=(yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Addition operator between matrices, returns a+b (defined in Math.h).
*/
YARP_math_API yarp::sig::Matrix operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Addition operator between matrices, returns a+b (defined in Math.h).
*/
YARP_math_API yarp::sig::Matrix& operator+=(yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Subtraction operator between a vector and a scalar (defined in Math.h).
* Subtract the scalar to all the elements of the vector.
*/
YARP_math_API yarp::sig::Vector operator-(const yarp::sig::Vector &a, const double &s);

/**
* Subtraction operator between a scalar and a vector (defined in Math.h).
* Multiply the vector by -1 and sum the scalar to the result.
*/
YARP_math_API yarp::sig::Vector operator-(const double &s, const yarp::sig::Vector &a);

/**
* Subtraction operator between a vector and a scalar (defined in Math.h).
* Subtract the scalar to all the elements of the vector.
*/
YARP_math_API yarp::sig::Vector& operator-=(yarp::sig::Vector &a, const double &s);

/**
* Subtraction operator between vectors, returns a-b (defined in Math.h).
*/
YARP_math_API yarp::sig::Vector operator-(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Subtraction operator between vectors, returns a-b (defined in Math.h).
*/
YARP_math_API yarp::sig::Vector& operator-=(yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Subtraction operator between matrices, returns a-b (defined in Math.h).
*/
YARP_math_API yarp::sig::Matrix operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Subtraction operator between matrices, returns a-b (defined in Math.h).
*/
YARP_math_API yarp::sig::Matrix& operator-=(yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Scalar-vector product operator (defined in Math.h).
* @param k a scalar
* @param b vector
* @return k*b
*/
YARP_math_API yarp::sig::Vector operator*(double k, const yarp::sig::Vector &b);

/**
* Vector-scalar product operator (defined in Math.h).
* @param b a vector
* @param k a scalar
* @return b*k
*/
YARP_math_API yarp::sig::Vector operator*(const yarp::sig::Vector &b, double k);

/**
* Vector-scalar product operator (defined in Math.h).
* @param b a vector
* @param k a scalar
* @return b*k
*/
YARP_math_API yarp::sig::Vector& operator*=(yarp::sig::Vector &b, double k);

/**
* Vector-matrix product operator (defined in Math.h).
* @param a is a vector (interpreted as a row)
* @param m is a matrix
* @return a*m
*/
YARP_math_API yarp::sig::Vector operator*(const yarp::sig::Vector &a,
    const yarp::sig::Matrix &m);

/**
* Vector-matrix product operator (defined in Math.h).
* @param a is a vector (interpreted as a row)
* @param m is a matrix
* @return a*m
*/
YARP_math_API yarp::sig::Vector& operator*=(yarp::sig::Vector &a, const yarp::sig::Matrix &m);

/**
* Matrix-vector product operator (defined in Math.h).
* @param a is a vector (interpreted as a column)
* @param m is a matrix
* @return m*a
*/
 YARP_math_API yarp::sig::Vector operator*(const yarp::sig::Matrix &m,
    const yarp::sig::Vector &a);

/**
* Matrix-matrix product operator (defined in Math.h).
* @param a a matrix
* @param b a matrix
* @return a*b
*/
YARP_math_API yarp::sig::Matrix operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Matrix-matrix product operator (defined in Math.h).
* @param a a matrix
* @param b a matrix
* @return a*b
*/
YARP_math_API yarp::sig::Matrix& operator*=(yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

/**
* Scalar-matrix product operator (defined in Math.h).
* @param k a scalar
* @param M a matrix
* @return k*M
*/
YARP_math_API yarp::sig::Matrix operator*(const double k, const yarp::sig::Matrix &M);

/**
* Matrix-scalar product operator (defined in Math.h).
* @param M a matrix
* @param k a scalar
* @return M*k
*/
YARP_math_API yarp::sig::Matrix operator*(const yarp::sig::Matrix &M, const double k);

/**
* Matrix-scalar product operator (defined in Math.h).
* @param M a matrix
* @param k a scalar
* @return M*k
*/
YARP_math_API yarp::sig::Matrix& operator*=(yarp::sig::Matrix &M, const double k);

/**
* Vector-vector element-wise product operator (defined in Math.h).
* @param a a vector
* @param b a vector
* @return a.*b (matlab notation)
*/
YARP_math_API yarp::sig::Vector operator*(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Vector-vector element-wise product operator (defined in Math.h).
* @param a a vector
* @param b a vector
* @return a.*b (matlab notation)
*/
YARP_math_API yarp::sig::Vector& operator*=(yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Quaternion-quaternion hamilton product operator (defined in Math.h).
* reference: "Stevens, Brian L., Frank L. Lewis, Aircraft Control and Simulation, Wiley–Interscience, 2nd Edition".
* @param a a quaternion
* @param b a quaternion
* @return a*b
*/
YARP_math_API yarp::math::Quaternion operator*(const yarp::math::Quaternion& a, const yarp::math::Quaternion& b);

/**
* Vector-vector element-wise division operator (defined in Math.h).
* @param a a vector
* @param b a vector
* @return a./b (matlab notation)
*/
YARP_math_API yarp::sig::Vector operator/(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Vector-vector element-wise division operator (defined in Math.h).
* @param a a vector
* @param b a vector
* @return a./b (matlab notation)
*/
YARP_math_API yarp::sig::Vector& operator/=(yarp::sig::Vector &a, const yarp::sig::Vector &b);

/**
* Vector-scalar division operator (defined in Math.h).
* @param b a vector
* @param k a scalar
* @return b/k
*/
YARP_math_API yarp::sig::Vector operator/(const yarp::sig::Vector &b, double k);

/**
* Vector-scalar division operator (defined in Math.h).
* @param b a vector
* @param k a scalar
* @return b/k
*/
YARP_math_API yarp::sig::Vector& operator/=(yarp::sig::Vector &b, double k);

/**
* Matrix-scalar division operator (defined in Math.h).
* @param M a matrix
* @param k a scalar
* @return M./k (matlab notation)
*/
YARP_math_API yarp::sig::Matrix operator/(const yarp::sig::Matrix &M, double k);

/**
* Matrix-scalar division operator (defined in Math.h).
* @param M a matrix
* @param k a scalar
* @return M./k (matlab notation)
*/
YARP_math_API yarp::sig::Matrix& operator/=(yarp::sig::Matrix &M, double k);


namespace yarp
{
    namespace math
    {
        /**
         * Matrix-Matrix concatenation by column (defined in Math.h).
         * @param m1 a matrix nXm
         * @param m2 a matrix rXm
         * @return a matrix (n+r)Xm
         */
        YARP_math_API yarp::sig::Matrix pile(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2);

        /**
         * Vector-Matrix concatenation (defined in Math.h). Add a row at the beginning of a matrix.
         * @param m a matrix nXm
         * @param v a vector m
         * @return a matrix (n+1)Xm
         */
        YARP_math_API yarp::sig::Matrix pile(const yarp::sig::Vector &v, const yarp::sig::Matrix &m);

        /**
         * Matrix-Vector concatenation (defined in Math.h). Add a row at the end of a matrix.
         * @param m a matrix nXm
         * @param v a vector m
         * @return a matrix (n+1)Xm
         */
        YARP_math_API yarp::sig::Matrix pile(const yarp::sig::Matrix &m, const yarp::sig::Vector &v);

        /**
         * Vector-Vector concatenation (defined in Math.h). Create a two row matrix by stacking two vectors.
         * @param v1 an n-vector
         * @param v2 an n-vector
         * @return a matrix 2Xn
         */
        YARP_math_API yarp::sig::Matrix pile(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2);

        /**
         * Matrix-Matrix concatenation by row (defined in Math.h).
         * @param m1 a matrix nXm
         * @param m2 a matrix nXr
         * @return a matrix nX(m+r)
         */
        YARP_math_API yarp::sig::Matrix cat(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2);

        /**
         * Matrix-Vector concatenation (defined in Math.h). Add a column at the end of a matrix.
         * @param m a matrix nXm
         * @param v a vector n
         * @return a matrix nX(m+1)
         */
        YARP_math_API yarp::sig::Matrix cat(const yarp::sig::Matrix &m, const yarp::sig::Vector &v);

        /**
         * Vector-Matrix concatenation (defined in Math.h). Add a column at the beginning of a matrix.
         * @param m a matrix nXm
         * @param v a vector n
         * @return a matrix nX(m+1)
         */
        YARP_math_API yarp::sig::Matrix cat(const yarp::sig::Vector &v, const yarp::sig::Matrix &m);

        /**
         * Vector-Vector concatenation (defined in Math.h). Create a vector by putting two vectors side by side.
         * @param v1 an n-vector
         * @param v2 an m-vector
         * @return a (n+m)-vector
         */
        YARP_math_API yarp::sig::Vector cat(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2);

        /**
         * Vector-scalar concatenation (defined in Math.h). Create a vector by putting the scalar at the end of the vector.
         * @param v an n-vector
         * @param s a scalar
         * @return a (n+1)-vector
         */
        YARP_math_API yarp::sig::Vector cat(const yarp::sig::Vector &v, double s);

        /**
         * Scalar-vector concatenation (defined in Math.h). Create a vector by putting the scalar at the beginning of the vector.
         * @param s a scalar
         * @param v an n-vector
         * @return a (n+1)-vector
         */
        YARP_math_API yarp::sig::Vector cat(double s, const yarp::sig::Vector &v);

        /**
         * Scalar-scalar concatenation (defined in Math.h). Create a vector containing the two specified scalar values.
         * @param s1 a scalar
         * @param s2 a scalar
         * @return a 2-vector
         */
        YARP_math_API yarp::sig::Vector cat(double s1, double s2);

        YARP_math_API yarp::sig::Vector cat(double s1, double s2, double s3);

        YARP_math_API yarp::sig::Vector cat(double s1, double s2, double s3, double s4);

        YARP_math_API yarp::sig::Vector cat(double s1, double s2, double s3, double s4, double s5);

        /**
        * Scalar product between vectors (defined in Math.h).
        * @return a^T*b, where a and b are column vectors
        */
        YARP_math_API double dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Outer product between vectors (defined in Math.h).
        * @return a*b^T, where a and b are column vectors
        */
        YARP_math_API yarp::sig::Matrix outerProduct(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Compute the cross product between two vectors (defined in Math.h).
        * @param a first input vector
        * @param b second input vector
        * @return axb
        */
        YARP_math_API yarp::sig::Vector cross(const yarp::sig::Vector &a, const yarp::sig::Vector &b);

        /**
        * Compute the cross product matrix, that is a 3-by-3 skew-symmetric matrix (defined in Math.h).
        * @param v the vector
        * @return the cross product matrix
        */
        YARP_math_API yarp::sig::Matrix crossProductMatrix(const yarp::sig::Vector &v);

        /**
        * Compute the cross product matrix, that is a 3-by-3 skew-symmetric matrix (defined in Math.h).
        * @param v the vector
        * @param res the cross product matrix
        * @return true if operation succeeded, false otherwise
        */
        YARP_math_API bool crossProductMatrix(const yarp::sig::Vector &v, yarp::sig::Matrix &res);

        /**
        * Returns the Euclidean norm of the vector (defined in Math.h).
        * @param v is the input vector.
        * @return ||v||.
        */
        YARP_math_API double norm(const yarp::sig::Vector &v);

        /**
        * Returns the Euclidean squared norm of the vector (defined in Math.h).
        * @param v is the input vector.
        * @return ||v||^2.
        */
        YARP_math_API double norm2(const yarp::sig::Vector &v);

        /**
        * Returns the maximum of the elements of a real vector (defined in Math.h).
        * @param v is the input vector.
        * @return max(v).
        */
        YARP_math_API double findMax(const yarp::sig::Vector &v);

        /**
        * Returns the minimum of the elements of a real vector (defined in Math.h).
        * @param v is the input vector.
        * @return min(v).
        */
        YARP_math_API double findMin(const yarp::sig::Vector &v);

        /**
        * Creates a vector of zeros (defined in Math.h).
        * @param s the size of the new vector
        * @return a copy of the new vector
        */
        YARP_math_API yarp::sig::Vector zeros(int s);

        /**
        * Creates a vector of ones (defined in Math.h).
        * @param s the size of the new vector
        * @return a copy of the new vector
        */
        YARP_math_API yarp::sig::Vector ones(int s);

        /**
        * Build an identity matrix (defined in Math.h).
        * @param r number of rows
        * @param c number of columns
        * @return the new matrix
        */
        YARP_math_API yarp::sig::Matrix eye(int r, int c);

        /**
        * Build a square identity matrix (defined in Math.h).
        * @param n number of rows and columns
        * @return the new matrix
        */
        YARP_math_API yarp::sig::Matrix eye(int n);

        /**
        * Build a matrix of zeros (defined in Math.h).
        * @param r number of rows
        * @param c number of columns
        */
        YARP_math_API yarp::sig::Matrix zeros(int r, int c);

        /**
        * Computes the determinant of a matrix (defined in Math.h).
        * @param in the matrix
        */
        YARP_math_API double det(const yarp::sig::Matrix& in);

        /**
        * Invert a square matrix using LU-decomposition (defined in Math.h).
        * @param in square matrix
        * @return the inverse of the matrix
        */
        YARP_math_API yarp::sig::Matrix luinv(const yarp::sig::Matrix& in);

        /**
        * Computes eigenvalues of the n-by-n real nonsymmetric matrix (defined in Math.h).
        * @param in nonsymmetric n-by-n matrix
        * @param real the real part of eigen values
        * @param img the imaginary part of eigen values
        * @return the real and imaginary part of the eigen values in separate matrices
        */
        YARP_math_API bool eigenValues(const yarp::sig::Matrix& in, yarp::sig::Vector &real, yarp::sig::Vector &img);

        /**
        * Invert a symmetric and positive definite matrix using Cholesky decomposition (defined in Math.h).
        * @param in symmetric and positive definite matrix
        * @return the inverse of the matrix
        */
        //YARP_math_API yarp::sig::Matrix chinv(const yarp::sig::Matrix& in);

        /**
        * Returns the sign of a real number: 1 if positive,
        * -1 if negative, 0 if equal to zero (defined in Math.h)
        * @param v is a real number.
        * @return sign(v).
        */
        YARP_math_API double sign(const double &v);

        /**
        * Returns the sign vector of a real vector, that is a vector
        * with 1 if the value is positive,
        * -1 if negative, 0 if equal to zero (defined in Math.h).
        * @param v is the input vector.
        * @return sign(v).
        */
        YARP_math_API yarp::sig::Vector sign(const yarp::sig::Vector &v);

        /**
        * Converts a dcm (direction cosine matrix) rotation matrix R to
        * axis/angle representation (defined in Math.h).
        * @param R is the input matrix.
        * @return 4 by 1 vector for the axis/angle representation.
        */
        YARP_math_API yarp::sig::Vector dcm2axis(const yarp::sig::Matrix &R);

        /**
        * Returns a dcm (direction cosine matrix) rotation matrix R from
        * axis/angle representation (defined in Math.h).
        * @param v is the axis/angle vector.
        * @return 4 by 4 homogeneous matrix with the rotation components
        *         in the top left 3 by 3 submatrix.
        */
        YARP_math_API yarp::sig::Matrix axis2dcm(const yarp::sig::Vector &v);

        /**
        * Converts a dcm (direction cosine matrix) rotation matrix to
        * euler angles (ZYZ) (defined in Math.h). Three angles are returned in a vector with
        * the following format:
        * \f[  \mathbf{v} = [\alpha, \beta, \gamma ]\f]
        * such that the returned matrix satisfies the following:
        * \f[  R = R_z(\alpha) R_y(\beta) R_z(\gamma) \f]
        * @param R is the input ZYZ rotation matrix.
        * @return 3 by 1 vector for the Euler angles representation.
        */
        YARP_math_API yarp::sig::Vector dcm2euler(const yarp::sig::Matrix &R);

        /**
        * Converts euler angles (ZYZ) vector in the corresponding dcm
        * (direction cosine matrix) rotation matrix (defined in Math.h). The three euler
        * angles are specified in a vector with the following structure:
        * \f[  \mathbf{v} = [\alpha, \beta, \gamma ]\f]
        * and the returned matrix is:
        * \f[  R = R_z(\alpha) R_y(\beta) R_z(\gamma) \f]
        * @param euler is the input vector (alpha=z-rotation,
        *              beta=y-rotation, gamma=z-rotation).
        * @return 4 by 4 homogeneous matrix representing the ZYZ
        *         rotation with the rotation components in the top left
        *         3 by 3 submatrix.
        */
        YARP_math_API yarp::sig::Matrix euler2dcm(const yarp::sig::Vector &euler);

        /**
        * Converts a dcm (direction cosine matrix) rotation matrix to
        * roll-pitch-yaw angles (defined in Math.h). Three angles are returned in a vector
        * with the following format:
        * \f[  \mathbf{v} = [\psi, \theta, \phi ]\f]
        * such that the returned matrix satisfies the following:
        * \f[  R = R_z(\phi) R_y(\theta) R_x(\psi) \f]
        * @param R is the input ZYX rotation matrix.
        * @return 3 by 1 vector for the roll pitch-yaw-angles representation.
        */
        YARP_math_API yarp::sig::Vector dcm2rpy(const yarp::sig::Matrix &R);

        /**
        * Converts roll-pitch-yaw angles in the corresponding dcm
        * (direction cosine matrix) rotation matrix (defined in Math.h). The three angles
        * are specified in a vector with the following structure:
        * \f[  \mathbf{v} = [\psi, \theta, \phi ]\f]
        * and the returned matrix is:
        * \f[  R = R_z(\phi) R_y(\theta) R_x(\psi) \f]
        * @param rpy is the input vector (\psi=roll
        *            x-rotation,\theta=pitch y-rotation, \phi=yaw
        *            z-rotation).
        * @return 4 by 4 homogeneous matrix representing the ZYX
        *         rotation with the rotation components in the top left
        *         3 by 3 submatrix.
        */
        YARP_math_API yarp::sig::Matrix rpy2dcm(const yarp::sig::Vector &rpy);

        /**
        * Converts a dcm (direction cosine matrix) rotation matrix to
        * yaw-roll-pitch angles (defined in Math.h). Three angles are returned in a vector
        * with the following format:
        * \f[  \mathbf{v} = [\phi, \theta, \psi ]\f]
        * such that the returned matrix satisfies the following:
        * \f[  R = R_x(\psi) R_y(\theta) R_z(\phi) \f]
        * @param R is the input XYZ rotation matrix.
        * @return 3 by 1 vector for the yaw-pitch-roll angles representation.
        */
        YARP_math_API yarp::sig::Vector dcm2ypr(const yarp::sig::Matrix &R);

        /**
        * Converts yaw-pitch-roll angles in the corresponding dcm
        * (direction cosine matrix) rotation matrix (defined in Math.h). The three angles
        * are specified in a vector with the following structure:
        * \f[  \mathbf{v} = [\phi, \theta, \psi ]\f]
        * and the returned matrix is:
        * \f[  R = R_x(\psi) R_y(\theta) R_z(\phi) \f]
        * @param ypr is the input vector (\phi=yaw
        *            z-rotation, \theta=pitch y-rotation, \psi=roll
        *            x-rotation).
        * @return 4 by 4 homogeneous matrix representing the XYZ
        *         rotation with the rotation components in the top left
        *         3 by 3 submatrix.
        */
        YARP_math_API yarp::sig::Matrix ypr2dcm(const yarp::sig::Vector &ypr);

        /**
        * Returns the inverse of a 4 by 4 rototranslational matrix (defined in Math.h).
        * @param H is the 4 by 4 rototranslational matrix.
        * @return inverse of 4 by 4 rototranslational matrix.
        *
        * @note about 5 times faster than pinv()
        */
        YARP_math_API yarp::sig::Matrix SE3inv(const yarp::sig::Matrix &H);

        /**
        * Returns the adjoint matrix of a given roto-translational
        * matrix (defined in Math.h). The adjoint is a (6x6) matrix: [R , S(r)*R; 0, R]
        * where R is the rotational part of H, and r the translational part.
        * @param H is the 4 by 4 rototranslational matrix.
        * @return the adjoint matrix
        */
        YARP_math_API yarp::sig::Matrix adjoint(const yarp::sig::Matrix &H);

        /**
        * Returns the inverse of the adjoint matrix of a given
        * roto-translational matrix (defined in Math.h). The inverse of an adjoint is a
        * (6x6) matrix: [R^T , -S(R^T*r)*R^T; 0 , R^T] where R is the
        * rotational part of H, and r the translational part.
        * @param H is the 4 by 4 rototranslational matrix.
        * @return the inverse of the adjoint matrix
        */
        YARP_math_API yarp::sig::Matrix adjointInv(const yarp::sig::Matrix &H);
    }
}

#endif // YARP_MATH_MATH_H
