// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#include <yarp/math/Math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_linalg.h>

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

Matrix yarp::math::operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    int n=a.cols();
	int m=a.rows();
    yarp::sig::Matrix ret(m,n);
    for (int r=0; r<m;r++)
		for (int c=0; c<n;c++)
			ret(r,c)=a(r,c)+b(r,c);
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

Matrix yarp::math::operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    int n=a.cols();
	int m=a.rows();
    yarp::sig::Matrix ret(m,n);
    for (int r=0; r<m;r++)
		for (int c=0; c<n;c++)
			ret(r,c)=a(r,c)-b(r,c);
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

Vector yarp::math::operator*(const yarp::sig::Vector &a, const yarp::sig::Matrix &m)
{
    // to be implemented
    Vector ret(m.cols());
    ret=0.0;

    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());
    return ret;
}

Vector yarp::math::operator*(const yarp::sig::Matrix &m, const yarp::sig::Vector &a)
{
    Vector ret(m.rows());
    ret=0.0;

    gsl_blas_dgemv(CblasNoTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());

    return ret;
}

Matrix yarp::math::luinv(const yarp::sig::Matrix& in) {
    int m = in.rows();
    int n = in.cols();
    int sign = 0;
    // assert m == n?

    Matrix LU(in);
    Matrix ret(m, n);
    gsl_permutation* permidx = gsl_permutation_calloc(m);

    gsl_linalg_LU_decomp((gsl_matrix *) LU.getGslMatrix(), permidx, &sign);
    gsl_linalg_LU_invert((gsl_matrix *) LU.getGslMatrix(), permidx, 
        (gsl_matrix *) ret.getGslMatrix());

    return ret;
}

/* depends on GSL 1.12, put back in when verified that 1.12 is standard
Matrix yarp::math::chinv(const yarp::sig::Matrix& in) {
    Matrix ret(in);

    gsl_linalg_cholesky_decomp((gsl_matrix *) ret.getGslMatrix());
    gsl_linalg_cholesky_invert((gsl_matrix *) ret.getGslMatrix());
        
    return ret;
}
*/
