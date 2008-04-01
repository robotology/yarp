#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/os/all.h> 



#include "PumaKinematics.h"



#include <yarp/MEIMotionControl.h>



using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;





#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>


#define RADTODEG 57.29577951308232100
#define DEGTORAD 0.01745329251994330
#define PI 3.14159265


Vector VectorProd(Vector vector1, Vector vector2)
{

	Vector ris(3);

	ris[0]=vector1[1] * vector2[2] - vector1[2] * vector2[1];
	ris[1]=vector1[2] * vector2[0] - vector1[0] * vector2[2];
	ris[2]=vector1[0] * vector2[1] - vector1[1] * vector2[0];

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


	// Misure dei link ottenute dal file PUMA260.KYN del codice di LLoyd
	 double				A2;// =	203.2;
	 double				D3;// =	126.24;
	 double				A3;// =	  0.0;
	 double				D4;// =	203.20;





	double h2;
	double h1;
	double h3;
	double h1_norm;

};


Puma200Resources::Puma200Resources()
{



	

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


PumaKinematics::PumaKinematics() 
{
	puma_resources = (void*) new Puma200Resources;
	ACE_ASSERT (puma_resources != NULL);

	jointVelocity.resize(6);
	vel3D.resize(6);
}

PumaKinematics::~PumaKinematics()
{

	if (puma_resources != NULL) 
		{
			delete (Puma200Resources *)puma_resources;
		}
	puma_resources = NULL;

}


/*
void PumaKinematics::UpdateTimeAndPos(double *NewPosition)
{
	Puma200Resources& r = RES (puma_resources);

	
	r.OldTime = r.NewTime;
	r.NewTime = time(0);
	
	for(int i = 0; i < 6; i++)
	{
		r.OldPosition[i] = r.NewPosition[i];
		r.NewPosition[i] = NewPosition[i];
	}

}


double PumaKinematics::GetJointSpeed(double *OldPosition, double *NewPosition, double OldTime, double NewTime)
{
	Puma200Resources& r = RES (puma_resources);
	
//	double time1 = time(0);
	double JointSpeed[6];
//	JointSpeed.resize(6);
//	double velocity[6] ;


	for(int i = 0; i < 6; i++)
	{
//		velocity[i]=r.OldPosition[i]+1.0;
//	JointSpeed[i] = (velocity[i] - r.OldPosition[i])/(time1 - r.OldTime);
	JointSpeed[i] = (r.NewPosition[i] - r.OldPosition[i])/(r.NewTime - r.OldTime);

	}

	return *JointSpeed;
}

*/






//this can give you the kiinematics fo each joint
Vector PumaKinematics::FwdPuma200Kin2(double *angle, int lastjoint)
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
	

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] = -sin(angle[1]);
	T2[2][1] = -cos(angle[1]);
	T2[1][2] =  1.0;
	T2[3][3] =  1.0;
	T2[1][3] =  127.0;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  203.3;

	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  203.2;


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


/*
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

}



Vector PumaKinematics::FwdPuma200Kin2(double *angle)
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
	
	
	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] = -sin(angle[1]);
	T2[2][1] = -cos(angle[1]);
	T2[1][2] =  1.0;
	T2[3][3] =  1.0;
	T2[1][3] =  127.0;




	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  203.3;



	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  203.2;




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

/*	
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





	Vector EndEffPos;

	EndEffPos.resize(6);


	double px = EndEffPos[0] = T[0][3];
	double py = EndEffPos[1] = T[1][3];
	double pz = EndEffPos[2] = T[2][3];


	double alpha, beta, gamma;
//	double alpha1, beta1, gamma1;
//	double aa, bb, cc,dd, ee, ff;

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
/*
int ii,jj;
			printf("\n T1\n");
		for(int ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T1[ii][jj]);
				}
			}

					printf("\n T2\n");
		for( ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T2[ii][jj]);
				}
			}

					printf("\n T3\n");
		for( ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T3[ii][jj]);
				}
			}

					printf("\n T4\n");
		for( ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T4[ii][jj]);
				}
			}

					printf("\n T5\n");
		for( ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T5[ii][jj]);
				}
			}

					printf("\n T6\n");
		for( ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T6[ii][jj]);
				}
			}

*/	
/*		printf("\n Kin");
		for( ii = 0 ; ii < 4; ii++)
			{
			printf("\n");
			for( jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T[ii][jj]);
				}
			}
*/


	//ZYZ Rotation yoshikawa notation
	alpha = atan2(T[1][2],T[0][2]);
	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
	gamma = atan2(T[2][1],-T[2][0]);









	
	EndEffPos[3] = alpha;
	EndEffPos[4] = beta;
	EndEffPos[5] = gamma;

 return EndEffPos;

}











Vector PumaKinematics::FwdPuma200Kin(double *angle)
{

	Puma200Resources& r = RES (puma_resources);

	Vector EndEffPos;

	EndEffPos.resize(6);

	double c1, c2, c3, c4, c5, c6;
	double s1, s2, s3, s4, s5, s6;
	double m511, m512, m521, m522;
	double m411, m412, m421, m422, m413, m423;
	double m211, m212, m221, m222, m213, m223, m214, m224;
	double T[6][6];
	double T11,T12,T13,T14,T21,T22,T23, T24,T31,T32,T33, T34,T41,T42,T43, T44;

	double a2 = 203.3; //mm
	double d3 = 127;
	double d4 = 203.2;

	c1 = cos(angle[0]);
	c2 = cos(angle[1]);
	c3 = cos(angle[2]);
	c4 = cos(angle[3]);
	c5 = cos(angle[4]);
	c6 = cos(angle[5]);

	s1 = sin(angle[0]);
	s2 = sin(angle[1]);
	s3 = sin(angle[2]);
	s4 = sin(angle[3]);
	s5 = sin(angle[4]);
	s6 = sin(angle[5]);

	double c23 = cos(angle[2] + angle[1]);
	double s23 = sin(angle[2] + angle[1]);

//	double c23 = cos(angle[2] + angle[3]);
//	double s23 = sin(angle[2] + angle[3]);

	m511 = c5 * c6;
	m512 =-c5 * s6;
	m521 = s5 * c6;
	m522 =-s5 * s6;

	m411 = m511 * c4 + s4 * s6;
	m412 = c4 * m512 + s4 * c6;
	m421 = m511 * s4 - c4 * s6;
	m422 = s4 * m512 - c4 * s6;
	m413 = c4 * s5;
	m423 = s4 * s5;

	m211 = c23 * m411 + s23 * m521;
	m212 = c23 * m412 + s23 * m522;
	m213 = c23 * m413 - c5 * s23;
	m214 = c2 * a2 + s23 * d4;
	m221 = s23 * m411 - c23 * m521;
	m222 = s23 * m412 - c23 * m522;
	m223 = s23 * m413 + c5 * c23;
	m224 = s2 * a2 - c23 * d4;


	T[0][0] =  T11	= c1 * m211 + s1 * m421;

	T[0][1] =  T12	= c1 * m212 + s1 * m422;
	T[0][2] =  T13	= c1 * m213 + s1 * m423;
	T[0][3] =  T14	= c1 * m214 + s1 * d3;			//px

	T[1][0] =  T21	= s1 * m211 - c1 * m421;
	
	T[1][1] =  T22	= s1 * m212 - c1 * m422;
	T[1][2] =  T23	= s1 * m213 - c1 * m423;
	T[1][3] =  T24	= s1 * m214 - c1 * d3;			//py
	
	T[2][0] =  T31	= m221;
	
	T[2][1] =  T32	= m222;
	T[2][2] =  T33	= m223;
	T[2][3] =  T34  = m224;							//pz
	
	T[3][0] =  T41 = 0.0;
	
	T[3][1] =  T42 = 0.0;
	T[3][2] =  T43 = 0.0;
	T[3][3] =  T44 = 1.0;



	
		printf("\n Kin\n");
		for(int ii = 0 ; ii < 4; ii++)
			{printf("\n");
			for(int jj = 0 ; jj < 4; jj++)
				{
				printf(" %lf",T[ii][jj] );
				}
			}

	if((T21==0.0) || (T33==0.0))
		printf("\nSomething is going wrong ..the fwd kinematics is not calculatin the right values for the rotation matrix!");

	EndEffPos[0] = T14;											//px
	EndEffPos[1] = T24;											//py
	EndEffPos[2] = T34;											//pz

		double alpha, beta, gamma;

	//ZYZ Rotation
	//beta  = atan2(sqrt(T[0][2] * T[0][2] + T[1][2] * T[1][2]),T[2][2]);
	beta  = -atan2((sqrt(T[0][2]*T[0][2]+T[1][2] * T[1][2])),T[2][2]);//check the sign
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
	






	printf("\n EndEffPos 1:%lf 2:%lf 3:%lf 4:%lf 5:%lf 6:%lf",EndEffPos[0],EndEffPos[1],EndEffPos[2],EndEffPos[3]*RADTODEG,EndEffPos[4]*RADTODEG,EndEffPos[5]*RADTODEG);


	return EndEffPos;
}







Matrix PumaKinematics::Puma200Jac(Vector &angle)
{
	double	j11,j12,j13,j14,j15,j16,
			j21,j22,j23,j24,j25,j26,
			j31,j32,j33,j34,j35,j36,
			j41,j42,j43,j44,j45,j46,
			j51,j52,j53,j54,j55,j56,
			j61,j62,j63,j64,j65,j66;
	
	double a2 = 203.3; //mm
	double d3 = 127;
	double d4 = 203.2;



	double c1 = cos(angle[0]);
	double c2 = cos(angle[1]);
	double c3 = cos(angle[2]);
	double c4 = cos(angle[3]);
	double c5 = cos(angle[4]);
	double c6 = cos(angle[5]);

	double s1 = sin(angle[0]);
	double s2 = sin(angle[1]);
	double s3 = sin(angle[2]);
	double s4 = sin(angle[3]);
	double s5 = sin(angle[4]);
	double s6 = sin(angle[5]);

	double c21m2 = cos(2*(angle[1]-angle[2]));
	double c21p2 = cos(2*(angle[1]+angle[2]));

	double c23 = cos(angle[2] + angle[1]);
	double s23 = sin(angle[2] + angle[1]);

	double csc5 = 1.0/sin(angle[5]);
	double csc4 = 1.0/sin(angle[4]);
	double csc3 = 1.0/sin(angle[3]);

	double cot3 = 1.0/tan(angle[3]);
	double cot4 = 1.0/tan(angle[4]);
	double cot23= 1.0/tan(angle[2] + angle[1]);

	double s2a1a2 = sin(2*(angle[2] + angle[1]));
	double s2a4 = sin(2*angle[4]);
	
	Matrix Puma200Jac;
	Puma200Jac.resize(6,6);
	Puma200Jac.zero(); 

			for(int j = 0 ; j < 6; j++)
		{
			printf("\n\n angle[%d]= %lf",j,angle[j]);
			printf("\ncos(angle[%d])= %lf",j,cos(angle[j]));
			printf("\nsin(angle[%d])= %lf",j,sin(angle[j]));
	

		}





//E1
j11 = d3 * c1 - s1 * (a2 * c2 + d4 * s23);
j12 = c1 * (d4 * c23 -a2 * s2);
j13 = c1 * (c2 + d4 * c23);
j14 = 0.0;
j15 = 0.0;
j16 = 0.0;

//E2
j21 = d3 * s1 + c1 * (a2*c2 + d4 * s23);
j22 = s1 * (d4 * c23 -a2 * s2);
j23 = s1 * (c2 + d4 * c23);
j24 = 0.0;
j25 = 0.0;
j26 = 0.0;

//E3
j31 = 0.0;
j32 = a2*c2 + d4 * s23;
j33 =    s2 + d4 * s23;
j34 = 0.0;
j35 = 0.0;
j36 = 0.0;


//E4
j41 = 0.0;
j42 = (c5 * s23 - c23 * c4 * s5)							/ (  sqrt( s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 * c4 * c5 * s23 * s5 + c23*c23 * s5*s5 ));
j43 = (c5 * s23 - c23 * c4 * s5)							/ (  sqrt( s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 * c4 * c5 * s23 * s5 + c23*c23 * s5*s5 ));
j44 = (s23 * s4 * s5 )										/ (  sqrt( s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 * c4 * c5 * s23 * s5 + c23*c23 * s5*s5 ));
j45 = (-c4 * c5 * s23 + c23 * s5 )							/ (  sqrt( s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 * c4 * c5 * s23 * s5 + c23*c23 * s5*s5 ));
j46 = 0.0;

if ((  sqrt( s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 * c4 * c5 * s23 * s5 + c23*c23 * s5*s5 ))==0.0)
{
	j42 = 9999999999999999.9;
	j43 = 9999999999999999.9;
	j44 = 9999999999999999.9;
	j45 = 9999999999999999.9;
}

//E5
j51 = 1.0;
j52 = -(s4*s5 * (c23*c5 + c4*s23*s5))			/ (c5*c5 * s23*s23 - 2 * c23*c4*c5*s23*s5 + (c23*c23 * c4*c4 + s4*s4) * s5*s5);
j53 = -(s4*s5 * (c23*c5 + c4*s23*s5))			/ (c5*c5 * s23*s23 - 2 * c23*c4*c5*s23*s5 + (c23*c23 * c4*c4 + s4*s4) * s5*s5);
j54 = (s5 * (c4*c5*s23 - c23*s5))				/ (c5*c5 * s23*s23 - 2 * c23*c4*c5*s23*s5 + (c23*c23 * c4*c4 + s4*s4) * s5*s5);
j55 = 1.0										/ (c5*c5 * csc3 * s23 + s23 * s4 * s5*s5 + c23 * cot23 * csc3 * s5*s5 - cot3 * s2a4  );
j56 = 0.0;
if((c5*c5 * s23*s23*s23 - 2 * c23*c4*c5*s23*s5 + (c23*c23 * c4*c4 + s4*s4) * s5*s5)==0.0)
{
	j52 = -9999999999999999.9;
	j53 = -9999999999999999.9;
	j54 = 9999999999999999.9;
}
	if((c5*c5 * csc3 * s23 + s23 * s4 * s5*s5 + c23 * cot23 * csc3 * s5*s5 - cot3 * s2a4   )==0.0)
	{
	j55 = 9999999999999999.9;
	}


//E6
j61 = 0.0;
j62 = 1.0							/ ( c5*cot3*( c4*cot4* s23*s23 - s2a1a2 ) + csc4*s23*s23*s4+c23*c23 * csc3 * s5);
j63 = 1.0							/ ( c5*cot3*( c4*cot4* s23*s23 - s2a1a2 ) + csc4*s23*s23*s4+c23*c23 * csc3 * s5);
j64 = (-2 * c5 * s23*s23 +c4 * s2a1a2 * s5)			/ (2 * s23*s23 * (c4*c4 * c5*c5 + s4*s4) + 2*c23*c23 * s5*s5 - c4 * s2a1a2 * s2a4);
j65 = -(s23 * s4 *(c23 * c5 + c4 * s23 * s5))		/ (s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 *c4 *c5 *s23 *s5 + c23*c23 * s5*s5);
j66 = 1.0;

if(( c5*cot3*( c4*cot4* s23*s23 - s2a1a2 ) + csc4*s23*s23*s4+c23*c23 * csc3 * s5)==0.0)
{
	j62 = 9999999999999999.9;
	j63 = 9999999999999999.9;
}

if((2 * s23*s23 * (c4*c4 * c5*c5 + s4*s4) + 2*c23*c23 * s5*s5 - c4 * sin(2*(angle[2] + angle[1])) * sin(2 * angle[4]))==0.0)
{
	j64 = 9999999999999999.9;
}
if((s23*s23 * (c4*c4 * c5*c5 + s4*s4) - 2 * c23 *c4 *c5 *s23 *s5 + c23*c23 * s5*s5)==0.0)
{
	j65 = -9999999999999999.9;
}




	
	Puma200Jac[0][0] = j11;
	Puma200Jac[0][1] = j12;
	Puma200Jac[0][2] = j13;
	Puma200Jac[0][3] = j14;
	Puma200Jac[0][4] = j15; 
	Puma200Jac[0][5] = j16; 

	Puma200Jac[1][0] = j21; 
	Puma200Jac[1][1] = j22; 
	Puma200Jac[1][2] = j23; 
	Puma200Jac[1][3] = j24; 
	Puma200Jac[1][4] = j25; 
	Puma200Jac[1][5] = j26; 

	Puma200Jac[2][0] = j31; 
	Puma200Jac[2][1] = j32; 
	Puma200Jac[2][2] = j33;
	Puma200Jac[2][3] = j34;
	Puma200Jac[2][4] = j35; 
	Puma200Jac[2][5] = j36; 

	Puma200Jac[3][0] = j41; 
	Puma200Jac[3][1] = j42;
	Puma200Jac[3][2] = j43;
	Puma200Jac[3][3] = j44;
	Puma200Jac[3][4] = j45;
	Puma200Jac[3][5] = j46;

	Puma200Jac[4][0] = j51;
	Puma200Jac[4][1] = j52;
	Puma200Jac[4][2] = j53;
	Puma200Jac[4][3] = j54;
	Puma200Jac[4][4] = j55;
	Puma200Jac[4][5] = j56;

	Puma200Jac[5][0] = j61;
	Puma200Jac[5][1] = j62;
	Puma200Jac[5][2] = j63;
	Puma200Jac[5][3] = j64;
	Puma200Jac[5][4] = j65;
	Puma200Jac[5][5] = j66;

	return Puma200Jac;

/*
	//dT11/dqi
	Puma200Jac[0][0] = j11 = c1 * (c5 * c6 * s4 - c4 * s6) - s1 * (c6 * s2 * s3 * s5 + c2 * c3 * (c4 * c5 * c6 + s4 * s6));
	Puma200Jac[0][1] = j12 = c1 * (c6 * c2 * s3 * s5 - s2 * c3 * (c4 * c5 * c6+ s4 * s6));
	Puma200Jac[0][2] = j13 = c1 * (c6 * s2 * c3 * s5 - c2 * s3 * (c4 * c5 * c6+ s4 * s6));
	Puma200Jac[0][3] = j14 = c1 * c2 * c3 * (-c5 * c6 * s4 + c4 * s6) + s1 * (c4 * c5 * c6+ s4 * s6);
	Puma200Jac[0][4] = j15 = -c6 * s1 * s4 * s5 + c1 * (c5 * c6 * s2 * s3 - c2 * c3 * c4 * c5 * s5);
	Puma200Jac[0][5] = j16 = s1 * (-c4 * c6 - c5 * s4 * s6) + c1 * (s6 * s2 * s3 * s5 + c2 * c3 * (s4 * c6 + c4 * c5 * s6));

	//dT12/dqi
	Puma200Jac[1][0] = j21 = c1 * (-c4 * s6 - c5 * s4 * s6) - s1 * (-s2 * s3 * s5 * s6 + c2 * c3 * (s4 * c6 - c4 * c5 * s6));
	Puma200Jac[1][1] = j22 = c1 * (-c2 * s3 * s5 * s6 + c3 * s2  * (s4 * c6 - c4 * c5 * s6));
	Puma200Jac[1][2] = j23 = c1 * (-c3 * c5 * s2 - c2 * c4 * s3 * s5);
	Puma200Jac[1][3] = j24 = s1 * (-c4 * s6 * c5 - s4 * s6) + c1 * c2 * c3 * (c4 * c6 + c5 * s4 * s6);
	Puma200Jac[1][4] = j25 = s1 * s4 * s5 * s6 + c1 * (-c5 * s2 * s3 * s6 + c2 * c3 * c4 * s5 * s6);
	Puma200Jac[1][5] = j26 = s1 * (-c4 * c6 - c5 * s4 * c6) + c1 * (-c6 * s2 * s3 * s5 + c2 * c3 * (-c4 * c5 * c6 -  s4 * s6);

	//dT13/dqi
	Puma200Jac[2][0] = j31 = c1 * (-c2 * c5 * c3 - c3 * c4 * s2 * s5);
	Puma200Jac[2][1] = j32 = c1 * s4 * s5 - s1 * (-c5 * s2 * s3 + c2 * c3 * c4 * c5);
	Puma200Jac[2][2] = j33 = c1 * (-c3 * s2 * s5 * s6 - c2 * s3  * (s4 * c6 - c4 * c5 * s6));
	Puma200Jac[2][3] = j34 = s1 * (-c4 * s6 * c5 - s4 * s6) + c1 * c2 * c3 * (c4 * c6 + c5 * s4 * s6);
	Puma200Jac[2][4] = j35 = c5 * s1 + s4 + c1 * (c2 * c3 * c4 * c5 + s2 * s3 * s5);
	Puma200Jac[2][5] = j36 = 0.0;
*/
/*	//dpx/dangle[i]
	Puma200Jac[0][0] = j11 = d3 * c1 - s1 * (a2 * c2 + d4 * s2 * s3);
	Puma200Jac[0][1] = j12 = c1 * (-a2 * s2 +d4 * c2 * c3);
	Puma200Jac[0][2] = j13 = c1 * (c2 + d4 * c3 * s2);
	Puma200Jac[0][3] = Puma200Jac[0][4] = Puma200Jac[0][5] = j14 = j15 = j16 = 0.0;

	//dpy/dangle[i]
	Puma200Jac[1][0] = j21 = d3 * s1 + c1 * (a2 * c2 + d4 * s2 * s3);
	Puma200Jac[1][1] = j22 = s1 * (-a2 * s2 +d4 * c2 * s3);
	Puma200Jac[1][2] = j23 = s1 * (c2 + d4 * c3 * s2);
	Puma200Jac[1][3] = Puma200Jac[1][4] = Puma200Jac[1][5] = j24 = j25 = j26 = 0.0;

	//dpz/dangle[i]
	Puma200Jac[2][0] = j31 = 0.0;
	Puma200Jac[2][1] = j32 = a2 * c2 - d4 * c3 * s2;
	Puma200Jac[2][2] = j33 = s2 - d4 * c2 * s3;
	Puma200Jac[2][3] = Puma200Jac[2][4] = Puma200Jac[2][5] = j34 = j35 = j36 = 0.0;


	return Puma200Jac;
*/
}

Vector PumaKinematics::GimmeZi(double *angle, int Rnumber)
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

/*
		printf("\n numero %d\n",Rnumber);
	for(int i = 0 ; i < 3; i++)
		{printf("\n");
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


Matrix PumaKinematics::GimmeR(double *angle, int Rnumber)
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
	
	

	
	T2[0][0] =  cos(angle[1]);
	T2[0][1] = -sin(angle[1]);
	T2[2][0] = -sin(angle[1]);
	T2[2][1] = -cos(angle[1]);
	T2[1][2] =  1.0;
	T2[3][3] =  1.0;
	T2[1][3] =  127.0;


	T3[0][0] =  cos(angle[2]);
	T3[0][1] = -sin(angle[2]);
	T3[1][0] =  sin(angle[2]);
	T3[1][1] =  cos(angle[2]);
	T3[2][2] =  1.0;
	T3[3][3] =  1.0;
	T3[0][3] =  203.3;


	T4[0][0] =  cos(angle[3]);
	T4[0][1] = -sin(angle[3]);
	T4[2][0] = -sin(angle[3]);
	T4[2][1] = -cos(angle[3]);
	T4[1][2] =  1.0;
	T4[3][3] =  1.0;
	T4[1][3] =  203.2;


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


	/*
	int ii,jj;

		for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T1[%d][%d]= %lf",ii,jj,T1[ii][jj]);
			}
		}

		for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T2[%d][%d]= %lf",ii,jj,T2[ii][jj]);
			}
		}
				for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T3[%d][%d]= %lf",ii,jj,T3[ii][jj]);
			}
		}
						for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T4[%d][%d]= %lf",ii,jj,T4[ii][jj]);
			}
		}
								for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T5[%d][%d]= %lf",ii,jj,T5[ii][jj]);
			}
		}
										for(jj = 0 ; jj < 4; jj++)
			{
			printf("\n ");
			for( ii = 0 ; ii < 4; ii++)
			{
			
				printf("T6[%d][%d]= %lf",ii,jj,T6[ii][jj]);
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

/*
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


Matrix PumaKinematics::PumaJac(Vector &angle)
{
/*
	double	j11,j12,j13,j14,j15,j16,
			j21,j22,j23,j24,j25,j26,
			j31,j32,j33,j34,j35,j36,
			j41,j42,j43,j44,j45,j46,
			j51,j52,j53,j54,j55,j56,
			j61,j62,j63,j64,j65,j66;




	double a2 = 203.3; //mm
	double d3 = 127;
	double d4 = 203.2;
*/	
	Matrix PumaJac;
	PumaJac.resize(6,6);
	PumaJac.zero(); 


	Vector posit1(6);
	Vector posit2(6);
	Vector posit3(6);
	Vector posit4(6);
	Vector posit5(6);
	Vector posit6(6);
	Vector positE(6);
	
	Vector z1(6);
	Vector z2(6);
	Vector z3(6);
	Vector z4(6);
	Vector z5(6);
	Vector z6(6);
	Vector z7(6);
	Vector z8(6);

	Vector tmp1(3);
	Vector tmp2(3);
	Vector tmp3(3);
	Vector tmp4(3);
	Vector tmp5(3);
	Vector tmp6(3);

	double *ANGLE = new double[6];
	int j,jj,ii;
	
	for( j = 0 ; j < 6; j++)
	{
		ANGLE[j]=angle[j];
	}
	
	positE=FwdPuma200Kin2(ANGLE);

	posit1=FwdPuma200Kin2(ANGLE,1);
	posit2=FwdPuma200Kin2(ANGLE,2);
	posit3=FwdPuma200Kin2(ANGLE,3);
	posit4=FwdPuma200Kin2(ANGLE,4);
	posit5=FwdPuma200Kin2(ANGLE,5);
	posit6=FwdPuma200Kin2(ANGLE,6);

	/*
				printf("\nposition of each link\n");
	
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n positE[%d]= %lf",jj,positE[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit1[%d]= %lf",jj,posit1[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit2[%d]= %lf",jj,posit2[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit3[%d]= %lf",jj,posit3[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit4[%d]= %lf",jj,posit4[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit5[%d]= %lf",jj,posit5[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit6[%d]= %lf",jj,posit6[jj]);
	}
*/
	z1 =  GimmeZi(ANGLE, 1);
	z2 =  GimmeZi(ANGLE, 2);
	z3 =  GimmeZi(ANGLE, 3);
	z4 =  GimmeZi(ANGLE, 4);
	z5 =  GimmeZi(ANGLE, 5);
	z6 =  GimmeZi(ANGLE, 6);


/*
	printf("\ndirection of z vectors\n");

			for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z1[%d]= %lf",jj,z1[jj]);
	}
			printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z2[%d]= %lf",jj,z2[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z3[%d]= %lf",jj,z3[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z4[%d]= %lf",jj,z4[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z5[%d]= %lf",jj,z5[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n z6[%d]= %lf",jj,z6[jj]);
	}
*/
	posit1=positE-posit1;
	posit2=positE-posit2;
	posit3=positE-posit3;
	posit4=positE-posit4;
	posit5=positE-posit5;
	posit6=positE-posit6;

/*	
	printf("\ndifferenza\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n positE[%d]= %lf",jj,positE[jj]);
					
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit1[%d]= %lf",jj,posit1[jj]);
					printf("\n z1[%d]= %lf",jj,z1[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit2[%d]= %lf",jj,posit2[jj]);
					printf("\n z2[%d]= %lf",jj,z2[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit3[%d]= %lf",jj,posit3[jj]);
					printf("\n z3[%d]= %lf",jj,z3[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit4[%d]= %lf",jj,posit4[jj]);
					printf("\n z4[%d]= %lf",jj,z4[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit5[%d]= %lf",jj,posit5[jj]);
					printf("\n z5[%d]= %lf",jj,z5[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n posit6[%d]= %lf",jj,posit6[jj]);
					printf("\n z6[%d]= %lf",jj,z6[jj]);
	}
*/
	tmp1 = VectorProd(z1 , posit1);
	tmp2 = VectorProd(z2 , posit2);
	tmp3 = VectorProd(z3 , posit3);
	tmp4 = VectorProd(z4 , posit4);
	tmp5 = VectorProd(z5 , posit5);
	tmp6 = VectorProd(z6 , posit6);
/*
			printf("\n\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp1[%d]= %lf",jj,tmp1[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp2[%d]= %lf",jj,tmp2[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp3[%d]= %lf",jj,tmp3[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp4[%d]= %lf",jj,tmp4[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp5[%d]= %lf",jj,tmp5[jj]);
	}
		printf("\n");
		for( jj = 0 ; jj < 3; jj++)
	{
					printf("\n tmp6[%d]= %lf",jj,tmp6[jj]);
	}
*/

	for(int i = 0 ; i < 6; i++)
	{
		printf("\n");
		for( j = 0 ; j < 6; j++)
			{
				ii=j;
				jj=i;

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
/*
printf("\n PumaJac");
	for( jj = 0 ; jj < 6; jj++)
		{
		printf("\n ");
		for( ii = 0 ; ii < 6; ii++)
			{
				printf("    %.1lf  ",PumaJac[jj][ii]);
			}
		}



	for( jj = 0 ; jj < 6; jj++)
		{
		printf("\n ");
		for( ii = 0 ; ii < 6; ii++)
			{
				printf("\n PumaJac[%d][%d]= %lf",ii,jj,PumaJac[ii][jj]);
			}
		}

*/


	return PumaJac;

}











Matrix PumaKinematics::Puma200InvJac(Matrix &Puma200Jac)
{
	Matrix Puma200InvJac = MatrixInverted(Puma200Jac);

	return Puma200InvJac;
}















double PumaKinematics::det2x2(const double a, const double b, const double c, const double d) 
{
return a * d - b * c;
}


double PumaKinematics::Det(Matrix &A)
{
	int nrows = A.rows();
	int ncols = A.cols();
	double det;
	
	if (nrows != ncols)
    	printf("\nCannot calculate determinant of non-square matrix");
	    
	return det =	  A[0][0] * det2x2( A[1][2], A[1][3], A[2][2], A[2][3] ) 
					- A[1][1] * det2x2( A[0][2], A[0][3], A[2][2], A[2][3] )
					+ A[2][1] * det2x2( A[0][2], A[0][3], A[1][2], A[1][3] );

}


Vector PumaKinematics::CrossProduct(Vector &a, Vector &b)
{
	Vector solution(3); 
   solution(0) = a[1] * b[2] - a[2] * b[1];
   solution(1) = a[2] * b[0] - a[0] * b[2];
   solution(2) = a[0] * b[1] - a[1] * b[0];

   return solution;
}


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
