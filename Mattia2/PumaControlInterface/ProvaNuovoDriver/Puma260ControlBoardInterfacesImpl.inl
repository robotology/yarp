// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2006 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include <math.h>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid repetations.

#include <ace/Log_Msg.h>
#include <stdio.h>
#include <ace/OS.h>

using namespace yarp::dev;
/*
//const double fwdCouple[] = {   0.0 ,      0.0 ,     0.0 ,   7876.986 ,   800.0 ,  4479.99092};

//const double AngToEnc[] = {46720.0 , -69973.3 , 42986.7 , -34808.88 ,  31507.68 , -25415.36};

//segni cambiati mattia 122007
const double invCouple3 = fwdCouple[3] / (AngToEnc[3] * AngToEnc[4]);
const double invCouple4 = -fwdCouple[4] / (AngToEnc[3] * AngToEnc[5]) + (fwdCouple[3] * fwdCouple[5]) /	(AngToEnc[3] * AngToEnc[4] * AngToEnc[5]);
const double invCouple5 = -fwdCouple[5] / (AngToEnc[4] * AngToEnc[5]);

const double invCouple3 = fwdCouple[3] / (angleToEncoders[3] * angleToEncoders[4]);
const double invCouple4 = -fwdCouple[4] / (angleToEncoders[3] * angleToEncoders[5]) + (fwdCouple[3] * fwdCouple[5]) /	(angleToEncoders[3] * angleToEncoders[4] * angleToEncoders[5]);
const double invCouple5 = -fwdCouple[5] / (angleToEncoders[4] * angleToEncoders[5]);
*/




/*
 * simple helper template to alloc memory.
 */
template <class T>
inline T* allocAndCheck(int size)
{
    T* t = new T[size];
    ACE_ASSERT (t != 0);
    ACE_OS::memset(t, 0, sizeof(T) * size);
    return t;
}

/*
 *
 */
template <class T>
inline void checkAndDestroy(T* &p) {
    if (p!=0) {
        delete [] p;
        p = 0;
    }
}



/*
 * ZeroManager
 */

class ZeroManager
{
public:
	double *myzeros;

	ZeroManager ();
	~ZeroManager ();

	bool initialize ();
	bool uninitialize ();
	void SetZeros( double *newzeros);
	void SetZero(int axis, double newzero);
	void GetZeros( double *newzeros);
	void GetZero(int axis, double &newzero);

};

ZeroManager::ZeroManager()
{
	myzeros = NULL;
	initialize();
}

ZeroManager::~ZeroManager () 
{ 
//	uninitialize(); 
}

bool ZeroManager::initialize ()
{
	myzeros			= new double[6];
	for (int i = 0; i<6; i++)
		{
			myzeros[i] = 0.0;
		}
	return true;
}
	

inline void ZeroManager::SetZeros( double *newzeros)
    {
        for (int k=0;k<6;k++)
		{
            myzeros[k]=newzeros[k];
//			myzeros[toHw(k)]=newzeros[k];
		}
    }

inline void ZeroManager::SetZero(int axis, double newzero)
    {
         myzeros[axis]=newzero;
//		 myzeros[axis]=newzero;
    }

	inline void ZeroManager::GetZeros( double *newzeros)
    {
//		printf("\n newzeros = ");
        for (int k=0;k<6;k++)
		{
			newzeros[k] = myzeros[k];
//			newzeros[toHw(k)] =myzeros[k];
//			printf(" %lf",newzeros[k]);

			

		}
    }

    inline void ZeroManager::GetZero(int axis, double &newzero)
    {
//		printf("\n newzeros = ");
        newzero = myzeros[axis];
//		newzero = myzeros[axis];
//		printf("axis %d zero =  %lf",axis,newzero);
    }



ZeroManager MyZero;














//////////////// Control Board Helper
class ControlBoardHelper
{
public:
	
	ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs): zeros(0), 
        signs(0),
        axisMap(0),
        invAxisMap(0),
        angleToEncoders(0)
    {
        nj=n;
        alloc(n);


		int i;
			


		

		 memcpy(axisMap, aMap, sizeof(int)*nj);
        
        if (zs!=0)
            memcpy(zeros, zs, sizeof(double)*nj);
        else
            memset(zeros, 0, sizeof(double)*nj);

       

        if (angToEncs!=0)
            memcpy(angleToEncoders, angToEncs, sizeof(double)*nj);
        else
            memset(angleToEncoders, 0, sizeof(double)*nj);

        // invert the axis map
   		memset (invAxisMap, 0, sizeof(int) * nj);
		
        for (i = 0; i < nj; i++)
		{
			int j;
			for (j = 0; j < nj; j++)
			{
				if (axisMap[j] == i)
				{
					invAxisMap[i] = j;
					break;
				}
			}
		}




				Property p;
    	p.fromConfigFile("MEIconfig.txt");

    
		if (!p.check("GENERAL")) {
        fprintf(stderr, "Cannot understand configuration parameters\n");
        return;
    	}


			
		Bottle& xtmp = p.findGroup("GENERAL").findGroup("FwdCouple");
    	ACE_ASSERT (xtmp.size() == nj+1);
//		printf("\n fwdCouple = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        fwdCouple[i-1] = xtmp.get(i).asDouble()*800.0;
//		printf("  %lf", fwdCouple[i-1]);
		}
/*

		printf("\n");


		p.findGroup("GENERAL").findGroup("AngToEnc");
    	ACE_ASSERT (xtmp.size() == nj+1);
		printf("\n angToEncs = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        angToEncs[i-1] = xtmp.get(i).asDouble();
		printf("  %lf", angToEncs[i-1]);
		}
		printf("\n");
*/	
		invCouple3 = -fwdCouple[3] / (angToEncs[3] * angToEncs[4]);
		invCouple4 = -fwdCouple[4] / (angToEncs[3] * angToEncs[5]) + (fwdCouple[3] *  fwdCouple[5]) /	(angToEncs[3] * angToEncs[4] * angToEncs[5]);
		invCouple5 = -fwdCouple[5] / (angToEncs[4] * angToEncs[5]);

	


/*
		printf("\n AngToEnc = ");
	    for ( i = 1; i < xtmp.size(); i++)
		{
        printf("  %lf", angToEncs[i-1]);
		}
		printf("\n");


		printf("\n invCouple3  %lf", invCouple3);
		printf("\n invCouple4  %lf", invCouple4);
		printf("\n invCouple5  %lf", invCouple5);

*/






    } 


    ~ControlBoardHelper() 
    {
        dealloc();
    }

    bool alloc(int n)
    {
        nj=n;
        if (nj<=0)
            return false;

        if (zeros!=0)
            dealloc();

		
        zeros=new double [nj];
        signs=new double [nj];
        axisMap=new int [nj];
        invAxisMap=new int [nj];
        angleToEncoders=new double [nj];
        ACE_ASSERT(zeros != 0 && signs != 0 && axisMap != 0 && invAxisMap != 0 && angleToEncoders != 0);


		fwdCouple=new double [nj];






        return true;
    }

    bool dealloc()
    {
//		delete [] invCouple;
		checkAndDestroy<double> (zeros);
        checkAndDestroy<double> (signs);
        checkAndDestroy<int> (axisMap);
        checkAndDestroy<int> (invAxisMap);
        checkAndDestroy<double> (angleToEncoders);
        return true;
    }

    inline int toHw(int axis)
    { return axisMap[axis]; }
    
    inline int toUser(int axis)
    { return invAxisMap[axis]; }

    //map a vector, no conversion
    inline void toUser(const double *hwData, double *user)
    {
        for (int k=0;k<nj;k++)
         user[toUser(k)]=hwData[k];
    }

     //map a vector, no conversion
    inline void toUser(const int *hwData, int *user)
    {
        for (int k=0;k<nj;k++)
         user[toUser(k)]=hwData[k];
    }

    //map a vector, no conversion
    inline void toHw(const double *usr, double *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }

     //map a vector, no conversion
    inline void toHw(const int *usr, int *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }


    inline double posA2E(double angle,  int j)
    {

		double *enc = new double[nj];
		double *ang = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				if(i!=j)
					ang[i]=0.0;
				else
					ang[i]=angle;
			}

		posA2E(ang, enc);

		return enc[j];
    }

    
	inline double posE2A(double encoder, int j)
    {
		int k=toUser(j);

		double *enc = new double[nj];
		double *ang = new double[nj];
		
		int i;
		
		for (i = 0; i < nj; i++)
			{
				if(i!=j)
					enc[i]=0.0;
				else
					enc[i]=encoder;
			}

		posE2A( enc, ang);

		return ang[j];

    }

    inline void velA2E(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
		double *encoders = new double[nj];
		double *angles = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				encoders[i]=0.0;
				if(i==j) angles[i]=ang;
				else angles[i] = 0.0;
			}

		velA2E(angles, encoders);
		enc =  encoders[j];
		delete [] encoders;
		delete [] angles;

    }

	inline void velA2E_abs(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
		double *encoders = new double[nj];
		double *angles = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				encoders[i]=0.0;
				if(i==j) angles[i]=ang;
				else angles[i] = 0.0;
			}

		velA2E_abs(angles, encoders);
		enc =  encoders[j];

		delete [] encoders;
		delete [] angles;
    }


	    inline double velE2A(double enc, int j)
    {
			double ang;
		double *encoders = new double[nj];
		double *angles = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				angles[i]=0.0;
				if(i==j) encoders[i]=enc;
				else encoders[i] = 0.0;
			}

		velE2A( encoders, angles);

		return ang = angles[j];
		
    }


    inline void velE2A(double enc, int j, double &ang, int &k)
    {

		double *encoders = new double[nj];
		double *angles = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				angles[i]=0.0;
				if(i==j) encoders[i]=enc;
				else encoders[i] = 0.0;
			}

		velE2A( encoders, angles);
		ang = angles[j];
		
		delete [] encoders;
		delete [] angles;
    }


	
	inline double velE2A_abs(double enc, int j)
    {
	double ang;
	double *encoders = new double[nj];
	double *angles = new double[nj];
	int i;
	for (i = 0; i < nj; i++)
	{
		angles[i]=NULL;
		if(i==j) encoders[i]=enc;
		else encoders[i] = 0.0;
	}

	velE2A_abs( encoders, angles);

	return ang = angles[j];
		
    }


	inline void velE2A_abs(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
		double *encoders = new double[nj];
		double *angles = new double[nj];
			int i;
			for (i = 0; i < nj; i++)
			{
				angles[i]=NULL;
				if(i==j) encoders[i]=enc;
				else encoders[i] = 0.0;
			}

		velE2A_abs( encoders, angles);

		ang = angles[j];
		
    }

    inline void accA2E(double ang, int j, double &enc, int &k)
    {
        velA2E(ang, j, enc, k);
    }
	
	inline void accA2E_abs(double ang, int j, double &enc, int &k)
    {
        velA2E_abs(ang, j, enc, k);
    }

    inline void accE2A(double enc, int j, double &ang, int &k)
    {
        velE2A(enc, j, ang, k);
    }

	inline void accE2A_abs(double enc, int j, double &ang, int &k)
    {
        velE2A_abs(enc, j, ang, k);
    }

	inline double accE2A(double enc, int j)
    {
        return velE2A(enc, j);
    }

	inline double accE2A_abs(double enc, int j)
    {
        return velE2A_abs(enc, j);
    }


	//////////////////Mattia
	
    //map a vector, convert from angles to encoders
    inline void posA2E(const double *ang, double *enc)
    {
		double *encval = new double[6];
		

	
		MyZero.GetZeros(encval);

		int i;
		for ( i = 0; i < 6; i++)
		{
			zeros[i]=encval[i];
			printf("\n zeros[%d] = %lf",i,zeros[i]);

		}

		
		for ( i = 0; i < 4; i++)
		{
			enc[i] = (ang[i]  * angleToEncoders[i] / 360.0) + zeros[i];
		}
	
		enc[4] = (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) +  zeros[4];

	
		enc[5] = (ang[5] * angleToEncoders[5] / 360.0) + (ang[3] * fwdCouple[4] / 360.0) +
			 (ang[4] * fwdCouple[5] / 360.0) +  zeros[5];


/*
		printf("\n posA2E");
		printf("\n enc[%d] = %lf",4,enc[4]);
		printf("\n ang[%d] = %lf",4,ang[4]);


		printf("\n angleToEncoders[%d] = %lf",4,angleToEncoders[4]);
		printf("\n ang[%d] = %lf",3,ang[3]);
		printf("\n fwdCouple[%d] = %lf",3,fwdCouple[3]);
		printf("\n zeros[%d] = %lf",4,zeros[4]);

		printf("\n ang[4] * angleToEncoders[4] / 360.0 = %lf",ang[4] * angleToEncoders[4] / 360.0);
		printf("\n ang[3] * fwdCouple[3] / 360.0 = %lf",ang[3] * fwdCouple[3] / 360.0);
		printf("\n zeros[%d] = %lf",4,zeros[4]);

		printf("\n----- (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) -  zeros[4] = %lf",
			(ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) -  zeros[4]);

		
		printf("\n----- (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) +  zeros[4] = %lf",
			(ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) +  zeros[4]);


		printf("\n ang[5] * angleToEncoders[5] / 360.0 = %lf",ang[5] * angleToEncoders[5] / 360.0);
		printf("\n (ang[3] * fwdCouple[4] / 360.0 = %lf",ang[3] * fwdCouple[4] / 360.0);
		printf("\n ang[4] * fwdCouple[5] / 360.0 = %lf",ang[4] * fwdCouple[5] / 360.0);
		printf("\n zeros[%d] = %lf",5,zeros[5]);

		printf("\n (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) +  zeros[4] = %lf",
			(ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) +  zeros[4]  );

		printf("\n (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) -  zeros[4] = %lf",
			(ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0) -  zeros[4]  );

*/


    }

    //map a vector, convert from encoders to angles
    inline void posE2A(const double *enc, double *ang)
    {
		double *encval = new double[6];
		
		MyZero.GetZeros(encval);

		int i;
		for ( i = 0; i < 6; i++)
		{
			zeros[i]=encval[i];
		}

	for ( i = 0; i < 4; i++)
	{
		ang[i] = (enc[i] - zeros[i]) * 360.0 / angleToEncoders[i] ;
	}

	double e3, e4, e5;
	e3 = (enc[3] - zeros[3]) * 360.0;
	e4 = (enc[4] - zeros[4]) * 360.0;   
	e5 = (enc[5] - zeros[5]) * 360.0;

	ang[4] =  e4 / angleToEncoders[4] + e3 * invCouple3;

	ang[5] = e5 / angleToEncoders[5] +  e3 * invCouple4 + e4 * invCouple5;

/*
		printf("\n posE2A");
		printf("\n e3 = %lf",e3);
		printf("\n enc[3] = %lf",enc[3]);
		printf("\n zeros[3] = %lf",zeros[3]);

		printf("\n e4 = %lf",e4);
		printf("\n enc[4] = %lf",enc[4]);
		printf("\n zeros[4] = %lf",zeros[4]);

		printf("\n e5 = %lf",e5);
		printf("\n enc[5] = %lf",enc[5]);
		printf("\n zeros[5] = %lf",zeros[5]);

		printf("\n enc[%d] = %lf",4,enc[4]);
		printf("\n ang[%d] = %lf",4,ang[4]);
		printf("\n angleToEncoders[%d] = %lf",4,angleToEncoders[4]);
		printf("\n e4 / angleToEncoders[4] = %lf",e4 / angleToEncoders[4]);
		printf("\n invCouple3 = %lf",invCouple3);
		printf("\n e3 * invCouple3 = %lf",e3 * invCouple3);
		printf("\n e4 / angleToEncoders[4] + e3 * invCouple3 = %lf",e4 / angleToEncoders[4] + e3 * invCouple3);

		printf("\n ang[%d] = %lf",3,ang[3]);
		printf("\n angleToEncoders[%d] = %lf",5,angleToEncoders[5]);
		printf("\n e5 / angleToEncoders[5] = %lf",e5 / angleToEncoders[5]);

		printf("\n invCouple4 = %lf",invCouple4);
		printf("\n e3 * invCouple4 = %lf",e3 * invCouple4);

		
		printf("\n invCouple5 = %lf",invCouple5);
		printf("\n e4 * invCouple5 = %lf",e4 * invCouple5);


		printf("\n zeros[%d] = %lf",3,zeros[3]);
		printf("\n zeros[%d] = %lf",4,zeros[4]);
		printf("\n zeros[%d] = %lf",5,zeros[5]);

*/
 }





    inline void velA2E(const double *ang, double *enc)
    {
		int i;
		
		for ( i = 0; i < 4; i++)
		{
			enc[i] = (ang[i]  * angleToEncoders[i] / 360.0);
		}

/*		
		enc[4] = (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0);

	
		enc[5] = (ang[5] * angleToEncoders[5] / 360.0) + (ang[3] * fwdCouple[4] / 360.0) +
			 (ang[4] * fwdCouple[5] / 360.0);

*/
		
			
		enc[4] = (ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0);

	
		enc[5] = (ang[5] * angleToEncoders[5] / 360.0) + (ang[3] * fwdCouple[4] / 360.0) +
			 (ang[4] * fwdCouple[5] / 360.0);
/*
		double safespeed=3000000;
		for ( i = 0; i < 6; i++)
		{
			if(enc[i]>safespeed)
			{
				printf("IT would like to go at %lf\nTOO FAST!!!!!!!!!!! I am going to SLOW DOWN to %lf",enc[i],safespeed);
				enc[i]=safespeed;
			}
		}

*/
    }

	inline void velA2E_abs(const double *ang, double *enc)
    {

		int i;
		

		for ( i = 0; i < 4; i++)
		{
			enc[i] = abs(ang[i]  * angleToEncoders[i] / 360.0);
		}

		enc[4] = abs((ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0));

	
		enc[5] = abs((ang[5] * angleToEncoders[5] / 360.0) + (ang[3] * fwdCouple[4] / 360.0) +
			 (ang[4] * fwdCouple[5] / 360.0));


/*	
		enc[4] = abs((ang[4] * angleToEncoders[4] / 360.0) + (ang[3] * fwdCouple[3] / 360.0));

	
		enc[5] = abs((ang[5] * angleToEncoders[5] / 360.0) + (ang[3] * fwdCouple[4] / 360.0) +
			 (ang[4] * fwdCouple[5] / 360.0));

		double safespeed=3000000;
		for ( i = 0; i < 6; i++)
		{
			if(enc[i]>safespeed)
			{
				printf("IT would like to go at %lf\nTOO FAST!!!!!!!!!!! \n**I am going to SLOW DOWN to %lf",enc[i],safespeed);
				enc[i]=safespeed;
			}
		}

*/

    }

    inline void velE2A(const double *enc, double *ang)
    {
 
	int i;
	for ( i = 0; i < 4; i++)
	{
		ang[i] = enc[i]  * 360.0 / angleToEncoders[i];
	}

	double e3, e4, e5;
	e3 = enc[3]  * 360.0;
	e4 = enc[4]  * 360.0;   
	e5 = enc[5]  * 360.0;
/*
	ang[4] =  e4 / angleToEncoders[4] + e3 * invCouple3;

	ang[5] = e5 / angleToEncoders[5] +  e3 * invCouple4 + e4 * invCouple5;
*/

	ang[4] =  e4 / angleToEncoders[4] + e3 * invCouple3;

	ang[5] = e5 / angleToEncoders[5] +  e3 * invCouple4 + e4 * invCouple5;


/*			//da togliere!!!!!
		double safespeed=10;
		for ( i = 0; i < 6; i++)
		{
			if(ang[i]>safespeed)
			{
				printf("IT would like to go at %lf\nTOO FAST!!!!!!!!!!! I am going to SLOW DOWN to %lf",ang[i],safespeed);
				ang[i]=safespeed;
			}
		}
*/

    }

	inline void velE2A_abs(const double *enc, double *ang)
    {
		
		int i;

		for ( i = 0; i < 4; i++)
	{
		ang[i] = abs(enc[i]  * 360.0 / angleToEncoders[i]);
	}

	double e3, e4, e5;
	e3 = enc[3]  * 360.0;
	e4 = enc[4]  * 360.0;   
	e5 = enc[5]  * 360.0;

	ang[4] =  abs(e4 / angleToEncoders[4] + e3 * invCouple3);

	ang[5] = abs(e5 / angleToEncoders[5] +  e3 * invCouple4 + e4 * invCouple5);

		double safespeed=10;
		for ( i = 0; i < 6; i++)
		{
			if(ang[i]>safespeed)
			{
				printf("IT would like to go at %lf\nTOO FAST!!!!!!!!!!! I am going to SLOW DOWN to %lf",ang[i],safespeed);
				ang[i]=safespeed;
			}
		}


    }

    inline void accA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

	inline void accA2E_abs(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accA2E_abs(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void accE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

	inline void accE2A_abs(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accE2A_abs(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline int axes()
    { return nj; }
        
 	int nj;

	double *zeros;
	double *signs;
	int *axisMap;
	int *invAxisMap;
	double *angleToEncoders;

	double *fwdCouple;


	double invCouple3;
	double invCouple4;
	double invCouple5;


};





inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }



template <class DERIVED, class IMPLEMENT> 
ImplementPositionControl<DERIVED, IMPLEMENT>::ImplementPositionControl(DERIVED *y)
{
    iPosition = dynamic_cast<IPositionControlRaw *>(y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementPositionControl<DERIVED, IMPLEMENT>::~ImplementPositionControl()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::positionMove(int j, double v)
{
//    int k;
    double enc;
//    castToMapper(helper)->posA2E(v, j, enc, k);
	enc = castToMapper(helper)->posA2E(v, j);
    return iPosition->positionMoveRaw(j, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setPositionMode()
{
    if (helper==0) return false;
    iPosition->setPositionMode();
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::positionMove(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp);
    
    return iPosition->positionMoveRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::relativeMove(int j, double delta)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);
    
    return iPosition->relativeMoveRaw(k,enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::relativeMove(const double *deltas)
{
    castToMapper(helper)->velA2E(deltas, temp);
    
    return iPosition->relativeMoveRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::checkMotionDone(int j, bool *flag)
{
    int k=castToMapper(helper)->toHw(j);
    
    return iPosition->checkMotionDoneRaw(k,flag);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::checkMotionDone(bool *flag)
{
    return iPosition->checkMotionDoneRaw(flag);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeed(int j, double sp)
{
    int k;
    double enc;
    //castToMapper(helper)->velA2E_abs(sp, j, enc, k);
	castToMapper(helper)->velA2E(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeeds(const double *spds)
{
    //castToMapper(helper)->velA2E_abs(spds, temp);
	castToMapper(helper)->velA2E(spds, temp);
    
    return iPosition->setRefSpeedsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;
    
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition->setRefAccelerationRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E_abs(accs, temp);
    
    return iPosition->setRefAccelerationsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeed(int j, double *ref)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    
    bool ret = iPosition->getRefSpeedRaw(k, &enc);
    
//    *ref=(castToMapper(helper)->velE2A_abs(enc, k));
	*ref=(castToMapper(helper)->velE2A(enc, k));
    
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeeds(double *spds)
{
    bool ret = iPosition->getRefSpeedsRaw(temp);
    //castToMapper(helper)->velE2A_abs(temp, spds);
	castToMapper(helper)->velE2A(temp, spds);
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAccelerations(double *accs)
{
    bool ret=iPosition->getRefAccelerationsRaw(temp);
    castToMapper(helper)->accE2A_abs(temp, accs);
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAccelerationRaw(k, &enc);
    
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::stop(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    
    return iPosition->stopRaw(k);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::stop()
{
    return iPosition->stopRaw();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getAxes(int *axis)
{
    (*axis)=castToMapper(helper)->axes();
    
    return true;     
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    checkAndDestroy(temp);

    return true;
}
/////////////////// Implement PostionControl

//////////////////// Implement VelocityControl
template <class DERIVED, class IMPLEMENT> ImplementVelocityControl<DERIVED, IMPLEMENT>::
ImplementVelocityControl(DERIVED *y)
{
    iVelocity = dynamic_cast<IVelocityControlRaw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> ImplementVelocityControl<DERIVED, IMPLEMENT>::
~ImplementVelocityControl()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }

    checkAndDestroy(temp);

    return true;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::getAxes(int *axes)
{
    (*axes)=castToMapper(helper)->axes();
    return true;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::setVelocityMode()
{
    return iVelocity->setVelocityMode();
}

template <class DERIVED, class IMPLEMENT>
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::velocityMove(int j, double sp)
{
    int k;
    double enc;

    castToMapper(helper)->velA2E(sp, j, enc, k);

    return iVelocity->velocityMoveRaw(k, enc);
}


template <class DERIVED, class IMPLEMENT>
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::velocityMove(const double *sp)
{
    castToMapper(helper)->velA2E(sp, temp);
    return iVelocity->velocityMoveRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;
    
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iVelocity->setRefAccelerationRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E_abs(accs, temp);
    
    return iVelocity->setRefAccelerationsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::getRefAccelerations(double *accs)
{
    bool ret=iVelocity->getRefAccelerationsRaw(temp);
    castToMapper(helper)->accE2A_abs(temp, accs);
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefAccelerationRaw(k, &enc);
    
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::stop(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    
    return iVelocity->stopRaw(k);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::stop()
{
    return iVelocity->stopRaw();
}
/////////////////////////////////////////////////////////////////

//////////////////// Implement PidControl interface
template <class DERIVED, class IMPLEMENT> ImplementPidControl<DERIVED, IMPLEMENT>::
ImplementPidControl(DERIVED *y)
{
    iPid= dynamic_cast<IPidControlRaw *> (y);
    helper = 0;        
    temp=0;
    tmpPids=0;
}

template <class DERIVED, class IMPLEMENT> ImplementPidControl<DERIVED, IMPLEMENT>::
~ImplementPidControl()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPidControl<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);
    tmpPids=new Pid[size];
    ACE_ASSERT (tmpPids != 0);
    
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementPidControl<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
        delete castToMapper(helper);
    helper=0;    

    checkAndDestroy(tmpPids);
    checkAndDestroy(temp);

    return true;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setPid(int j, const Pid &pid)
{
    int k=castToMapper(helper)->toHw(j);
    return iPid->setPidRaw(k,pid);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setPids(const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }
    
    return iPid->setPidsRaw(tmpPids);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setReference(int j, double ref)
{
    int k=0;
    double enc;
	enc = castToMapper(helper)->posA2E(ref, j);

    return iPid->setReferenceRaw(j, enc);
//    castToMapper(helper)->posA2E(ref, j, enc, k);

//    return iPid->setReferenceRaw(k, enc);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setReferences(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp);

    return iPid->setReferencesRaw(temp);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setErrorLimit(int j, double limit)
{
//    int k;
    double enc = castToMapper(helper)->posA2E(limit, j);
    
    return iPid->setErrorLimitRaw(j, enc);

//    castToMapper(helper)->posA2E(limit, j, enc, k);
    
//    return iPid->setErrorLimitRaw(k, enc);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setErrorLimits(const double *limits)
{
    castToMapper(helper)->posA2E(limits, temp);

    return iPid->setErrorLimitsRaw(temp);
}

 
template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getError(int j, double *err)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getErrorRaw(k, &enc);

    *err=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getErrors(double *errs)
{
    bool ret;
    ret=iPid->getErrorsRaw(temp);

    castToMapper(helper)->velE2A(temp, errs);

    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getOutput(int j, double *out)
{
    int k;

    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getOutputRaw(k, out);

    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getOutputs(double *outs)
{
    bool ret=iPid->getOutputsRaw(temp);

    castToMapper(helper)->toUser(temp, outs);

    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getPid(int j, Pid *pid)
{

	int k=castToMapper(helper)->toHw(j);
    bool ret=iPid->getPidRaw(k, pid);

    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getPids(Pid *pids)
{
    bool ret=iPid->getPidsRaw(tmpPids);
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
        pids[castToMapper(helper)->toUser(j)]=tmpPids[j];
    
    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getReference(int j, double *ref)
{
    bool ret;
    int k;
    double enc;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getReferenceRaw(k, &enc);
    
    *ref=castToMapper(helper)->posE2A(enc, k);
    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getReferences(double *refs)
{
    bool ret;
    ret=iPid->getReferencesRaw(temp);

    castToMapper(helper)->posE2A(temp, refs);
    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getErrorLimit(int j, double *ref)
{
    bool ret;
    int k;
    double enc;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getErrorLimitRaw(k, &enc);
    
    *ref=castToMapper(helper)->posE2A(enc, k);
    return ret;
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::getErrorLimits(double *refs)
{
    bool ret;
    ret=iPid->getErrorLimitsRaw(temp);

    castToMapper(helper)->posE2A(temp, refs);
    return ret;
}

 
template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::resetPid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->resetPidRaw(k);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::enablePid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->enablePidRaw(k);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::disablePid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->disablePidRaw(k);
}

template<class DERIVED, class IMPLEMENT>
bool ImplementPidControl<DERIVED, IMPLEMENT>::setOffset(int j, double v)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->setOffsetRaw(k, v);
}

////////////////////////
// Encoder Interface Implementation
template <class DERIVED, class IMPLEMENT> 
ImplementEncoders<DERIVED, IMPLEMENT>::ImplementEncoders(DERIVED *y)
{
    iEncoders= dynamic_cast<IEncodersRaw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementEncoders<DERIVED, IMPLEMENT>::~ImplementEncoders()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementEncoders<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementEncoders<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    
    checkAndDestroy(temp);

    return true;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return true;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::resetEncoder(int j)
{
    int k;


    k=castToMapper(helper)->toHw(j);
	iEncoders->getEncoderRaw(k,temp);
	MyZero.SetZero(k,temp[k]);
    return iEncoders->resetEncoderRaw(k);
}




template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::resetEncoders()
{
	iEncoders->getEncodersRaw(temp);
	MyZero.SetZeros(temp);

    return iEncoders->resetEncodersRaw();
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::setEncoder(int j, double val)
{
//    int k;
    double enc;

	
    enc = castToMapper(helper)->posA2E(val, j);

    return iEncoders->setEncoderRaw(j, enc);
//    castToMapper(helper)->posA2E(val, j, enc, k);

//    return iEncoders->setEncoderRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::setEncoders(const double *val)
{
    castToMapper(helper)->posA2E(val, temp);

    return iEncoders->setEncodersRaw(temp);
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoder(int j, double *v)
{
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderRaw(k, &enc);

    *v=castToMapper(helper)->posE2A(enc, k);
    
	printf("\n enc[%d]=%lf ang[%d]=%lf",k,enc,j,v[k]);



    return ret;
}
    
template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoders(double *v)
{
    bool ret;
    ret=iEncoders->getEncodersRaw(temp);

    castToMapper(helper)->posE2A(temp, v);
    
    return ret;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoderSpeed(int j, double *v)
{
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderSpeedRaw(k, &enc);

    *v=castToMapper(helper)->velE2A(enc, k);
    
    return ret;
}
    
template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoderSpeeds(double *v)
{
    bool ret;
    ret=iEncoders->getEncoderSpeedsRaw(temp);

    castToMapper(helper)->velE2A(temp, v);
    
    return ret;
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoderAcceleration(int j, double *v)
{
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(helper)->accE2A(enc, k);
    
    return ret;
}
    
template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoderAccelerations(double *v)
{
    bool ret;
    ret=iEncoders->getEncoderAccelerationsRaw(temp);

    castToMapper(helper)->accE2A(temp, v);
    
    return ret;
}

////////////////////////
// ControlCalibration Interface Implementation
template <class DERIVED, class IMPLEMENT> 
ImplementControlCalibration<DERIVED, IMPLEMENT>::ImplementControlCalibration(DERIVED *y)
{
    iCalibrate= dynamic_cast<IControlCalibrationRaw *> (y);
	helper = 0;        
    temp=0;

}

template <class DERIVED, class IMPLEMENT> 
ImplementControlCalibration<DERIVED, IMPLEMENT>::~ImplementControlCalibration()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);
	
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    
    checkAndDestroy(temp);

    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration<DERIVED, IMPLEMENT>::calibrate(int j, double tmp)
{
	IEncodersRaw *enc = dynamic_cast <IEncodersRaw *>(this);
	int k=castToMapper(helper)->toHw(j);
	int size=castToMapper(helper)->nj;
	int *amap=castToMapper(helper)->axisMap;
	double *angleToEncoders=castToMapper(helper)->angleToEncoders;

	double *encval = new double [6] ;
	
	double myzeroenc = castToMapper(helper)->zeros[j];
	printf("\nmyzeroenc[%d]=%lf",j,myzeroenc);

	bool calib = iCalibrate->calibrateRaw(k, encval[k]);
	enc->getEncodersRaw(encval);
	printf("encval[%d]=%lf",j,encval[j]);

	
	MyZero.SetZero(j,encval[j]);

    return calib;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration<DERIVED, IMPLEMENT>::done(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return iCalibrate->doneRaw(k);
}

////////////////////////

////////////////////////
// ControlCalibration2 Interface Implementation
template <class DERIVED, class IMPLEMENT> 
ImplementControlCalibration2<DERIVED, IMPLEMENT>::ImplementControlCalibration2(DERIVED *y)
{
    iCalibrate= dynamic_cast<IControlCalibration2Raw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementControlCalibration2<DERIVED, IMPLEMENT>::~ImplementControlCalibration2()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration2<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration2<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    
    checkAndDestroy(temp);

    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration2<DERIVED, IMPLEMENT>::calibrate2(int axis, unsigned int type, double p1, double p2, double p3)
{
    int k=castToMapper(helper)->toHw(axis);

    return iCalibrate->calibrate2Raw(k, type, p1, p2, p3);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlCalibration2<DERIVED, IMPLEMENT>::done(int j)
{
    int k=castToMapper(helper)->toHw(j);

    return iCalibrate->doneRaw(k);
}


///////////////// ImplementControlLimits
template <class DERIVED, class IMPLEMENT> 
ImplementControlLimits<DERIVED, IMPLEMENT>::ImplementControlLimits(DERIVED *y)
{
    iLimits= dynamic_cast<IControlLimitsRaw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementControlLimits<DERIVED, IMPLEMENT>::~ImplementControlLimits()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlLimits<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    // not sure if fix from next line to the line after is correct, hope so
    //helper=(void *)(new ControlBoardHelper(size, amap, enc, zeros));
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementControlLimits<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
        delete castToMapper(helper);
    helper=0;

    checkAndDestroy(temp);

    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlLimits<DERIVED, IMPLEMENT>::setLimits(int j, double min, double max)
{
    double minEnc;
    double maxEnc;

    int k=0;
    minEnc = castToMapper(helper)->posA2E(min, j);
    maxEnc = castToMapper(helper)->posA2E(max, j);

    if( (max > min) && (minEnc > maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = minEnc;
        minEnc = maxEnc;
        maxEnc = temp;
    }

    return iLimits->setLimitsRaw(k, minEnc, maxEnc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementControlLimits<DERIVED, IMPLEMENT>::getLimits(int j, double *min, double *max)
{
    double minEnc;
    double maxEnc;

    int k=castToMapper(helper)->toHw(j);
    bool ret=iLimits->getLimitsRaw(k, &minEnc, &maxEnc);

    *min=castToMapper(helper)->posE2A(minEnc, k);
    *max=castToMapper(helper)->posE2A(maxEnc, k);

    if( (*max < *min) && (minEnc < maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = *min;
        *min = *max;
        *max = temp;
    }
    return ret;
}

//////////////////////////////
///////////////// Implement 
template <class DERIVED, class IMPLEMENT> 
ImplementAmplifierControl<DERIVED, IMPLEMENT>::ImplementAmplifierControl(DERIVED *y)
{
    iAmplifier= dynamic_cast<IAmplifierControlRaw *> (y);
    helper = 0;        
    dTemp=0;
    iTemp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementAmplifierControl<DERIVED, IMPLEMENT>::~ImplementAmplifierControl()
{
    uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    // not sure if fix from next line to the line after is correct, hope so
    //helper=(void *)(new ControlBoardHelper(size, amap, enc, zeros));
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    dTemp=new double[size];
    ACE_ASSERT (dTemp != 0);
    iTemp=new int[size];
    ACE_ASSERT (iTemp != 0);
    
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
        delete castToMapper(helper);
    
    delete [] dTemp;
    delete [] iTemp;
    
    helper=0;
    dTemp=0;
    iTemp=0;
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::enableAmp(int j)
{
    int k=castToMapper(helper)->toHw(j);

    return iAmplifier->enableAmpRaw(k);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::disableAmp(int j)
{
    int k=castToMapper(helper)->toHw(j);

    return iAmplifier->disableAmpRaw(k);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::getCurrents(double *currs)
{
    bool ret=iAmplifier->getCurrentsRaw(dTemp);

    castToMapper(helper)->toUser(dTemp, currs);

    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::getCurrent(int j, double *c)
{
    int k=castToMapper(helper)->toHw(j);

    bool ret=iAmplifier->getCurrentRaw(k, c);

    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::setMaxCurrent(int j, double v)
{
    int k=castToMapper(helper)->toHw(j);

    return iAmplifier->setMaxCurrentRaw(k, v);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::getAmpStatus(int *st)
{
    bool ret=iAmplifier->getAmpStatusRaw(iTemp);

    castToMapper(helper)->toUser(iTemp, st);

    return ret;
}
/////////////////////////////
