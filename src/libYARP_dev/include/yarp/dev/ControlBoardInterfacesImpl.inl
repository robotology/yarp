// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2006 Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// removed references to ACE

#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include <math.h>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid multiple definitions.

//#include <ace/Log_Msg.h>
//#include <stdio.h>
//#include <ace/OS_NS_stdlib.h>
//#include <ace/OS_NS_stdio.h>

#include <string.h> // for memset
#include <stdlib.h> // for exit
#include <stdio.h> // for printf
#define _YARP_ASSERT(x) { if (!(x)) { printf("memory allocation failure\n"); exit(1); } }

/*
 * simple helper template to alloc memory.
 */
template <class T>
inline T* allocAndCheck(int size)
{
    T* t = new T[size];
    _YARP_ASSERT (t != 0);
    memset(t, 0, sizeof(T) * size);
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
        _YARP_ASSERT(zeros != 0 && signs != 0 && axisMap != 0 && invAxisMap != 0 && angleToEncoders != 0);

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

    inline void posA2E(double ang, int j, double &enc, int &k)
    {
        enc=(ang+zeros[j])*angleToEncoders[j];
        k=toHw(j);
    }

    inline double posA2E(double ang, int j)
    {
        return (ang+zeros[j])*angleToEncoders[j];
    }

    inline void posE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);

        ang=(enc/angleToEncoders[k])-zeros[k];
    }

    inline double posE2A(double enc, int j)
    {
        int k=toUser(j);
        
        return (enc/angleToEncoders[k])-zeros[k];
    }

    inline void velA2E(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*angleToEncoders[j];
    }

	inline void velA2E_abs(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*fabs(angleToEncoders[j]);
    }

    inline void velE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/angleToEncoders[k];
    }

	inline void velE2A_abs(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/fabs(angleToEncoders[k]);
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

    inline double velE2A(double enc, int j)
    {
        int k=toUser(j);
        return enc/angleToEncoders[k];
    }

	inline double velE2A_abs(double enc, int j)
    {
        int k=toUser(j);
        return enc/fabs(angleToEncoders[k]);
    }


    inline double accE2A(double enc, int j)
    {
        return velE2A(enc, j);
    }

	inline double accE2A_abs(double enc, int j)
    {
        return velE2A_abs(enc, j);
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

	inline void velA2E_abs(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velA2E_abs(ang[j], j, tmp, index);
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

	inline void velE2A_abs(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velE2A_abs(enc[j], j, tmp, index);
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
    uninitialize();
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
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeeds(const double *spds)
{
    castToMapper(helper)->velA2E_abs(spds, temp);
    
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
    
    *ref=(castToMapper(helper)->velE2A_abs(enc, k));
    
    return ret;
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeeds(double *spds)
{
    bool ret = iPosition->getRefSpeedsRaw(temp);
    castToMapper(helper)->velE2A_abs(temp, spds);
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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);

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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);

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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
    tmpPids=new Pid[size];
    _YARP_ASSERT (tmpPids != 0);
    
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
    castToMapper(helper)->posA2E(ref, j, enc, k);

    return iPid->setReferenceRaw(k, enc);
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
    int k;
    double enc;
    castToMapper(helper)->posA2E(limit, j, enc, k);
    
    return iPid->setErrorLimitRaw(k, enc);
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
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPid->getPidRaw(k, pid);
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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
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

    return iEncoders->resetEncoderRaw(k);
}


template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::resetEncoders()
{
    return iEncoders->resetEncodersRaw();
}

template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::setEncoder(int j, double val)
{
    int k;
    double enc;

    castToMapper(helper)->posA2E(val, j, enc, k);

    return iEncoders->setEncoderRaw(k, enc);
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
    
    return ret;
}
    
template <class DERIVED, class IMPLEMENT>
bool ImplementEncoders<DERIVED, IMPLEMENT>::getEncoders(double *v)
{
    bool ret;
    castToMapper(helper)->axes();
    
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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
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
bool ImplementControlCalibration<DERIVED, IMPLEMENT>::calibrate(int j, double p)
{
    int k=castToMapper(helper)->toHw(j);

    return iCalibrate->calibrateRaw(k, p);
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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
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
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
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
    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(helper)->posA2E(min, j, minEnc, k);
    castToMapper(helper)->posA2E(max, j, maxEnc, k);

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
    double minEnc=0;
    double maxEnc=0;

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
    _YARP_ASSERT (helper != 0);
    dTemp=new double[size];
    _YARP_ASSERT (dTemp != 0);
    iTemp=new int[size];
    _YARP_ASSERT (iTemp != 0);
    
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

template <class DERIVED, class IMPLEMENT> 
bool ImplementAmplifierControl<DERIVED, IMPLEMENT>::getAmpStatus(int k, int *st)
{
    int j=castToMapper(helper)->toHw(k);
    bool ret=iAmplifier->getAmpStatusRaw(j, st);

    return ret;
}

/////////////////////////////
