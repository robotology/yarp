/*
* Author: Lorenzo Natale, Ugo Pattacini
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Log.h>
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_linalg.h>

#include <gsl/gsl_version.h>
#include <gsl/gsl_eigen.h>

#include <yarp/gsl/Gsl.h>

using namespace yarp::gsl;
using namespace yarp::sig;

gsl_vector_view getView(const Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(const_cast<double *>(v.data()), v.size());
    return ret;
}

gsl_vector_view getView(Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(v.data(),
        v.size());
    return ret;
}


Vector yarp::math::operator*(const Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector ret((size_t)m.cols());

    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *) GslMatrix(m).getGslMatrix(),
        (const gsl_vector *) GslVector(a).getGslVector(), 0.0,
        (gsl_vector *)GslVector(ret).getGslVector());
    return ret;
}

Vector& yarp::math::operator*=(Vector &a, const Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector a2(a);
    a.resize(m.cols());
    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *)GslMatrix(m).getGslMatrix(),
        (const gsl_vector *) GslVector(a2).getGslVector(), 0.0,
        (gsl_vector *)GslVector(a).getGslVector());
    return a;
}

Vector yarp::math::operator*(const Matrix &m, const Vector &a)
{
    yAssert((size_t)m.cols()==a.size());
    Vector ret((size_t)m.rows());
    ret=0.0;

    gsl_blas_dgemv(CblasNoTrans, 1.0, (const gsl_matrix *)GslMatrix(m).getGslMatrix(),
        (const gsl_vector *)GslVector(a).getGslVector(), 0.0,
        (gsl_vector *)GslVector(ret).getGslVector());

    return ret;
}

Matrix yarp::math::operator*(const Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix c(a.rows(), b.cols());
    cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans,
        c.rows(), c.cols(), a.cols(),
        1.0, a.data(), a.cols(), b.data(), b.cols(), 0.0,
        c.data(), c.cols());

    return c;
}

Matrix& yarp::math::operator*=(Matrix &a, const Matrix &b)
{
    yAssert(a.cols()==b.rows());
    Matrix a2(a);   // a copy of a
    a.resize(a.rows(), b.cols());
    cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans,
        a2.rows(), b.cols(), a2.cols(),
        1.0, a2.data(), a2.cols(), b.data(), b.cols(), 0.0,
        a.data(), a.cols());
    return a;
}

Matrix yarp::math::pile(const Matrix &m1, const Matrix &m2)
{
    int c = m1.cols();
    yAssert(c==m2.cols());
    int r1 = m1.rows();
    int r2 = m2.rows();
    Matrix res(r1+r2, c);
    cblas_dcopy(r1*c, m1.data(), 1, res.data(), 1); // copy first r1 rows
    cblas_dcopy(r2*c, m2.data(), 1, res[r1], 1);    // copy last r2 rows
    return res;
}

Matrix yarp::math::pile(const Matrix &m, const Vector &v)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);
    cblas_dcopy(r*c, m.data(), 1, res.data(), 1);  // copy first r rows
    cblas_dcopy(c, v.data(), 1, res[r], 1);         // copy last row
    return res;
}

Matrix yarp::math::pile(const Vector &v, const Matrix &m)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);
    cblas_dcopy(c, v.data(), 1, res.data(), 1);         // copy first row
    cblas_dcopy(r*c, m.data(), 1, res[1], 1);           // copy last r rows
    return res;
}

Matrix yarp::math::pile(const Vector &v1, const Vector &v2)
{
    size_t n = v1.size();
    yAssert(n==v2.size());
    Matrix res(2, (int)n);
    cblas_dcopy(n, v1.data(), 1, res.data(), 1);         // copy first row
    cblas_dcopy(n, v2.data(), 1, res[1], 1);           // copy last r rows
    return res;
}

Matrix yarp::math::cat(const Matrix &m1, const Matrix &m2)
{
    int r = m1.rows();
    yAssert(r==m2.rows());
    int c1 = m1.cols();
    int c2 = m2.cols();
    Matrix res(r, c1+c2);
    for(int i=0;i<r;i++){
        cblas_dcopy(c1, m1[i], 1, res[i], 1);       // copy first c1 cols of i-th row
        cblas_dcopy(c2, m2[i], 1, res[i]+c1, 1);    // copy last c2 cols of i-th row
    }
    return res;
}

Matrix yarp::math::cat(const Matrix &m, const Vector &v)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);
    for(int i=0;i<r;i++){
        cblas_dcopy(c, m[i], 1, res[i], 1);     // copy first c cols of i-th row
        res(i,c) = v(i);                        // copy last element of i-th row
    }
    return res;
}

Matrix yarp::math::cat(const Vector &v, const Matrix &m)
{
    int r = m.rows();
    yAssert((size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);
    for(int i=0;i<r;i++){
        res(i,0) = v(i);                        // copy first element of i-th row
        cblas_dcopy(c, m[i], 1, res[i]+1, 1);   // copy last c cols of i-th row
    }
    return res;
}

Vector yarp::math::cat(const Vector &v1, const Vector &v2)
{
    int n1 = v1.size();
    int n2 = v2.size();
    Vector res(n1+n2);
    cblas_dcopy(n1, v1.data(), 1, res.data(), 1);       // copy first n1 elements
    cblas_dcopy(n2, v2.data(), 1, res.data()+n1, 1);    // copy last n2 elements
    return res;
}

Vector yarp::math::cat(const Vector &v, double s)
{
    int n = v.size();
    Vector res(n+1);
    cblas_dcopy(n, v.data(), 1, res.data(), 1);     // copy first n elements
    res(n) = s;                                       // copy last element
    return res;
}

Vector yarp::math::cat(double s, const Vector &v)
{
    int n = v.size();
    Vector res(n+1);
    res(0) = s;                                       // copy last element
    cblas_dcopy(n, v.data(), 1, res.data()+1, 1);     // copy first n elements
    return res;
}

double yarp::math::dot(const Vector &a, const Vector &b)
{
    yAssert(a.size()==b.size());
    return cblas_ddot(a.size(), a.data(),1, b.data(),1);
}

double yarp::math::norm(const Vector &v)
{
    return gsl_blas_dnrm2((const gsl_vector*) GslVector(v).getGslVector());
}

bool yarp::math::eigenValues(const Matrix& in, Vector &real, Vector &img)
{
    // return error for non-square matrix
    if(in.cols() != in.rows())
        return false;

    real.clear();
    img.clear();

#if (GSL_MAJOR_VERSION >= 2 || (GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= 14))
    size_t n = in.rows();
    gsl_vector_complex *eval = gsl_vector_complex_alloc(n);
    gsl_matrix_complex *evec = gsl_matrix_complex_alloc(n, n);
    gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc(n);
    gsl_eigen_nonsymmv ((gsl_matrix *) GslMatrix(in).getGslMatrix(), eval, evec, w);
    for(size_t i=0; i<n; i++)
    {
        gsl_complex eval_i = gsl_vector_complex_get(eval, i);
        real.push_back(GSL_REAL(eval_i));
        img.push_back(GSL_IMAG(eval_i));
    }
    gsl_eigen_nonsymmv_free(w);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);
    return true;

#else
    return false;
#endif
}


