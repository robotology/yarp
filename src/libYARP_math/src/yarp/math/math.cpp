/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>
#include <yarp/math/Quaternion.h>

#include <yarp/eigen/Eigen.h>

#include <Eigen/Eigenvalues>

#include <cmath>
#include <cassert>

using namespace yarp::eigen;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(MATH, "yarp.math")
}

Vector operator+(const Vector &a, const double &s)
{
    Vector ret(a);
    return ret+=s;
}

Vector operator+(const double &s, const Vector &a)
{
    return a+s;
}

Vector& operator+=(Vector &a, const double &s)
{
    size_t l = a.size();
    for(size_t i=0; i<l; i++)
        a[i] += s;
    return a;
}

Vector operator+(const Vector &a, const Vector &b)
{
    Vector ret(a);
    return ret+=b;
}

Vector& operator+=(Vector &a, const Vector &b)
{
    size_t s=a.size();
    yCAssert(MATH, s==b.size());
    for (size_t k=0; k<s;k++)
        a[k]+=b[k];
    return a;
}

Matrix operator+(const Matrix &a, const Matrix &b)
{
    Matrix ret(a);
    return ret+=b;
}

Matrix& operator+=(Matrix &a, const Matrix &b)
{
    size_t n=a.cols();
    size_t m=a.rows();
    yCAssert(MATH, m==b.rows() && n==b.cols());
    for (size_t r=0; r<m;r++)
        for (size_t c=0; c<n;c++)
            a(r,c)+=b(r,c);
    return a;
}

Vector operator-(const Vector &a, const double &s)
{
    Vector ret(a);
    return ret-=s;
}

Vector operator-(const double &s, const Vector &a)
{
    size_t l = a.size();
    Vector ret(l);
    for(size_t i=0; i<l; i++)
        ret[i] = s-a[i];
    return ret;
}

Vector& operator-=(Vector &a, const double &s)
{
    size_t l = a.size();
    for(size_t i=0; i<l; i++)
        a[i] -= s;
    return a;
}

Vector operator-(const Vector &a, const Vector &b)
{
    Vector ret(a);
    return ret-=b;
}

Vector& operator-=(Vector &a, const Vector &b)
{
    size_t s=a.size();
    yCAssert(MATH, s==b.size());
    for (size_t k=0; k<s;k++)
        a[k]-=b[k];
    return a;
}

Matrix operator-(const Matrix &a, const Matrix &b)
{
    Matrix ret(a);
    return ret-=b;
}

Matrix& operator-=(Matrix &a, const Matrix &b)
{
    size_t n=a.cols();
    size_t m=a.rows();
    yCAssert(MATH, m==b.rows());
    yCAssert(MATH, n==b.cols());
    for (size_t r=0; r<m;r++)
        for (size_t c=0; c<n;c++)
            a(r,c)-=b(r,c);
    return a;
}

Vector operator*(double k, const Vector &b)
{
    return operator*(b,k);
}

Vector operator*(const Vector &a, double k)
{
    Vector ret(a);
    return ret*=k;
}

Vector& operator*=(Vector &a, double k)
{
    size_t size=a.size();
    for (size_t i = 0; i < size; i++)
        a[i]*=k;
    return a;
}

Vector operator*(const Vector &a, const Matrix &m)
{
    yCAssert(MATH, a.size()==(size_t)m.rows());
    Vector ret((size_t)m.cols());

    toEigen(ret) = toEigen(m).transpose()*toEigen(a);

    return ret;
}

Vector& operator*=(Vector &a, const Matrix &m)
{
    yCAssert(MATH, a.size()==(size_t)m.rows());
    Vector a2(a);
    a.resize(m.cols());

    toEigen(a) = toEigen(m).transpose()*toEigen(a2);

    return a;
}

Vector operator*(const Matrix &m, const Vector &a)
{
    yCAssert(MATH, (size_t)m.cols()==a.size());
    Vector ret((size_t)m.rows(),0.0);

    toEigen(ret) = toEigen(m)*toEigen(a);

    return ret;
}

Matrix operator*(const Matrix &a, const Matrix &b)
{
    yCAssert(MATH, a.cols()==b.rows());
    Matrix c(a.rows(), b.cols());

    toEigen(c) = toEigen(a)*toEigen(b);

    return c;
}

Matrix& operator*=(Matrix &a, const Matrix &b)
{
    yCAssert(MATH, a.cols()==b.rows());
    Matrix a2(a);   // a copy of a
    a.resize(a.rows(), b.cols());

    toEigen(a) = toEigen(a2)*toEigen(b);

    return a;
}

Matrix operator*(const double k, const Matrix &M)
{
    Matrix res(M);
    return res*=k;
}

Matrix operator*(const Matrix &M, const double k)
{
    return operator*(k,M);
}

Matrix& operator*=(Matrix &M, const double k)
{
    for (size_t r=0; r<M.rows(); r++)
        for (size_t c=0; c<M.cols(); c++)
            M(r,c)*=k;
    return M;
}

Vector operator*(const Vector &a, const Vector &b)
{
    Vector res(a);
    return res*=b;
}

Vector& operator*=(Vector &a, const Vector &b)
{
    size_t n =a.length();
    yCAssert(MATH, n==b.length());
    for (size_t i=0; i<n; i++)
        a[i]*=b[i];
    return a;
}

Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
    return Quaternion(a.w()*b.x() + a.x()*b.w() + a.y()*b.z() - a.z()*b.y(),
                      a.w()*b.y() + a.y()*b.w() + a.z()*b.x() - a.x()*b.z(),
                      a.w()*b.z() + a.z()*b.w() + a.x()*b.y() - a.y()*b.x(),
                      a.w()*b.w() - a.x()*b.x() - a.y()*b.y() - a.z()*b.z());
}

Vector operator/(const Vector &a, const Vector &b)
{
    Vector res(a);
    return res/=b;
}

Vector& operator/=(Vector &a, const Vector &b)
{
    size_t n =a.length();
    yCAssert(MATH, n==b.length());
    for (size_t i=0; i<n; i++)
        a[i]/=b[i];
    return a;
}

Vector operator/(const Vector &b, double k)
{
    Vector res(b);
    return res/=k;
}

Vector& operator/=(Vector &b, double k)
{
    size_t n=b.length();
    yCAssert(MATH, k!=0.0);
    for (size_t i = 0; i < n; i++)
        b[i]/=k;
    return b;
}

Matrix operator/(const Matrix &M, const double k)
{
    Matrix res(M);
    return res/=k;
}

Matrix& operator/=(Matrix &M, const double k)
{
    yCAssert(MATH, k!=0.0);
    int rows=M.rows();
    int cols=M.cols();
    for (int r=0; r<rows; r++)
        for (int c=0; c<cols; c++)
            M(r,c)/=k;
    return M;
}

Matrix yarp::math::pile(const Matrix &m1, const Matrix &m2)
{
    size_t c = m1.cols();
    yCAssert(MATH, c==m2.cols());
    size_t r1 = m1.rows();
    size_t r2 = m2.rows();
    Matrix res(r1+r2, c);

    toEigen(res).block(0,0,r1,c) = toEigen(m1);
    toEigen(res).block(r1,0,r2,c) = toEigen(m2);

    return res;
}

Matrix yarp::math::pile(const Matrix &m, const Vector &v)
{
    int c = m.cols();
    yCAssert(MATH, (size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    toEigen(res).block(0,0,r,c) = toEigen(m);
    toEigen(res).block(r,0,1,c) = toEigen(v).transpose();

    return res;
}

Matrix yarp::math::pile(const Vector &v, const Matrix &m)
{
    int c = m.cols();
    yCAssert(MATH, (size_t)c==v.size());
    int r = m.rows();
    Matrix res(r+1, c);

    toEigen(res).block(0,0,1,c) = toEigen(v).transpose();
    toEigen(res).block(1,0,r,c) = toEigen(m);

    return res;
}

Matrix yarp::math::pile(const Vector &v1, const Vector &v2)
{
    size_t n = v1.size();
    yCAssert(MATH, n==v2.size());
    Matrix res(2, (int)n);

    toEigen(res).block(0,0,1,n) = toEigen(v1);
    toEigen(res).block(1,0,1,n) = toEigen(v2);

    return res;
}

Matrix yarp::math::cat(const Matrix &m1, const Matrix &m2)
{
    size_t r = m1.rows();
    yCAssert(MATH, r==m2.rows());
    size_t c1 = m1.cols();
    size_t c2 = m2.cols();
    Matrix res(r, c1+c2);

    toEigen(res).block(0,0,r,c1)  = toEigen(m1);
    toEigen(res).block(0,c1,r,c2) = toEigen(m2);

    return res;
}

Matrix yarp::math::cat(const Matrix &m, const Vector &v)
{
    int r = m.rows();
    yCAssert(MATH, (size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    toEigen(res).block(0,0,r,c)  = toEigen(m);
    toEigen(res).block(0,c,r,1) = toEigen(v);

    return res;
}

Matrix yarp::math::cat(const Vector &v, const Matrix &m)
{
    int r = m.rows();
    yCAssert(MATH, (size_t)r==v.size());
    int c = m.cols();
    Matrix res(r, c+1);

    toEigen(res).block(0,0,r,1) = toEigen(v);
    toEigen(res).block(0,1,r,c) = toEigen(m);

    return res;
}

Vector yarp::math::cat(const Vector &v1, const Vector &v2)
{
    size_t n1 = v1.size();
    size_t n2 = v2.size();
    Vector res(n1+n2);

    toEigen(res).segment(0,n1) = toEigen(v1);
    toEigen(res).segment(n1,n2) = toEigen(v2);

    return res;
}

Vector yarp::math::cat(const Vector &v, double s)
{
    size_t n = v.size();
    Vector res(n+1);

    toEigen(res).segment(0,n) = toEigen(v);
    res(n) = s;

    return res;
}

Vector yarp::math::cat(double s, const Vector &v)
{
    size_t n = v.size();
    Vector res(n+1);

    res(0) = s;
    toEigen(res).segment(1,n) = toEigen(v);

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

double yarp::math::dot(const Vector &a, const Vector &b)
{
    yCAssert(MATH, a.size()==b.size());

    return toEigen(a).dot(toEigen(b));
}


Matrix yarp::math::outerProduct(const Vector &a, const Vector &b)
{
    size_t s = a.size();
    yCAssert(MATH, s==b.size());
    Matrix res(s, s);
    for(size_t i=0;i<s;i++)
        for(size_t j=0;j<s;j++)
            res(i,j) = a(i) * b(j);
    return res;
}

Vector yarp::math::cross(const Vector &a, const Vector &b)
{
    yCAssert(MATH, a.size()==3);
    yCAssert(MATH, b.size()==3);
    Vector v(3);
    v[0]=a[1]*b[2]-a[2]*b[1];
    v[1]=a[2]*b[0]-a[0]*b[2];
    v[2]=a[0]*b[1]-a[1]*b[0];
    return v;
}

Matrix yarp::math::crossProductMatrix(const Vector &v)
{
    yCAssert(MATH, v.size()==3);
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
    return toEigen(v).norm();
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

Vector yarp::math::zeros(int s)
{
    return Vector(s, 0.0);
}

Vector yarp::math::ones(int s)
{
    return Vector(s, 1.0);
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

/*
Matrix yarp::math::chinv(const Matrix& in)
{
    Matrix ret(in);

    return ret;
}
*/

double yarp::math::sign(const double &v)
{
    return ((v==0.0)?0.0:((v>0.0)?1.0:-1.0));
}


Vector yarp::math::sign(const Vector &v)
{
    Vector ret(v.length());
    for (size_t i=0; i<v.length(); i++)
        ret[i]=sign(v[i]);

    return ret;
}

Vector yarp::math::dcm2axis(const Matrix &R)
{
    yCAssert(MATH, (R.rows()>=3) && (R.cols()>=3));

    Vector v(4);
    v[0]=R(2,1)-R(1,2);
    v[1]=R(0,2)-R(2,0);
    v[2]=R(1,0)-R(0,1);
    v[3]=0.0;
    double r=yarp::math::norm(v);
    double theta=atan2(0.5*r,0.5*(R(0,0)+R(1,1)+R(2,2)-1));

    if (r<1e-9)
    {
        // if we enter here, then
        // R is symmetric; this can
        // happen only if the rotation
        // angle is 0 (R=I) or 180 degrees
        Matrix A=R.submatrix(0,2,0,2);
        Matrix U(3,3), V(3,3);
        Vector S(3);

        // A=I+sin(theta)*S+(1-cos(theta))*S^2
        // where S is the skew matrix.
        // Given a point x, A*x is the rotated one,
        // hence if Ax=x then x belongs to the rotation
        // axis. We have therefore to find the kernel of
        // the linear application (A-I).
        SVD(A-eye(3,3),U,S,V);

        v[0]=V(0,2);
        v[1]=V(1,2);
        v[2]=V(2,2);
        r=yarp::math::norm(v);
    }

    v=(1.0/r)*v;
    v[3]=theta;

    return v;
}

Matrix yarp::math::axis2dcm(const Vector &v)
{
    yCAssert(MATH, v.length()>=4);

    Matrix R=eye(4,4);

    double theta=v[3];
    if (theta==0.0)
        return R;

    double c=cos(theta);
    double s=sin(theta);
    double C=1.0-c;

    double xs =v[0]*s;
    double ys =v[1]*s;
    double zs =v[2]*s;
    double xC =v[0]*C;
    double yC =v[1]*C;
    double zC =v[2]*C;
    double xyC=v[0]*yC;
    double yzC=v[1]*zC;
    double zxC=v[2]*xC;

    R(0,0)=v[0]*xC+c;
    R(0,1)=xyC-zs;
    R(0,2)=zxC+ys;
    R(1,0)=xyC+zs;
    R(1,1)=v[1]*yC+c;
    R(1,2)=yzC-xs;
    R(2,0)=zxC-ys;
    R(2,1)=yzC+xs;
    R(2,2)=v[2]*zC+c;

    return R;
}

Vector yarp::math::dcm2euler(const Matrix &R)
{
    yCAssert(MATH, (R.rows()>=3) && (R.cols()>=3));

    Vector v(3);
    bool singularity=false;
    if (R(2,2)<1.0)
    {
        if (R(2,2)>-1.0)
        {
            v[0]=atan2(R(1,2),R(0,2));
            v[1]=acos(R(2,2));
            v[2]=atan2(R(2,1),-R(2,0));
        }
        else
        {
            // Not a unique solution: gamma-alpha=atan2(R10,R11)
            singularity=true;
            v[0]=-atan2(R(1,0),R(1,1));
            v[1]=M_PI;
            v[2]=0.0;
        }
    }
    else
    {
        // Not a unique solution: gamma+alpha=atan2(R10,R11)
        singularity=true;
        v[0]=atan2(R(1,0),R(1,1));
        v[1]=0.0;
        v[2]=0.0;
    }

    if (singularity)
        yCWarning(MATH, "dcm2euler() in singularity: choosing one solution among multiple");

    return v;
}

Matrix yarp::math::euler2dcm(const Vector &v)
{
    yCAssert(MATH, v.length()>=3);

    Matrix Rza=eye(4,4); Matrix Ryb=eye(4,4);  Matrix Rzg=eye(4,4);
    double alpha=v[0];   double ca=cos(alpha); double sa=sin(alpha);
    double beta=v[1];    double cb=cos(beta);  double sb=sin(beta);
    double gamma=v[2];   double cg=cos(gamma); double sg=sin(gamma);

    Rza(0,0)=ca; Rza(1,1)=ca; Rza(1,0)= sa; Rza(0,1)=-sa;
    Rzg(0,0)=cg; Rzg(1,1)=cg; Rzg(1,0)= sg; Rzg(0,1)=-sg;
    Ryb(0,0)=cb; Ryb(2,2)=cb; Ryb(2,0)=-sb; Ryb(0,2)= sb;

    return Rza*Ryb*Rzg;
}

Vector yarp::math::dcm2rpy(const Matrix &R)
{
    yCAssert(MATH, (R.rows()>=3) && (R.cols()>=3));

    Vector v(3);
    bool singularity=false;
    if (R(2,0)<1.0)
    {
        if (R(2,0)>-1.0)
        {
            v[0]=atan2(R(2,1),R(2,2));
            v[1]=asin(-R(2,0));
            v[2]=atan2(R(1,0),R(0,0));
        }
        else
        {
            // Not a unique solution: psi-phi=atan2(-R12,R11)
            singularity = true;
            v[0]=0.0;
            v[1]=M_PI/2.0;
            v[2]=-atan2(-R(1,2),R(1,1));
        }
    }
    else
    {
        // Not a unique solution: psi+phi=atan2(-R12,R11)
        singularity = true;
        v[0]=0.0;
        v[1]=-M_PI/2.0;
        v[2]=atan2(-R(1,2),R(1,1));
    }

    if (singularity)
        yCWarning(MATH, "dcm2rpy() in singularity: choosing one solution among multiple");

    return v;
}

Matrix yarp::math::rpy2dcm(const Vector &v)
{
    yCAssert(MATH, v.length()>=3);

    Matrix Rz=eye(4,4); Matrix Ry=eye(4,4);   Matrix Rx=eye(4,4);
    double roll=v[0];   double cr=cos(roll);  double sr=sin(roll);
    double pitch=v[1];  double cp=cos(pitch); double sp=sin(pitch);
    double yaw=v[2];    double cy=cos(yaw);   double sy=sin(yaw);

    Rz(0,0)=cy; Rz(1,1)=cy; Rz(0,1)=-sy; Rz(1,0)= sy;   // z-rotation with yaw
    Ry(0,0)=cp; Ry(2,2)=cp; Ry(0,2)= sp; Ry(2,0)=-sp;   // y-rotation with pitch
    Rx(1,1)=cr; Rx(2,2)=cr; Rx(1,2)=-sr; Rx(2,1)= sr;   // x-rotation with roll

    return Rz*Ry*Rx;
}

Vector yarp::math::dcm2ypr(const yarp::sig::Matrix &R)
{
    yCAssert(MATH, (R.rows() >= 3) && (R.cols() >= 3));

    Vector v(3); // yaw pitch roll

    if (R(0, 2)<1.0)
    {
        if (R(0, 2)>-1.0)
        {
            v[0] = atan2(-R(0, 1), R(0, 0));
            v[1] = asin(R(0, 2));
            v[2] = atan2(-R(1, 2), R(2, 2));
        }
        else // == -1
        {
            // Not a unique solution: psi-phi=atan2(-R12,R11)
            v[0] = 0.0;
            v[1] = -M_PI / 2.0;
            v[2] = -atan2(R(1, 0), R(1, 1));
        }
    }
    else // == +1
    {
        // Not a unique solution: psi+phi=atan2(-R12,R11)
        v[0] = 0.0;
        v[1] = M_PI / 2.0;
        v[2] = atan2(R(1, 0), R(1, 1));
    }

    return v;
}

Matrix yarp::math::ypr2dcm(const Vector &v)
{
    yCAssert(MATH, v.length() >= 3);

    Matrix Rz = eye(4, 4); Matrix Ry = eye(4, 4);   Matrix Rx = eye(4, 4);
    double roll = v[2];   double cr = cos(roll);  double sr = sin(roll);
    double pitch = v[1];  double cp = cos(pitch); double sp = sin(pitch);
    double yaw = v[0];    double cy = cos(yaw);   double sy = sin(yaw);

    Rz(0, 0) = cy; Rz(1, 1) = cy; Rz(0, 1) = -sy; Rz(1, 0) = sy;   // z-rotation with yaw
    Ry(0, 0) = cp; Ry(2, 2) = cp; Ry(0, 2) = sp; Ry(2, 0) = -sp;   // y-rotation with pitch
    Rx(1, 1) = cr; Rx(2, 2) = cr; Rx(1, 2) = -sr; Rx(2, 1) = sr;   // x-rotation with roll

    return Rx*Ry*Rz;
}

Matrix yarp::math::SE3inv(const Matrix &H)
{
    yCAssert(MATH, (H.rows()==4) && (H.cols()==4));

    Vector p(4);
    p[0]=H(0,3);
    p[1]=H(1,3);
    p[2]=H(2,3);
    p[3]=1.0;

    Matrix invH=H.transposed();
    p=invH*p;

    invH(0,3)=-p[0];
    invH(1,3)=-p[1];
    invH(2,3)=-p[2];
    invH(3,0)=invH(3,1)=invH(3,2)=0.0;

    return invH;
}

Matrix yarp::math::adjoint(const Matrix &H)
{
    yCAssert(MATH, (H.rows()==4) && (H.cols()==4));

    // the skew matrix coming from the translational part of H: S(r)
    Matrix S(3,3);
    S(0,0)= 0.0;    S(0,1)=-H(2,3); S(0,2)= H(1,3);
    S(1,0)= H(2,3); S(1,1)= 0.0;    S(1,2)=-H(0,3);
    S(2,0)=-H(1,3); S(2,1)= H(0,3); S(2,2)= 0.0;

    S = S*H.submatrix(0,2,0,2);

    Matrix A(6,6); A.zero();
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            A(i,j)     = H(i,j);
            A(i+3,j+3) = H(i,j);
            A(i,j+3)   = S(i,j);
        }
    }

    return A;
}

Matrix yarp::math::adjointInv(const Matrix &H)
{
    yCAssert(MATH, (H.rows()==4) && (H.cols()==4));

    // R^T
    Matrix Rt = H.submatrix(0,2,0,2).transposed();
    // R^T * r
    Vector Rtp = Rt*H.getCol(3).subVector(0,2);

    Matrix S(3,3);
    S(0,0)= 0.0;    S(0,1)=-Rtp(2); S(0,2)= Rtp(1);
    S(1,0)= Rtp(2); S(1,1)= 0.0;    S(1,2)=-Rtp(0);
    S(2,0)=-Rtp(1); S(2,1)= Rtp(0); S(2,2)= 0.0;

    S = S*Rt;

    Matrix A(6,6); A.zero();
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            A(i,j)     = Rt(i,j);
            A(i+3,j+3) = Rt(i,j);
            A(i,j+3)   = -S(i,j);
        }
    }

    return A;
}
