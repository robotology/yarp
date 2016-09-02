/*
* Author: Lorenzo Natale, Ugo Pattacini
* Copyright (C) 2007 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Log.h>
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>

#include <cmath>

using namespace yarp::sig;

Vector yarp::math::operator+(const Vector &a, const double &s)
{
    Vector ret(a);
    return ret+=s;
}

Vector yarp::math::operator+(const double &s, const Vector &a)
{
    return a+s;
}

Vector& yarp::math::operator+=(Vector &a, const double &s)
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

Matrix yarp::math::operator+(const Matrix &a, const Matrix &b)
{
    Matrix ret(a);    
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

Vector yarp::math::operator-(const Vector &a, const double &s)
{
    Vector ret(a);
    return ret-=s;
}

Vector yarp::math::operator-(const double &s, const Vector &a)
{
    size_t l = a.size();
    Vector ret(l);
    for(size_t i=0; i<l; i++)
        ret[i] = s-a[i];
    return ret;
}

Vector& yarp::math::operator-=(Vector &a, const double &s)
{
    size_t l = a.size();
    for(size_t i=0; i<l; i++)
        a[i] -= s;
    return a;
}

Vector yarp::math::operator-(const Vector &a, const Vector &b)
{
    Vector ret(a);
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
    Matrix ret(a);
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

Vector yarp::math::operator/(const Vector &b, double k)
{
    Vector res(b);
    return res/=k;
}

Vector& yarp::math::operator/=(Vector &b, double k)
{
    int n=b.length();
    yAssert(k!=0.0);
    for (int i=0;i<n;i++)
        b[i]/=k;
    return b;
}

Matrix yarp::math::operator/(const Matrix &M, const double k)
{
    Matrix res(M);
    return res/=k;
}

Matrix& yarp::math::operator/=(Matrix &M, const double k)
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

#ifndef YARP_NO_DEPRECATED
bool yarp::math::eingenValues(const Matrix& in, Vector &real, Vector &img) {
    return eigenValues(in, real, img);
}
#endif // YARP_NO_DEPRECATED

/* depends on GSL 1.12, put back in when verified that 1.12 is standard
Matrix yarp::math::chinv(const Matrix& in) 
{
    Matrix ret(in);

    gsl_linalg_cholesky_decomp((gsl_matrix *) GslMartix(ret).getGslMatrix());
    gsl_linalg_cholesky_invert((gsl_matrix *) GslMartix(ret).getGslMatrix());

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
    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("dcm2axis() failed");
        return Vector(0);
    }

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
    if (v.length()<4)
    {
        yError("axis2dcm() failed");
        return Matrix(0,0);
    }

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
    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("dcm2euler() failed");
        return Vector(0);
    }

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
        yWarning("dcm2euler() in singularity: choosing one solution among multiple");

    return v;
}

Matrix yarp::math::euler2dcm(const Vector &v)
{
    if (v.length()<3)
    {
        yError("euler2dcm() failed");
        return Matrix(0,0);
    }

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
    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("dcm2rpy() failed");
        return Vector(0);
    }

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
            v[0]=0.0;
            v[1]=M_PI/2.0;
            v[2]=-atan2(-R(1,2),R(1,1));
        }
    }
    else
    {
        // Not a unique solution: psi+phi=atan2(-R12,R11)
        v[0]=0.0;
        v[1]=-M_PI/2.0;
        v[2]=atan2(-R(1,2),R(1,1));
    }

    if (singularity)
        yWarning("dcm2rpy() in singularity: choosing one solution among multiple");

    return v;
}

Matrix yarp::math::rpy2dcm(const Vector &v)
{
    if (v.length()<3)
    {
        yError("rpy2dcm() failed");
        return Matrix(0,0);
    }

    Matrix Rz=eye(4,4); Matrix Ry=eye(4,4);   Matrix Rx=eye(4,4);
    double roll=v[0];   double cr=cos(roll);  double sr=sin(roll);
    double pitch=v[1];  double cp=cos(pitch); double sp=sin(pitch);
    double yaw=v[2];    double cy=cos(yaw);   double sy=sin(yaw);
    
    Rz(0,0)=cy; Rz(1,1)=cy; Rz(0,1)=-sy; Rz(1,0)= sy;   // z-rotation with yaw
    Ry(0,0)=cp; Ry(2,2)=cp; Ry(0,2)= sp; Ry(2,0)=-sp;   // y-rotation with pitch
    Rx(1,1)=cr; Rx(2,2)=cr; Rx(1,2)=-sr; Rx(2,1)= sr;   // x-rotation with roll

    return Rz*Ry*Rx;
}

Vector yarp::math::dcm2quat(const Matrix &R)
{
    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("dcm2quat() failed");
        return Vector(0);
    }
    
    Vector q(4,0.0);
    double tr=R(0,0)+R(1,1)+R(2,2);

    if (tr>0.0)
    {
        double sqtrp1=sqrt(tr+1.0);
        double sqtrp12=2.0*sqtrp1;
        q[0]=0.5*sqtrp1;
        q[1]=(R(1,2)-R(2,1))/sqtrp12;
        q[2]=(R(2,0)-R(0,2))/sqtrp12;
        q[3]=(R(0,1)-R(1,0))/sqtrp12;
    }
    else if ((R(1,1)>R(0,0)) && (R(1,1)>R(2,2)))
    {
        double sqdip1=sqrt(R(1,1)-R(0,0)-R(2,2)+1.0);            
        q[2]=0.5*sqdip1; 
        
        if (sqdip1>0.0)
            sqdip1=0.5/sqdip1;
        
        q[0]=(R(2,0)-R(0,2))*sqdip1; 
        q[1]=(R(0,1)+R(1,0))*sqdip1; 
        q[3]=(R(1,2)+R(2,1))*sqdip1; 
    }
    else if (R(2,2)>R(0,0))
    {
        double sqdip1=sqrt(R(2,2)-R(0,0)-R(1,1)+1.0);            
        q[3]=0.5*sqdip1; 
        
        if (sqdip1>0.0)
            sqdip1=0.5/sqdip1;
        
        q[0]=(R(0,1)-R(1,0))*sqdip1;
        q[1]=(R(2,0)+R(0,2))*sqdip1; 
        q[2]=(R(1,2)+R(2,1))*sqdip1; 
    }
    else
    {
        double sqdip1=sqrt(R(0,0)-R(1,1)-R(2,2)+1.0);            
        q[1]=0.5*sqdip1;
        
        if (sqdip1>0.0)
            sqdip1=0.5/sqdip1;
        
        q[0]=(R(1,2)-R(2,1))*sqdip1; 
        q[2]=(R(0,1)+R(1,0))*sqdip1; 
        q[3]=(R(2,0)+R(0,2))*sqdip1; 
    }

    return q;
}

Matrix yarp::math::quat2dcm(const Vector &q)
{
    if (q.length()<4)
    {
        yError("quat2dcm() failed");
        return Matrix(0,0);
    }

    Vector qin=(1.0/norm(q))*q;

    Matrix R=eye(4,4);
    R(0,0)=qin[0]*qin[0]+qin[1]*qin[1]-qin[2]*qin[2]-qin[3]*qin[3];
    R(0,1)=2.0*(qin[1]*qin[2]+qin[0]*qin[3]);
    R(0,2)=2.0*(qin[1]*qin[3]-qin[0]*qin[2]);
    R(1,0)=2.0*(qin[1]*qin[2]-qin[0]*qin[3]);
    R(1,1)=qin[0]*qin[0]-qin[1]*qin[1]+qin[2]*qin[2]-qin[3]*qin[3];
    R(1,2)=2.0*(qin[2]*qin[3]+qin[0]*qin[1]);
    R(2,0)=2.0*(qin[1]*qin[3]+qin[0]*qin[2]);
    R(2,1)=2.0*(qin[2]*qin[3]-qin[0]*qin[1]);
    R(2,2)=qin[0]*qin[0]-qin[1]*qin[1]-qin[2]*qin[2]+qin[3]*qin[3];

    return R;
}

Matrix yarp::math::SE3inv(const Matrix &H)
{    
    if ((H.rows()!=4) || (H.cols()!=4))
    {
        yError("SE3inv() failed");
        return Matrix(0,0);
    }

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
    if ((H.rows()!=4) || (H.cols()!=4))
    {
        yError("adjoint() failed: roto-translational matrix sized %dx%d instead of 4x4",
               H.rows(),H.cols());
        return Matrix(0,0);
    }

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
    if ((H.rows()!=4) || (H.cols()!=4))
    {
        yError("adjointInv() failed: roto-translational matrix sized %dx%d instead of 4x4",
               H.rows(),H.cols());
        return Matrix(0,0);
    }
    
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

