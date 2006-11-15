// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Alexandre Bernardino, Julio Gomes
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


/// general purpose stuff.
#include <yarp/os/Time.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_msg.h>
#include <ace/Sched_Params.h>

/// specific to this device driver.
#include "JrkerrMotionControl.h"
#include "../dd_orig/include/nmccom.h"
#include "../dd_orig/include/picservo.h"
#include "../dd_orig/include/sio_util.h"


#include "../../../ControlBoardInterfacesImpl.inl"


typedef int (*PV) (const char *fmt, ...);

using namespace yarp::os;
using namespace yarp::dev;

bool NOT_YET_IMPLEMENTED(const char *txt)
{
    ACE_OS::fprintf(stderr, "%s not yet implemented for JrkerrMotionControl\n", txt);

   return false;
}

JrkerrMotionControlParameters::JrkerrMotionControlParameters(int nj)
{
    _axisMap = allocAndCheck<int>(nj);
    _angleToEncoder = allocAndCheck<double>(nj);
    _zeros = allocAndCheck<double>(nj);

    _pids=allocAndCheck<Pid>(nj);
    _limitsMax=allocAndCheck<double>(nj);
    _limitsMin=allocAndCheck<double>(nj);
    _currentLimits=allocAndCheck<double>(nj);
	_velocityLimits=allocAndCheck<double>(nj);
	_accelerationLimits=allocAndCheck<double>(nj);
	_errorLimits=allocAndCheck<double>(nj);
    memset(_limitsMin, 0, sizeof(double)*nj);
    memset(_limitsMax, 0, sizeof(double)*nj);
    memset(_currentLimits, 0, sizeof(double)*nj);
	memset(_velocityLimits, 0, sizeof(double)*nj);
	memset(_accelerationLimits, 0, sizeof(double)*nj);
	memset(_errorLimits, 0, sizeof(double)*nj);


	_servoRate = allocAndCheck<int>(nj);
	_deadBand = allocAndCheck<int>(nj);
	_tickperturn = allocAndCheck<int>(nj);
   	_njoints = nj;

}

JrkerrMotionControlParameters::~JrkerrMotionControlParameters()
{
    checkAndDestroy<double>(_zeros);
    checkAndDestroy<double>(_angleToEncoder);
    checkAndDestroy<int>(_axisMap);
    
    checkAndDestroy<Pid>(_pids);
    checkAndDestroy<double>(_limitsMax);
    checkAndDestroy<double>(_limitsMin);
	checkAndDestroy<double>(_velocityLimits);
	checkAndDestroy<double>(_accelerationLimits);	
	checkAndDestroy<double>(_currentLimits);
	checkAndDestroy<double>(_errorLimits);

	checkAndDestroy<int>(_servoRate);
	checkAndDestroy<int>(_deadBand);
	checkAndDestroy<int>(_tickperturn);
}

///
///
///



class JrkerrRS485Resources
{
private:
    JrkerrRS485Resources (const JrkerrRS485Resources&);
    void operator= (const JrkerrRS485Resources&);

public:
	JrkerrRS485Resources ();
	~JrkerrRS485Resources ();

	bool initialize (const JrkerrMotionControlParameters& parms);
	bool uninitialize ();


	inline int getJoints (void) const { return _njoints; }
	
public:

	char _comPort[6];
	int _baudRate;
	int _njoints;
	int _groupAddr;
    JRKerrSendCmd jrkerrcmd;
};

JrkerrRS485Resources::JrkerrRS485Resources ()
{
	//some defaults
	_baudRate=9600;
	_njoints=0;
	_groupAddr = 0xF0; //240
}

JrkerrRS485Resources::~JrkerrRS485Resources () 
{ 
	uninitialize(); 
}

bool JrkerrRS485Resources::initialize (const JrkerrMotionControlParameters& parms)
{

	ACE_OS::sprintf(_comPort, "COM%d",parms._comPort);
	_baudRate = parms._baudRate;
	_njoints = parms._njoints;
	_groupAddr = parms._groupAddr;
	if(parms._njoints != NmcInit( _comPort, _baudRate, &jrkerrcmd))
	{
		ACE_OS::printf("Invalid configuration\n");
        return false;
    }
	else
		ACE_OS::printf("%d Modules Found at %s\n", _njoints, _comPort);

	//setting the group address for all joints
	for( int i = 0; i < _njoints; i++ )
	{
		if(!NmcSetGroupAddr(i+1, _groupAddr, (i==0? TRUE : FALSE), &jrkerrcmd))
			return false;
	}
    return true;
}


bool JrkerrRS485Resources::uninitialize ()
{
    //Sometimes gives SIO error messages !
	/*
	NmcHardReset(0xFF, &jrkerrcmd);
    NmcShutdown( &jrkerrcmd); 
	*/
	
	return true;
}


/*

/*
 *
 */

inline JrkerrRS485Resources& RES(void *res) { return *(JrkerrRS485Resources *)res; }

JrkerrMotionControl::JrkerrMotionControl() : 
    ImplementPositionControl<JrkerrMotionControl, IPositionControl>(this),
    ImplementVelocityControl<JrkerrMotionControl, IVelocityControl>(this),
    ImplementPidControl<JrkerrMotionControl, IPidControl>(this),
    ImplementEncoders<JrkerrMotionControl, IEncoders>(this),
    ImplementControlCalibration<JrkerrMotionControl, IControlCalibration>(this),
    ImplementAmplifierControl<JrkerrMotionControl, IAmplifierControl>(this),
    ImplementControlLimits<JrkerrMotionControl, IControlLimits>(this),
    _done(0),
    _mutex(1)
{
	system_resources = (void *) new JrkerrRS485Resources;
	ACE_ASSERT (system_resources != NULL);
}


JrkerrMotionControl::~JrkerrMotionControl ()
{
	if (system_resources != NULL)
		delete (JrkerrRS485Resources *)system_resources;
	system_resources = NULL;
}


bool JrkerrMotionControl::open (const JrkerrMotionControlParameters &p)
{
	
	_mutex.wait();

    JrkerrRS485Resources& r = RES (system_resources);

	// used for printing debug messages.
    _p = p._p;
	
	
	/* TO DEBUG */
	if (!r.initialize (p))
	{
		_mutex.post();
		return false;
	}

	ImplementPositionControl<JrkerrMotionControl, IPositionControl>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);
    
    ImplementVelocityControl<JrkerrMotionControl, IVelocityControl>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);
  
    ImplementPidControl<JrkerrMotionControl, IPidControl>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);

    ImplementEncoders<JrkerrMotionControl, IEncoders>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);

    ImplementControlCalibration<JrkerrMotionControl, IControlCalibration>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);

    ImplementAmplifierControl<JrkerrMotionControl, IAmplifierControl>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);

    ImplementControlLimits<JrkerrMotionControl, IControlLimits>::
        initialize(p._njoints, p._axisMap, p._angleToEncoder, p._zeros);

	// temporary variables used by the ddriver.
	_ref_positions = allocAndCheck<double>(r.getJoints());		
	_ref_speeds = allocAndCheck<double>(r.getJoints());
	_ref_accs = allocAndCheck<double>(r.getJoints());
	_kp = allocAndCheck<int>(r.getJoints());
	_kd = allocAndCheck<int>(r.getJoints());
	_ki = allocAndCheck<int>(r.getJoints());
	_il = allocAndCheck<int>(r.getJoints());
	_ol = allocAndCheck<int>(r.getJoints());
	_cl = allocAndCheck<int>(r.getJoints());
	_el = allocAndCheck<int>(r.getJoints());
	_sr = allocAndCheck<int>(r.getJoints());
	_dc = allocAndCheck<int>(r.getJoints());
	_pl = allocAndCheck<int>(r.getJoints());
	_nl = allocAndCheck<int>(r.getJoints());
	_tk = allocAndCheck<int>(r.getJoints());
	_vl = allocAndCheck<int>(r.getJoints());
	_al = allocAndCheck<int>(r.getJoints());
	_en = allocAndCheck<int>(r.getJoints());
	_mode = allocAndCheck<int>(r.getJoints());


	int i;
	for(i = 0; i < r.getJoints(); i++)
	{
		_kp[i] = (int)p._pids[i].kp;
		_kd[i] = (int)p._pids[i].kd;
		_ki[i] = (int)p._pids[i].ki;
		_il[i] = (int)p._pids[i].max_int;
		_ol[i] = (int)p._pids[i].max_output;
		_cl[i] = (int)p._currentLimits[i];
		_el[i] = (int)p._errorLimits[i];
		_sr[i] = (int)p._servoRate[i];
		_dc[i] = (int)p._deadBand[i];
		_pl[i] = (int)p._limitsMax[i]*p._angleToEncoder[i];
		_nl[i] = (int)p._limitsMin[i]*p._angleToEncoder[i];
		_tk[i] = (int)p._tickperturn[i];
		_vl[i] = (int)(p._velocityLimits[i]*_tk[i]/360.0/1961.0);
		_al[i] = (int)(p._accelerationLimits[i]*_tk[i]/360.0/3845521.0);
		_en[i] = 1; //default is enabled


		//initialize microcontroller with defaults
		setPidRaw(i, p._pids[i]);
	}
	
	

	ACE_OS::printf("Initializing %d axes at port  %d\n", r._njoints, p._comPort);
	for(i = 0; i < r.getJoints(); i++)
	{
		ACE_OS::printf("Axis %d:\n",i);
		ACE_OS::printf("\tkp \t%d \tki \t%d \tkd \t%d\n",_kp[i], _ki[i], _kd[i]);
		ACE_OS::printf("\til \t%d \tol \t%d \tcl \t%d\n",_il[i], _ol[i], _cl[i]);
		ACE_OS::printf("\tel \t%d \tsr \t%d \tdc \t%d\n",_el[i], _sr[i], _dc[i]);
		ACE_OS::printf("\tpl \t%d \tnl \t%d \ttk \t%d\n",_pl[i], _nl[i], _tk[i]);
		ACE_OS::printf("\tvl \t%d \tal \t%d \ten \t%d\n",_vl[i], _al[i], _en[i]);
	}

	_mutex.post ();

	return true;
}

bool JrkerrMotionControl::open(yarp::os::Searchable& config) {
    Property p;
    p.fromString(config.toString());

    if (!p.check("GENERAL")) {
        ACE_DEBUG((LM_DEBUG, "Cannot understand configuration parameters\n"));
        return false;
    }

    int i;
    int nj = p.findGroup("GENERAL").find("Joints").asInt();
    JrkerrMotionControlParameters params(nj);
    params._njoints = nj;

	

    ////// GENERAL
    Bottle &xtmp = p.findGroup("GENERAL").findGroup("AxisMap");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._axisMap[i-1] = xtmp.get(i).asInt();
    xtmp = p.findGroup("GENERAL").findGroup("Encoder");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._angleToEncoder[i-1] = xtmp.get(i).asDouble();
    xtmp = p.findGroup("GENERAL").findGroup("Zeros");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._zeros[i-1] = xtmp.get(i).asDouble();
    
    ////// PIDS
    int j=0;
    for(j=0;j<nj;j++)
    {
        char tmp[80];
        sprintf(tmp, "Pid%d", j); 
        xtmp = p.findGroup("PIDS").findGroup(tmp);
        params._pids[j].kp = xtmp.get(1).asDouble();
        params._pids[j].kd = xtmp.get(2).asDouble();
        params._pids[j].ki = xtmp.get(3).asDouble();
    
        params._pids[j].max_int = xtmp.get(4).asDouble();
        params._pids[j].max_output = xtmp.get(5).asDouble();
        
        params._pids[j].scale = xtmp.get(6).asDouble();
        params._pids[j].offset = xtmp.get(7).asDouble();
    }

    /////// LIMITS
    xtmp = p.findGroup("LIMITS").findGroup("Currents");
 	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._currentLimits[i-1]=xtmp.get(i).asDouble();

	xtmp = p.findGroup("LIMITS").findGroup("Velocity");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._velocityLimits[i-1]=xtmp.get(i).asDouble();

	xtmp = p.findGroup("LIMITS").findGroup("Acceleration");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._accelerationLimits[i-1]=xtmp.get(i).asDouble();

	xtmp = p.findGroup("LIMITS").findGroup("Error");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._errorLimits[i-1]=xtmp.get(i).asDouble();


    xtmp = p.findGroup("LIMITS").findGroup("Max");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._limitsMax[i-1]=xtmp.get(i).asDouble();

    xtmp = p.findGroup("LIMITS").findGroup("Min");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._limitsMin[i-1]=xtmp.get(i).asDouble();

//  JRKERR SPECIFIC STUFF

	params._comPort = p.findGroup("JRKERR").find("ComPort").asInt();
    params._baudRate = p.findGroup("JRKERR").find("BaudRate").asInt();
	params._groupAddr = p.findGroup("JRKERR").find("GroupAddress").asInt();
    
	xtmp = p.findGroup("JRKERR").findGroup("ServoRate");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._servoRate[i-1]=xtmp.get(i).asInt();

	xtmp = p.findGroup("JRKERR").findGroup("TickPerTurn");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._tickperturn[i-1]=xtmp.get(i).asInt();

	xtmp = p.findGroup("JRKERR").findGroup("DeadBand");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) params._deadBand[i-1]=xtmp.get(i).asInt();

    params._p = ACE_OS::printf;
    return open(params);
}


bool JrkerrMotionControl::close (void)
{
	JrkerrRS485Resources& d = RES(system_resources);

    ImplementPositionControl<JrkerrMotionControl, IPositionControl>::uninitialize ();
    ImplementVelocityControl<JrkerrMotionControl, IVelocityControl>::uninitialize();
    ImplementPidControl<JrkerrMotionControl, IPidControl>::uninitialize();
    ImplementEncoders<JrkerrMotionControl, IEncoders>::uninitialize();
    ImplementControlCalibration<JrkerrMotionControl, IControlCalibration>::uninitialize();
    ImplementAmplifierControl<JrkerrMotionControl, IAmplifierControl>::uninitialize();
    ImplementControlLimits<JrkerrMotionControl, IControlLimits>::uninitialize();

    checkAndDestroy<double> (_ref_positions);
    checkAndDestroy<double> (_ref_speeds);
    checkAndDestroy<double> (_ref_accs);
	checkAndDestroy<int> (_kp);
	checkAndDestroy<int> (_kd);
	checkAndDestroy<int> (_ki);
	checkAndDestroy<int> (_il);
	checkAndDestroy<int> (_ol);
	checkAndDestroy<int> (_cl);
	checkAndDestroy<int> (_el);
	checkAndDestroy<int> (_sr);
	checkAndDestroy<int> (_dc);
	checkAndDestroy<int> (_pl);
	checkAndDestroy<int> (_nl);
	checkAndDestroy<int> (_tk);
	checkAndDestroy<int> (_vl);
	checkAndDestroy<int> (_al);
	checkAndDestroy<int> (_en);
	checkAndDestroy<int> (_mode);

	int ret = d.uninitialize ();

	return ret;
}


///
/// Not used
///
void JrkerrMotionControl::run ()
{
	JrkerrRS485Resources& r = RES (system_resources);
}

// return the number of controlled axes.
bool JrkerrMotionControl::getAxes(int *ax)
{
	JrkerrRS485Resources& r = RES(system_resources);
    *ax = r.getJoints();
    return true;
}

// LATER: can be optimized.
bool JrkerrMotionControl::setPidRaw (int axis, const Pid &pid)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_kp[axis] = pid.kp;
    _kd[axis] = pid.kd;
    _ki[axis] = pid.ki;
    _il[axis] = pid.max_int;
    _ol[axis] = pid.max_output;   
    // not used pid.scale; 
    // not used pid.offset;
	ServoSetGain(axis+1, _kp[axis], _ki[axis], _kd[axis], 
		_il[axis], _ol[axis], _cl[axis], _el[axis], 
		_sr[axis], _dc[axis], &(r.jrkerrcmd));
	return true;
}

bool JrkerrMotionControl::setOffsetRaw(int j, double v)
{
	// not used pid.offset;
	return true;
}

bool JrkerrMotionControl::getPidRaw (int axis, Pid *out)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	out[axis].kp = _kp[axis];
    out[axis].kd = _kd[axis];
    out[axis].ki = _ki[axis];
    out[axis].max_int = _il[axis];
    out[axis].max_output = _ol[axis];
    out[axis].scale = 1; // not used 
    out[axis].offset = 0; // not used
	return true;
}

bool JrkerrMotionControl::getPidsRaw (Pid *out)
{
    JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for (int i = 0; i < r.getJoints(); i++) {
		getPidRaw(i, &out[i] );
    }
	_mutex.post();
	return true;
}


bool JrkerrMotionControl::setPidsRaw(const Pid *pids)
{
	JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for (int i = 0; i < r.getJoints(); i++) {
		setPidRaw(i, pids[i] );
    }
	_mutex.post();
	return true;
}

/// cmd is a SingleAxis poitner with 1 double arg
bool JrkerrMotionControl::setReferenceRaw (int axis, double ref)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	_ref_positions[axis] = ref;
	_mutex.post();

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool JrkerrMotionControl::setReferencesRaw (const double *refs)
{
	JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		_ref_positions[i] = refs[i];
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::setErrorLimitRaw(int axis, double limit)
{
    JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	_el[axis] = limit;
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::setErrorLimitsRaw(const double *limit)
{
    JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		_el[i] = limit[i];
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::getErrorRaw(int axis, double *err)
{
	return NOT_YET_IMPLEMENTED("getErrorRaw");
}

bool JrkerrMotionControl::getErrorsRaw(double *errs)
{
	return NOT_YET_IMPLEMENTED("getErrorsRaw");
}

bool JrkerrMotionControl::getOutputRaw(int axis, double *out)
{
	return NOT_YET_IMPLEMENTED("getOutputRaw");
}

bool JrkerrMotionControl::getOutputsRaw(double *outs)
{
	return NOT_YET_IMPLEMENTED("getOuputsRaw");
}

bool JrkerrMotionControl::getReferenceRaw(int axis, double *ref)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	*ref = _ref_positions[axis];
	_mutex.post();

    return true;
}

bool JrkerrMotionControl::getReferencesRaw(double *ref)
{
	JrkerrRS485Resources& r = RES(system_resources);
	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++)
		ref[i] = _ref_positions[i];
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::getErrorLimitRaw(int axis, double *err)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	*err = _el[axis];
	_mutex.post();

    return true;
}

bool JrkerrMotionControl::getErrorLimitsRaw(double *errs)
{
	JrkerrRS485Resources& r = RES(system_resources);
	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++)
		errs[i] = _el[i];
	_mutex.post();

    return true;
}

bool JrkerrMotionControl::resetPidRaw(int j)
{
    return NOT_YET_IMPLEMENTED("resetPidRaw");
}

bool JrkerrMotionControl::enablePidRaw(int axis)
{
	//By default it is initialized
	return NOT_YET_IMPLEMENTED("enablePidRaw");
}

bool JrkerrMotionControl::disablePidRaw(int axis)
{
	//How to disable ?
	return NOT_YET_IMPLEMENTED("disablePidRaw");
}

bool JrkerrMotionControl::setPositionMode()
{
	//should allow per axis setting
    return NOT_YET_IMPLEMENTED("setPositionMode");
}

bool JrkerrMotionControl::setVelocityMode()
{
	//should allow per axis setting
    return NOT_YET_IMPLEMENTED("setVelocityMode");
}

bool JrkerrMotionControl::positionMoveRaw(int axis, double ref)
{
	
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	int res=0;
	unsigned char mode;
	double par;

	int overlimit=0;

  
    if ( ref > _pl[axis] )
    {
       par = _pl[axis];
       overlimit = 1;
	}
    else if ( ref < _nl[axis] )
    {	
      par = _nl[axis];
      overlimit = 1;
    }
	else
		par = ref;

	if (!ENABLED (axis))
	{
		// still fills the _ref_position structure.
		_ref_positions[axis] = par;
		return true;
	}


	mode = LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW;   //trapezoidal mode

	_mutex.wait();

	ServoLoadTraj(axis+1, mode, (long)par, _ref_speeds[axis], _ref_accs[axis], 0, &(r.jrkerrcmd) );

	_mutex.post();

	
    if(overlimit)
      printf("WARNING!!!!! Exceeding joint limits %f\n", axis, ref);
 
	return true;
}

bool JrkerrMotionControl::positionMoveRaw(const double *refs)
{
	JrkerrRS485Resources& r = RES(system_resources);
	for(int i = 0; i < r.getJoints(); i++)
	{
		if(!positionMoveRaw(i, refs[i]))
			return false;
		
	}
	return true;
}

bool JrkerrMotionControl::relativeMoveRaw(int j, double delta)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

bool JrkerrMotionControl::relativeMoveRaw(const double *deltas)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

/// check motion done, single axis.
bool JrkerrMotionControl::checkMotionDoneRaw(int axis, bool *ret)
{
    JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	unsigned char statbyte;

	_mutex.wait();
	do
	{
       NmcNoOp(axis+1, &(r.jrkerrcmd));	//poll controller to get current status data
       statbyte = NmcGetStat(axis+1, &(r.jrkerrcmd));
	}
    while ( !(statbyte & MOVE_DONE) );  //wait for MOVE_DONE bit to go HIGH
	_mutex.post();


	return true;
}

/// check motion done, multiple axis.
/// cmd is a pointer to a bool
bool JrkerrMotionControl::checkMotionDoneRaw (bool *ret)
{
	return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");
}

bool JrkerrMotionControl::setRefSpeedRaw(int axis, double sp)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	_ref_speeds[axis] = sp;
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::setRefSpeedsRaw(const double *spds)
{
	JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		_ref_speeds[i] = spds[i];
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::setRefAccelerationRaw(int axis, double acc)
{
    JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	_ref_accs[axis] = acc;
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::setRefAccelerationsRaw(const double *accs)
{
	JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		_ref_accs[i] = accs[i];
	_mutex.post();

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool JrkerrMotionControl::getRefSpeedsRaw (double *spds)
{
    JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		spds[i] = _ref_speeds[i];
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::getRefSpeedRaw (int axis, double *spd)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	*spd = _ref_speeds[axis];
	_mutex.post();

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool JrkerrMotionControl::getRefAccelerationsRaw (double *accs)
{
	JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	for(int i = 0; i < r.getJoints(); i++ )
		accs[i] = _ref_accs[i];
	_mutex.post();

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool JrkerrMotionControl::getRefAccelerationRaw (int axis, double *accs)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	*accs = _ref_accs[axis];
	_mutex.post();

	return true;
}

bool JrkerrMotionControl::stopRaw(int axis)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	ServoStopMotor( axis+1, AMP_ENABLE | STOP_ABRUPT, &(r.jrkerrcmd));
	_mutex.post();
    return true;
}

bool JrkerrMotionControl::stopRaw()
{
    JrkerrRS485Resources& r = RES(system_resources);

	_mutex.wait();
	/*for(int i = 0; i < r.getJoints(); i++ )
		ServoStopMotor( i+1, AMP_ENABLE | STOP_ABRUPT, &(r.jrkerrcmd));*/
	ServoStopMotor( r._groupAddr, AMP_ENABLE | STOP_ABRUPT, &(r.jrkerrcmd));
	_mutex.post();
    return true;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool JrkerrMotionControl::velocityMoveRaw (int axis, double sp)
{
	int res=0;
	unsigned char mode;
	float acc,pos;
	double par;

	int overlimit=0;

	if ( sp > _vl[axis] )
	{
      par = _vl[axis];
      overlimit = 1;
	}
	else if ( sp < -_vl[axis] )
	{
      par = -_vl[axis];
      overlimit = 1;
	}
	else
		par = sp;

    if (par > 0)
    {
		pos = _pl[axis];
    }
    else
	{
		pos = _nl[axis];
    }


	if (!ENABLED (axis))
	{
		_ref_speeds[axis] = par;
		return true;
	}

    if(overlimit)
        printf("WARNING!!!!! Motor %d with excessive velocity %lf\n", axis, sp);
   

	mode = LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW ;	//trapezoidal mode
    if( par < 0) 
        par = -par;

	// values in ticks/cycle
    par = par / 1961.0;
    acc = _ref_accs[axis]/3845521.0; 
	//values to the board
	long lvel = (long)(par*65536);
	long lacc = (long)(acc*65536);
    
    //printf("vai andar a %f %d\n", par, (long)(par*65536));

	JrkerrRS485Resources& r = RES(system_resources);
	_mutex.wait();
	
    ServoLoadTraj(axis+1, mode, (long)pos, lvel, lacc, 0, &(r.jrkerrcmd) );
	_mutex.post();
	return true;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool JrkerrMotionControl::velocityMoveRaw (const double *sp)
{
	JrkerrRS485Resources& r = RES(system_resources);
	for(int i = 0; i < r.getJoints(); i++)
	{
		if(!velocityMoveRaw(i, sp[i]))
			return false;
		
	}
	return true;
}

bool JrkerrMotionControl::setEncoderRaw(int j, double val)
{
	return NOT_YET_IMPLEMENTED("setEncoderRaw");
}

bool JrkerrMotionControl::setEncodersRaw(const double *vals)
{
	return NOT_YET_IMPLEMENTED("setEncodersRaw");
}

bool JrkerrMotionControl::resetEncoderRaw(int axis)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	_mutex.wait();
	ServoResetPos( axis+1, &(r.jrkerrcmd));
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::resetEncodersRaw()
{
	JrkerrRS485Resources& r = RES(system_resources);
	
	_mutex.wait();
	/*for(int i=0; i < r.getJoints(); i++){
		ServoResetPos( i+1, &(r.jrkerrcmd));
	}*/
	ServoResetPos( r._groupAddr, &(r.jrkerrcmd));
    _mutex.post();
	return true;
}

bool JrkerrMotionControl::getEncodersRaw(double *v)
{
	JrkerrRS485Resources& r = RES(system_resources);
	
	unsigned char stat_items = SEND_POS;
	_mutex.wait();
	/*for(int i=0; i < r.getJoints(); i++){
		NmcReadStatus(i+1, stat_items, &(r.jrkerrcmd));
		v[i] = (double)ServoGetPos(i+1, &(r.jrkerrcmd));
	}*/
	//NmcDefineStatus(r._groupAddr, SEND_POS,  &(r.jrkerrcmd) );
	//NmcNoOp(r._groupAddr,  &(r.jrkerrcmd) );
	for(int i=0; i < r.getJoints(); i++){
		NmcDefineStatus(i+1, SEND_POS,  &(r.jrkerrcmd) );
		NmcNoOp(i+1,  &(r.jrkerrcmd) );
		v[i] = (double)ServoGetPos(i+1, &(r.jrkerrcmd));
	};
	/*NmcNoOp(r._groupAddr,  &(r.jrkerrcmd) );
	for(i=0; i < r.getJoints(); i++){
		v[i] = (double)ServoGetPos(i+1, &(r.jrkerrcmd));
	};*/
    _mutex.post();
	return true;
}

bool JrkerrMotionControl::getEncoderRaw(int axis, double *v)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	unsigned char stat_items = SEND_POS;
	_mutex.wait();
    NmcReadStatus(axis+1, stat_items, &(r.jrkerrcmd));
    *v = (double)ServoGetPos(axis+1, &(r.jrkerrcmd));
    _mutex.post();
	return true;
}

bool JrkerrMotionControl::getEncoderSpeedsRaw(double *v)
{
  	JrkerrRS485Resources& r = RES(system_resources);

	unsigned char stat_items = SEND_VEL;
	_mutex.wait();
	/*for(int i=0; i < r.getJoints(); i++){
		NmcReadStatus(i+1, stat_items, &(r.jrkerrcmd));
		v[i] = (double)ServoGetVel(i+1, &(r.jrkerrcmd));
	}*/
	NmcDefineStatus(r._groupAddr, SEND_VEL,  &(r.jrkerrcmd) );
	NmcNoOp(r._groupAddr,  &(r.jrkerrcmd) );
	for(int i=0; i < r.getJoints(); i++){
		v[i] = (double)ServoGetVel(i+1, &(r.jrkerrcmd));
	}
    _mutex.post();
    _mutex.post();
	return true;
}

bool JrkerrMotionControl::getEncoderSpeedRaw(int axis, double *v)
{
    JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;

	unsigned char stat_items = SEND_VEL;
	_mutex.wait();
    NmcReadStatus(axis+1, stat_items, &(r.jrkerrcmd));
    *v = (double)ServoGetVel(axis+1, &(r.jrkerrcmd));
    _mutex.post();
	return true;
}

bool JrkerrMotionControl::getEncoderAccelerationsRaw(double *v)
{
  	return NOT_YET_IMPLEMENTED("getEncoderAccs");
}

bool JrkerrMotionControl::getEncoderAccelerationRaw(int j, double *v)
{
    return NOT_YET_IMPLEMENTED("getEncoderAcc");
}

bool JrkerrMotionControl::disableAmpRaw(int axis)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	_mutex.wait();
	ServoStopMotor( axis+1, AMP_ENABLE | MOTOR_OFF, &(r.jrkerrcmd));
	_en[axis] = false;
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::enableAmpRaw(int axis)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	_mutex.wait();
	ServoStopMotor( axis+1, AMP_ENABLE | MOTOR_OFF, &(r.jrkerrcmd));
	_en[axis] = true;
	_mutex.post();
	return true;
}

// bcast
bool JrkerrMotionControl::getCurrentsRaw(double *cs)
{
	return NOT_YET_IMPLEMENTED("getCurrentsRaw");
}

// bcast currents
bool JrkerrMotionControl::getCurrentRaw(int axis, double *c)
{
	return NOT_YET_IMPLEMENTED("getCurrentRaw");
}

bool JrkerrMotionControl::setMaxCurrentRaw(int axis, double v)
{
    JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	_mutex.wait();
	_cl[axis] = v;
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::calibrateRaw(int axis, double p)
{
    return NOT_YET_IMPLEMENTED("calibrateRaw");
}

bool JrkerrMotionControl::doneRaw(int axis)
{
    return NOT_YET_IMPLEMENTED("doneRaw");
}

bool JrkerrMotionControl::setPrintFunction(int (*f) (const char *fmt, ...))
{
	_mutex.wait();
	JrkerrRS485Resources& r = RES(system_resources);
	_p = f;
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::getAmpStatusRaw(int *st)
{
	return NOT_YET_IMPLEMENTED("getAmpStatusRaw");
}

bool JrkerrMotionControl::setLimitsRaw(int axis, double min, double max)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	_mutex.wait();
	_nl[axis] = min;
	_pl[axis] = max;
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::getLimitsRaw(int axis, double *min, double *max)
{
	JrkerrRS485Resources& r = RES(system_resources);
	if(!(axis >= 0 && axis < r.getJoints())) 
		return false;
	_mutex.wait();
	*min = _nl[axis];
	*max = _pl[axis];
	_mutex.post();
	return true;
}

bool JrkerrMotionControl::loadBootMemory()
{
	return NOT_YET_IMPLEMENTED("loadBootMemory");
}

bool JrkerrMotionControl::saveBootMemory ()
{
	return NOT_YET_IMPLEMENTED("saveBootMemory");
}

inline bool JrkerrMotionControl::ENABLED (int axis)
{
	return _en[axis];
}

