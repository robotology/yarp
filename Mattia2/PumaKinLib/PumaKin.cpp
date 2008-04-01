
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_linalg.h> 
#include <gsl/gsl_blas.h>


#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>


#include <MEIMotionControl.h>

//#include <yarp/os/all.h>

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <cmath.h>
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
//using namespace yarp::math;
using namespace std;


#include <cmath>
#include "nr.h"
using namespace std;






#define RADTODEG 57.29577951308232100
#define DEGTORAD 0.01745329251994330
#define PI 3.14159265


#define M_PI 3.14159265358979323846




#include "PumaKin.h"



double SQdet(Matrix a)
{

	int forder=a.cols();
    double determinant = 1;

  int i,j,k;
  double  mult;
  double deter=1;
  for(i=0;i<forder;i++)
  {
	for(j=0;j<forder;j++)
	{
	  mult=a[j][i]/a[i][i];
	  for(k=0;k<forder;k++)
	  {
		if(i==j) break;
		a[j][k]=a[j][k]-a[i][k]*mult;
	  }
	}
  }
  for(i=0;i<forder;i++)
  {
	deter=deter*a[i][i];
  }
  return(deter);

}



Vector zeros(int s)
{
    Vector ret;
    ret.resize(s);
    ret.zero();
    return ret;
}
Matrix zeros(int r, int c)
{
    Matrix ret;
    ret.resize(r,c);
    ret.zero();
    return ret;
}

Vector operator-(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]-b[k];
    return ret;
}

double dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b)
{
    double ret;
    ret=cblas_ddot(a.size(), a.data(),1, b.data(),1);
    return ret;
}

Vector operator*(const yarp::sig::Vector &a, const yarp::sig::Matrix &m)
{
    // to be implemented
    Vector ret(m.cols());
    ret=0.0;

    gsl_blas_dgemv(CblasTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());
    return ret;
}

Vector operator*(const yarp::sig::Matrix &m, const yarp::sig::Vector &a)
{
    Vector ret(m.rows());
    ret=0.0;

    gsl_blas_dgemv(CblasNoTrans, 1.0, (const gsl_matrix *) m.getGslMatrix(), 
        (const gsl_vector *) a.getGslVector(), 0.0, 
        (gsl_vector *) ret.getGslVector());

    return ret;
}
Matrix operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    int n=a.cols();
	int m=a.rows();
    yarp::sig::Matrix ret(m,n);
    for (int r=0; r<m;r++)
		for (int c=0; c<n;c++)
			ret(r,c)=a(r,c)-b(r,c);
    return ret;
}

Vector operator*(const Vector &a, double k)
{
    int size=a.size();
    Vector ret(size);
    int i;
    for(i=0;i<size;i++)
        ret[i]=a[i]*k;

    return ret;
}

Vector operator*(double k, const Vector &b)
{
    return operator*(b,k);
}

Matrix operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    Matrix c(a.rows(), b.cols());
    cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                   c.rows(), c.cols(), a.cols(),
                   1.0, a.data(), a.cols(), b.data(), b.cols(), 0.0,
                   c.data(), c.cols());

    return c;
}
Vector operator+(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]+b[k];
    return ret;
}
Matrix operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b)
{
    int n=a.cols();
	int m=a.rows();
    yarp::sig::Matrix ret(m,n);
    for (int r=0; r<m;r++)
		for (int c=0; c<n;c++)
			ret(r,c)=a(r,c)+b(r,c);
    return ret;
}
/**
* Perform SVD decomposition on a NxM matrix. Golub Reinsch method.
*/
void SVD(const Matrix &in, 
            Matrix &U,
            Vector &S,
            Matrix &V)
{
    U=in;
    Vector work;
    work.resize(U.cols());
    gsl_linalg_SV_decomp((gsl_matrix *) U.getGslMatrix(),
        (gsl_matrix *) V.getGslMatrix(),
        (gsl_vector *) S.getGslVector(),
        (gsl_vector *) work.getGslVector());
}
/**
* Perform the moore-penrose pseudo-inverse on 
* a NxM matrix. 
* 
*/
Matrix pinvCD(const Matrix &in)
{
	int m = in.rows();
	int n = in.cols();
	Matrix U(m,n);
	Vector Sdiag(n);
	Matrix V(n,n);

	SVD(in, U, Sdiag, V);

	Matrix Spinv = zeros(n,n);
	for (int c=0;c<n; c++)
	{
		for(int r=0;r<n;r++)
		{
			if ( r==c && Sdiag(c)!=0 )
				Spinv(r,c) = 1/Sdiag(c);
			else
				Spinv(r,c) = 0;
		}
	}
	Matrix UT=U.transposed();
	Matrix ris2(6,6);
	
	double detV=SQdet(V);
	double detSpinv=SQdet(Spinv);
	double detUT=SQdet(UT);
	double ris=detV*detSpinv*detUT;
//	int i;
/*
		printf("\n V ");
		for( i = 0 ; i < n; i++)
		{
			printf("\n");
			for(int j = 0 ; j < n; j++)
			{		
				printf("  %lf",V[i][j]);
			}
		}

				printf("\n Spinv ");
		for( i = 0 ; i < n; i++)
		{
			printf("\n");
			for(int j = 0 ; j < n; j++)
			{		
				printf("  %lf",Spinv[i][j]);
			}
		}
				printf("\n UT ");
		for( i = 0 ; i < n; i++)
		{
			printf("\n");
			for(int j = 0 ; j < n; j++)
			{		
				printf("  %lf",UT[i][j]);
			}
		}
*/		ris2=V*Spinv*UT;
/*		printf("\n ris2 ");
		for( i = 0 ; i < n; i++)
		{
			printf("\n");
			for(int j = 0 ; j < n; j++)
			{		
				printf("  %lf",ris2[i][j]);
			}
		}

	printf("\n\n ris=%.10lf detV=%.10lf detSpinv=%.10lf detUT=%.10lf",ris,detV,detSpinv,detUT);
*/	return V*Spinv*UT;
}

Vector Quat2Euler(Vector Quat)
{
	Vector axis(3);
/*	//double Angle = 2 * acos(Quat(0));
	double scale = Quat(1) + Quat(2) + Quat(3);
	axis(0) = Quat(1) / scale;
	axis(1) = Quat(2) / scale;
	axis(2) = Quat(3) / scale;
	
	return axis;
*/
	/** Quat can be non-normalised quaternion */

	double heading,attitude, bank;
    double sqw = Quat(0)*Quat(0);
    double sqx = Quat(1)*Quat(1);
    double sqy = Quat(2)*Quat(2);
    double sqz = Quat(3)*Quat(3);
	double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	double test = Quat(1)*Quat(2) + Quat(3)*Quat(0);
	if (test > 0.499*unit) { // singularity at north pole
		heading = 2 * atan2(Quat(1),Quat(0));
		attitude = PI/2;
		bank = 0;
			
		axis(0) = heading;
		axis(1) = attitude;
		axis(2) = bank;
		return axis ;
	}
	if (test < -0.499*unit) { // singularity at south pole
		heading = -2 * atan2(Quat(1),Quat(0));
		attitude = -PI/2;
		bank = 0;
			
		axis(0) = heading;
		axis(1) = attitude;
		axis(2) = bank;
		return axis ;
	}
    heading = atan2(2*Quat(2)*Quat(0)-2*Quat(1)*Quat(3) , sqx - sqy - sqz + sqw);
	attitude = asin(2*test/unit);
	bank = atan2(2*Quat(1)*Quat(0)-2*Quat(2)*Quat(3) , -sqx + sqy - sqz + sqw);

	axis(0) = heading;		//roll
	axis(1) = attitude;		//pitch
	axis(2) = bank;			//roll
	
	return axis;
}


Vector Quat2Axis(Vector Quat)
{
	Vector axis(3);
	//double Angle = 2 * acos(Quat(0));
	double scale = Quat(1) + Quat(2) + Quat(3);
	axis(0) = Quat(1) / scale;
	axis(1) = Quat(2) / scale;
	axis(2) = Quat(3) / scale;
	
	return axis;

}










Vector RotMat2Quat(Matrix RotationMatrix)
{
/*
	for(int i = 0 ; i < 3; i++)
		{printf("  \n");
		for(int j = 0 ; j < 3; j++)
			{		
				printf("  %lf",RotationMatrix[i][j]);
			}
		}
*/
	//double T = RotationMatrix[0][0] + RotationMatrix[1][1] + RotationMatrix[2][2] + 1.0;
	double T = RotationMatrix[0][0] + RotationMatrix[1][1] + RotationMatrix[2][2] ;
	if(T <=0)
		printf("\n RotationMatrix is not special orthogonal !!!");


	Vector Quat(4);

//	If the trace of the matrix is greater than zero, then the result is:

     double S = 0.5 / sqrt(T);

//	 double S =  1/sqrt(T);
     Quat(0) = 0.25 / S;
     Quat(1) = ( RotationMatrix[2][1] - RotationMatrix[1][2] ) * S;
     Quat(2) = ( RotationMatrix[0][2] - RotationMatrix[2][0] ) * S;
     Quat(3) = ( RotationMatrix[1][0] - RotationMatrix[0][1] ) * S;

/*	 	
	 printf("\n Quat = ");
	 for( i = 0 ; i < 4; i++)
		{
		 printf(" %.1lf ",Quat(i));
		 }
	 printf("\n" );

//If the trace of the matrix is less than or equal to zero then identify which major diagonal element has the greatest value.

	if ((RotationMatrix[0][0] > RotationMatrix[1][1] )&(RotationMatrix[0][0] > RotationMatrix[2][2] )) { 
	   S = sqrt( 1.0 + RotationMatrix[0][0] - RotationMatrix[1][1] - RotationMatrix[2][2]  ) * 2.0; // S=4*Quat(1) 
	   Quat(0) = (RotationMatrix[1][2] - RotationMatrix[2][1]) / S;
	   Quat(1) = 0.25 * S;
	   Quat(2) = (RotationMatrix[0][1] + RotationMatrix[1][0]) / S; 
	   Quat(3) = (RotationMatrix[0][2]  + RotationMatrix[2][0]) / S; 
	} else if ((RotationMatrix[1][1]  > RotationMatrix[2][2] )) { 
	   S = sqrt( 1.0 + RotationMatrix[1][1] - RotationMatrix[0][0] - RotationMatrix[2][2]  ) * 2.0; // S=4*Quat(2)
	   Quat(0) = (RotationMatrix[0][2]  - RotationMatrix[2][0]) / S;
	   Quat(1) = (RotationMatrix[0][1] + RotationMatrix[1][0]) / S; 
	   Quat(2) = 0.25 * S;
	   Quat(3) = (RotationMatrix[1][2] + RotationMatrix[2][1]) / S; 
	} else { 
	   S = sqrt( 1.0 + RotationMatrix[2][2]  - RotationMatrix[0][0] - RotationMatrix[1][1] ) * 2.0; // S=4*Quat(3)
	   Quat(0) = (RotationMatrix[0][1] - RotationMatrix[1][0]) / S;
	   Quat(1) = (RotationMatrix[0][2]  + RotationMatrix[2][0]) / S; 
	   Quat(2) = (RotationMatrix[1][2] + RotationMatrix[2][1]) / S; 
	   Quat(3) = 0.25 * S;
	} 

	printf("\n Quat = ");
	for( i = 0 ; i < 4; i++)
		{
		printf(" %.1lf ",Quat(i));

	}
	printf("\n" );
	*/	
return Quat;
}





Matrix Quat2RotMat(Vector Quat)
{

Matrix RotMat(3,3);
RotMat.zero();

   double  xx      = Quat(1) * Quat(1);
   double  xy      = Quat(1) * Quat(2);
   double  xz      = Quat(1) * Quat(3);
   double  xw      = Quat(1) * Quat(0);

   double  yy      = Quat(2) * Quat(2);
   double  yz      = Quat(2) * Quat(3);
   double  yw      = Quat(2) * Quat(0);


   double  zz      = Quat(3) * Quat(3);
   double  zw      = Quat(3) * Quat(0);

RotMat[0][0]  = 1.0 - 2.0 * ( yy + zz );
RotMat[0][1]  =     2.0 * ( xy - zw );
RotMat[0][2] =     2.0 * ( xz + yw );

RotMat[1][0]  =     2.0 * ( xy + zw );
RotMat[1][1]  = 1.0 - 2.0 * ( xx + zz );
RotMat[1][2]  =     2.0 * ( yz - xw );

RotMat[2][0]  =     2.0 * ( xz - yw );
RotMat[2][1]  =     2.0 * ( yz + xw );
RotMat[2][2] = 1.0 - 2.0 * ( xx + yy );

//RotMat[0][3]  = m13 = m23 = m30 = m31 = m32 = 0;
//RotMat[3][3] = 1.0;


return RotMat;
}

Matrix Quat2RotMat4x4(Vector Quat)
{

Matrix RotMat(4,4);
RotMat.eye();

   double  xx      = Quat(1) * Quat(1);
   double  xy      = Quat(1) * Quat(2);
   double  xz      = Quat(1) * Quat(3);
   double  xw      = Quat(1) * Quat(0);

   double  yy      = Quat(2) * Quat(2);
   double  yz      = Quat(2) * Quat(3);
   double  yw      = Quat(2) * Quat(0);


   double  zz      = Quat(3) * Quat(3);
   double  zw      = Quat(3) * Quat(0);

RotMat[0][0]  = 1.0 - 2.0 * ( yy + zz );
RotMat[0][1]  =     2.0 * ( xy - zw );
RotMat[0][2] =     2.0 * ( xz + yw );

RotMat[1][0]  =     2.0 * ( xy + zw );
RotMat[1][1]  = 1.0 - 2.0 * ( xx + zz );
RotMat[1][2]  =     2.0 * ( yz - xw );

RotMat[2][0]  =     2.0 * ( xz - yw );
RotMat[2][1]  =     2.0 * ( yz + xw );
RotMat[2][2] = 1.0 - 2.0 * ( xx + yy );

//RotMat[0][3]  = m13 = m23 = m30 = m31 = m32 = 0;
//RotMat[3][3] = 1.0;


return RotMat;
}







Vector dcm2axis(Matrix R)
{
	double x,y,z,r,r11,r22,r33, den;
	double  theta;
	double  pi = M_PI;
	Vector v(4);

	x=R(2,1)-R(1,2);
	y=R(0,2)-R(2,0);
	z=R(1,0)-R(0,1);
	
	r11=R(0,0);
	r22=R(1,1);
	r33=R(2,2);
	den=r11+r22+r33-1;

	r = sqrt(x*x+y*y+z*z);

	theta = atan2(r ,den);


	if(abs(fmod(theta,pi)*1000.0)>1.0)
	{
		v(0)=x/r;
		v(1)=y/r;
		v(2)=z/r;
		v(3)=theta;
	}
	else
		v=zeros(4);

	return v;
}

Matrix axis2dcm(Vector v)
{

	double c = cos(v(3));
	double s = sin(v(3));
	double C = 1-c;

	double xs,ys,zs,xC,yC,zC,xyC,yzC,zxC;

	Matrix R(3,3);

xs = v(0)*s;   
ys = v(1)*s;   
zs = v(2)*s;
xC = v(0)*C;   
yC = v(1)*C;   
zC = v(2)*C;
xyC = v(0)*yC;
yzC = v(1)*zC;
zxC = v(2)*xC;

R(0,0) = v(0)*xC+c;
R(0,1) = xyC-zs;
R(0,2) = zxC+ys;
R(1,0) = xyC+zs;
R(1,1) = v(1)*yC+c;
R(1,2) = yzC-xs;
R(2,0) = zxC-ys;
R(2,1) = yzC+xs;
R(2,2) = v(2)*zC+c;


/*
xs = v(1)*s;   
ys = v(2)*s;   
zs = v(3)*s;
xC = v(1)*C;   
yC = v(2)*C;   
zC = v(3)*C;
xyC = v(1)*yC;
yzC = v(2)*zC;
zxC = v(3)*xC;

R(1,1) = v(1)*xC+c;
R(1,2) = xyC-zs;
R(1,3) = zxC+ys;
R(2,1) = xyC+zs;
R(2,2) = v(2)*yC+c;
R(2,3) = yzC-xs;
R(3,1) = zxC-ys;
R(3,2) = yzC+xs;
R(3,3) = v(3)*zC+c;
*/
return R;
}


Matrix axis2dcm4x4(Vector v)
{

	double c = cos(v(3));
	double s = sin(v(3));
	double C = 1-c;

	double xs,ys,zs,xC,yC,zC,xyC,yzC,zxC;

	Matrix R(4,4);
	R.eye();

xs = v(0)*s;   
ys = v(1)*s;   
zs = v(2)*s;
xC = v(0)*C;   
yC = v(1)*C;   
zC = v(2)*C;
xyC = v(0)*yC;
yzC = v(1)*zC;
zxC = v(2)*xC;

R(0,0) = v(0)*xC+c;
R(0,1) = xyC-zs;
R(0,2) = zxC+ys;
R(1,0) = xyC+zs;
R(1,1) = v(1)*yC+c;
R(1,2) = yzC-xs;
R(2,0) = zxC-ys;
R(2,1) = yzC+xs;
R(2,2) = v(2)*zC+c;


/*
xs = v(1)*s;   
ys = v(2)*s;   
zs = v(3)*s;
xC = v(1)*C;   
yC = v(2)*C;   
zC = v(3)*C;
xyC = v(1)*yC;
yzC = v(2)*zC;
zxC = v(3)*xC;

R(1,1) = v(1)*xC+c;
R(1,2) = xyC-zs;
R(1,3) = zxC+ys;
R(2,1) = xyC+zs;
R(2,2) = v(2)*yC+c;
R(2,3) = yzC-xs;
R(3,1) = zxC-ys;
R(3,2) = yzC+xs;
R(3,3) = v(3)*zC+c;
*/
return R;
}




Vector CrossProd(Vector vector1, Vector vector2)
{
	Vector ris(3);

	ris[0]=vector1[1] * vector2[2] - vector1[2] * vector2[1];
	ris[1]=vector1[2] * vector2[0] - vector1[0] * vector2[2];
	ris[2]=vector1[0] * vector2[1] - vector1[1] * vector2[0];

	return ris; 
}

double DotProd(Vector vector1, Vector vector2)
{
	double ris=0.0;

	for (int i = 0; i < min(vector1.size(), vector2.size()); i++)
	ris = ris + vector1[i] * vector2[i];

	return ris;
}

Matrix EulerZYZ(double &z1, double &y, double &z2)
{
	Matrix phiMatrix(3,3);
	phiMatrix[0][0] = cos(z1)*cos(y)*cos(z2)-sin(z1)*sin(z2);
	phiMatrix[0][1] = -cos(z1)*cos(y)*sin(z2)-sin(z1)*cos(z2);
	phiMatrix[0][2] = cos(z1)*sin(y);
	phiMatrix[1][0] = sin(z1)*cos(y)*cos(z2)+cos(z1)*sin(z2);
	phiMatrix[1][1] = -sin(z1)*cos(y)*sin(z2)+cos(z1)*cos(z2);
	phiMatrix[1][2] = sin(z1)*sin(y);
	phiMatrix[2][0] = -sin(y)*cos(z2);
	phiMatrix[2][1] = sin(y)*sin(z2);
	phiMatrix[2][2] = cos(y);

	return phiMatrix;
	
}



//product 3x3 matrices
Matrix MProduct(Matrix &Left,Matrix &Right)
{
	int i,i2,i3;
	Matrix Data(3,3);
	for(i=0;i<3;i++)
		for(i2=0;i2<3;i2++)
		{
			double Total = 0.0;
			for(i3=0;i3<3;i3++)
			{
				Total += Left[i][i3] * Right[i3][i2];
			}
//			printf("\n***Total[%d][%d] = %lf",i,i2,Total);
			Data[i][i2] = Total;
		}
		return Data;
}

Matrix MProduct6(Matrix &Left,Matrix &Right)
{
	int i,i2,i3;
	Matrix Data(3,3);
	for(i=0;i<6;i++)
		for(i2=0;i2<6;i2++)
		{
			double Total = 0.0;
			for(i3=0;i3<6;i3++)
			{
				Total += Left[i][i3] * Right[i3][i2];
			}
//			printf("\n***Total[%d][%d] = %lf",i,i2,Total);
			Data[i][i2] = Total;
		}
		return Data;
}

Matrix MatProduct3(Matrix &Left,Matrix &Right)
{
	int i,i2,i3;
	Matrix Data(3,3);
	for(i=0;i<3;i++)
		for(i2=0;i2<3;i2++)
		{
			double Total = 0.0;
			for(i3=0;i3<3;i3++)
			{
				Total += Left[i][i3] * Right[i3][i2];
			}
//			printf("\n***Total[%d][%d] = %lf",i,i2,Total);
			Data[i][i2] = Total;
		}
		return Data;
}


Matrix MatProduct6(Matrix &Left,Matrix &Right)
{
	int i,i2,i3;
	Matrix Data(6,6);
	for(i=0;i<6;i++)
		for(i2=0;i2<6;i2++)
		{
			double Total = 0.0;
			for(i3=0;i3<6;i3++)
			{
				Total += Left[i][i3] * Right[i3][i2];
			}
//			printf("\n***Total[%d][%d] = %lf",i,i2,Total);
			Data[i][i2] = Total;
		}
		return Data;
}



Matrix ZRot(Matrix &OldFrame,double phi)
{
	Matrix phiMatrix(3,3),TmpMatrix2(3,3);
	phiMatrix[0][0] = cos(phi);
	phiMatrix[0][1] = -sin(phi);
	phiMatrix[0][2] = 0.0;
	phiMatrix[1][0] = sin(phi);
	phiMatrix[1][1] = cos(phi);
	phiMatrix[1][2] = 0.0;
	phiMatrix[2][0] = 0.0;
	phiMatrix[2][1] = 0.0;
	phiMatrix[2][2] = 1.0;

	return MProduct(phiMatrix,OldFrame);
}

Matrix YRot(Matrix &OldFrame,double theta)
{
	Matrix thetaMatrix(3,3);

	thetaMatrix[0][0] = cos(theta);
	thetaMatrix[0][1] = 0.0;;
	thetaMatrix[0][2] = sin(theta);
	thetaMatrix[1][0] = 0.0;
	thetaMatrix[1][1] = 1.0;
	thetaMatrix[1][2] = 0.0;
	thetaMatrix[2][0] = -sin(theta);
	thetaMatrix[2][1] = 0.0;
	thetaMatrix[2][2] = cos(theta);

	return MProduct(thetaMatrix,OldFrame);
}

Matrix XRot(Matrix &OldFrame,double psi)
{
	Matrix psiMatrix(3,3);

	psiMatrix[0][0] = 1.0;
	psiMatrix[0][1] = 0.0;;
	psiMatrix[0][2] = 0.0;
	psiMatrix[1][0] = 0.0;
	psiMatrix[1][1] = cos(psi);
	psiMatrix[1][2] = -sin(psi);
	psiMatrix[2][0] = 0.0;
	psiMatrix[2][1] = sin(psi);
	psiMatrix[2][2] = cos(psi);

	return MProduct(psiMatrix,OldFrame);
}





//Vector-Matrix dimension 3 product
Vector VMProduct(Vector &v,Matrix &m) 
{
     Vector temp(3);
     int col;
     
     for (col = 0; col < 3; col++)
     {
         temp[col] = v[0] * m[0][col]
           + v[1] * m[1][col]
           + v[2] * m[2][col];
          
     }
     return temp;

}


//Matrix-Vector dimension 3 product
 Vector MVProduct3 (Matrix &m, Vector &v)
 {
     Vector temp(3);
     int row, col;
     
     for (row = 0; row < 3; row++)
     {
		 temp[row]=0.0;
		 for (col = 0; col < 3; col++)
			{
			 
			 temp[row]+= m[row][col]*v[col];
		 }
			 /*
         temp[row] = m[row][0] * v[0]
           + m[row][1] * v[1]
           + m[row][2] * v[2];
		   */
     }
     return temp;
 }
 

 Vector MVProduct6 (Matrix &m, Vector &v)
 {
     Vector temp(6);
     int row, col;
     
     for (row = 0; row < 6; row++)
     {
		 temp[row]=0.0;
		 for (col = 0; col < 6; col++)
			{
			 
			 temp[row]+= m[row][col]*v[col];
		 }
			 /*
         temp[row] = m[row][0] * v[0]
           + m[row][1] * v[1]
           + m[row][2] * v[2];
		   */
     }
     return temp;
 }

Matrix EulerRotation(Matrix &OldFrame,double phi,double theta,double psi)
{
	Matrix phiMatrix(3,3),thetaMatrix(3,3),psiMatrix(3,3),TmpMatrix(3,3),TmpMatrix2(3,3);

	phiMatrix[0][0] = cos(phi);
	phiMatrix[0][1] = -sin(phi);
	phiMatrix[0][2] = 0.0;
	phiMatrix[1][0] = sin(phi);
	phiMatrix[1][1] = cos(phi);
	phiMatrix[1][2] = 0.0;
	phiMatrix[2][0] = 0.0;
	phiMatrix[2][1] = 0.0;
	phiMatrix[2][2] = 1.0;


	thetaMatrix[0][0] = cos(theta);
	thetaMatrix[0][1] = 0.0;;
	thetaMatrix[0][2] = sin(theta);
	thetaMatrix[1][0] = 0.0;
	thetaMatrix[1][1] = 1.0;
	thetaMatrix[1][2] = 0.0;
	thetaMatrix[2][0] = -sin(theta);
	thetaMatrix[2][1] = 0.0;
	thetaMatrix[2][2] = cos(theta);

	psiMatrix[0][0] = cos(psi);
	psiMatrix[0][1] = -sin(psi);
	psiMatrix[0][2] = 0.0;
	psiMatrix[1][0] = sin(psi);
	psiMatrix[1][1] = cos(psi);
	psiMatrix[1][2] = 0.0;
	psiMatrix[2][0] = 0.0;
	psiMatrix[2][1] = 0.0;
	psiMatrix[2][2] = 1.0;

	TmpMatrix=MProduct(phiMatrix,thetaMatrix);
/*	printf("\n");
	for(	int i=0;i<3;i++)
	{
		for(	int j=0;j<3;j++)
			{
		printf("M[%d][%d]=%lf",i,j,TmpMatrix[i][j]);
		printf("psi[%d][%d]=%lf",i,j,psiMatrix[i][j]);
		}
		printf("\n");
	}
*/	
	TmpMatrix2=MProduct(TmpMatrix,psiMatrix);
 /*	printf("\n");
	for(i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
			{
		printf("M2[%d][%d]=%lf",i,j,TmpMatrix2[i][j]);
		}
		printf("\n");
	}
*/	
	Matrix TmpMatrix3 = MProduct(TmpMatrix2,OldFrame);
/*	printf("\n");
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
			{
		printf("M3[%d][%d]=%lf",i,j,TmpMatrix3[i][j]);
		}
		printf("\n");
	}
*/	Matrix TmpMatrix4 = MProduct(OldFrame,TmpMatrix2);
/*		printf("\n");
	for(i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
			{
		printf("M4[%d][%d]=%lf",i,j,TmpMatrix4[i][j]);
		}
		printf("\n");
	}
*/	
	return MProduct(TmpMatrix2,OldFrame);


}



Vector Traslation (Vector &OldFrame, Vector &Tras)
{
	Vector NewFrame(3);
	
	for(int i=0;i<3;i++)
	{
		NewFrame[i] = OldFrame[i] + Tras[i];
	}

	return NewFrame;
}



/*******Start PumaResources********************/

class Puma200Resources
{
private:
    Puma200Resources (const Puma200Resources&);
    void operator= (const Puma200Resources&);

public:
	 Puma200Resources ();
	 ~Puma200Resources ();


 	
	double *OldPosition;
	double *NewPosition;

	double OldTime;
	double NewTime;




};


Puma200Resources::Puma200Resources()
{

 	OldPosition = new double[6];
	NewPosition = new double[6];


	double A2 =	203.2;
	double D3 =	126.24;
	double A3 =	  0.0;
	double D4 =	203.20;



	
//	OldTime = Time::now();
	OldTime = time(0);
	NewTime = time(0);

	for(int i=0 ; i<6 ; i++)
	{
		OldPosition[i] = 0.0;
		NewPosition[i] = 0.0;
	}

}



Puma200Resources::~Puma200Resources()
{

 	delete []	OldPosition;
	delete []	NewPosition;
}

/*******End PumaResources********************/


inline Puma200Resources& RES(void *res) { return *(Puma200Resources *)res; }


PumaKinematics::PumaKinematics() {


puma_resources = (void*) new Puma200Resources;
ACE_ASSERT (puma_resources != NULL);


}

PumaKinematics::~PumaKinematics()
{

if (puma_resources != NULL) {
delete (Puma200Resources *)puma_resources;
}
puma_resources = NULL;

}



//this can give you the kiinematics fo each joint
Matrix PumaKinematics::FwdPuma200Kin(double *DEGangle, int lastjoint)
{

				int ii;
	double *angle = new double[6];
	for(ii = 0 ; ii < 6; ii++)
		{
		angle[ii]=DEGangle[ii]*DEGTORAD;
//		printf("\nDEGangle[ii]=%lf",DEGangle[ii]);
	}



	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);

	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();

	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;


/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/

	if (lastjoint==1)
	T=T1;
	if (lastjoint==2)
	T=T1*T2;
	if (lastjoint==3)
	T=T1*T2*T3;
	if (lastjoint==4)
	T=T1*T2*T3*T4;
	if (lastjoint==5)
	T=T1*T2*T3*T4*T5;
	if (lastjoint==6)
	T=T1*T2*T3*T4*T5*T6;

	return T;

/*
	Vector EndEffPos;

	EndEffPos.resize(6);


	double px = EndEffPos[0] = T[0][3];
	double py = EndEffPos[1] = T[1][3];
	double pz = EndEffPos[2] = T[2][3];

	double alpha, beta, gamma;

	//ZYZ Rotation notation
	beta  = atan2(sqrt(T[0][2] * T[0][2] + T[1][2] * T[1][2]),T[2][2]);
//	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	alpha = atan2(T[1][2],T[0][2]);
	gamma = atan2(T[2][1] ,-T[2][0]);

		if (beta == 0.0)
	{
		 alpha = 0.0;
		 gamma = atan2(-T[0][1] ,T[0][0] );
	}

	if (beta == 180.0 * DEGTORAD)
	{
		alpha = 0.0;
		gamma = atan2(T[0][1],-T[0][0] );
	}



	EndEffPos[3] = alpha;
	EndEffPos[4] = beta;
	EndEffPos[5] = gamma;


 return EndEffPos;
*/
}



Matrix PumaKinematics::FwdPuma200Kin(double *DEGangle)
{
			int ii;
	double *angle = new double[6];
	for(ii = 0 ; ii < 6; ii++)
		{
		angle[ii]=DEGangle[ii]*DEGTORAD;
	//	printf("\nDEGangle[ii]=%lf",DEGangle[ii]);
	}

	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);
	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();

	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;



/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/



	T=T1*T2*T3*T4*T5*T6;


return T;

/*
	Vector EndEffPos;

	EndEffPos.resize(6);


	double px = EndEffPos[0] = T[0][3];
	double py = EndEffPos[1] = T[1][3];
	double pz = EndEffPos[2] = T[2][3];


	double alpha, beta, gamma;
	beta  = atan2(sqrt(T[0][2] * T[0][2] + T[1][2] * T[1][2]),T[2][2]);
//	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	alpha = atan2(T[1][2],T[0][2]);
	gamma = atan2(T[2][1] ,-T[2][0]);
	
	if (beta == 0.0)
	{
		alpha = 0.0;
		gamma = atan2(-T[0][1] ,T[0][0] );
	}

	if (beta == 180.0 * DEGTORAD)
	{
		alpha = 0.0;
		gamma = atan2(T[0][1],-T[0][0] );
	}
*/
/*
	//ZYZ Rotation yoshikawa notation
	alpha = atan2(T[1][2],T[0][2]);
	beta  = atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	gamma = atan2(T[2][1],-T[2][0]);

*/
/*



	
	EndEffPos[3] = alpha;
	EndEffPos[4] = beta;
	EndEffPos[5] = gamma;

 return EndEffPos;
*/
}


Matrix PumaKinematics::FwdPuma200Kin(Vector &DEGangle, int lastjoint)
{
			int ii;
	Vector angle(6);;
	for(ii = 0 ; ii < 6; ii++)
		{
		angle[ii]=DEGangle[ii]*DEGTORAD;
//		printf("\nDEGangle[ii]=%lf",DEGangle[ii]);
	}

	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);

	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();

	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;


/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/

	if (lastjoint==1)
	T=T1;
	if (lastjoint==2)
	T=T1*T2;
	if (lastjoint==3)
	T=T1*T2*T3;
	if (lastjoint==4)
	T=T1*T2*T3*T4;
	if (lastjoint==5)
	T=T1*T2*T3*T4*T5;
	if (lastjoint==6)
	T=T1*T2*T3*T4*T5*T6;

	return T;

/*
	Vector EndEffPos;

	EndEffPos.resize(6);


	double px = EndEffPos[0] = T[0][3];
	double py = EndEffPos[1] = T[1][3];
	double pz = EndEffPos[2] = T[2][3];

	double alpha, beta, gamma;

	//ZYZ Rotation notation
	beta  = atan2(sqrt(T[0][2] * T[0][2] + T[1][2] * T[1][2]),T[2][2]);
//	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	alpha = atan2(T[1][2],T[0][2]);
	gamma = atan2(T[2][1] ,-T[2][0]);

		if (beta == 0.0)
	{
		 alpha = 0.0;
		 gamma = atan2(-T[0][1] ,T[0][0] );
	}

	if (beta == 180.0 * DEGTORAD)
	{
		alpha = 0.0;
		gamma = atan2(T[0][1],-T[0][0] );
	}



	EndEffPos[3] = alpha;
	EndEffPos[4] = beta;
	EndEffPos[5] = gamma;


 return EndEffPos;
*/
}



Matrix PumaKinematics::FwdPuma200Kin(Vector &DEGangle)
{
				int ii;
	Vector angle(6);
	for(ii = 0 ; ii < 6; ii++)
		{
		angle[ii]=DEGangle[ii]*DEGTORAD;
//		printf("\nDEGangle[ii]=%lf",DEGangle[ii]);
	}


	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);
	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();

	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;

/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/



	T=T1*T2*T3*T4*T5*T6;


return T;

/*
	Vector EndEffPos;

	EndEffPos.resize(6);


	double px = EndEffPos[0] = T[0][3];
	double py = EndEffPos[1] = T[1][3];
	double pz = EndEffPos[2] = T[2][3];


	double alpha, beta, gamma;
	beta  = atan2(sqrt(T[0][2] * T[0][2] + T[1][2] * T[1][2]),T[2][2]);
//	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	alpha = atan2(T[1][2],T[0][2]);
	gamma = atan2(T[2][1] ,-T[2][0]);
	
	if (beta == 0.0)
	{
		alpha = 0.0;
		gamma = atan2(-T[0][1] ,T[0][0] );
	}

	if (beta == 180.0 * DEGTORAD)
	{
		alpha = 0.0;
		gamma = atan2(T[0][1],-T[0][0] );
	}
*/
/*
	//ZYZ Rotation yoshikawa notation
	alpha = atan2(T[1][2],T[0][2]);
	beta  = atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	gamma = atan2(T[2][1],-T[2][0]);

*/
/*



	
	EndEffPos[3] = alpha;
	EndEffPos[4] = beta;
	EndEffPos[5] = gamma;

 return EndEffPos;
*/
}




Vector PumaKinematics::GimmeZi(double *angle, int Rnumber)//angle in RAD
{




	Vector Zi(3);
	Vector Xi(3);
	Vector ez(3);
	ez.zero();

	Vector ex(3);
	ex.zero();

	Matrix R(3,3);

	ez[2]=1.0;
	ex[0]=1.0;


	R=GimmeR(angle,Rnumber);
	Zi = R * ez;

	R=GimmeR(angle,Rnumber);
	Xi = R * ex;

/*Debug printing
	printf("\n numero %d\n",Rnumber);
	for(int i = 0 ; i < 3; i++)
		{
		printf("\n");
		for(int j = 0 ; j < 3; j++)
			{		
				printf("\n R[%d][%d]= %lf",i,j,R[i][j]);
	
			}
		}

		printf("\n\n");
		for(int j = 0 ; j < 3; j++)
			{		
				printf("\n ez[%d]= %lf",j,ez[j]);
	
			}
				printf("\n\n");
				for(int j = 0 ; j < 3; j++)
			{		
			printf("\n Zi[%d]= %lf",j,Zi[j]);

	
			}
			printf("\n\n");
				for( j = 0 ; j < 3; j++)
			{		
				printf("\n Xi[%d]= %lf",j,Xi[j]);
	
			}
*/
	return Zi;

}

Vector PumaKinematics::GimmePos(double *angle,int Rnumber)//angle in RAD
{
	
	Vector Pos4(4);
	Vector Pos(3);
	Vector e(4);
	e.zero();


	Matrix R(4,4);

	e[3]=1.0;


	R=GimmeR4(angle,Rnumber);
	Pos4 = R * e;

	Pos(0)=Pos4(0);
	Pos(1)=Pos4(1);
	Pos(2)=Pos4(2);


	return Pos;

}





Matrix PumaKinematics::GimmeR4(double *angle,int Rnumber)//angle in RAD
{

	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);
	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();



	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;


/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/





	Matrix MyR4(3,3);

	if (Rnumber==1)
		MyR4=T1;
	
	if (Rnumber==2)
		MyR4=T1*T2;
	
	if (Rnumber==3)
		MyR4=T1*T2*T3;

	if (Rnumber==4)
		MyR4=T1*T2*T3*T4;

	if (Rnumber==5)
		MyR4=T1*T2*T3*T4*T5;
	
	if (Rnumber==6)
		MyR4=T1*T2*T3*T4*T5*T6;

/*for debugging
		for(int i = 0 ; i < 4; i++)
		{
		printf("\n\n");
		for(int j = 0 ; j < 4; j++)
			{		
				printf("MyR4[%d][%d]= %lf",i,j,MyR4[i][j]);
			}
		}
*/

return MyR4;
}






Matrix PumaKinematics::GimmeR(double *angle,int Rnumber)//angle in RAD
{

	Matrix T1(4,4);
	Matrix T2(4,4);
	Matrix T3(4,4);
	Matrix T4(4,4);
	Matrix T5(4,4);
	Matrix T6(4,4);
	Matrix T(4,4);
	T1.zero();
	T2.zero();
	T3.zero();
	T4.zero();
	T5.zero();
	T6.zero();
	T.zero();



	T1[0][0] = cos(angle[0]);
	T1[0][1] =-sin(angle[0]);
	T1[1][0] = sin(angle[0]);
	T1[1][1] = cos(angle[0]);
	T1[2][2] = 1.0;
	T1[3][3] = 1.0;
	T1[2][3] = 330.3;

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] =  sin(angle[1]);
	T2[2][1] =  cos(angle[1]);
	T2[1][2] = -1.0;
	T2[3][3] =  1.0;
	T2[1][3] = -124.46;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  198.12;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  222.25;


	T5[0][0] =  cos(angle[4]);
	T5[0][1] = -sin(angle[4]);
	T5[2][0] =  sin(angle[4]);
	T5[2][1] =  cos(angle[4]);
	T5[1][2] = -1.0;
	T5[3][3] =  1.0;


	T6[0][0] =  cos(angle[5]);
	T6[0][1] = -sin(angle[5]);
	T6[2][0] = -sin(angle[5]);
	T6[2][1] = -cos(angle[5]);
	T6[1][2] =  1.0;
	T6[3][3] =  1.0;
	T6[1][3] =  55.88;
//	T6[1][3] =  0.0;

/*Debug printing 

	int ii,jj;

	for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		
	for(jj = 0 ; jj < 4; jj++)
	{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
	

	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("\n T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}

	
	  for(jj = 0 ; jj < 4; jj++)
		{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
		
		  for(jj = 0 ; jj < 4; jj++)
		  {
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
				printf("\n T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
			}
		}
*/





	Matrix MyR4(3,3);

	if (Rnumber==1)
		MyR4=T1;
	
	if (Rnumber==2)
		MyR4=T1*T2;
	
	if (Rnumber==3)
		MyR4=T1*T2*T3;

	if (Rnumber==4)
		MyR4=T1*T2*T3*T4;

	if (Rnumber==5)
		MyR4=T1*T2*T3*T4*T5;
	
	if (Rnumber==6)
		MyR4=T1*T2*T3*T4*T5*T6;

/*for debugging
		for(int i = 0 ; i < 4; i++)
		{
		printf("\n\n");
		for(int j = 0 ; j < 4; j++)
			{		
				printf("MyR4[%d][%d]= %lf",i,j,MyR4[i][j]);
			}
		}
*/


	Matrix MyR3(3,3);


	for(int i = 0 ; i < 3; i++)
		{
//		printf("\n\n");
		for(int j = 0 ; j < 3; j++)
			{		
				MyR3[i][j]=MyR4[i][j];
//				printf("MyR3[%d][%d]= %lf",i,j,MyR3[i][j]);
			}
		}

return MyR3;
}


 Matrix PumaKinematics::PumaJac(double *angle)//in DEGREES not in RAD
{
	
//	Vector angle(6);

	double *ANGLE = new double[6];
	for(int j = 0 ; j < 6; j++)ANGLE[j]=angle[j]*DEGTORAD;


	Matrix PumaJac;
	PumaJac.resize(6,6);
	PumaJac.zero(); 


	Vector posit1(3);
	Vector posit2(3);
	Vector posit3(3);
	Vector posit4(3);
	Vector posit5(3);
	Vector posit6(3);
	Vector positE(3);
	
	Vector posit1tmp(6);
	Vector posit2tmp(6);
	Vector posit3tmp(6);
	Vector posit4tmp(6);
	Vector posit5tmp(6);
	Vector posit6tmp(6);
	Vector positEtmp(6);
	
	Vector z1(3);
	Vector z2(3);
	Vector z3(3);
	Vector z4(3);
	Vector z5(3);
	Vector z6(3);
	Vector z7(3);
	Vector z8(3);

	Vector tmp1(3);
	Vector tmp2(3);
	Vector tmp3(3);
	Vector tmp4(3);
	Vector tmp5(3);
	Vector tmp6(3);


	//positE=FwdPuma200Kin(ANGLE);
	//positEtmp=FwdPuma200Kin(ANGLE);
/*
	posit1tmp=FwdPuma200Kin(ANGLE,1);
	posit2tmp=FwdPuma200Kin(ANGLE,2);
	posit3tmp=FwdPuma200Kin(ANGLE,3);
	posit4tmp=FwdPuma200Kin(ANGLE,4);
	posit5tmp=FwdPuma200Kin(ANGLE,5);
	posit6tmp=FwdPuma200Kin(ANGLE,6);
*/
	posit1=GimmePos(ANGLE,1);
	posit2=GimmePos(ANGLE,2);
	posit3=GimmePos(ANGLE,3);
	posit4=GimmePos(ANGLE,4);
	posit5=GimmePos(ANGLE,5);
	posit6=GimmePos(ANGLE,6);



/*
	for( jj = 0 ; jj < 3; jj++)
	{
		 posit1(jj) = posit1tmp(jj);
		 posit2(jj) = posit2tmp(jj);
		 posit3(jj) = posit3tmp(jj);
		 posit4(jj) = posit4tmp(jj);
		 posit5(jj) = posit5tmp(jj);
		 posit6(jj) = posit6tmp(jj);
		 positE(jj) = positEtmp(jj);
	}
*/

	z1 =  GimmeZi(ANGLE, 1);
	z2 =  GimmeZi(ANGLE, 2);
	z3 =  GimmeZi(ANGLE, 3);
	z4 =  GimmeZi(ANGLE, 4);
	z5 =  GimmeZi(ANGLE, 5);
	z6 =  GimmeZi(ANGLE, 6);

/*
	posit1=positE-posit1;
	posit2=positE-posit2;
	posit3=positE-posit3;
	posit4=positE-posit4;
	posit5=positE-posit5;
	posit6=positE-posit6;
*/
	posit1=posit6-posit1;
	posit2=posit6-posit2;
	posit3=posit6-posit3;
	posit4=posit6-posit4;
	posit5=posit6-posit5;
	posit6=posit6-posit6;
/*
			printf("\n posit1\n");
		for(  ii = 0 ; ii < 6; ii++)
			{
				printf(" %lf", posit1[ii]  );
			}

					printf("\n posit2\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit2[ii]  );
			}
					printf("\n posit3\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit3[ii]  );
			}
					printf("\n posit4\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit4[ii]  );
			}
					printf("\n posit5\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit5[ii]  );
			}
					printf("\n posit6\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit6[ii]  );
			}
*/
	tmp1 = CrossProd(z1 , posit1);
	tmp2 = CrossProd(z2 , posit2);
	tmp3 = CrossProd(z3 , posit3);
	tmp4 = CrossProd(z4 , posit4);
	tmp5 = CrossProd(z5 , posit5);
	tmp6 = CrossProd(z6 , posit6);


/*
			printf("\n tmp1\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp1[ii]  );
			}

					printf("\n tmp2\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp2[ii]  );
			}
					printf("\n tmp3\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp3[ii]  );
			}
					printf("\n tmp4\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp4[ii]  );
			}
					printf("\n tmp5\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp5[ii]  );
			}
					printf("\n tmp6\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp6[ii]  );
			}
*/


	for(int i = 0 ; i < 6; i++)
	{
//		printf("\n");
		for(int j = 0 ; j < 6; j++)
			{
				int ii=j;
				int jj=i;

//				printf("\n i=[%d]j=[%d]",i,j);
				if(i==0)
				{
					if(j<3)		
					{
						PumaJac[j][i]=tmp1[j];
//						printf("\n ****PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
					}
					if(j>2)	{	PumaJac[j][i]=z1[j-3];	}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);

				if(i==1)
				{
					if(j<3)	{	PumaJac[j][i]=tmp2[j];}
					if(j>2)	{	PumaJac[j][i]=z2[j-3];}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);

				if(i==2)
				{
					if(j<3)	{	PumaJac[j][i]=tmp3[j];}
					if(j>2)	{	PumaJac[j][i]=z3[j-3];}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);



				if(i==3)
				{
					if(j<3)	{	PumaJac[j][i]=tmp4[j];}
					if(j>2)	{	PumaJac[j][i]=z4[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

				if(i==4)
				{
					if(j<3)	{	PumaJac[j][i]=tmp5[j];}
					if(j>2)	{	PumaJac[j][i]=z5[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

				if(i==5)
				{
					if(j<3)	{	PumaJac[j][i]=tmp6[j];}
					if(j>2)	{	PumaJac[j][i]=z6[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

			}
	}


	return PumaJac;

}




 Matrix PumaKinematics::PumaJac(Vector &angle)//in DEGREES not in RAD
{
	
//	Vector angle(6);
	double *ANGLE = new double[6];
	for(int jj = 0 ; jj < 6; jj++)ANGLE[jj]=angle[jj]*DEGTORAD;



	Matrix PumaJac;
	PumaJac.resize(6,6);
	PumaJac.zero(); 


	Vector posit1(3);
	Vector posit2(3);
	Vector posit3(3);
	Vector posit4(3);
	Vector posit5(3);
	Vector posit6(3);
	Vector positE(3);
	
	Vector posit1tmp(6);
	Vector posit2tmp(6);
	Vector posit3tmp(6);
	Vector posit4tmp(6);
	Vector posit5tmp(6);
	Vector posit6tmp(6);
	Vector positEtmp(6);
	
	Vector z1(3);
	Vector z2(3);
	Vector z3(3);
	Vector z4(3);
	Vector z5(3);
	Vector z6(3);
	Vector z7(3);
	Vector z8(3);

	Vector tmp1(3);
	Vector tmp2(3);
	Vector tmp3(3);
	Vector tmp4(3);
	Vector tmp5(3);
	Vector tmp6(3);


	//positE=FwdPuma200Kin(ANGLE);
	//positEtmp=FwdPuma200Kin(ANGLE);
/*
	posit1tmp=FwdPuma200Kin(ANGLE,1);
	posit2tmp=FwdPuma200Kin(ANGLE,2);
	posit3tmp=FwdPuma200Kin(ANGLE,3);
	posit4tmp=FwdPuma200Kin(ANGLE,4);
	posit5tmp=FwdPuma200Kin(ANGLE,5);
	posit6tmp=FwdPuma200Kin(ANGLE,6);
*/
	posit1=GimmePos(ANGLE,1);
	posit2=GimmePos(ANGLE,2);
	posit3=GimmePos(ANGLE,3);
	posit4=GimmePos(ANGLE,4);
	posit5=GimmePos(ANGLE,5);
	posit6=GimmePos(ANGLE,6);



/*
	for( jj = 0 ; jj < 3; jj++)
	{
		 posit1(jj) = posit1tmp(jj);
		 posit2(jj) = posit2tmp(jj);
		 posit3(jj) = posit3tmp(jj);
		 posit4(jj) = posit4tmp(jj);
		 posit5(jj) = posit5tmp(jj);
		 posit6(jj) = posit6tmp(jj);
		 positE(jj) = positEtmp(jj);
	}
*/

	z1 =  GimmeZi(ANGLE, 1);
	z2 =  GimmeZi(ANGLE, 2);
	z3 =  GimmeZi(ANGLE, 3);
	z4 =  GimmeZi(ANGLE, 4);
	z5 =  GimmeZi(ANGLE, 5);
	z6 =  GimmeZi(ANGLE, 6);

/*
	posit1=positE-posit1;
	posit2=positE-posit2;
	posit3=positE-posit3;
	posit4=positE-posit4;
	posit5=positE-posit5;
	posit6=positE-posit6;
*/
	posit1=posit6-posit1;
	posit2=posit6-posit2;
	posit3=posit6-posit3;
	posit4=posit6-posit4;
	posit5=posit6-posit5;
	posit6=posit6-posit6;
/*
			printf("\n posit1\n");
		for(  ii = 0 ; ii < 6; ii++)
			{
				printf(" %lf", posit1[ii]  );
			}

					printf("\n posit2\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit2[ii]  );
			}
					printf("\n posit3\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit3[ii]  );
			}
					printf("\n posit4\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit4[ii]  );
			}
					printf("\n posit5\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit5[ii]  );
			}
					printf("\n posit6\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", posit6[ii]  );
			}
*/
	tmp1 = CrossProd(z1 , posit1);
	tmp2 = CrossProd(z2 , posit2);
	tmp3 = CrossProd(z3 , posit3);
	tmp4 = CrossProd(z4 , posit4);
	tmp5 = CrossProd(z5 , posit5);
	tmp6 = CrossProd(z6 , posit6);


/*
			printf("\n tmp1\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp1[ii]  );
			}

					printf("\n tmp2\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp2[ii]  );
			}
					printf("\n tmp3\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp3[ii]  );
			}
					printf("\n tmp4\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp4[ii]  );
			}
					printf("\n tmp5\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp5[ii]  );
			}
					printf("\n tmp6\n");
		for(  ii = 0 ; ii < 3; ii++)
			{
				printf(" %lf", tmp6[ii]  );
			}
*/


	for(int i = 0 ; i < 6; i++)
	{
//		printf("\n");
		for(int j = 0 ; j < 6; j++)
			{
				int ii=j;
				int jj=i;

//				printf("\n i=[%d]j=[%d]",i,j);
				if(i==0)
				{
					if(j<3)		
					{
						PumaJac[j][i]=tmp1[j];
//						printf("\n ****PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
					}
					if(j>2)	{	PumaJac[j][i]=z1[j-3];	}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);

				if(i==1)
				{
					if(j<3)	{	PumaJac[j][i]=tmp2[j];}
					if(j>2)	{	PumaJac[j][i]=z2[j-3];}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);

				if(i==2)
				{
					if(j<3)	{	PumaJac[j][i]=tmp3[j];}
					if(j>2)	{	PumaJac[j][i]=z3[j-3];}
				}
//				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);



				if(i==3)
				{
					if(j<3)	{	PumaJac[j][i]=tmp4[j];}
					if(j>2)	{	PumaJac[j][i]=z4[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

				if(i==4)
				{
					if(j<3)	{	PumaJac[j][i]=tmp5[j];}
					if(j>2)	{	PumaJac[j][i]=z5[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

				if(i==5)
				{
					if(j<3)	{	PumaJac[j][i]=tmp6[j];}
					if(j>2)	{	PumaJac[j][i]=z6[j-3];}
//					printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
				}
				

			}
	}


	return PumaJac;

}







//Converts Radiants to Encoders Values (it takes in consideration the wrist coupling too)
void RadToEnc( double *ang, double *encoders,  MEIMotionControlParameters &_parameters,  double *zeros)
{
	// 6 joints only.
	int i;
	int j;






	for (i = 0; i < 4; i++)
	{
		j = _parameters._axis_map[i];
		encoders[i] = (ang[j] * _parameters._encoderToAngles[i] / (2.0 * PI) + zeros[i]);
	}

	encoders[4] = (ang[_parameters._axis_map[4]] * _parameters._encoderToAngles[4] / (2.0 * PI)) +
			 (ang[_parameters._axis_map[3]] * _parameters._fwdCouple[3] / (2.0 * PI)) +
			  zeros[4];

	
	encoders[5] = (ang[_parameters._axis_map[5]] * _parameters._encoderToAngles[5] / (2.0 * PI)) +
		     (ang[_parameters._axis_map[3]] * _parameters._fwdCouple[4] / (2.0 * PI)) +
			 (ang[_parameters._axis_map[4]] * _parameters._fwdCouple[5] / (2.0 * PI)) +
			  zeros[5];


}


//Converts Encoders Values to Radiants (it takes in consideration the wrist coupling too)
void EncToRad( double *ang, double *encoders,  MEIMotionControlParameters &_parameters,  double *zeros)
{
	// 6 joints only.
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		j = _parameters._axis_map[i];
		ang[j] = (encoders[i] - zeros[i]) * 2.0 * PI / _parameters._encoderToAngles[i];
	}

	double e3, e4, e5;
	e3 = (encoders[3] - zeros[3]) * 2.0 * PI;
	e4 = (encoders[4] - zeros[4]) * 2.0 * PI;   //Mattia082007
	e5 = (encoders[5] - zeros[5]) * 2.0 * PI;

	ang[_parameters._axis_map[4]] =  e4 / _parameters._encoderToAngles[4] +
			    e3 * _parameters._invCouple[3];

	ang[_parameters._axis_map[5]] = e5 / _parameters._encoderToAngles[5] +
			   e3 * _parameters._invCouple[4] + e4 * _parameters._invCouple[5];

/*	//mattia nov 07
	for (i = 0; i < 6; i++)
	{
		ang[j] = -ang[j];
	}
*/


}


double alpha(Matrix &Jac, Vector &deltae)
{
	Matrix JT = Jac.transposed();

	Matrix JJt = Jac*JT;
	Vector JJTdeltae = JJt * deltae;

	double  alpha = dot(deltae,JJTdeltae);
	alpha = alpha / dot(JJTdeltae,JJTdeltae);
	
	return alpha;

}	


double variance(Vector number)
{
   int    i, N;
   double  sum=0.0, sumSquares=0.0;
   double  mean, variance;

   N=number.length();
   // Execute loop N times
   // Start with i=1 and increment on completion of loop. 
   // Exit loop when i = N+1;
   for(i=1; i<=N; i++ )
   {
       sum = sum + number[i];
       sumSquares = sumSquares + number[i]*number[i];
   }

   mean = sum/N;
   variance = sumSquares/N - mean*mean;
   
   printf("\nThe mean of the data is %lf and the variance is %lf", mean, variance );

   return 0;
}

double magnitude(Vector vector)
{
	int size = vector.length();
		double magnitude;
		magnitude=0.0;

		for(int i=0;i<size;i++)
		{
			magnitude += vector(i)*vector(i);
		}
	magnitude = sqrt(magnitude);
	return magnitude;
}

void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}


static void VisGaussJordanSolveDMatrix(Matrix& A)
{
    int n = A.rows(), m = A.cols(), i, j, k;
    
    // Reduce to triangular form
    for (i = 0; i < n; i++) {
        
        // Make sure diagonal entry is non-zero
        if (A[i][i] == 0.0) {
            for (j = i+1; j < n; j++)
                if (A[j][i] != 0.0)   // find non-zero entry
                    break;
             if (j >= n)       // if failed, matrix is singular
	 			assert (0);
            //throw CVisError("matrix is singular", eviserrorOpFailed,
            //         "VisGaussJordanSolveDMatrix()", __FILE__, __LINE__);
            for (k = i; k < m; k++)
                A[i][k] += A[j][k];
        }

        // VisDMatrixScale this row to unity
        double aii_inv = 1.0 / A[i][i];
        A[i][i] = 1.0;
        for (k = i+1; k < m; k++)
            A[i][k] *= aii_inv;

        // Subtract from other rows
        for (j = i+1; j < n; j++) {
            double aij = A[j][i];
            A[j][i] = 0.0;
            for (k = i+1; k < m; k++)
                A[j][k] -= aij * A[i][k];
        }
    }

    // Back substitute
    for (i = n-1; i > 0; i--) {
        for (j = 0; j < i; j++)  {
            double aji = A[j][i];
            A[j][i] = 0.0;
            for (k = n; k < m; k++)
                A[j][k] -= aji * A[i][k];
        }
    }
}






Matrix MatrixInverted(Matrix &A) 
{
	
	int n = A.rows();
	int m = A.cols();


    Matrix matrixInverse;
	matrixInverse.resize(n, m);

    // Use Gauss-Jordan elimination
    int i, j;
    Matrix matrixT(n, 2*n);
    for (i = 0; i < n; i++)       // copy into a single system
        for (j = 0; j < n; j++)
		{
            matrixT[i][j] = A[i][j];
			matrixT[i][j+n] = (i == j);
		}
    VisGaussJordanSolveDMatrix(matrixT);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            matrixInverse[i][j] = matrixT[i][j+n];

return matrixInverse;



}





Matrix AxisAngle2RotMat(Vector K)
{
	Matrix RotMat(4,4);

	RotMat[0][0] = K[0]*K[0]*(1-cos(K[3]))+cos(K[3]);
	RotMat[0][1] = K[0]*K[1]*(1-cos(K[3]))-K[2]*sin(K[3]);
	RotMat[0][2] = K[0]*K[2]*(1-cos(K[3]))+K[1]*sin(K[3]);

	RotMat[1][0] = K[0]*K[1]*(1-cos(K[3]))+K[2]*sin(K[3]);
	RotMat[1][1] = K[1]*K[1]*(1-cos(K[3]))+cos(K[3]);
	RotMat[1][2] = K[1]*K[2]*(1-cos(K[3]))-K[0]*sin(K[3]);

	RotMat[2][0] = K[0]*K[2]*(1-cos(K[3]))-K[1]*sin(K[3]);
	RotMat[2][1] = K[1]*K[2]*(1-cos(K[3]))+K[0]*sin(K[3]);
	RotMat[2][2] = K[2]*K[2]*(1-cos(K[3]))+cos(K[3]);
	return RotMat;

}

Vector RotMat2AxisAngle(Matrix RotMat)
{
	Vector K(4);

	K(3) = acos((RotMat[0][0]+RotMat[1][1]+RotMat[2][2]-1)/2);

	K(0) = ( 1/ (2*sin(K(3)) ) * RotMat[2][1]-RotMat[1][2] );
	K(1) = ( 1/ (2*sin(K(3)) ) * RotMat[0][2]-RotMat[2][0] );
	K(2) = ( 1/ (2*sin(K(3)) ) * RotMat[1][0]-RotMat[0][1] );

	return K;

}


/*
Matrix axis2dcm(Vector v)
{
// gets a rotation matrix R from a axis-angle representation

	Matrix R(4,4);

double c = cos(v(4));
double s = sin(v(4));
double C = 1-c;

double xs = v(1)*s;  
double ys = v(2)*s;  
double zs = v(3)*s;
double xC = v(1)*C;  
double yC = v(2)*C;  
double zC = v(3)*C;
double xyC = v(1)*yC;
double yzC = v(2)*zC;
double zxC = v(3)*xC;

R(1,1) = v(1)*xC+c;
R(1,2) = xyC-zs;
R(1,3) = zxC+ys;
R(2,1) = xyC+zs;
R(2,2) = v(2)*yC+c;
R(2,3) = yzC-xs;
R(3,1) = zxC-ys;
R(3,2) = yzC+xs;
R(3,3) = v(3)*zC+c;

return R;
}


Vector dcm2axis(Matrix R)
{

//converts a rotation matrix R to the axis-angle representation
Vector v(4);

double x = R(3,2)-R(2,3);
double y = R(1,3)-R(3,1);
double z = R(2,1)-R(1,2);
double r = sqrt(x*x+y*y+z*z); // this is 2*sin(theta)
theta = atan2(0.5*r, 0.5*(R(1,1)+R(2,2)+R(3,3)-1));

//if abs(abs(theta)-pi/2) > 1e-4,
if abs(mod(theta,pi)) > 1e-4,
    v(1) = x/r;
    v(2) = y/r;
    v(3) = z/r;
    v(4) = theta;
else
    v = zero();
return v;

}

*/