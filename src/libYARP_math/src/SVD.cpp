/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

/**
* \infile routines for Singular Value Decomposition
*/

#include <yarp/math/SVD.h>
#include <gsl/gsl_linalg.h>
#include <yarp/math/Math.h>

#include <yarp/gsl/Gsl.h>

using namespace yarp::sig;
using namespace yarp::gsl;


/**
* Factorize the M-by-N matrix 'in' into the singular value decomposition in = U S V^T.
* The diagonal elements of the singular value matrix S are stored in the vector S.
* The singular values are non-negative and form a non-increasing sequence from S_1 to S_N. 
* The matrix V contains the elements of V in untransposed form. To form the product U S V^T it 
* is necessary to take the transpose of V. This routine uses the Golub-Reinsch SVD algorithm.
* Defining K as min(M, N) the the input matrices are:
* @param in input M-by-N matrix to decompose
* @param U output M-by-K orthogonal matrix
* @param S output K-dimensional vector containing the diagonal entries of the diagonal matrix S
* @param V output N-by-K orthogonal matrix
* @note The routine computes the \a thin version of the SVD. Mathematically, the \a full SVD is
*       defined with U and V as square orthogonal matrices and S as an M-by-N diagonal matrix.
*       If U, S, V do not have the expected sizes they are resized automatically.
*/

void yarp::math::SVD(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    int m=in.rows(), n=in.cols(), k = (m<n)?m:n;
    Vector work(k);
    if((int)S.size()!=k) S.resize(k);
    if(m>=n)
    {
        U = in;
        if(V.rows()!=n || V.cols()!=k) V.resize(n,k);
        gsl_linalg_SV_decomp((gsl_matrix *) GslMatrix(U).getGslMatrix(),
            (gsl_matrix *) GslMatrix(V).getGslMatrix(),
            (gsl_vector *) GslVector(S).getGslVector(),
            (gsl_vector *) GslVector(work).getGslVector());
        return;
    }
    // since in GSL svd is not implemented for fat matrices I have to compute the svd
    // of the transpose of 'in', and swap U and V
    if(U.rows()!=m || U.cols()!=k) U.resize(m,k);
    V = in.transposed();
    gsl_linalg_SV_decomp((gsl_matrix *)GslMatrix(V).getGslMatrix(),
        (gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector(),
        (gsl_vector *) GslVector(work).getGslVector());
}

/**
* Perform SVD decomposition on a MxN matrix (for M >= N).
* Modified Golub Reinsch method. Fast for M>>N.
*/
void yarp::math::SVDMod(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    U=in;
    Vector work;
    work.resize(U.cols());
    Matrix X(U.cols(), U.cols());
    gsl_linalg_SV_decomp_mod((gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_matrix *) GslMatrix(X).getGslMatrix(),
        (gsl_matrix *) GslMatrix(V).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector(),
        (gsl_vector *) GslVector(work).getGslVector());
}

/**
* Perform SVD decomposition on an M-by-N matrix (for M >= N) using the Jacobi method. The Jacobi method
* can compute singular values to higher relative accuracy than Golub-Reinsch algorithms.
*/
void yarp::math::SVDJacobi(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    U=in;
    gsl_linalg_SV_decomp_jacobi((gsl_matrix *) GslMatrix(U).getGslMatrix(),
        (gsl_matrix *) GslMatrix(V).getGslMatrix(),
        (gsl_vector *) GslVector(S).getGslVector());
}

/**
* Perform the moore-penrose pseudo-inverse of a matrix.
* @param in input matrix 
* @param tol singular values less than tol are set to zero
* @return pseudo-inverse of the matrix 'in'
*/
Matrix yarp::math::pinv(const Matrix &in, double tol)
{
    int m = in.rows(), n = in.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    Vector Sdiag(k);

    yarp::math::SVD(in, U, Sdiag, V);

    Matrix Spinv = zeros(k,k);
    for (int c=0;c<k; c++)
        if ( Sdiag(c)> tol)
            Spinv(c,c) = 1/Sdiag(c);
    return V*Spinv*U.transposed();
}

void yarp::math::pinv(const Matrix &in, Matrix &out, double tol)
{
    int m = in.rows(), n = in.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    Vector Sdiag(k);

    yarp::math::SVD(in, U, Sdiag, V);

    Matrix Spinv = zeros(k,k);
    for (int c=0;c<k; c++)
        if ( Sdiag(c)> tol)
            Spinv(c,c) = 1/Sdiag(c);
    out = V*Spinv*U.transposed();
}

Matrix yarp::math::pinv(const Matrix &in, Vector &sv, double tol)
{
    int m = in.rows(), n = in.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    if((int)sv.size()!=k)
        sv.resize(k);

    yarp::math::SVD(in, U, sv, V);

    Matrix Spinv = zeros(k,k);
    for (int c=0;c<k; c++)
        if ( sv(c)> tol)
            Spinv(c,c) = 1/sv(c);

    return V*Spinv*U.transposed();
}

void yarp::math::pinv(const Matrix &in, Matrix &out, Vector &sv, double tol)
{
    int m = in.rows(), n = in.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    if((int)sv.size()!=k)
        sv.resize(k);

    yarp::math::SVD(in, U, sv, V);

    Matrix Spinv = zeros(k,k);
    for (int c=0;c<k; c++)
        if ( sv(c)> tol)
            Spinv(c,c) = 1/sv(c);

    out = V*Spinv*U.transposed();
}

Matrix yarp::math::pinvDamped(const Matrix &in, Vector &sv, double damp)
{
    Matrix out(in.cols(), in.rows());
    pinvDamped(in, out, sv, damp);
    return out;
}

Matrix yarp::math::pinvDamped(const Matrix &in, double damp)
{
    int k = in.rows()<in.cols() ? in.rows() : in.cols();
    Vector sv(k);
    Matrix out(in.cols(), in.rows());
    pinvDamped(in, out, sv, damp);
    return out;
}

void yarp::math::pinvDamped(const Matrix &in, Matrix &out, double damp)
{
    int k = in.rows()<in.cols() ? in.rows() : in.cols();
    Vector sv(k);
    pinvDamped(in, out, sv, damp);
}

void yarp::math::pinvDamped(const Matrix &in, Matrix &out, Vector &sv, double damp)
{
    int m = in.rows(), n = in.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    if((int)sv.size()!=k)
        sv.resize(k);

    yarp::math::SVD(in, U, sv, V);

    Matrix Spinv = zeros(k,k);
    double damp2 = damp*damp;
    for (int c=0;c<k; c++)
        Spinv(c,c) = sv(c) / (sv(c)*sv(c) + damp2);

    out = V*Spinv*U.transposed();
}

Matrix yarp::math::projectionMatrix(const Matrix &A, double tol)
{
    Matrix out(A.rows(),A.rows());
    projectionMatrix(A, out, tol);
    return out;
}

void yarp::math::projectionMatrix(const Matrix &A, Matrix &out, double tol)
{
    int m = A.rows(), n = A.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    Vector Sdiag(k);
    yarp::math::SVD(A, U, Sdiag, V);
    Matrix UT = U.transposed();
    for(int c=0;c<m; c++)
        if(Sdiag(c) <= tol)
            UT.setRow(c, zeros(m));
    out = U*UT;
}

Matrix yarp::math::nullspaceProjection(const Matrix &A, double tol)
{
    Matrix out(A.cols(),A.cols());
    nullspaceProjection(A, out, tol);
    return out;
}

void yarp::math::nullspaceProjection(const Matrix &A, Matrix &out, double tol)
{
    int m = A.rows(), n = A.cols(), k = m<n?m:n;
    Matrix U(m,k), V(n,k);
    Vector Sdiag(k);
    yarp::math::SVD(A, U, Sdiag, V);
    Matrix VT = V.transposed();
    for (int c=0;c<n; c++)
        if ( Sdiag(c)<= tol)
            VT.setRow(c, zeros(n));
    out = eye(n) - V*VT;
}
