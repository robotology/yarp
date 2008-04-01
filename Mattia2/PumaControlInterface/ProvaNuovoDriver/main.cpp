// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 MAttia Castelnovi
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/



#include<conio.h>


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_linalg.h>

#include <ace/OS.h>
#include <yarp/os/RateThread.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include "MEIMotionControl.h"

#include <yarp/os/BinPortable.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>


#include <iostream>

#include "Pcdsp.h"
#include <PumaCalibrator/PumaCalibrator.h>

#include <yarp/os/Time.h>


using namespace yarp::sig;
using namespace yarp::dev;

#include <PumaKin.h>


#define RADTODEG 57.29577951308232100
#define DEGTORAD 0.01745329251994330
#define PI 3.14159265



//posizione estesa 180.000000  90.000000  -90.000000  0.000000  0.000000  0.000000

bool print1 = 1;



//loop for control
class ControlLoopThread : public RateThread {
public:
	ControlLoopThread(int r):RateThread(r){}
	double MyTime,StepTime,StartingTime,ThisTime;
	double prev;
	double Beginning;
	long prev1;
    int ct ;

	BufferedPort< BinPortable<int> > GoalPort;            // Create a port.
	
	IPositionControl	*Thpos;
	IEncoders			*Thenc;

	double *ThHomePos;
	double *spds;

	BinPortable<int> *mytarget;
	BinPortable<int> *myTEMPtarget;


	virtual void afterStart(bool s)
	{
		if (s)
			printf("\nControlLoopThread started successfully\n");
		else
			printf("\nControlLoopThread did not start\n");
	}


	// if I press "q" it quits!
	virtual bool checkbutton()
	{
		bool finish = 0;
		unsigned char key;
		if(kbhit())
		{
			key = getch();
			if((key=='q')||(key=='Q'))
			{
				printf("\n SomeOne pressed 'Q' button!!\n I am going toi ESC!!!");
				finish = 1;
//				printf("\n finish stopped= %d ",finish);
			}
		}
		return finish;
	}



//
inline	Matrix ThTrajectoryGenerator(Matrix Goalmatrix1, Matrix Actualmatrix2)
{
//////////////////////////////////////////////
	Vector Error3(3);
	Vector Error(6);
	Matrix ErrorMatrix(4,4);
	ErrorMatrix.eye();
	Matrix ErrorMatrixp(4,4);
	Vector tmp(4);
	Vector actualtmp(4);
	Vector goaltmp(4);
	Vector Trajtmp(4);
	Matrix matrix1p(3,3);
	Matrix matrix2p(3,3);
	Matrix tmp1(3,3);
	Vector tmp2(3), tmp3(3);
	tmp3.zero();
	int i,j;


	Matrix TrajMat(4,4);
	ErrorMatrix.eye();
	TrajMat.eye();

	int mmForStep=100;
	int Nstep,distance,AngleStep;


		for( i = 0 ; i < 3; i++)
		{
			for( j = 0 ; j < 3; j++)
			{	
				matrix1p[i][j]=Goalmatrix1[i][j];
				matrix2p[i][j]=Actualmatrix2[i][j];
				ErrorMatrix[i][j]=0.0;
			}
		}


//ThTrajectoryGenerator
	ErrorMatrixp =  MatrixInverted(matrix2p) * matrix1p;

	Error3[0] = ErrorMatrix[0][3] =  Actualmatrix2[0][3] - Goalmatrix1[0][3];
	Error3[1] = ErrorMatrix[1][3] =  Actualmatrix2[1][3] - Goalmatrix1[1][3];
	Error3[2] = ErrorMatrix[2][3] =  Actualmatrix2[2][3] - Goalmatrix1[2][3];

		for( i = 0 ; i < 3; i++)
		{
		
			for( j = 0 ; j < 3; j++)
			{
					ErrorMatrix[i][j] =  ErrorMatrixp[i][j];
			}
		}

	fprintf(Mfp,"\nThTrajectoryGenerator ErrorMatrix ");
	for( i = 0 ; i < 4; i++)
		{
		fprintf(Mfp,"\n");
		for( j = 0 ; j < 4; j++)
			{	
				fprintf(Mfp,"  %lf",ErrorMatrix[i][j]);
			}
		}


	tmp = dcm2axis(ErrorMatrix);
	actualtmp = dcm2axis(Actualmatrix2);
	goaltmp = dcm2axis(Goalmatrix1);


	fprintf(Mfp,"\ntmp");
	for( j = 0 ; j < 4; j++)
	{		
			fprintf(Mfp,"  %lf",tmp[j]);
	}

	fprintf(Mfp,"\n\n\n actualtmp");
	for( j = 0 ; j < 4; j++)
	{		
			fprintf(Mfp,"  %lf",actualtmp[j]);
	}

	fprintf(Mfp,"\n goaltmp");
	
	for( j = 0 ; j < 4; j++)
	{		
			fprintf(Mfp,"  %lf",goaltmp[j]);
	}

	distance = ThVectMagn(Error3);

		//number of steps, it depends on mmForStep
		Nstep = distance/mmForStep;
		fprintf(Mfp,"\n\n Nstep = %d",Nstep);

		AngleStep = (int)(tmp[3]/0.4);
		fprintf(Mfp,"\n AngleStep = %d",AngleStep);

		//calculate the next position using steps
		//by now we don-t manage orientation

			for(i = 0 ; i < 4; i++)
				{
					//next postitin of the trajectory is actual position plus a step
					Trajtmp[i] = actualtmp[i]+(goaltmp[i]-actualtmp[i])/AngleStep;
				}

	TrajMat = axis2dcm4x4(Trajtmp);
 


	for(i = 0 ; i < 4; i++)
		{
			//next postitin of the trajectory is actual position plus a step
			TrajMat[i][3]=Actualmatrix2[i][3] + (Goalmatrix1[i][3]-Actualmatrix2[i][3])/Nstep;
		}

	fprintf(Mfp,"\n Trajtmp");
	for( j = 0 ; j < 4; j++)
	{		
			fprintf(Mfp,"  %lf",Trajtmp[j]);
	}

		//if it is close do not use steps
//			if (distance < mmForStep)TrajMat=Goalmatrix1;
	if ((Nstep <= 1)&&(AngleStep <= 1))TrajMat=Goalmatrix1;

	fprintf(Mfp,"\n \n TrajMat\n");
	for( i=0;i<4;i++)
	{
		for( j=0;j<4;j++)
			{
			fprintf(Mfp,"  %lf",TrajMat[i][j]);
		}
		fprintf(Mfp," \n");
	}
	
	return TrajMat;

}





// calculate the determinant
inline double get_det(gsl_matrix * A) {
  int sign=0; 
  double det=0.0; 
  int row_sq = A->size1;

  gsl_permutation * p = gsl_permutation_calloc(row_sq);
  gsl_matrix * tmp_ptr = gsl_matrix_calloc(row_sq, row_sq);
  
  int * signum = &sign;
  
  gsl_matrix_memcpy(tmp_ptr, A);
  gsl_linalg_LU_decomp(tmp_ptr, p, signum);
  det = gsl_linalg_LU_det(tmp_ptr, *signum);
  gsl_permutation_free(p);
  gsl_matrix_free(tmp_ptr);
  return det;
}


////////////////////////////////////////////////////////////////////////////////

//given 2 matrices with the kinematics it returns a vector with
//the distance as a vector
//this DISTANCE is CARTESIAN not in the jopint space
Vector MatrixDiffToVector(Matrix matrix1, Matrix matrix2 )
{
	Vector Error(6);
	Matrix ErrorMatrix(4,4);
	ErrorMatrix.eye();
	Matrix ErrorMatrixp(3,3);
	Vector tmp(4);
	Matrix matrix1p(3,3);
	Matrix matrix2p(3,3);
	Matrix tmp1(3,3);
	Vector tmp2(3), tmp3(3);
	tmp3.zero();
	int i,j;


		for( i = 0 ; i < 3; i++)
		{
			for( j = 0 ; j < 3; j++)
			{		
				matrix1p[i][j]=matrix1[i][j];
				matrix2p[i][j]=matrix2[i][j];
				ErrorMatrix[i][j]=0.0;
				matrix1[i][j]=0.0;
				matrix2[i][j]=0.0;
			}
		}

	
	ErrorMatrixp =  MatrixInverted(matrix2p) * matrix1p;

	for( i = 0 ; i < 3; i++)
		{
		for( j = 0 ; j < 3; j++)
			{		
				ErrorMatrix[i][j]=ErrorMatrixp[i][j];
			}
		}

	ErrorMatrix[0][3] =  matrix1[0][3] - matrix2[0][3];
	ErrorMatrix[1][3] =  matrix1[1][3] - matrix2[1][3];
	ErrorMatrix[2][3] =  matrix1[2][3] - matrix2[2][3];
  
	fprintf(Mfp,"\n\nMatrixDiffToVector ErrorMatrix ");
		for( i = 0 ; i < 4; i++)
			{
				fprintf(Mfp,"\n");
				for( j = 0 ; j < 4; j++)
					{	
						fprintf(Mfp,"  %lf",ErrorMatrix[i][j]);
					}
			}

		tmp = dcm2axis(ErrorMatrix);

		for( j = 0 ; j < 3; j++)
		{		
			tmp2[j]=tmp[j];
		}

		//tmp2 e' l'angolo, tmp0 e' l'asse.
		tmp3=tmp2*tmp(3);

		fprintf(Mfp,"\ntmp3");
		for( j = 0 ; j < 3; j++)
		{		
			fprintf(Mfp,"  %lf",tmp3[j]);
		}


		if((ErrorMatrix[0][0] == 1.0) && (ErrorMatrix[1][1] == 1.0) && (ErrorMatrix[2][2] == 1.0))
		{
			fprintf(Mfp,"\nsingularities go the quaternion\n");
			Error(0)=ErrorMatrix(0,3) ;
			Error(1)=ErrorMatrix(1,3) ;
			Error(2)=ErrorMatrix(2,3) ;
			Error(3)=0.0;
			Error(4)=0.0;
			Error(5)=0.0;
		}

		else
		{

			Error(0)=ErrorMatrix(0,3) ;
			Error(1)=ErrorMatrix(1,3) ;
			Error(2)=ErrorMatrix(2,3) ;
			Error(3)=tmp3(0) * RADTODEG;
			Error(4)=tmp3(1) * RADTODEG;
			Error(5)=tmp3(2) * RADTODEG;
		}

	return Error;
}




////////////////////////////////////////////////////////////////////////////////



inline	Vector CalulatePID(Vector ActualV, Vector PreviousV, Vector PreviousSum, double steptime)
	{

		Vector P;
		P.resize(6);
		Vector I;
		I.resize(6);
		Vector D;
		D.resize(6);

		Vector NewV;
		NewV.resize(6);

		for ( i = 0; i < 6; i++)
		{
        P(i) = Kp(i) * ActualV(i) ; 
		I(i) = Ki(i) * PreviousSum(i);
		D(i) = Kd(i) * (ActualV(i) - PreviousV(i))/steptime;
		NewV(i) = P(i) + I(i) + D(i);
		}
		if(steptime < 0.007)
			NewV.zero();

		if (print == 1)	
		{
		fprintf(Mfp," \n\n P");
			for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %.7lf",(double)P[i]);
				
			}

		fprintf(Mfp," \n I");
			for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %.7lf",(double)I[i]);
				
			}
		fprintf(Mfp," \n D");
			for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %.7lf",(double)D[i]);
			}

		}
		return NewV;
	}


virtual void  initialization(
		IEncoders * original_enc_p,
		IVelocityControl * original_vel,
		double * original_encval_p,
		Vector originalStartPosition)
	{
		ThAngleGoalPosition.resize(6);
		ThAngleStartPosition.resize(6);
		ThAngleActualPosition.resize(6);

		steptime = 0.0;

		ThGoalPosition.resize(4,4);
		ThTrajectoryGoalPosition.resize(4,4);
		ThActualPosition.resize(4,4);
		
		my_enc_p    = original_enc_p;
		my_encval_p = original_encval_p;
		Thvel = original_vel;

		mytarget = NULL;
		myTEMPtarget = NULL;
		

		ThI.resize(6,6);
		ThI.eye();
		ThJacobian.resize(6,6);
		ThJacPinv.resize(6,6);
		TmpM.resize(6,6);
		ThMyErr.resize(6);
		ThMyErrore.resize(6);
		ThMyPID.resize(6);
		ThTempEr.resize(6);
		ThTempEr.resize(6);
		ThTempEr.zero();
		ThOldTempErSum.resize(6);
		ThOldTempErSum.zero();
		
		TempEr.resize(6);
		ThTE.resize(3);

		ThDeltaT.resize(6);
		ThDeltaT.zero();

		ThGain.resize(6);

		prev = 0;
		prev1 = 0;
		ct = 0;
		count= 0;

		singularity = 0;
		firstround=1;
		ALLtotalsteptime = 0.0;

		ThAngleStartPosition = originalStartPosition;

		Thspds = new double[6];
		ThOldspds = new double[6];
		Thaccs = new double[6];
		ThHomePos = new double[6];
		spds = new double[6];
		

		printf("\nStarting ControlLoopThread\n");

		printf("\n\n ThAngleGoalPosition ");
		for(int i=0; i<6; i++)
		{
			ThAngleGoalPosition[i]+=originalStartPosition[i];
			printf("  %lf",ThAngleGoalPosition[i]);
			Thspds[i]=0.0;
			spds[i]=0.0;
			ThOldspds[i]=0.0;
			ThHomePos[i]=0.0;
		}

		
		Kp.resize(6);
		Ki.resize(6);
		Kd.resize(6);

	
		Property p;
    
		p.fromConfigFile("MEIconfig.txt");
		
		if (!p.check("GENERAL")) {
			fprintf(stderr, "Cannot understand configuration parameters\n");
        
			return;
    
		}


	const int nj = 6;
	Bottle& xtmp = p.findGroup("STARTINGVALUES").findGroup("SafePosition");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n ThAngleStartPosition = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        ThAngleStartPosition[i-1] = xtmp.get(i).asDouble();
		ThAngleActualPosition[i-1] = ThAngleStartPosition[i-1];
		printf("  %.1lf", ThAngleStartPosition[i-1]);
		}
	printf("\n");


	xtmp = p.findGroup("STARTINGVALUES").findGroup("Acc");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n ThAcc = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        Thaccs[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", Thaccs[i-1]);
		}
	printf("\n");

		xtmp = p.findGroup("STARTINGVALUES").findGroup("Speed1");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n Speed = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        spds[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", spds[i-1]);
		}
	printf("\n");



	xtmp = p.findGroup("STARTINGVALUES").findGroup("Gain");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n ThGain = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        ThGain[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", ThGain[i-1]);
		}

	xtmp = p.findGroup("STARTINGVALUES").findGroup("Kp");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n Kp =    ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        Kp[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", Kp[i-1]);
		}


	xtmp = p.findGroup("STARTINGVALUES").findGroup("Ki");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n Ki =    ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        Ki[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", Ki[i-1]);
		}


	xtmp = p.findGroup("STARTINGVALUES").findGroup("Kd");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n Kd =    ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        Kd[i-1] = xtmp.get(i).asDouble();
		printf("  %.1lf", Kd[i-1]);
		}


				

	safespeed = 300.0;
	debug = 1;  //time check
	print = 1;
	checktime = 1;
	excel = 1;



	if((print == 1)||(print1 == 1))
	{
		Mfp=fopen("Mattiapumakinfile.txt","w");
		  if(!Mfp)printf("\n something wrong opening file ***  Mattiapumakinfile.txt  **** for writing  debug data");
		else
			printf("\n opened file ***  Mattiapumakinfile.txt  **** for writing  debug data");

	}

	if((debug == 1)||(checktime == 1))
	{
		Mfp1=fopen("TempiDuri.txt","w");
		if(!Mfp1)printf("\n something wrong opening file ***  TempiDuri.txt  **** for writing  time data");
		else 
			printf("\n opened file 1***  TempiDuri.txt  **** for writing  time data");
	}

		if((excel == 1))
	{

		Mfp3=fopen("excel.txt","w");
		if(!Mfp3)printf("\n something wrong opening file ***  excel.txt  **** for writing  time data");
		else 
			printf("\n opened file 1***  excel.txt  **** for writing  time data");

		Mfp5=fopen("error.txt","w");
		if(!Mfp5)printf("\n something wrong opening file ***  error.txt  **** for writing  time data");
		else 
			printf("\n opened file 1***  error.txt  **** for writing  time data");


		Mfp4=fopen("PID.txt","w");
		if(!Mfp4)printf("\n something wrong opening file ***  PID.txt  **** for writing  time data");
		else 
			printf("\n opened file 1***  PID.txt  **** for writing  time data");

	}
		



	Startingtime = Time::now();
	Lasttime=Startingtime;

			//lets give the commmand and update the velocity
		Thvel->setVelocityMode();
		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			fprintf(Mfp1,"\n ****** inizializzazione1   steptime is %lf ms count = %d****\n", ThisTime*1000.0,count);
			Lasttime = Time::now(); 
		}
		Thvel->setRefAccelerations(Thaccs);
		printf("\n accs setted!");
		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			fprintf(Mfp1,"\n ******  inizializzazione2  steptime is %lf ms count = %d****\n", ThisTime*1000.0,count);
			Lasttime = Time::now(); 
		}

/*
					//////////////////////////////////portread part
			GoalPort.open("/in");     // Give it a name on the network.
            printf("Starting ReadThread\n");
			printf("\nport opened\n");
			Time::delay(1);
			Network::connect("/out","/in"); // connect ports.
			printf("\nread port connected \n");
//			counter = 0;
//            return true;
			//////////////////////////////////portread part
	
*/

	}




//compute magnitude of a vector
inline 	 double ThVectMagn(Vector vector)
	{
		int size = vector.length();
			double magnitude=0.0;
			for(int i=0;i<size;i++)
			{
				//if you need to be more accurate uncomment this line
//				vector(i)*=1000000000000000.0;
				magnitude += vector(i)*vector(i);
			}
		magnitude = sqrt(magnitude);
//		magnitude/= 1000000000000000.0;
		return magnitude;
	}





inline	virtual void  controlloop()
{
	



 	}




inline virtual void GetEnc()
	{
			my_enc_p->getEncoders(my_encval_p);
	}

//takes joint angles values from shell
Vector ThAskForNewPosition()
{
	Vector jointPosition(6);
	//ask for the new position
	double j1,j2,j3, j4,j5,j6;

	printf("\n please digit 6 integers divided by SPACE for the next position of the end effector");
			 printf("\n angle n 1 = ");scanf("%lf",&j1);
			 printf(" angle n 2 = ");scanf("%lf",&j2);
			 printf(" angle n 3 = ");scanf("%lf",&j3);
			 printf(" angle n 4 = ");scanf("%lf",&j4);
			 printf(" angle n 5 = ");scanf("%lf",&j5);
			 printf(" angle n 6 = ");scanf("%lf",&j6);
	
	jointPosition(0) = j1;
	jointPosition(1) = j2;
	jointPosition(2) = j3;
	jointPosition(3) = j4;
	jointPosition(4) = j5;
	jointPosition(5) = j6;

	printf("\n it seems to me you have written %lf %lf %lf %lf %lf %lf\n",
		jointPosition(0),jointPosition(1),jointPosition(2),jointPosition(3),jointPosition(4),jointPosition(5));
	return jointPosition;
}


inline virtual void run() {

	if ((debug == 1)||(checktime == 1))
		{
			ThisTime = Time::now()-Lasttime;
				
		fprintf(Mfp1,"\n\n\n ******BETWEEN 2 LOOPS   steptime is %lf, timenow = %lf lasttime = %lf  \n", ThisTime*1000.0,(Time::now())*1000.0,Lasttime*1000.0);
		
		}

	Lasttime = Time::now();
	steptime = 0.0;
	totalsteptime = 0.0;
	
	count = 0;
	if ((debug == 1)||(checktime == 1))
		{//1
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
		
	my_enc_p->getEncoders(my_encval_p);
	
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	
	
	
	double *encval = new double[6];
	int flag=0;

	if(firstround==1)
	{
		Thvel->setVelocityMode();
		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			Lasttime = Time::now(); 
		}
		Thvel->setRefAccelerations(Thaccs);

		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}




		while(flag==0)
		{
		
			for( i=0; i<6; i++)	
			{
				ThAngleActualPosition[i]+=my_encval_p[i];
			}		

			printf(" \n\n\n ThActualPosition at the beginning\n");
			ThActualPosition = ThPuma.FwdPuma200Kin(ThAngleActualPosition);
			for( i=0;i<4;i++)
				{
					for(int j=0;j<4;j++)
						{
							printf("  %lf",  ThActualPosition[i][j]);
						}
					printf(" \n");
				}

		
			printf(" \nmy_encval_p");
			for(int j=0;j<6;j++)
				{
					printf("  %lf",  my_encval_p[j]);
				}

	
			//load starting position 
			Property p; 
    		p.fromConfigFile("MEIconfig.txt");
			Bottle& xtmp = p.findGroup("STARTINGVALUES").findGroup("SafePosition");
			ACE_ASSERT (xtmp.size() == 6+1);
		    for ( i = 1; i < xtmp.size(); i++)
				{
					ThAngleStartPosition[i-1] = xtmp.get(i).asDouble();
				}
			
			double j1,j2,j3,j4,j5,j6,j7;

			Vector angle4(4);
			Vector angle3(3);

			angle4 = dcm2axis(ThActualPosition);

			printf("\nangle4");
		
	
			for( j = 0 ; j < 4; j++)
			{		
				printf("  %lf",angle4[j]);
			}
	
			//tmp2 e' l'angolo, tmp0 e' l'asse.
			angle3[0]=angle4[0]*angle4[3];
			angle3[1]=angle4[1]*angle4[3];
			angle3[2]=angle4[2]*angle4[3];





			//ask for the goal position
			printf("\n please digit 3 integers for the next position");
			printf("\n and 4 integers for the next orientation of the end effector");

			
			printf("\n x position = ");scanf("%lf",&j1);
			printf(" y position = ");scanf("%lf",&j2);
			printf(" z position = ");scanf("%lf",&j3);
			printf(" x orientation = ");scanf("%lf",&j4);
			printf(" y orientation = ");scanf("%lf",&j5);
			printf(" z orientation = ");scanf("%lf",&j6);
			printf(" orientation angle  = ");scanf("%lf",&j7);


		
			Vector tmp(4);

			tmp[0] = j4;
			tmp[1] = j5;
			tmp[2] = j6;
			tmp[3] = j7;

		
			ThGoalPosition[0][3] =  j1;
			ThGoalPosition[1][3] =  j2;
			ThGoalPosition[2][3] =  j3;
			ThGoalPosition[3][3] =  1.0;

			ThGoalPosition = axis2dcm4x4(tmp);

			ThGoalPosition[0][3] =  j1;
			ThGoalPosition[1][3] =  j2;
			ThGoalPosition[2][3] =  j3;
			ThGoalPosition[3][3] =  1.0;

			
			if (print1 == 1)	
				{
					fprintf(Mfp,"\n\n controlloop i start in");
					for( i = 0 ; i < 6; i++)
						{
							fprintf(Mfp,"  %lf",my_encval_p[i]);
						}
					my_enc_p->getEncoders(encval);
					fprintf(Mfp,"\n\n encval is");
					for( i = 0 ; i < 6; i++)fprintf(Mfp,"  %lf",encval[i]);
					fprintf(Mfp,"\n\n my_encval_p is");
					for( i = 0 ; i < 6; i++)fprintf(Mfp,"  %lf",my_encval_p[i]);
					}

	
	
			printf("\n do yuo like it? 0/1 ");scanf("%d",&flag);
		
		}


		ThMyErr.zero();
		ThOldTempEr=ThMyErr;
		firstround=2;
		totalsteptime=0.0;
		prev = Time::now();
		Beginning = Time::now();

	}

	//compute actual position
	for( i=0; i<6; i++)	ThAngleActualPosition[i] = ThAngleStartPosition[i]+my_encval_p[i];
	
	if (debug == 1)
	{//5
		ThisTime = Time::now()-Lasttime;
		count++;
		totalsteptime+=ThisTime;
		fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
		Lasttime = Time::now(); 
	}

	//compute fwd kinematics from angle to actual position		
	ThActualPosition = ThPuma.FwdPuma200Kin(ThAngleActualPosition);

		//ricordarsi di riattivare la ricezione del goal dall porta
//		myTEMPtarget = GoalPort.read(false);         // Read from the port. DONT Wait until data arrives.


		if (myTEMPtarget != NULL)
			mytarget = myTEMPtarget;

	//  printf("Got (%d,%d)\n", mytarget.x, mytarget.y);

		if(firstround==2)
		{
			firstround=3;
			fprintf(Mfp," \n************************\n ThAngleActualPosition at the beginning\n");
		for(int j=0;j<6;j++)
		{
			fprintf(Mfp,"  %lf",  ThAngleActualPosition[j]);
		}
		fprintf(Mfp," \n\n\nThActualPosition at the beginning\n");
		for( i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
				{
				fprintf(Mfp,"  %lf",  ThActualPosition[i][j]);
			}
			fprintf(Mfp," \n");
		}
		fprintf(Mfp," \n ThGoalPosition at the beginning\n");
		for( i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
				{
				fprintf(Mfp,"  %lf",  ThGoalPosition[i][j]);
			}
			fprintf(Mfp," \n");
		}

	
		fprintf(Mfp," \n");
		}
	
			
	if (print == 1)
		{
		fprintf(Mfp,"\n\n\n\n*************************************************\n ThAngleActualPosition ");
		for(int i=0; i<6; i++)
		{
			fprintf(Mfp,"  %lf",ThAngleActualPosition[i]);
		}
		fprintf(Mfp,"\n ThAngleGoalPosition ");
		for( i=0; i<6; i++)
		{
			fprintf(Mfp,"  %lf",ThAngleGoalPosition[i]);
		}
		fprintf(Mfp,"\n*************************************************");

	}

		if (debug == 1)
		{//5
			ThisTime = Time::now()-Lasttime;
			count++;
		totalsteptime+=ThisTime;
		fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
		Lasttime = Time::now();
		}




	//compute trajectory
	//if the goal is too far a newgoal takes itas place
	ThTrajectoryGoalPosition = ThTrajectoryGenerator(ThGoalPosition, ThActualPosition);

	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}

	if (print == 1)	
	{
  	fprintf(Mfp," \n\n***********************************************\n ThActualPosition\n");
		for( i=0;i<4;i++)
		{
			for( j=0;j<4;j++)
				{
				fprintf(Mfp,"  %lf",ThActualPosition[i][j]);
			}
			fprintf(Mfp," \n");
		}

	fprintf(Mfp," \n ThGoalPosition\n");
	for( i=0;i<4;i++)
		{
		for( j=0;j<4;j++)
				{
				fprintf(Mfp,"  %lf",ThGoalPosition[i][j]);
			}
			fprintf(Mfp," \n");
		}
	fprintf(Mfp,"\n*********************************************** \n");
	}

	
	if (print == 1)	
	{//5

		fprintf(Mfp," \n\n ThAngleGoalPosition");
		for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %lf",ThAngleGoalPosition[i]);
			}
		fprintf(Mfp," \n ThAngleActualPosition");
		for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %lf",ThAngleActualPosition[i]);
			}
	}
	///////////////////////

	//update jacobian
	ThJacobian = ThPuma.PumaJac(ThAngleActualPosition);

	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
		
	//get the determinant
	det = get_det(( gsl_matrix *) ThJacobian.getGslMatrix());
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}


	if (print == 1)	
	{
		fprintf(Mfp,"\n\n -------------------------------det  %lf",det);
	}

	//compute inverse jacobian
	ThJacPinv = pinvCD(ThJacobian);
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}


	//remember last error (useful for PID)
	ThOldTempEr = ThMyErr;
	if (debug == 1)
		{//10
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}

	//compute distance between actual position and goal position
	ThMyErr=MatrixDiffToVector(ThTrajectoryGoalPosition,ThActualPosition);
	
	if (print == 1)	
	{
		fprintf(Mfp,"\n\n ThMyErr");
		for( i = 0 ; i < 6; i++)
		{
			fprintf(Mfp,"  %lf",ThMyErr[i]);
		}
	}
	
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}

	//compute how long does a loop take
    steptime = (Time::now()-prev);
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	//update prev
	prev = Time::now();
	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	//update the integral
		
	for( i=0; i<6; i++)
	{	
		ThOldTempErSum(i)+=(ThMyErr(i)*steptime);
	}
	


	ALLtotalsteptime += steptime;

	if (debug == 1)
		{//15
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	
		
	if (print == 1)	
		{
			fprintf(Mfp5," \n\n ThMyErr");
			for( i = 0 ; i < 6; i++)
				{
					fprintf(Mfp5,"%lf ",ThMyErr[i]);
				}
			fprintf(Mfp5,"\n ");
			fprintf(Mfp," \n\n ThOldTempEr");
			for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %lf",ThOldTempEr[i]);
			}
		
			fprintf(Mfp," \n\n ThOldTempErSum");
			for( i = 0 ; i < 6; i++)
				{
					fprintf(Mfp,"  %lf",ThOldTempErSum[i]);
				}

			fprintf(Mfp,"\n steptime %lf",steptime);
		}

	//use the PID
	ThMyErrore = CalulatePID(ThMyErr, ThOldTempEr, ThOldTempErSum,steptime);

	if (print == 1)	
		{
		fprintf(Mfp," \n\nThMyErrore");
		for( i = 0 ; i < 6; i++)
		{
			fprintf(Mfp,"  %lf",ThMyErrore[i]);
		}
	}

		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
			
	
	//compute the angle we need to move each joint (it is a velocity)
	ThDeltaT = ThJacPinv * ThMyErrore;

	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);		Lasttime = Time::now();
		}
	
	if (print == 1)	
	{
		fprintf(Mfp,"\n\n ThDeltaT");
		for( i = 0 ; i < 6; i++)
			{
				fprintf(Mfp,"  %lf",ThDeltaT[i]);
			}
	}	

	//how many loops I have done since now?
	ct++;

	//if determinant is too small we are near a singularity
	//in this situation we should do something!
	if((det<10000.0)&&(det>-10000.0))
	{
		printf("\n************ AVVICINAMENTO SINGOLARITA' det = %lf *********************",det);
		safespeed = 10.0;
		singularity = 1;
	}
	else 
	{
		safespeed = 250.0;
		singularity = 0;
	}

	if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	
	for( i = 0 ; i < 6; i++)
		{
			
			ThOldspds[i] = Thspds[i];
			Thspds[i] = (double)ThDeltaT[i]*(double)ThGain[i];

			if(singularity == 1)
				Thspds[i] = ThOldspds[i];

			//prevent WIND UP
			if(Thspds[i]>safespeed)
			{
				Thspds[i]=safespeed;
				printf("\n!-----I am limiting the speed on joint %d at %lf-------------!",i,	Thspds[i]);
			}
			if(Thspds[i]<-safespeed)
			{
				Thspds[i]=-safespeed;
				printf("\n!-----I am limiting the speed on joint %d at %lf-------------!",i,	Thspds[i]);
			}
		}

		if (print == 1)	
		{
			fprintf(Mfp,"\n\nThGain:");
			for( i = 0 ; i < 6; i++)fprintf(Mfp,"  %lf",ThGain[i]);
		}

		if (print == 1)	
		{
			fprintf(Mfp,"\n\n\nThspds:");
			for( i = 0 ; i < 6; i++)fprintf(Mfp,"  %lf",Thspds[i]);
		}	

	
	
		if (excel == 1)
		{
		
			if (firstround == 3)
			{
				fprintf(Mfp3,"AP0,GP0,sp0,AP1,GP1,sp1,AP2,GP2,sp2,AP3,GP3,sp3,AP4,GP4,sp4,AP5,GP5,sp5\n ");
				fprintf(Mfp4,"\ngoalx goaly goalz actualx actualy actualz Err0 Err1 Err2 distance\n ");
				firstround = 0;
			}
		for(int i=0; i<6; i++)
		{
			fprintf(Mfp3,"  %.2lf , %.2lf , %.2lf ,",ThAngleActualPosition[i],ThAngleGoalPosition[i],Thspds[i]);
		}

		fprintf(Mfp3,"\n");

	}


		
		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}

		check = Thvel->velocityMove(Thspds);
		
		if (debug == 1)
		{
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			Lasttime = Time::now();
		}
	
		if (print1 == 1)	
		{
						
			my_enc_p->getEncoders(encval);
		}


		if ((print == 1))	
		{
			fprintf(Mfp,"\n\n ****** steptime %lf average %lf ct %d total %lf****\n\n",steptime*1000.0, (totalsteptime*1000.0)/(double)ct, ct,totalsteptime*1000.0);
		}


		
		if ((debug == 1)||(checktime == 1))
		{//20
			ThisTime = Time::now()-Lasttime;
			count++;
			totalsteptime+=ThisTime;
			fprintf(Mfp1,"\n ******    steptime is %lf ms count = %d****--total by now:%.2lf \n", ThisTime*1000.0,count, totalsteptime*1000.0);
			fprintf(Mfp1,"\n\n ****** steptime %lf average %lf ct %d total %lf****\n\n",steptime*1000.0, (totalsteptime*1000.0)/(double)ct, ct,totalsteptime*1000.0);
			Lasttime = Time::now();
		}




	    }

    virtual void threadRelease()
	{


	

		printf("\n you are in;");
		my_enc_p->getEncoders(my_encval_p);
		for( i=0; i<6; i++)		printf("\n %lf", my_encval_p[i]);

		printf("\nGoodbye from ControlLoopThread\n");
		if (Thspds != NULL)			delete [] Thspds;
		if (Thaccs != NULL)			delete [] Thaccs;


		if((debug == 1)||(checktime == 1))
		{
			fprintf(Mfp1,"\n ******    average %lf ct %d total %lf****\n", totalsteptime*1000.0/(double)ct, ct,totalsteptime*1000.0);
			fclose(Mfp1);
		}
		if((print == 1)||(print1 == 1))
		{
			fprintf(Mfp,"\n ******    average %lf ct %d total %lf****\n", totalsteptime*1000.0/(double)ct, ct,totalsteptime*1000.0);
			fclose(Mfp);
		}

		if((excel == 1))
		{
			
			fclose(Mfp3);
			fclose(Mfp4);
			fclose(Mfp5);
		}
		
		//////////////////////////////////portread part
/*
		printf("\nclosing ports\n");
		GoalPort.close();
		printf("\nDisconnecting\n");
		Network::disconnect("/out","/in");
*/		
		//////////////////////////////////portread part



	}


private:
	double det;
	int i,j;

	double safespeed;
	bool debug;
	bool print;
	bool checktime;
	bool excel;
	
	
	bool singularity;
	int count;
	double Startingtime;;
	double Lasttime;


	


	int check;

	
	IEncoders * my_enc_p;
	double    * my_encval_p;
	IVelocityControl	*Thvel;
	Vector ThAngleGoalPosition;
	Vector ThAngleStartPosition;
	Vector ThAngleActualPosition;
	Matrix ThGoalPosition;
	Matrix ThTrajectoryGoalPosition;
	Matrix ThActualPosition;
	PumaKinematics ThPuma;
	double steptime;

	double totalsteptime;
	double ALLtotalsteptime;
	int firstround;

	double *Thspds;
	double *ThOldspds;
	double *Thaccs;
	FILE *Mfp;
	FILE *Mfp1;
	FILE *Mfp3;
	FILE *Mfp4;
	FILE *Mfp5;
	
		Matrix ThI;
		Matrix ThJacobian;
		Matrix ThJacPinv;
		Matrix TmpM;
		Vector ThMyErr;
		Vector ThMyErrore;
		Vector ThMyPID;
		Vector ThTempEr;
		Vector ThOldTempEr;
		Vector ThOldTempErSum;
		Vector TempEr;
		Vector ThTE;

		Vector ThDeltaT;
		Vector ThGain;   
		PumaCalibrator PumaCal;

		Vector Kd;
		Vector Kp;
		Vector Ki;


};








//takes joint angles values from shell
Vector AskForNewPosition()
{
	Vector jointPosition(6);
	//ask for the new position

	double j1,j2,j3,j4,j5,j6;


	printf("\n please digit 6 integers divided by SPACE for the next position of the end effector");
			 printf("\n angle n 1 = ");scanf("%lf",&j1);
			 printf("\n angle n 2 = ");scanf("%lf",&j2);
			 printf("\n angle n 3 = ");scanf("%lf",&j3);
			 printf("\n angle n 4 = ");scanf("%lf",&j4);
			 printf("\n angle n 5 = ");scanf("%lf",&j5);
			 printf("\n angle n 6 = ");scanf("%lf",&j6);
	
	jointPosition(0) = j1;
	jointPosition(1) = j2;
	jointPosition(2) = j3;
	jointPosition(3) = j4;
	jointPosition(4) = j5;
	jointPosition(5) = j6;

	printf("\n it seems to me you have written %lf %lf %lf %lf %lf %lf\n",
		jointPosition(0),jointPosition(1),jointPosition(2),jointPosition(3),jointPosition(4),jointPosition(5));
	return jointPosition;
}













/*
main for the controller 
*/
int main(int argc, char *argv[]) {

	
		int mymilliseconds=15;
		int contomio = 1000;
		ControlLoopThread MyTh(mymilliseconds); //16ms is 60Hz
	

		bool stop =0;
	


	
	double *SafePosition = new double[6];
	double *NewPosition = new double[6];
	double *TempPosition = new double[6];
	



	Vector jointStartingPosition(6);
	Vector jointGoalPosition(6);

	PumaKinematics Puma;
	Matrix GoalPosition(4,4);
	Matrix ActualPosition(4,4);
	int i;
	
	Time::turboBoost();
	double *spds = new double[6];
	double *accs = new double[6];
	double *encval = new double[6];

	for( i=0; i<6; i++)
	{	
		spds[i]=0.0;
		accs[i]=0.0;
		encval[i]=0.0;
		SafePosition[i]=0.0;
		NewPosition[i]=0.0;
		TempPosition[i]=0.0;
	}

	Property p; 
    p.fromConfigFile("MEIconfig.txt");

	int nj = 6;
	Bottle& xtmp = p.findGroup("STARTINGVALUES").findGroup("SafePosition");
	ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n jointStartingPosition = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        jointStartingPosition[i-1] = xtmp.get(i).asDouble();
		printf("  %lf", jointStartingPosition[i-1]);
		}
	printf("\n");

	ActualPosition = Puma.FwdPuma200Kin(jointStartingPosition);

  	printf(" \n\n\n ActualPosition at the beginning\n");
		for( i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
				{
				printf("  %lf",  ActualPosition[i][j]);
			}
			printf(" \n");
		}



	xtmp = p.findGroup("STARTINGVALUES").findGroup("SafePosition");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n SafePosition = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        SafePosition[i-1] = xtmp.get(i).asDouble();
		printf("  %lf", SafePosition[i-1]);
		}
	printf("\n");


	xtmp = p.findGroup("STARTINGVALUES").findGroup("Acc");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n ThAcc = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        accs[i-1] = xtmp.get(i).asDouble();
		printf(" %.1lf", accs[i-1]);
		}


	xtmp = p.findGroup("STARTINGVALUES").findGroup("Speed1");
    ACE_ASSERT (xtmp.size() == nj+1);
	printf("\n Speed = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        spds[i-1] = xtmp.get(i).asDouble();
		printf(" %lf", spds[i-1]);
		}



    if (!p.check("STARTINGVALUES")) {
        fprintf(stderr, "Cannot understand configuration parameters\n");
        return false;
    }



     PolyDriver calib("pumacalibrator");
     if (!calib.isValid())
         {
             printf("Calibrator not available, Here are the known devices:\n");
             printf("%s", Drivers::factory().toString().c_str());
             return 0;
         }
    
    // create a device
    PolyDriver armdd("meiMotionControl");
    if (!armdd.isValid()) 
		{
		    printf("Device not available.  Here are the known devices:\n");
			printf("%s", Drivers::factory().toString().c_str());
			return 0;
	    }


    IPidControl			*pid;
    IAmplifierControl	*amp;
    IPositionControl	*pos;
	IEncoders			*enc;
	IVelocityControl	*vel;


    int j = 0;	

	Pid * pd = NULL;
	pd = new Pid[6];




	bool ok;
    ICalibrator *ical;
    IControlCalibration *arm_cal;

    ok = armdd.view(pid);
    ok &= armdd.view(amp);
    ok &= armdd.view(pos);
	ok &= armdd.view(enc);
	ok &= armdd.view(vel);

	
	pid->getPids(pd);	

    // only local:
    ok &= calib.view(ical);
    ok &= armdd.view(arm_cal);


	//Let's calibrate
    arm_cal->setCalibrator(ical);
    arm_cal->calibrate();



			
	enc->getEncoders(encval);



    if (!ok) {
        printf("Problems acquiring interfaces\n");
        return 0;
    }

	double *myvel = new double [6];

    //// enable amps the robot
    int jnts;
    pos->getAxes(&jnts);


	for (i = 0; i < jnts; i++) {
		amp->enableAmp(i);
		pid->enablePid(i);
		myvel[i]= jointGoalPosition[i];
	}

	

	double j1,j2,j3,j4,j5,j6;
	int flag = 0;
	PumaKinematics MyPuma;
	Matrix GoalPos(4,4);


	while (flag == 0)
		{

		j1 = 0;
		j2 = 0;
		j3 = 0;
		j4 = 0;
		j5 = 0;
		j6 = 0;


		printf("\n please digit 6 integers for the NEW starting position of the end effector");
		printf("\n angle n 1 = ");scanf("%lf",&j1);
		printf(" angle n 2 = ");scanf("%lf",&j2);
		printf(" angle n 3 = ");scanf("%lf",&j3);
		printf(" angle n 4 = ");scanf("%lf",&j4);
		printf(" angle n 5 = ");scanf("%lf",&j5);
		printf(" angle n 6 = ");scanf("%lf",&j6);
		
		NewPosition[0] = j1;
		NewPosition[1] = j2;
		NewPosition[2] = j3;
		NewPosition[3] = j4;
		NewPosition[4] = j5;
		NewPosition[5] = j6;

		
	enc->getEncoders(encval);
	printf(" \n encval\n");
	for( i=0; i<6; i++)
	{
		TempPosition[i]=NewPosition[i]+jointStartingPosition[i];
		printf("  %lf",encval[i]);
	}


		GoalPos = MyPuma.FwdPuma200Kin(TempPosition);

  		printf(" \n\n\n GoalPos\n");
			for( i=0;i<4;i++)
			{
				for( j=0;j<4;j++)
					{
					printf("  %lf",GoalPos[i][j]);
				}
				printf(" \n");
			}


	
			
		pos->setPositionMode();
		printf("\n you would like to go in %lf %lf %lf %lf %lf %lf",
			NewPosition[0],NewPosition[1],NewPosition[2],NewPosition[3],NewPosition[4],NewPosition[5]);

		printf("\n do yuo like it? 0/1 ");scanf("%d",&flag);







}		
		pos->setRefSpeeds(spds);
		printf("\n speed setted!");
		pos->setRefAccelerations(accs);
		printf("\n accs setted!");
		pos->positionMove(NewPosition);
		printf("\n moved!");

		enc->getEncoders(encval);
		for( i=0; i<6; i++)
		printf("\n fine encval %d=%.10lf", i,encval[i]);

		printf("\nMyTh rate is %d[ms]\n", mymilliseconds);
		MyTh.initialization(enc,vel,encval,jointStartingPosition);



		printf("\n\nStarting thread...");
		bool oki=MyTh.start();
		if (!oki)
		{
			printf("ratethread failed to initialize, returning\n");
			return -1;
		}

		while(stop == 0) 
		{

			stop = MyTh.checkbutton();
		}


		printf("stopping threads...\n");
		MyTh.stop();
		printf("thread stopped...\n");

		

///////////////////////////////////////////////////////////////////////////////////////////


		
		if (spds != NULL)		delete [] spds;	
		if (accs != NULL)		delete [] accs;	
		if (encval != NULL)		delete [] encval;	
		if (pd != NULL)			delete [] pd;	
		if (myvel != NULL)		delete [] myvel;	

		if (armdd.isValid())  armdd.close();


		printf("\n close ALL...\n");


    return 0;
}
