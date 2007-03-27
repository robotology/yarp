// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/math/Math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_cblas.h>

gsl_vector_view getView(const yarp::sig::Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(const_cast<double *>(v.data()), v.size());
    return ret;
}

gsl_vector_view getView(yarp::sig::Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(v.data(),
                                             v.size());
    return ret;
}

using namespace yarp::sig;

Vector yarp::math::operator+(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]+b[k];
    return ret;
}
 
Vector yarp::math::operator-(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]-b[k];
    return ret;
}

double yarp::math::dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b)
{
    double ret;
    ret=cblas_ddot(a.size(), a.data(),1, b.data(),1);
    return ret;
}

Vector yarp::math::operator*(const Vector &a, double k)
{
    int size=a.size();
    Vector ret(size);
    int i;
    for(i=0;i<size;i++)
        ret[i]=a[i]*k;

    return ret;
}

Vector yarp::math::operator*(double k, const Vector &b)
{
    return operator*(b,k);
}

Matrix yarp::math::operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    Matrix c(a.rows(), b.cols());
    cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                   c.rows(), c.cols(), a.cols(),
                   1.0, a.data(), a.cols(), b.data(), b.cols(), 0.0,
                   c.data(), c.cols());

    return c;
}

Matrix yarp::math::eye(int r, int c)
{
    Matrix ret;
    ret.resize(r,c);
    ret.eye();
    return ret;
}

Matrix yarp::math::zeros(int r, int c)
{
    Matrix ret;
    ret.resize(r,c);
    ret.zero();
    return ret;
}

Vector yarp::math::zeros(int s)
{
    Vector ret;
    ret.resize(s);
    ret.zero();
    return ret;
}
