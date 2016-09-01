/*
* Author: Lorenzo Natale, Ugo Pattacini
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Log.h>
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>

#include <yarp/eigen/Eigen.h>

#include <cassert>

using namespace yarp::eigen;
using namespace yarp::sig;


Vector yarp::math::operator*(const Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector ret((size_t)m.cols());

    assert(false);

    return ret;
}

Vector& yarp::math::operator*=(Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector a2(a);
    a.resize(m.cols());

    assert(false);

    return a;
}

Vector yarp::math::operator*(const Matrix &m, const Vector &a)
{
    yAssert((size_t)m.cols()==a.size());
    Vector ret((size_t)m.rows());
    ret=0.0;

    assert(false);

    return ret;
}

Matrix yarp::math::operator*(const Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix c(a.rows(), b.cols());

    assert(false);

    return c;
}

Matrix& yarp::math::operator*=(Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix a2(a);   // a copy of a
    a.resize(a.rows(), b.cols());

    assert(false);

    return a;
}

Matrix yarp::math::pile(const Matrix &m1, const Matrix &m2)
{
    int c = m1.cols();
    yAssert(c==m2.cols());
    int r1 = m1.rows();
    int r2 = m2.rows();
    Matrix res(r1+r2, c);

    assert(false);

    return res;
}

Matrix yarp::math::pile(const Matrix &m, const Vector &v)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    assert(false);

    return res;
}

Matrix yarp::math::pile(const Vector &v, const Matrix &m)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    assert(false);

    return res;
}

Matrix yarp::math::pile(const Vector &v1, const Vector &v2)
{
    size_t n = v1.size();
    yAssert(n==v2.size());
    Matrix res(2, (int)n);

    assert(false);

    return res;
}

Matrix yarp::math::cat(const Matrix &m1, const Matrix &m2)
{
    int r = m1.rows();
    yAssert(r==m2.rows());
    int c1 = m1.cols();
    int c2 = m2.cols();
    Matrix res(r, c1+c2);

    assert(false);

    return res;
}

Matrix yarp::math::cat(const Matrix &m, const Vector &v)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    assert(false);

    return res;
}

Matrix yarp::math::cat(const Vector &v, const Matrix &m)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    assert(false);

    return res;
}

Vector yarp::math::cat(const Vector &v1, const Vector &v2)
{
    int n1 = v1.size();
    int n2 = v2.size();
    Vector res(n1+n2);

    assert(false);

    return res;
}

Vector yarp::math::cat(const Vector &v, double s)
{
    int n = v.size();
    Vector res(n+1);

    assert(false);

    return res;
}

Vector yarp::math::cat(double s, const Vector &v)
{
    int n = v.size();
    Vector res(n+1);

    assert(false);

    return res;
}

double yarp::math::dot(const Vector &a, const Vector &b)
{
    yAssert(a.size()==b.size());

    assert(false);

    return 0.0;
}

double yarp::math::norm(const Vector &v)
{
    assert(false);

    return 0.0;
}

bool yarp::math::eigenValues(const Matrix& in, Vector &real, Vector &img)
{
    // return error for non-square matrix
    if(in.cols() != in.rows())
        return false;

    real.clear();
    img.clear();

    assert(false);

    return true;
}


