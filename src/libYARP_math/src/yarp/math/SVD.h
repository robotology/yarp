/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_SVD_H
#define YARP_MATH_SVD_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>

namespace yarp
{
    namespace math
    {
        /**
        * Factorize the M-by-N matrix 'in' into the singular value decomposition in = U S V^T (defined in SVD.h).
        * The diagonal elements of the singular value matrix S are stored in the vector S.
        * The singular values are non-negative and form a non-increasing sequence from S_1 to S_N.
        * The matrix V contains the elements of V in untransposed form. To form the product U S V^T it
        * is necessary to take the transpose of V.
        * Defining K as min(M, N) the the input matrices are:
        * @param in input M-by-N matrix to decompose
        * @param U output M-by-K orthogonal matrix
        * @param S output K-dimensional vector containing the diagonal entries of the diagonal matrix S
        * @param V output N-by-K orthogonal matrix
        *
        * @note If U, S, V do not have the expected sizes they are resized automatically.
        *
        * @note The routine computes the \a thin version of the SVD. Mathematically, the \a full SVD is
        *       defined with U and V as square orthogonal matrices and S as an M-by-N diagonal matrix.
        *
        * @note This function uses the Jacobi SVD algorithm.
        */
        void YARP_math_API SVD(const yarp::sig::Matrix &in,
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform SVD decomposition on a MxN matrix (for M >= N) (defined in SVD.h).
        *
        * @note If U, S, V do not have the expected sizes they are resized automatically.
        *
        * @note This function uses the Jacobi SVD algorithm.
        */
        void YARP_math_API SVDMod(const yarp::sig::Matrix &in,
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);
        /**
        * Perform SVD decomposition on a matrix using the Jacobi method (defined in SVD.h). The Jacobi method
        * can compute singular values to higher relative accuracy than Golub-Reinsch algorithms.
        *
        * @note If U, S, V do not have the expected sizes they are resized automatically.
        *
        * @note This function uses the Jacobi SVD algorithm.
        */
        void YARP_math_API SVDJacobi(const yarp::sig::Matrix &in,
            yarp::sig::Matrix &U,
            yarp::sig::Vector &S,
            yarp::sig::Matrix &V);

        /**
        * Perform the moore-penrose pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param tol singular values less than tol are set to zero
        * @return pseudo-inverse of the matrix 'in'
        */
        yarp::sig::Matrix YARP_math_API pinv(const yarp::sig::Matrix &in, double tol=0.0);

        /**
        * Perform the moore-penrose pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param out pseudo-inverse of the matrix 'in'
        * @param tol singular values less than tol are set to zero
        */
        void YARP_math_API pinv(const yarp::sig::Matrix &in, yarp::sig::Matrix &out, double tol=0.0);

        /**
        * Perform the moore-penrose pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param sv vector containing the singular values of the input matrix
        * @param tol singular values less than tol are set to zero
        * @return pseudo-inverse of the matrix 'in'
        */
        yarp::sig::Matrix YARP_math_API pinv(const yarp::sig::Matrix &in, yarp::sig::Vector &sv, double tol=0.0);

        /**
        * Perform the moore-penrose pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param out pseudo-inverse of the matrix 'in'
        * @param sv vector containing the singular values of the input matrix
        * @param tol singular values less than tol are set to zero
        */
        void YARP_math_API pinv(const yarp::sig::Matrix &in, yarp::sig::Matrix &out, yarp::sig::Vector &sv, double tol=0.0);

        /**
        * Perform the damped pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param sv vector containing the singular values of the input matrix
        * @param damp damping factor
        */
        yarp::sig::Matrix YARP_math_API pinvDamped(const yarp::sig::Matrix &in, yarp::sig::Vector &sv, double damp);

        /**
        * Perform the damped pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param damp damping factor
        */
        yarp::sig::Matrix YARP_math_API pinvDamped(const yarp::sig::Matrix &in, double damp);

        /**
        * Perform the damped pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param out damped pseudo-inverse of the matrix 'in'
        * @param damp damping factor
        */
        void YARP_math_API pinvDamped(const yarp::sig::Matrix &in, yarp::sig::Matrix &out, double damp);

        /**
        * Perform the damped pseudo-inverse of a matrix (defined in SVD.h).
        * @param in input matrix
        * @param out damped pseudo-inverse of the matrix 'in'
        * @param sv vector containing the singular values of the input matrix
        * @param damp damping factor
        */
        void YARP_math_API pinvDamped(const yarp::sig::Matrix &in, yarp::sig::Matrix &out, yarp::sig::Vector &sv, double damp);

        /**
        * Compute the projection matrix of A, that is defined as A times its pseudoinverse: A*pinv(A) (defined in SVD.h).
        * Multiplying this projection matrix times a vector projects the vector in the range of A.
        * @param A input matrix
        * @param tol singular values less than tol are set to zero
        * @return The projection matrix associated with the range of A
        */
        yarp::sig::Matrix YARP_math_API projectionMatrix(const yarp::sig::Matrix &A, double tol=0.0);

        /**
        * Compute the projection matrix of A, that is defined as A times its pseudoinverse: A*pinv(A) (defined in SVD.h).
        * Multiplying this projection matrix times a vector projects the vector in the range of A.
        * @param A input matrix
        * @param out the projection matrix associated with the range of A
        * @param tol singular values less than tol are set to zero
        */
        void YARP_math_API projectionMatrix(const yarp::sig::Matrix &A, yarp::sig::Matrix &out, double tol=0.0);

        /**
        * Compute the nullspace projection matrix of A, that is defined as the difference between the
        * identity matrix and the pseudoinverse of A times A: (I - pinv(A)*A) (defined in SVD.h).
        * Multiplying this null projection matrix times a vector projects the vector in the nullspace of A.
        * @param A input matrix
        * @param tol singular values less than tol are set to zero
        * @return The projection matrix associated with the nullspace of A
        */
        yarp::sig::Matrix YARP_math_API nullspaceProjection(const yarp::sig::Matrix &A, double tol=0.0);

        /**
        * Compute the nullspace projection matrix of A, that is defined as the difference between the
        * identity matrix and the pseudoinverse of A times A: (I - pinv(A)*A) (defined in SVD.h).
        * Multiplying this projection matrix times a vector projects the vector in the range of A.
        * @param A input matrix
        * @param out the projection matrix associated with the nullspace of A
        * @param tol singular values less than tol are set to zero
        */
        void YARP_math_API nullspaceProjection(const yarp::sig::Matrix &A, yarp::sig::Matrix &out, double tol=0.0);

    }
}

#endif // YARP_MATH_SVD_H
