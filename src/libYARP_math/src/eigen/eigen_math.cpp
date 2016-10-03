/*
* Author: Lorenzo Natale, Ugo Pattacini
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Log.h>
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>

#include <yarp/eigen/Eigen.h>

// For eigenvalues
#include <Eigen/Eigenvalues>

#include <cassert>

using namespace yarp::eigen;
using namespace yarp::sig;


Vector yarp::math::operator*(const Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector ret((size_t)m.cols());

    toEigen(ret) = toEigen(m).transpose()*toEigen(a);

    return ret;
}

Vector& yarp::math::operator*=(Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector a2(a);
    a.resize(m.cols());

    toEigen(a) = toEigen(m).transpose()*toEigen(a2);

    return a;
}

Vector yarp::math::operator*(const Matrix &m, const Vector &a)
{
    yAssert((size_t)m.cols()==a.size());
    Vector ret((size_t)m.rows(),0.0);

    toEigen(ret) = toEigen(m)*toEigen(a);

    return ret;
}

Matrix yarp::math::operator*(const Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix c(a.rows(), b.cols());

    toEigen(c) = toEigen(a)*toEigen(b);

    return c;
}

Matrix& yarp::math::operator*=(Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix a2(a);   // a copy of a
    a.resize(a.rows(), b.cols());

    toEigen(a) = toEigen(a2)*toEigen(b);

    return a;
}

Matrix yarp::math::pile(const Matrix &m1, const Matrix &m2)
{
    int c = m1.cols();
    yAssert(c==m2.cols());
    int r1 = m1.rows();
    int r2 = m2.rows();
    Matrix res(r1+r2, c);

    toEigen(res).block(0,0,r1,c) = toEigen(m1);
    toEigen(res).block(r1,0,r2,c) = toEigen(m2);

    return res;
}

Matrix yarp::math::pile(const Matrix &m, const Vector &v)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    toEigen(res).block(0,0,r,c) = toEigen(m);
    toEigen(res).block(r,0,1,c) = toEigen(v).transpose();

    return res;
}

Matrix yarp::math::pile(const Vector &v, const Matrix &m)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    toEigen(res).block(0,0,1,c) = toEigen(v).transpose();
    toEigen(res).block(1,0,r,c) = toEigen(m);

    return res;
}

Matrix yarp::math::pile(const Vector &v1, const Vector &v2)
{
    size_t n = v1.size();
    yAssert(n==v2.size());
    Matrix res(2, (int)n);

    toEigen(res).block(0,0,1,n) = toEigen(v1);
    toEigen(res).block(1,0,1,n) = toEigen(v2);

    return res;
}

Matrix yarp::math::cat(const Matrix &m1, const Matrix &m2)
{
    int r = m1.rows();
    yAssert(r==m2.rows());
    int c1 = m1.cols();
    int c2 = m2.cols();
    Matrix res(r, c1+c2);

    toEigen(res).block(0,0,r,c1)  = toEigen(m1);
    toEigen(res).block(0,c1,r,c2) = toEigen(m2);

    return res;
}

Matrix yarp::math::cat(const Matrix &m, const Vector &v)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    toEigen(res).block(0,0,r,c)  = toEigen(m);
    toEigen(res).block(0,c,r,1) = toEigen(v);

    return res;
}

Matrix yarp::math::cat(const Vector &v, const Matrix &m)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    toEigen(res).block(0,0,r,1) = toEigen(v);
    toEigen(res).block(0,1,r,c) = toEigen(m);

    return res;
}

Vector yarp::math::cat(const Vector &v1, const Vector &v2)
{
    int n1 = v1.size();
    int n2 = v2.size();
    Vector res(n1+n2);

    toEigen(res).segment(0,n1) = toEigen(v1);
    toEigen(res).segment(n1,n2) = toEigen(v2);

    return res;
}

Vector yarp::math::cat(const Vector &v, double s)
{
    int n = v.size();
    Vector res(n+1);

    toEigen(res).segment(0,n) = toEigen(v);
    res(n) = s;

    return res;
}

Vector yarp::math::cat(double s, const Vector &v)
{
    int n = v.size();
    Vector res(n+1);

    res(0) = s;
    toEigen(res).segment(1,n) = toEigen(v);

    return res;
}

double yarp::math::dot(const Vector &a, const Vector &b)
{
    yAssert(a.size()==b.size());

    return toEigen(a).dot(toEigen(b));
}

double yarp::math::norm(const Vector &v)
{
    return toEigen(v).norm();
}

double yarp::math::det(const Matrix& in)
{
    return toEigen(in).determinant();
}

Matrix yarp::math::luinv(const Matrix& in)
{
    int m = in.rows();
    int n = in.cols();

    Matrix ret(m, n);

    toEigen(ret) = toEigen(in).inverse();

    return ret;
}

bool yarp::math::eigenValues(const Matrix& in, Vector &real, Vector &img)
{
    // return error for non-square matrix
    if(in.cols() != in.rows())
        return false;

    int n = in.cols();
    real.resize(n);
    img.resize(n);

    // Get eigenvalues
    Eigen::EigenSolver< Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > es(toEigen(in));

    for(int i=0; i < n; i++)
    {
        std::complex<double> lambda = es.eigenvalues()[i];
        real(i) = lambda.real();
        img(i)  = lambda.imag();
    }

    return true;
}


