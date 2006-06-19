#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include <yarp/EsdMotionControl.h>

#include <ace/Log_Msg.h>
#include <stdio.h>
#include <ace/OS.h>

template <class T>
inline void checkAndDestroy(T *p)
{
    if (p!=0)
        delete [] p;
}

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

    inline int axes()
    { return nj; }
        

 	int nj;

	double *zeros;
	double *signs;
	int *axisMap;
	int *invAxisMap;
	double *angleToEncoders;
};

inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }

using namespace yarp::dev;

template <class DERIVED, class IMPLEMENT> 
ImplementPositionControl<DERIVED, IMPLEMENT>::ImplementPositionControl(DERIVED *y)
{
    iPosition = dynamic_cast<IPositionControlRaw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> 
ImplementPositionControl<DERIVED, IMPLEMENT>::~ImplementPositionControl()
{
    if (helper!=0) uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::positionMove(int j, double v)
{
    int k;
    double enc;
    castToMapper(helper)->posA2E(v, j, enc, k);
    return iPosition->positionMoveRaw(k, enc);
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
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeeds(const double *spds)
{
    castToMapper(helper)->velA2E(spds, temp);
    
    return iPosition->setRefSpeedsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;
    
    castToMapper(helper)->accA2E(acc, j, enc, k);
    return iPosition->setRefAccelerationRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E(accs, temp);
    
    return iPosition->setRefAccelerationsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeed(int j, double *ref)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    
    bool ret = iPosition->getRefSpeedRaw(k, &enc);
    
    *ref=(castToMapper(helper)->velE2A(enc, k));
    
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeeds(double *spds)
{
    bool ret=iPosition->getRefSpeedsRaw(temp);
    castToMapper(helper)->velE2A(temp, spds);
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAccelerations(double *accs)
{
    bool ret=iPosition->getRefAccelerationsRaw(temp);
    castToMapper(helper)->accE2A(temp, accs);
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAccelerationRaw(k, &enc);
    
    *acc=castToMapper(helper)->accE2A(enc, k);
    
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
bool ImplementPositionControl<DERIVED, IMPLEMENT>:: initialize (int size, int *amap, double *enc, double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    temp=new double [size];
    
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
        delete castToMapper(helper);
    
    delete [] temp;
    
    helper=0;
    temp=0;
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
    if (helper!=0) uninitialize();
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>:: initialize (int size, int *amap, double *enc)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    temp=new double [size];
    
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
        delete castToMapper(helper);
    
    delete [] temp;
    
    helper=0;
    temp=0;
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
    
    castToMapper(helper)->accA2E(acc, j, enc, k);
    return iVelocity->setRefAccelerationRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E(accs, temp);
    
    return iVelocity->setRefAccelerationsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::getRefAccelerations(double *accs)
{
    bool ret=iVelocity->getRefAccelerationsRaw(temp);
    castToMapper(helper)->accE2A(temp, accs);
    return true;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementVelocityControl<DERIVED, IMPLEMENT>::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefAccelerationRaw(k, &enc);
    
    *acc=castToMapper(helper)->accE2A(enc, k);
    
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
