// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/math/Math.h>
#include <yarp/os/Log.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_linalg.h>

#include <gsl/gsl_version.h>
#include <gsl/gsl_eigen.h>

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

Vector yarp::math::operator+(const yarp::sig::Vector &a, const double &s)
{
    Vector ret(a);
    return ret+=s;
}

Vector yarp::math::operator+(const double &s, const yarp::sig::Vector &a)
{
    return a+s;
}

Vector& yarp::math::operator+=(yarp::sig::Vector &a, const double &s)
{
    size_t l = a.size();
    for(size_t i=0; i<l; i++)
        a[i] += s;
    return a;
}

Vector yarp::math::operator+(const Vector &a, const Vector &b)
{
    Vector ret(a);
    return ret+=b;
}

Vector& yarp::math::operator+=(Vector &a, const Vector &b)
{
    size_t s=a.size();
    yAssert(s==b.size());
    for (size_t k=0; k<s;k++)
        a[k]+=b[k];
    return a;
}

Matrix yarp::math::operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    yarp::sig::Matrix ret(a);    
    return ret+=b;
}

Matrix& yarp::math::operator+=(Matrix &a, const Matrix &b)
{
    int n=a.cols();
    int m=a.rows();
    yAssert(m==b.rows() && n==b.cols());
    for (int r=0; r<m;r++)
        for (int c=0; c<n;c++)
            a(r,c)+=b(r,c);
    return a;
}

Vector yarp::math::operator-(const yarp::sig::Vector &a, const double &s)
{
    Vector ret(a);
    return ret-=s;
}

Vector yarp::math::operator-(const double &s, const yarp::sig::Vector &a)
{
    size_t l = a.size();
    Vector ret(l);
    for(size_t i=0; i<l; i++)
        ret[i] = s-a[i];
    return ret;
}

Vector& yarp::math::operator-=(yarp::sig::Vector &a, const double &s)
{
    size_t l = a.size();
    for(size_t i=0; i<l; i++)
        a[i] -= s;
    return a;
}

Vector yarp::math::operator-(const Vector &a, const Vector &b)
{
    yarp::sig::Vector ret(a);
    return ret-=b;
}

Vector& yarp::math::operator-=(Vector &a, const Vector &b)
{
    size_t s=a.size();
    yAssert(s==b.size());
    for (size_t k=0; k<s;k++)
        a[k]-=b[k];
    return a;
}

Matrix yarp::math::operator-(const Matrix &a, const Matrix &b)
{
    yarp::sig::Matrix ret(a);
    return ret-=b;
}

Matrix& yarp::math::operator-=(Matrix &a, const Matrix &b)
{
    int n=a.cols();
    int m=a.rows();
    yAssert(m==b.rows());
    yAssert(n==b.cols());
    for (int r=0; r<m;r++)
        for (int c=0; c<n;c++)
            a(r,c)-=b(r,c);
    return a;
}

Vector yarp::math::operator*(const Vector &a, double k)
{
    Vector ret(a);
    return ret*=k;
}

Vector& yarp::math::operator*=(Vector &a, double k)
{
    int size=a.size();
    for(int i=0;i<size;i++)
        a[i]*=k;
    return a;
}

Vector yarp::math::operator*(double k, const Vector &b)
{
    return operator*(b,k);
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

Matrix yarp::math::operator*(const double k, const Matrix &M)
{
    Matrix res(M);
    return res*=k;
}

Matrix yarp::math::operator*(const Matrix &M, const double k)
{
    return operator*(k,M);
}

Matrix& yarp::math::operator*=(Matrix &M, const double k)
{
    for (int r=0; r<M.rows(); r++)
        for (int c=0; c<M.cols(); c++)
            M(r,c)*=k;
    return M;
}

Vector yarp::math::operator*(const Vector &a, const Vector &b)
{
    Vector res(a);
    return res*=b;
}

Vector& yarp::math::operator*=(Vector &a, const Vector &b)
{
    size_t n =a.length();
    yAssert(n==b.length());
    for (size_t i=0; i<n; i++)
        a[i]*=b[i];
    return a;
}

Vector yarp::math::operator*(const yarp::sig::Vector &a, const yarp::sig::Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector ret((size_t)m.cols());

    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());
    return ret;
}

Vector& yarp::math::operator*=(yarp::sig::Vector &a, const yarp::sig::Matrix &m)
{
    yAssert(a.size()==(size_t)m.rows());
    Vector a2(a);
    a.resize(m.cols());
    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a2.getGslVector(), 0.0, 
        (gsl_vector *) a.getGslVector());
    return a;
}

Vector yarp::math::operator*(const yarp::sig::Matrix &m, const yarp::sig::Vector &a)
{
    yAssert((size_t)m.cols()==a.size());
    Vector ret((size_t)m.rows());
    ret=0.0;

    gsl_blas_dgemv(CblasNoTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());

    return ret;
}

Vector yarp::math::operator/(const Vector &a, const Vector &b)
{
    Vector res(a);
    return res/=b;
}

Vector& yarp::math::operator/=(Vector &a, const Vector &b)
{
    size_t n =a.length();
    yAssert(n==b.length());
    for (size_t i=0; i<n; i++)
        a[i]/=b[i];
    return a;
}

Vector yarp::math::operator/(const yarp::sig::Vector &b, double k)
{
    Vector res(b);
    return res/=k;
}

Vector& yarp::math::operator/=(yarp::sig::Vector &b, double k)
{
    int n=b.length();
    yAssert(k!=0.0);
    for (int i=0;i<n;i++)
        b[i]/=k;
    return b;
}

Matrix yarp::math::operator/(const yarp::sig::Matrix &M, const double k)
{
    Matrix res(M);
    return res/=k;
}

Matrix& yarp::math::operator/=(yarp::sig::Matrix &M, const double k)
{
    yAssert(k!=0.0);
    int rows=M.rows();
    int cols=M.cols();
    for (int r=0; r<rows; r++)
        for (int c=0; c<cols; c++)
            M(r,c)/=k;
    return M;
}

Matrix yarp::math::eye(int r, int c)
{
    Matrix ret;
    ret.resize(r,c);
    ret.eye();
    return ret;
}

Matrix yarp::math::eye(int n)
{
    return eye(n,n);
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
    return Vector(s, 0.0);
}

Vector yarp::math::ones(int s)
{
    return Vector(s, 1.0);
}

Matrix yarp::math::pile(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2)
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

Matrix yarp::math::pile(const yarp::sig::Matrix &m, const yarp::sig::Vector &v)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);
    cblas_dcopy(r*c, m.data(), 1, res.data(), 1);  // copy first r rows
    cblas_dcopy(c, v.data(), 1, res[r], 1);         // copy last row
    return res;
}

Matrix yarp::math::pile(const yarp::sig::Vector &v, const yarp::sig::Matrix &m)
{
    int c = m.cols();
    yAssert((size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);
    cblas_dcopy(c, v.data(), 1, res.data(), 1);         // copy first row
    cblas_dcopy(r*c, m.data(), 1, res[1], 1);           // copy last r rows    
    return res;
}

Matrix yarp::math::pile(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2)
{
    size_t n = v1.size();
    yAssert(n==v2.size());
    Matrix res(2, (int)n);
    cblas_dcopy(n, v1.data(), 1, res.data(), 1);         // copy first row
    cblas_dcopy(n, v2.data(), 1, res[1], 1);           // copy last r rows    
    return res;
}

Matrix yarp::math::cat(const yarp::sig::Matrix &m1, const yarp::sig::Matrix &m2)
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

Matrix yarp::math::cat(const yarp::sig::Matrix &m, const yarp::sig::Vector &v)
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

Matrix yarp::math::cat(const yarp::sig::Vector &v, const yarp::sig::Matrix &m)
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

Vector yarp::math::cat(const yarp::sig::Vector &v1, const yarp::sig::Vector &v2)
{
    int n1 = v1.size();
    int n2 = v2.size();
    Vector res(n1+n2);
    cblas_dcopy(n1, v1.data(), 1, res.data(), 1);       // copy first n1 elements
    cblas_dcopy(n2, v2.data(), 1, res.data()+n1, 1);    // copy last n2 elements
    return res;
}

Vector yarp::math::cat(const yarp::sig::Vector &v, double s)
{
    int n = v.size();
    Vector res(n+1);
    cblas_dcopy(n, v.data(), 1, res.data(), 1);     // copy first n elements
    res(n) = s;                                       // copy last element
    return res;
}

Vector yarp::math::cat(double s, const yarp::sig::Vector &v)
{
    int n = v.size();
    Vector res(n+1);    
    res(0) = s;                                       // copy last element
    cblas_dcopy(n, v.data(), 1, res.data()+1, 1);     // copy first n elements
    return res;
}

Vector yarp::math::cat(double s1, double s2)
{
    Vector res(2);
    res(0) = s1;
    res(1) = s2;
    return res;
}

Vector yarp::math::cat(double s1, double s2, double s3)
{
    Vector res(3);
    res(0) = s1;
    res(1) = s2;
    res(2) = s3;
    return res;
}

Vector yarp::math::cat(double s1, double s2, double s3, double s4)
{
    Vector res(4);
    res(0) = s1;
    res(1) = s2;
    res(2) = s3;
    res(3) = s4;
    return res;
}

Vector yarp::math::cat(double s1, double s2, double s3, double s4, double s5)
{
    Vector res(5);
    res(0) = s1;
    res(1) = s2;
    res(2) = s3;
    res(3) = s4;
    res(4) = s5;
    return res;
}

double yarp::math::dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b)
{
    yAssert(a.size()==b.size());
    return cblas_ddot(a.size(), a.data(),1, b.data(),1);
}

Matrix yarp::math::outerProduct(const Vector &a, const Vector &b)
{
    size_t s = a.size();
    yAssert(s==b.size());
    Matrix res(s, s);
    for(size_t i=0;i<s;i++)
        for(size_t j=0;j<s;j++)
            res(i,j) = a(i) * b(j);
    return res;
}

Vector yarp::math::cross(const Vector &a, const Vector &b)
{
    yAssert(a.size()==3);
    yAssert(b.size()==3);
    Vector v(3);
    v[0]=a[1]*b[2]-a[2]*b[1];
    v[1]=a[2]*b[0]-a[0]*b[2];
    v[2]=a[0]*b[1]-a[1]*b[0];
    return v;
}

bool yarp::math::cross(const Vector &a, const Vector &b, Vector &out)
{
    if(a.size()!=3 || b.size()!=3)
        return false;
    if(out.size()!=3)
        out.resize(3);
    out[0]=a[1]*b[2]-a[2]*b[1];
    out[1]=a[2]*b[0]-a[0]*b[2];
    out[2]=a[0]*b[1]-a[1]*b[0];
    return true;
}

Matrix yarp::math::crossProductMatrix(const Vector &v)
{
    yAssert(v.size()==3);
    Matrix res = zeros(3,3);
    res(1,0) = v(2); 
    res(0,1) = -v(2); 
    res(2,0) = -v(1); 
    res(0,2) = v(1);
    res(2,1) = v(0); 
    res(1,2) = -v(0);
    return res;
}

bool yarp::math::crossProductMatrix(const Vector &v, Matrix &res)
{
    if(v.size()!=3)
        return false;
    if(res.cols()!=3 || res.rows()!=3)
        res.resize(3,3);
    res(0,0) = res(1,1) = res(2,2) = 0.0;
    res(1,0) = v(2); 
    res(0,1) = -v(2); 
    res(2,0) = -v(1); 
    res(0,2) = v(1);
    res(2,1) = v(0); 
    res(1,2) = -v(0);
    return true;
}

double yarp::math::norm(const Vector &v)
{
    return gsl_blas_dnrm2((const gsl_vector*) v.getGslVector());
}

double yarp::math::norm2(const Vector &v)
{
    return dot(v,v);
}

double yarp::math::findMax(const Vector &v)
{
    if (v.length()<=0)
        return 0.0;
    double ret=v[0];
    for (size_t i=1; i<v.size(); i++)
        if (v[i]>ret)
            ret=v[i];
    return ret;
}

double yarp::math::findMin(const Vector &v)
{
    if (v.length()<=0)
        return 0.0;    
    double ret=v[0];
    for (size_t i=1; i<v.length(); i++)
        if (v[i]<ret)
            ret=v[i];
    return ret;
}


double yarp::math::det(const yarp::sig::Matrix& in) {
    int m = in.rows();
    double ret;
    int sign = 0;

    Matrix LU(in);

    gsl_permutation* permidx = gsl_permutation_calloc(m);
    gsl_linalg_LU_decomp((gsl_matrix *) LU.getGslMatrix(), permidx, &sign);
    ret = gsl_linalg_LU_det((gsl_matrix *) LU.getGslMatrix(), sign); 
    gsl_permutation_free(permidx);

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
    gsl_permutation_free(permidx);
    return ret;
}

bool yarp::math::eingenValues(const yarp::sig::Matrix& in, yarp::sig::Vector &real, yarp::sig::Vector &img) {

    // return error for non-square matrix
    if(in.cols() != in.rows())
        return false;

    real.clear();
    img.clear();

#if (GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= 14)
    size_t n = in.rows();
    gsl_vector_complex *eval = gsl_vector_complex_alloc(n);
    gsl_matrix_complex *evec = gsl_matrix_complex_alloc(n, n);
    gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc(n);    
    gsl_eigen_nonsymmv ((gsl_matrix *)in.getGslMatrix(), eval, evec, w);
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

/* depends on GSL 1.12, put back in when verified that 1.12 is standard
Matrix yarp::math::chinv(const yarp::sig::Matrix& in) {
Matrix ret(in);

gsl_linalg_cholesky_decomp((gsl_matrix *) ret.getGslMatrix());
gsl_linalg_cholesky_invert((gsl_matrix *) ret.getGslMatrix());

return ret;
}
*/
