// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale, Giorgio Metta, Francesco Nori
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <stdio.h>
//#include <ace/os.h>
#include <math.h>
#include <yarp/JamesHead.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Time.h>

#include "../../../ControlBoardInterfacesImpl.inl"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

const double    P_GAIN=0.008;
const int       NC_RATE=20;
const double    IN_POSITION_THRESHOLD = 0.1;
const double    REL_WEIGHT = 0.5;					//WARNING: has to be set between 0 and 1!!

const int HEAD_JOINTS=7;

void computeTendonsLength(double &d1, double &d2, double &d3, double Roll, double Pitch);
void computeTendonsLength2(double &d1, double &d2, double &d3, double Roll, double Pitch);
void computeModifiedPitchRoll(double Yaw, double Roll, double Pitch, double &Roll_hat, double &Pitch_hat);
void computeOriginalPitchRoll(double Yaw, double &Roll, double &Pitch, double Roll_hat, double Pitch_hat);

class HeadControl;
inline HeadControl* my_cast(void *p)
{
    return static_cast<HeadControl *>(p);
}

class HeadControl: public RateThread
{
private:
    PolyDriver *ddInertia;
    PolyDriver *ddHead;
    
    IGenericSensor *isensor;
    IVelocityControl *ivel;
    IPidControl *ipid;
    IEncoders *iencs;
    IAmplifierControl *iamps;
    IControlCalibration *icalib;
    IPositionControl *ipos;

	Semaphore mutex2;

    double *velocityCmds;
    double *positionCmds;
    double *positions;

	double roll_d;
	double roll_v;
	double timeRoll; 
	double tfRoll;
	double prevRoll_d;

	double pitch_d;
	double pitch_v;
	double timePitch; 
	double tfPitch;
	double prevPitch_d;


public:
    HeadControl(int r, PolyDriver *inertia, PolyDriver *head): RateThread(r),
                                                               ddInertia(0),
                                                               ddHead(0),
                                                               inertiaValue(3),
                                                               encoders(0),
                                                               nAxes(0)
    {
        pGain=P_GAIN;

        if (inertia!=0)
            ddInertia=inertia;
        if (head!=0)
            ddHead=head;

        vCmds=new double [4];

        positions=new double [HEAD_JOINTS];
        positionCmds=new double [HEAD_JOINTS];
        velocityCmds=new double [HEAD_JOINTS];

        for(int k=0; k<HEAD_JOINTS; k++)
            {
                positions[k]=0.0;
                positionCmds[k]=0.0;
                velocityCmds[k]=0.0;
            }

		roll_d = 0;
		roll_v = 0;
		timeRoll = 0; 
		tfRoll = 0;
		prevRoll_d = 0;

		pitch_d = 0;
		pitch_v = 0;
		timePitch = 0; 
		tfPitch = 0;
		prevPitch_d = 0;
    }
    
    ~HeadControl()
    {
        delete [] vCmds;
        delete [] positions;
        delete [] velocityCmds;
        delete [] positionCmds;
    }

    bool getRefAcceleration(int j, double *acc)
    {
		mutex.wait();
        for(int k=0;k<5;k++)
            ipos->getRefAcceleration(j, acc);

		mutex.post();
        return true;
    }

    bool getRefAccelerations(double *accs)
    {
		mutex.wait();

        for(int k=0;k<5;k++)
            ipos->getRefAcceleration(k, accs+k);

		mutex.post();
        return true; 
    }

    bool getRefSpeed(int j, double *sp)
    {
		mutex.wait();
        for(int k=0;k<5;k++)
            ipos->getRefSpeed(j, sp);

		mutex.post();
        return true;
    }

    bool getRefSpeeds(double *sps)
    {
		mutex.wait();
        for(int k=0;k<5;k++)
            ipos->getRefSpeed(k, sps+k);

		mutex.post();
        return true; 
    }

    bool setRefSpeeds(const double *sp)
    {
		mutex.wait();
        for(int k=0;k<5;k++)
            ipos->setRefSpeed(k, sp[k]);
		roll_v  = sp[5];
		pitch_v = sp[6];
		mutex.post();
        return true;
    }

    bool setRefSpeed(int j, double sp)
    {
		mutex.wait();
        if (j<5)
            ipos->setRefSpeed(j, sp);
		if (j==5)
			roll_v  = sp; 
		if (j==6)
			pitch_v = sp;
		mutex.post();
        return true;
    }

    bool setRefAcceleration(int j, double acc)
    {
		mutex.wait();
        if (j<5)
            ipos->setRefAcceleration(j, acc);
    
		mutex.post();
        return true;
    }

    bool setRefAccelerations(const double *acc)
    {
		mutex.wait();
        for(int k=0;k<5;k++)
            ipos->setRefAcceleration(k, acc[k]);

		mutex.post();
        return true;
    }

    bool positionMove(int j, double pos)
    {
        mutex.wait();

        if (j<5)
		{
			positionCmds[j]=pos;
            ipos->positionMove(j, pos);
		}
		if (j==5)
		{
			prevRoll_d = positionCmds[j];
			positionCmds[j] = pos;
			tfRoll  = 1000*fabs(positionCmds[j] - prevRoll_d)/roll_v; 
			timeRoll = 0;
		}
		if (j==6)
		{
			prevPitch_d = positionCmds[j];
			positionCmds[j] = pos;
			tfPitch  = 1000*fabs(positionCmds[j] - prevPitch_d)/pitch_v; 
			timePitch = 0;
		}
	
		mutex.post();

        return true;
    }

    bool positionMove(const double *pos)
    {
        mutex.wait();
        for(int k=0;k<HEAD_JOINTS;k++)
            {
                positionCmds[k]=pos[k];
                if (k<5)
                    ipos->positionMove(k, pos[k]);
				if (k==5)
				{
					prevRoll_d = positionCmds[k];
					positionCmds[k] = pos[k];
					tfRoll  = 1000*fabs(positionCmds[k] - prevRoll_d)/roll_v; 
					timeRoll = 0;
				}
				if (k==6)
				{
					prevPitch_d = positionCmds[k];
					positionCmds[k] = pos[k];
					tfPitch  = 1000*fabs(positionCmds[k] - prevPitch_d)/pitch_v; 
					timePitch = 0;
				}
            }
        mutex.post();
       
        return true;
    }

    bool velocityMove(int j, double vel)
    {
        mutex.wait();
        velocityCmds[j]=vel;

        if (j<5)
            ivel->velocityMove(j, vel);
		mutex.post();

        return true;
    }

	bool velPosMove_RAT_(const double *vel)
    {
        mutex.wait();
		for(int k=0;k<HEAD_JOINTS;k++)
		{
			//velocityCmds[k]=vel[k];
			if (k<5)
				ivel->velocityMove(k, vel[k]);
			else
			{
				if (k==5)
				{
					prevRoll_d = positionCmds[k];
					positionCmds[k] = vel[k];
					tfRoll  = 1000*fabs(positionCmds[k] - prevRoll_d)/roll_v; 
					timeRoll = 0;
				}
				if (k==6)
				{
					prevPitch_d = positionCmds[k];
					positionCmds[k] = vel[k];
					tfPitch  = 1000*fabs(positionCmds[k] - prevPitch_d)/pitch_v; 
					timePitch = 0;
				}
			}
		}
		mutex.post();

		return true;
    }

	bool resetEncoder(int j)
	{
		bool ret=false;
		if (j<4)
		{
			mutex.wait();
			ret=iencs->resetEncoder(j);
			mutex.post();
		}
		return ret;
	}

    bool velocityMove(const double *vel)
    {
		velPosMove_RAT_(vel);

	//        mutex.wait();
	//        for(int k=0;k<HEAD_JOINTS;k++)
	//            {
    //				velocityCmds[k]=vel[k];
    //				if (k<5)
    //				ivel->velocityMove(k, vel[k]);
    //			}
    //		mutex.post();

        return true;
    }

    bool relativeMove(int j, double d)
    {
		mutex.wait();

        if (j<5)
            ipos->relativeMove(j,d);
        
		mutex.post();
        return true;
    }

    bool relativeMove(const double *dq)
    { 
		mutex.wait();
        for(int k=0;k<HEAD_JOINTS;k++)
            {
                if (k<5)
                    ipos->relativeMove(k,dq[k]);
            }
        
		mutex.post();
        return true;
    }

    bool checkMotionDone(int j, bool *fl)
    {
		mutex.wait();
        if (j<5)
            ipos->checkMotionDone(j, fl);
		mutex.post();
        return true;
    }

    bool checkMotionDone(bool *fl)
    {
		mutex.wait();
        for(int k=0;k<HEAD_JOINTS;k++)
            {
                if (k<5)
                    ipos->checkMotionDone(k,&fl[k]);
                else
                    fl[k]=true;
            }
		mutex.post();

        return true;
    }

    bool enablePid(int j)
    {
		bool ret=false;
		mutex.wait();
        if (j<5)
            ret=ipid->enablePid(j);
        
		mutex.post();
		return ret;
    }

    bool disableAmp(int j)
    {
		bool ret=false;
		mutex.wait();
        if (j<5)
            ret=iamps->disableAmp(j);
        
		mutex.post();
		return ret;

    }

    bool enableAmp(int j)
    {
		bool ret=false;
		mutex.wait();
        if (j<5)
            ret=iamps->enableAmp(j);
        
		mutex.post();

		return ret;
    }

    bool disablePid(int j)
    {
		bool ret=false;
		mutex.wait();
        if (j<5)
            ret=ipid->disablePid(j);
        mutex.post();
		return ret;
    }

    bool halt(int j)
    { return true;}

    bool halt()
    {return true;}

    bool getEncoders(double *v)
    {
        mutex2.wait();
        memcpy(v, positions, sizeof(double)*HEAD_JOINTS);
        mutex2.post();

        return true;
    }

    bool getEncoder(int j, double *v)
    {
        mutex.wait();
        *v=positions[j];
        mutex.post();

        return true;
    }

    bool start()
    {
        ACE_OS::printf("NeckControl::starting...");

        bool ok;
        if (ddInertia==0)
            return false;
        if (ddHead==0)
            return false;

        ACE_OS::printf("successful\n");

        ok=ddInertia->view(isensor);
        if (!ok)
            {
                ACE_OS::printf("NeckControl::Error getting IGenericSensor interface, returning false\n");
                return false;
            }

        // calibration
        isensor->calibrate(0,0);
 
        ok=ddHead->view(ivel);
        ok=ok && ddHead->view(iencs);
        ok=ok && ddHead->view(iamps);
        ok=ok && ddHead->view(ipid);
        ok=ok && ddHead->view(icalib);
        ok=ok && ddHead->view(ipos);
        
        if (!ok)
            {
                ACE_OS::printf("NeckControl::Error getting head interfaces, returning false\n");
                return false;
            }

		mutex.wait();
        iencs->getAxes(&nAxes);
		mutex.post();
        encoders=new double [nAxes];
        return RateThread::start();
    }

	void enableControl()
	{
		ipid->enablePid(4);
		ipid->enablePid(5);
        ipid->enablePid(6);
        ipid->enablePid(7);

		iamps->enableAmp(4);
        iamps->enableAmp(5);
        iamps->enableAmp(6);
        iamps->enableAmp(7);
	}

	void disableControl()
	{
		ipid->disablePid(4);
        ipid->disablePid(5);
        ipid->disablePid(6);
        ipid->disablePid(7);

		iamps->disableAmp(4);
        iamps->disableAmp(5);
        iamps->disableAmp(6);
        iamps->disableAmp(7);
	}

    bool threadInit()
    {
		mutex.wait();
        ivel->setRefAcceleration(5, 1000);
        ivel->setRefAcceleration(6, 1000);
        ivel->setRefAcceleration(7, 1000);

		enableControl();
		
		ACE_OS::printf("NeckControl::Starting calibration sequence");
		yawCalibrate();
		rollPitchCalibrate();

		mutex.post();
		return true;
    }

	void yawCalibrate()
	{
		double pos;
		bool done;

		ACE_OS::printf("Calibrating yaw\n");

		icalib->calibrate(4, -200);

		while(!icalib->done(4))
			ACE_OS::printf(".");

		ACE_OS::printf("\nMoving to the center:");

		iencs->getEncoder(4, &pos);
		ipos->setRefSpeed(4,20);
		ipos->positionMove(4, 90);

		done = false;
		while(!done)
            {
                if (ipos->checkMotionDone(4, &done))
                    ACE_OS::printf(".");
                else
                    ACE_OS::printf("CheckMotionDone returned false\n");
            }
		
		ACE_OS::printf("\nYaw calibration terminated");
 
	}

	void rollPitchCalibrate()
	{
		ACE_OS::printf("Calibrating roll and pitch\n");

		bool inPosition = false;

        double roll_des=0;
        double pitch_des=0;

		ACE_OS::printf("Trying to reach zero configuration: ");
		while(!inPosition)
            {
                ACE_OS::printf(".");

                isensor->read(inertiaValue);

                roll=inertiaValue[0];
                pitch=inertiaValue[1];

                vCmds[0]=-600*(roll-roll_des);
                vCmds[1]=500*(pitch-pitch_des)+400*(roll-roll_des);
                vCmds[2]=-500*(pitch-pitch_des)+400*(roll-roll_des);
 
                vCmds[0]*=pGain;
                vCmds[1]*=pGain;
                vCmds[2]*=pGain;

                if (fabs(roll - roll_des) < IN_POSITION_THRESHOLD)
                    {
                        if (fabs(pitch - pitch_des) < IN_POSITION_THRESHOLD)
                            {
                                ACE_OS::printf("\nZero Configuration Reached\n");

                                inPosition = true;

                                vCmds[0]=0;
                                vCmds[1]=0;
                                vCmds[2]=0;
                            }
                    }

                //ivel->setRefAcceleration(5, 1000);
                //ivel->setRefAcceleration(6, 1000);
                //ivel->setRefAcceleration(7, 1000);

                ivel->velocityMove(5, vCmds[0]);
                ivel->velocityMove(6, vCmds[1]);
                ivel->velocityMove(7, vCmds[2]);
            }

		double pos;
		bool done;

		ACE_OS::printf("Pulling the cables:");
		icalib->calibrate(5, 400);
		icalib->calibrate(6, 400);
		icalib->calibrate(7, 400);

        int k=0;
		for (k = 5; k <8; k++)
            {
                while(!icalib->done(k))
                    ACE_OS::printf(".");
            }

		ACE_OS::printf("\nReleasing cable:");
		for (k = 5; k <8; k++)
            {
                iencs->getEncoder(k, &pos);
                ipos->setRefSpeed(k,5);
                ipos->positionMove(k, pos-5);
            }

		for (k = 5; k <8; k++)
            {
                done = false;
                while(!done)
                    {
                        if (ipos->checkMotionDone(k, &done))
                            ACE_OS::printf(".");
                        else
                            ACE_OS::printf("CheckMotionDone returned false\n");
                    }
            }
		
		ACE_OS::printf("\n");

	    disableControl();

		iencs->resetEncoder(5);
		iencs->resetEncoder(6);
		iencs->resetEncoder(7);

		enableControl();
	}

	/*
	 * Generates a minimum jerk profile
	 * as a reference trajectory for 
	 * the roll and pitch movements
	 */

	void updateDesiredRollPitch()
	{
		mutex.wait();
		timeRoll = timeRoll + NC_RATE;
		double tau = timeRoll/tfRoll;
		if (tau < 0 || tau > 1)
		{
			if (tau > 1)
				roll_d=positionCmds[5];
			else
				roll_d=prevRoll_d;
		}
		else
		{
			roll_d = prevRoll_d + (prevRoll_d - positionCmds[5])*
				(15*pow(tau, 4) - 6*pow(tau, 5) - 10*pow(tau, 3));
		}

		timePitch = timePitch + NC_RATE;
		tau = timePitch/tfPitch;
		if (tau < 0 || tau > 1)
		{
			if (tau > 1)
				pitch_d=positionCmds[6];
			else
				pitch_d=prevPitch_d;
		}
		else
		{
			pitch_d = prevPitch_d + (prevPitch_d - positionCmds[6])*
				(15*pow(tau, 4) - 6*pow(tau, 5) - 10*pow(tau, 3));
		}
		mutex.post();
	}

    void run()
    {
        static int count=0;
        double d1;              //length[cm] of the cable attached to joint 7
        double d2;              //length[cm] of the cable attached to joint 6
        double d3;              //length[cm] of the cable attached to joint 5


        isensor->read(inertiaValue);

        mutex2.wait();
		iencs->getEncoders(encoders);
		

        roll=inertiaValue[0];
        pitch=inertiaValue[1];
        yaw=encoders[4];

        positions[0]=encoders[0];
        positions[1]=encoders[1];
        positions[2]=encoders[2];
        positions[3]=encoders[3];
        positions[4]=encoders[4];
        positions[5]=roll;
        positions[6]=pitch;
		mutex2.post();

		updateDesiredRollPitch();
 		
		//accounts for the fact that the base of the neck
		//is not affected by the yaw (pan of the head)
		//while the sensor is affected.
		double pitch_hat;
		double roll_hat;

		double pitch_d_hat;
		double roll_d_hat;

		computeModifiedPitchRoll(yaw-90, roll, pitch, roll_hat, pitch_hat);
		computeOriginalPitchRoll(yaw-90, roll_d_hat, pitch_d_hat, roll_d, pitch_d);

		computeTendonsLength(d1, d2, d3, roll_d_hat, -pitch_d_hat);

	    vCmds[0]=-600*(roll_hat-roll_d_hat);
		vCmds[1]=500*(pitch_hat-pitch_d_hat)+400*(roll_hat-roll_d_hat);
		vCmds[2]=-500*(pitch_hat-pitch_d_hat)+400*(roll_hat-roll_d_hat);

		vCmds[0]*=pGain;
        vCmds[1]*=pGain;
        vCmds[2]*=pGain;

        vCmds[0]=-(1-REL_WEIGHT)*(encoders[5]-d3) + REL_WEIGHT*vCmds[0];
        vCmds[1]=-(1-REL_WEIGHT)*(encoders[6]-d1) + REL_WEIGHT*vCmds[1];
        vCmds[2]=-(1-REL_WEIGHT)*(encoders[7]-d2) + REL_WEIGHT*vCmds[2];

        double t1=Time::now();

		mutex.wait();
        ivel->velocityMove(5, vCmds[0]);
        ivel->velocityMove(6, vCmds[1]);
        ivel->velocityMove(7, vCmds[2]);
		mutex.post();
        
        double t2=Time::now();
        
        count++;
        static double dT=0;
        dT+=t2-t1;
        
        if (count%100==0)
            {
                fprintf(stderr, "%.2lf %.2lf %.2lf ", (encoders[5]-d3), (encoders[6]-d2), (encoders[7]-d1));
				//fprintf(stderr, "%.2lf %.2lf %.2lf ", (encoders[5]-d3_tmp), (encoders[6]-d2_tmp), (encoders[7]-d1_tmp));
                //fprintf(stderr, "%.2lf %.2lf %.2lf ", (-d1*3.14/180)+4, (-d2*3.14/180)+4, (-d3*3.14/180)+4);
				//fprintf(stderr, "%.2lf %.2lf %.2lf ", (-d1_tmp*3.14/180)+4, (-d2_tmp*3.14/180)+4, (-d3_tmp*3.14/180)+4);
                fprintf(stderr, "Inertial: %.2lf %.2lf %.2lf %.2lf", roll_hat, pitch_hat, roll_d_hat, pitch_d_hat);
                //fprintf(stderr, "Inertial: %.2lf %.2lf", roll, pitch);
                fprintf(stderr, "dT=%.3lf\n", (dT)/100);
                dT=0;
            }

    }

    void threadRelease()
    {
		fprintf(stderr, "HeadControl::releasing...");
		mutex.wait();
        ivel->velocityMove(5, 0);
        ivel->velocityMove(6, 0);
        ivel->velocityMove(7, 0);

		disableControl();

		mutex.post();

		delete [] encoders;
        encoders=0;

		fprintf(stderr, "done\n");
    }
    
    double roll;
    double pitch;
    double yaw;

    double cmd_roll;
    double cmd_pitch;

    Vector inertiaValue;
    double *encoders;

    double *vCmds;
    double pGain;
    int nAxes;
    Semaphore mutex;
    
};

JamesHead::JamesHead():
    ImplementPositionControl<JamesHead, IPositionControl>(this),
    ImplementVelocityControl<JamesHead, IVelocityControl>(this),
    ImplementEncoders<JamesHead, IEncoders>(this),
    ImplementPidControl<JamesHead, IPidControl>(this),
    ImplementAmplifierControl<JamesHead, IAmplifierControl>(this),
    ImplementControlLimits<JamesHead, IControlLimits>(this),
    controller(0),
    axisMap(0),
    angleToEncoder(0),
    zeros(0)
{
    nj=HEAD_JOINTS;

    axisMap=new int[nj];
    angleToEncoder=new double [nj];
    zeros=new double [nj];

    for(int k=0;k<nj;k++)
        {
            axisMap[k]=k;
            angleToEncoder[k]=1.0;
            zeros[k]=0.0;
        }
}

JamesHead::~JamesHead()
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        delete c;

    c=0;
}

bool JamesHead::open(yarp::os::Searchable& config)
{
    Property headParams;
    Property inertialParams;

    headParams.fromString(config.toString());
    headParams.setMonitor(config.getMonitor(),
                          "headParams"); // pass on any monitoring

    ACE_OS::fprintf(stderr, "Opening JamesHead device\n");

	//////////// head
    Bottle& head = headParams.findGroup("HEAD","section for robot head");
    Value& hdevice = head.find("device");
    Value& hsubdevice = head.find("subdevice");

	headParams.put("device", hdevice);
    headParams.put("subdevice", hsubdevice);

	// create a device for the head 
	ddHead.open(headParams);
    if (!ddHead.isValid()) 
        {  
            ACE_OS::printf("JamesHead: head device either not found or could not open\n");
            return false;
        }
    
    // initialize interfaces

    ImplementPositionControl<JamesHead, IPositionControl>::
        initialize(nj, axisMap, angleToEncoder, zeros);
    ImplementVelocityControl<JamesHead, IVelocityControl>::
        initialize(nj, axisMap, angleToEncoder, zeros);
    ImplementEncoders<JamesHead, IEncoders>::
        initialize(nj, axisMap, angleToEncoder, zeros);
    ImplementPidControl<JamesHead, IPidControl>::
        initialize(nj, axisMap, angleToEncoder, zeros);

	ImplementAmplifierControl<JamesHead, IAmplifierControl>::
        initialize(nj, axisMap, angleToEncoder, zeros);

   	ImplementControlLimits<JamesHead, IControlLimits>::
        initialize(nj, axisMap, angleToEncoder, zeros);

    //////////// inertial
    inertialParams.fromString(config.toString());
    Value &idevice=inertialParams.findGroup("INERTIAL").find("device");
    Value &isubdevice=inertialParams.findGroup("INERTIAL").find("subdevice");

	inertialParams.put("device", idevice);
	inertialParams.put("subdevice", isubdevice);

	// create a device for the arm 
	ddInertia.open(inertialParams);
    if (!ddInertia.isValid()) 
        {  
            ACE_OS::printf("JamesHead: inertia device either not found or could not open");
            return false;
        } 

    // instantiate inertia and head
    controller = new HeadControl(NC_RATE, &ddInertia, &ddHead);
    HeadControl *c=my_cast(controller);

    c->start();
    return true;
}

bool JamesHead::close()
{
	fprintf(stderr, "JamesHead::calling close\n");
    HeadControl *c=my_cast(controller);

    if (c!=0)
        {
            fprintf(stderr, "JamesHead::calling close\n");
            c->stop();
        }

    return true;
}

bool JamesHead::getAxes(int *ax)
{
	fprintf(stderr, "JamesHead::getAxes: %d\n", nj);
    *ax=nj; 
    return true;
}

bool JamesHead::resetEncoderRaw(int j)
{
	HeadControl *c=my_cast(controller);

	return c->resetEncoder(j);
}

bool JamesHead::resetEncodersRaw()
{
    return false;
}
 
bool JamesHead::setEncoderRaw(int j, double val)
{
    return true;
}

bool JamesHead::setEncodersRaw(const double *vals)
{

    return true;
}

bool JamesHead::getEncoderRaw(int j, double *v)
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getEncoder(j, v);

    return true;
}

bool JamesHead::getEncodersRaw(double *encs)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getEncoders(encs);

    return true;
}

bool JamesHead::getEncoderSpeedRaw(int j, double *sp)
{    
    return true;
}

bool JamesHead::getEncoderSpeedsRaw(double *spds)
{    
    return true;
}
    
bool JamesHead::getEncoderAccelerationRaw(int j, double *spds)
{    
    return true;
}

bool JamesHead::getEncoderAccelerationsRaw(double *accs)
{    
    return true;
}

bool JamesHead::setPositionMode()
{    
    return true;
}

bool JamesHead::positionMoveRaw(int j, double ref)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->positionMove(j, ref);

    return true;
}

bool JamesHead::positionMoveRaw(const double *refs)
{   
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->positionMove(refs);

    return true;
}

bool JamesHead::relativeMoveRaw(int j, double delta)
{ 
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->relativeMove(j, delta);

    return true;
}

bool JamesHead::relativeMoveRaw(const double *deltas)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->relativeMove(deltas);

    return true;
}
    
bool JamesHead::checkMotionDoneRaw(int j, bool *flag)
{   
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->checkMotionDone(j, flag);

    return true;
}

bool JamesHead::checkMotionDoneRaw(bool *flag)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->checkMotionDone(flag);

    return true;
}

bool JamesHead::setRefSpeedRaw(int j, double sp)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->setRefSpeed(j, sp);

    return true;
}

bool JamesHead::setRefSpeedsRaw(const double *spds)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->setRefSpeeds(spds);

    return true;
}

bool JamesHead::setRefAccelerationRaw(int j, double acc)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->setRefAcceleration(j, acc);

    return true;
}

bool JamesHead::setRefAccelerationsRaw(const double *accs)
{  
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->setRefAccelerations(accs);

    return true;
}

bool JamesHead::getRefSpeedRaw(int j, double *ref)
{   
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getRefSpeed(j, ref);

    return true;
}

bool JamesHead::getRefSpeedsRaw(double *spds)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getRefSpeeds(spds);

    return true;
}

bool JamesHead::getRefAccelerationRaw(int j, double *acc)
{   
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getRefAcceleration(j, acc);

    return true;
}

bool JamesHead::getRefAccelerationsRaw(double *accs)
{   
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->getRefAccelerations(accs);

    return true;
}

bool JamesHead::stopRaw(int j)
{    
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->halt(j);

    return true;
}

bool JamesHead::stopRaw()
{  
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->stop();

    return true;
}

bool JamesHead::velocityMoveRaw(int j, double v)
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->velocityMove(j, v);

    return true;
}

bool JamesHead::velocityMoveRaw(const double *v)
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        c->velocityMove(v);

    return true;
}

bool JamesHead::disablePidRaw(int j)
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        return c->disablePid(j);
    else
        return false;
}

bool JamesHead::enablePidRaw(int j)
{
    HeadControl *c=my_cast(controller);

    if (c!=0)
        return c->enablePid(j);
    else
        return false;
}

bool JamesHead::enableAmpRaw(int j)
{
    HeadControl *c=my_cast(controller);
    if (c!=0)
        return c->enableAmp(j);
    else
        return false;
}

bool JamesHead::disableAmpRaw(int j)
{
    HeadControl *c=my_cast(controller);
    if (c!=0)
        return c->disableAmp(j);
    else
        return false;
}

#include <math.h>

const double PI=3.14159265;
const double TOLERANCE = 0.0001;

void computeTendonsLength(double &d1, double &d2, double &d3, double Roll, double Pitch)
{

	double L        = 4.0;      // length of the spring
    double L_2      = 2.0;      // length of rigid part of the spring
    double L_cables = 5.2;      // distance between tendons
	double R_capstan= 1.0;		// radius of the capstan
	double Spring_R = 1.5/2;		// radius of the spring
	double s1, s2, s3;
	double s4;
	double t0;

    Roll = Roll * PI / 180;
    Pitch = Pitch * PI / 180;

	if ((fabs(Roll) < TOLERANCE) && (fabs(Pitch) < TOLERANCE))
        {
            d1 = 0.0;
            d2 = 0.0;
            d3 = 0.0;
        }
	else
        {
			s1 = pow(-(-cos(Pitch)*L_cables*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))-2.0*sin(Pitch)*L-2.0*sin(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch))+2.0*sin(Pitch)*cos(Roll)*cos(Pitch)*L+2.0*sin(Pitch)*cos(Roll)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch)))/sqrt(1.0-pow(cos(Roll 
			),2.0)*pow(cos(Pitch),2.0))/acos(cos(Roll)*cos(Pitch))/2.0-L_cables/2.0,2.0);      s3 = pow((3.0*sin(Roll)*sin(Pitch)*L_cables*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))+cos(Roll)*L_cables*sqrt(3.0)*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))-6.0*sin(Roll)*cos(Pitch)*L-6.0*sin(Roll)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch 
			))+6.0*sin(Roll)*pow(cos(Pitch),2.0)*cos(Roll)*L+6.0*sin(Roll)*pow(cos(Pitch 
			),2.0)*cos(Roll)*Spring_R*acos(cos(Roll)*cos(Pitch)))/sqrt(1.0-pow(cos(Roll 
			),2.0)*pow(cos(Pitch),2.0))/acos(cos(Roll)*cos(Pitch))/6.0-L_cables*sqrt(3.0)/6.0,2.0);      s4 = pow(-3.0*cos(Roll)*sin(Pitch)*L_cables*acos(cos(Roll)*cos(Pitch))+sin(Roll)*L_cables*sqrt(3.0)*acos(cos(Roll)*cos(Pitch))+6.0*sqrt(1.0-pow(cos(Roll 
			),2.0)*pow(cos(Pitch),2.0))*L+6.0*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch 
			),2.0))*Spring_R*acos(cos(Roll)*cos(Pitch)),2.0)/pow(acos(cos(Roll)*cos(Pitch 
			)),2.0)/36.0;      s2 = s3+s4;      t0 = s1+s2; 
			
			d1 = sqrt(t0)-L;
			d1 = -d1 / R_capstan * (180.0/PI);

			s1 = pow(-(cos(Pitch)*L_cables*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))-2.0*sin(Pitch)*L-2.0*sin(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch))+2.0*sin(Pitch)*cos(Roll)*cos(Pitch)*L+2.0*sin(Pitch)*cos(Roll)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch)))/sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))/acos(cos(Roll)*cos(Pitch))/2.0+L_cables/2.0,2.0);      s3 = pow((-3.0*sin(Roll)*sin(Pitch)*L_cables*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))+cos(Roll)*L_cables*sqrt(3.0)*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))-6.0*sin(Roll)*cos(Pitch)*L-6.0*sin(Roll)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch 
			))+6.0*sin(Roll)*pow(cos(Pitch),2.0)*cos(Roll)*L+6.0*sin(Roll)*pow(cos(Pitch 
			),2.0)*cos(Roll)*Spring_R*acos(cos(Roll)*cos(Pitch)))/sqrt(1.0-pow(cos(Roll 
			),2.0)*pow(cos(Pitch),2.0))/acos(cos(Roll)*cos(Pitch))/6.0-L_cables*sqrt(3.0)/6.0,2.0);      s4 = pow(3.0*cos(Roll)*sin(Pitch)*L_cables*acos(cos(Roll)*cos(Pitch))+sin(Roll)*L_cables*sqrt(3.0)*acos(cos(Roll)*cos(Pitch))+6.0*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*L+6.0*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*Spring_R*acos(cos(Roll)*cos(Pitch)),2.0)/pow(acos(cos(Roll)*cos(Pitch)),2.0)/36.0;      s2 = s3+s4;      t0 = s1+s2; 

			d2 = sqrt(t0)-L; 
			d2 = -d2 / R_capstan * (180.0/PI);

			s1 = pow(sin(Pitch),2.0)*pow(-L-Spring_R*acos(cos(Roll)*cos(Pitch))+cos(Roll 
			)*cos(Pitch)*L+cos(Roll)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch)),2.0)/(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))/pow(acos(cos(Roll)*cos(Pitch)),2.0);      s2 = pow((-cos(Roll)*L_cables*sqrt(3.0)*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*acos(cos(Roll)*cos(Pitch))-3.0*sin(Roll)*cos(Pitch)*L-3.0*sin(Roll 
			)*cos(Pitch)*Spring_R*acos(cos(Roll)*cos(Pitch))+3.0*sin(Roll)*pow(cos(Pitch 
			),2.0)*cos(Roll)*L+3.0*sin(Roll)*pow(cos(Pitch),2.0)*cos(Roll)*Spring_R*acos(cos(Roll)*cos(Pitch)))/sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))/acos(cos(Roll)*cos(Pitch))/3.0+L_cables*sqrt(3.0)/3.0,2.0)+pow(-sin(Roll)*L_cables 
			*sqrt(3.0)*acos(cos(Roll)*cos(Pitch))+3.0*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*L+3.0*sqrt(1.0-pow(cos(Roll),2.0)*pow(cos(Pitch),2.0))*Spring_R 
			*acos(cos(Roll)*cos(Pitch)),2.0)/pow(acos(cos(Roll)*cos(Pitch)),2.0)/9.0;      t0 = s1+s2; 

			d3 = sqrt(t0)-L;
			d3 = -d3 / R_capstan * (180.0/PI);
        }
}


void computeModifiedPitchRoll(double Yaw, double Roll, double Pitch, double &Roll_hat, double &Pitch_hat)
{

    Roll = Roll * PI / 180;
    Pitch = Pitch * PI / 180;
	Yaw = Yaw * PI / 180;

	double x, x_hat;
	double y, y_hat;
	double z, z_hat;



	x = -sin(Pitch) * cos(Roll);
	y = -sin(Roll);
	z = cos(Pitch) * cos(Roll);

	x_hat = x*cos(Yaw) + y*sin(Yaw);
	y_hat = -x*sin(Yaw) + y*cos(Yaw); 
	z_hat = z;

	Pitch_hat = atan2(-x_hat, z_hat);
	Roll_hat = atan2(-y_hat, z_hat/cos(Pitch_hat));

	Pitch_hat = Pitch_hat *180/PI;
	Roll_hat = Roll_hat *180/PI;

}

void computeOriginalPitchRoll(double Yaw, double &Roll, double &Pitch, double Roll_hat, double Pitch_hat)
{

    Roll_hat = Roll_hat * PI / 180;
    Pitch_hat = Pitch_hat * PI / 180;
	Yaw = Yaw * PI / 180;

	double x, x_hat;
	double y, y_hat;
	double z, z_hat;



	x_hat = -sin(Pitch_hat) * cos(Roll_hat);
	y_hat = -sin(Roll_hat);
	z_hat = cos(Pitch_hat) * cos(Roll_hat);

	x = x_hat*cos(Yaw) + y_hat*sin(Yaw);
	y = -x_hat*sin(Yaw) + y_hat*cos(Yaw); 
	z = z_hat;

	Pitch = atan2(-x, z);
	Roll = atan2(-y, z/cos(Pitch));

	Pitch = Pitch *180/PI;
	Roll = Roll *180/PI;

}
