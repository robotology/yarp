// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/dev/ControlBoardInterfaces.h>
#include <ace/Log_Msg.h>
#include <stdio.h>

using namespace yarp::dev;

template <class T>
inline void checkAndDestroy(T *p)
{
    if (p!=0)
        delete [] p;
}

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
        
        memcpy(axisMap, aMap, sizeof(int)*nj);
        memcpy(zeros, zs, sizeof(double)*nj);
        memcpy(angleToEncoders, angToEncs, sizeof(double)*nj);
        // invert the axis map
   		memset (invAxisMap, 0, sizeof(int) * nj);
		int i;
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


    }

    bool alloc(int n)
    {
        nj=n;
        if (nj<=0)
            return false;

        if (zeros==0)
            dealloc();

        zeros=new double [nj];
        signs=new double [nj];
        axisMap=new int [nj];
        invAxisMap=new int [nj];
        angleToEncoders=new double [nj];

        return true;
    }

    bool dealloc()
    {
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
    inline void toHw(const double *usr, double *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }

    inline void posA2E(double ang, int j, double &enc, int &k)
    {
        enc=(ang*angleToEncoders[j])+zeros[j];
        k=toHw(j);
    }

    inline void posE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=(zeros[k]-enc)/angleToEncoders[k];
    }

    inline void velA2E(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*angleToEncoders[j];
    }

    inline void velE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/angleToEncoders[k];
    }

    inline void accA2E(double ang, int j, double &enc, int &k)
    {
        velA2E(ang, j, enc, k);
    }

    inline void accE2A(double enc, int j, double &ang, int &k)
    {
        velE2A(enc, j, ang, k);
    }

    inline double velE2A(double enc, int j)
    {
        int k=toUser(j);
        return enc/angleToEncoders[k];
    }

    inline double accE2A(double enc, int j)
    {
        return velE2A(enc, j);
    }

    //map a vector, convert from angles to encoders
    inline void posA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    //map a vector, convert from encoders to angles
    inline void posE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void velA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void velE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
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

 	int nj;

	double *zeros;
	double *signs;
	int *axisMap;
	int *invAxisMap;
	double *angleToEncoders;
};

inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }

bool printDebug(const char *string)
{
    fprintf(stderr, "%s\n", string);
    return true;
}

IPositionControl2::IPositionControl2(IPositionControl *pos, int nj,
                                      const int *axisMap,
                                      const double *angToEncs,
                                      const double *zeros):temp(0),
iPosition(0),
helper(0)
{
    //later check for null pointer
    iPosition=pos;

    //later alloc pointer
    temp=0;
    ACE_ASSERT (temp != 0); //alloc temp

    helper=(void *)(new ControlBoardHelper(nj, axisMap, angToEncs, zeros));

    ACE_ASSERT(false); //initializa mapper object
}

IPositionControl2::~IPositionControl2()
{
    if (helper!=0)
    delete castToMapper(helper);
    
    if (temp!=0)
        delete [] temp;
}

bool IPositionControl2::positionMove(int j, double v)
{
    fprintf(stderr, "Calling IPositionControl2::positionMove2\n");
    return iPosition->positionMove(j, v);
}

bool IPositionControl2::setPositionMode()
{
    fprintf(stderr, "Calling IPositionControl2::setPositionMode\n");
    return true;
}

bool IPositionControl2::positionMove(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp);

    return iPosition->positionMove(temp);
}

bool IPositionControl2::relativeMove(int j, double delta)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition->relativeMove(k,enc);
}
    
bool IPositionControl2::relativeMove(const double *deltas)
{
    castToMapper(helper)->velA2E(deltas, temp);

    return iPosition->relativeMove(temp);
}

bool IPositionControl2::checkMotionDone(int j, bool *flag)
{
    int k=castToMapper(helper)->toHw(j);

    return iPosition->checkMotionDone(k,flag);
}

bool IPositionControl2::checkMotionDone(bool *flag)
{
    return iPosition->checkMotionDone(flag);
}

bool IPositionControl2::setRefSpeed(int j, double sp)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iPosition->setRefSpeed(k, enc);
}

bool IPositionControl2::setRefSpeeds(const double *spds)
{
    castToMapper(helper)->velA2E(spds, temp);

    return iPosition->setRefSpeeds(temp);
}

bool IPositionControl2::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;

    castToMapper(helper)->accA2E(acc, j, enc, k);
    return iPosition->setRefAcceleration(k, enc);
}

bool IPositionControl2::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E(accs, temp);

    return iPosition->setRefAccelerations(temp);
}

bool IPositionControl2::getRefSpeed(int j, double *ref)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPosition->getRefSpeed(k, &enc);

    *ref=(castToMapper(helper)->velE2A(enc, k));
    
    return true;
}

bool IPositionControl2::getRefSpeeds(double *spds)
{
    bool ret=iPosition->getRefSpeeds(temp);
    castToMapper(helper)->velE2A(temp, spds);
    return true;
}

bool IPositionControl2::getRefAccelerations(double *accs)
{
    bool ret=iPosition->getRefSpeeds(temp);
    castToMapper(helper)->accE2A(temp, accs);
    return true;
}

bool IPositionControl2::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAcceleration(k, &enc);
    
    *acc=castToMapper(helper)->accE2A(enc, k);
    
    return ret;
}

bool IPositionControl2::stop(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition->stop(k);
}

bool IPositionControl2::stop()
{
    return iPosition->stop();
}
