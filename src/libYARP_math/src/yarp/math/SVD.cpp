/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* \infile routines for Singular Value Decomposition
*/

#include <yarp/math/SVD.h>
#include <yarp/math/Math.h>

#include <yarp/eigen/Eigen.h>

#include <Eigen/SVD>


using namespace yarp::sig;
using namespace yarp::eigen;

void yarp::math::SVD(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    SVDJacobi(in,U,S,V);
}

void yarp::math::SVDMod(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    SVDJacobi(in,U,S,V);
}

void yarp::math::SVDJacobi(const Matrix &in, Matrix &U, Vector &S, Matrix &V)
{
    Eigen::JacobiSVD< Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > svd(toEigen(in), Eigen::ComputeThinU | Eigen::ComputeThinV);

    U.resize(svd.matrixU().rows(),svd.matrixU().cols());
    toEigen(U) = svd.matrixU();

    S.resize(svd.singularValues().size());
    toEigen(S) = svd.singularValues();

    V.resize(svd.matrixV().rows(),svd.matrixV().cols());
    toEigen(V) = svd.matrixV();
}

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
    int m = A.rows();
    int n = A.cols();
    int k = std::min(m, n);
    Matrix U(m, k);
    Matrix V(n, k);
    Vector Sdiag(k);
    yarp::math::SVD(A, U, Sdiag, V);
    Matrix UT = U.transposed();
    for(int c = 0; c < k; c++) {
        if(Sdiag(c) <= tol) {
            UT.setRow(c, zeros(m));
        }
    }
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
    int m = A.rows();
    int n = A.cols();
    int k = std::min(m, n);
    Matrix U(m,k);
    Matrix V(n,k);
    Vector Sdiag(k);
    yarp::math::SVD(A, U, Sdiag, V);
    Matrix VT = V.transposed();
    for (int c = 0; c < k; c++) {
        if (Sdiag(c) <= tol) {
            VT.setRow(c, zeros(n));
        }
    }
    out = eye(n) - V*VT;
}
