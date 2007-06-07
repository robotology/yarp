// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: MEIMotionControl.cpp,v 1.1 2007-06-07 10:05:41 babybot Exp $
///
///

/// general purpose stuff.
#include <yarp/os/Time.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <ace/Sched_Params.h>


/// specific to this device driver.
#include "yarp/MEIMotionControl.h"


#include "../../../ControlBoardInterfacesImpl.inl"

// MEI
#define MEI_WINNT
#define PCDSP_LIB
//#include "../dd_orig/include/pcdsp.h"
	
#include "../dd_orig/include/idsp.h"

char __filename[256] = "arm.ini";





#define BUF_SIZE 2047


typedef int (*PV) (const char *fmt, ...);


using namespace yarp::os;
using namespace yarp::dev;


bool NOT_IMPLEMENTED_YET(const char *txt)
{
    ACE_OS::fprintf(stderr, "%s not yet implemented for MEIMotionControl\n", txt);

    return false;
}

/**
 * Max number of addressable cards in this implementation.
 */

MEIMotionControlParameters::MEIMotionControlParameters(int nj)
{
		_highPIDs = NULL;
		_lowPIDs = NULL;
		_zeros = NULL;
		_signs = NULL;
		_axis_map = NULL;
		_inv_axis_map = NULL;
		_encoderToAngles = NULL;
		_fwdCouple = NULL;
		_invCouple = NULL;
		_stiffPID = NULL;
		_limitsMax = NULL;
		_limitsMin = NULL;
		_maxDAC = NULL;
		_njoints = 0;
		_pids=NULL;
//		meiPortAddr = 0x300;   /*0x0300 ?*/
		ioPorts = 0x00;
		
		_njoints= _BabybotArm::_nj;

		_realloc(_njoints);
		int i;
		for(i = 0; i<_njoints; i++) 
		{
			
			_highPIDs[i] = _BabybotArm::_highPIDs[i];
			_lowPIDs[i] = _BabybotArm::_lowPIDs[i];
			_zeros[i] = _BabybotArm::_zeros[i];
			_axis_map[i] = _BabybotArm::_axis_map[i];
			_signs[i] = _BabybotArm::_signs[i];
			_encoderToAngles[i] = _BabybotArm::_encoders[i]*_BabybotArm::_encWheels[i];
			_fwdCouple[i] = _BabybotArm::_fwdCouple[i];
			_stiffPID[i] = _BabybotArm::_stiffPID[i];
			_maxDAC[i] = _BabybotArm::_maxDAC[i];
//			_limitsMax[i] = _BabybotArm::_limitsMax[i];
//			_limitsMin[i] = _BabybotArm::_limitsMin[i];


		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) +
						(_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);

		// invert the axis map.
		ACE_OS::memset (_inv_axis_map, 0, sizeof(int) * _njoints);
		for (i = 0; i < _njoints; i++)
		{
			int j;
			for (j = 0; j < _njoints; j++)
			{
				if (_axis_map[j] == i)
				{
					_inv_axis_map[i] = j;
					break;
				}
			}
		}
//	}


}

MEIMotionControlParameters::~MEIMotionControlParameters()
{
 		if (_encoderToAngles != NULL)
			delete [] _encoderToAngles;
		if (_highPIDs != NULL)
			delete [] _highPIDs;
		if (_lowPIDs != NULL)
			delete [] _lowPIDs;
		if (_zeros != NULL)
			delete [] _zeros;
		if (_signs != NULL)
			delete [] _signs;
		if (_axis_map != NULL)
			delete [] _axis_map;
		if (_invCouple != NULL)
			delete [] _invCouple;
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
		if (_limitsMax != NULL)
			delete [] _limitsMax;
		if (_limitsMin != NULL)
			delete [] _limitsMin;
		if (_inv_axis_map != NULL)
			delete [] _inv_axis_map;
		if (_fwdCouple != NULL)
			delete [] _fwdCouple;
		if (_pids != NULL) delete [] _pids;

}


	/**
	 * Copies an existing parameter instance into this one.
	 * @param peer is the reference to the object to copy in.
	 * @return true always.
	 */
int MEIMotionControlParameters::copy (const MEIMotionControlParameters& peer)
	{
		_njoints = peer._njoints;

		if (_njoints != 0)
		{
			_realloc (_njoints);

			memcpy (_highPIDs, peer._highPIDs, sizeof(LowLevelPID) * _njoints);
			memcpy (_lowPIDs, peer._lowPIDs, sizeof(LowLevelPID) * _njoints);
			memcpy (_zeros, peer._zeros, sizeof(double) * _njoints);
			memcpy (_signs, peer._signs, sizeof(double) * _njoints);
			memcpy (_axis_map, peer._axis_map, sizeof(int) * _njoints);
			memcpy (_inv_axis_map, peer._inv_axis_map, sizeof(int) * _njoints);
			memcpy (_encoderToAngles, peer._encoderToAngles, sizeof(double) * _njoints);
			memcpy (_fwdCouple, peer._fwdCouple, sizeof(double) * _njoints);
			memcpy (_invCouple, peer._invCouple, sizeof(double) * _njoints);
			memcpy (_stiffPID, peer._stiffPID, sizeof(int) * _njoints);
			memcpy (_maxDAC, peer._maxDAC, sizeof(double) * _njoints);
			memcpy (_limitsMax, peer._limitsMax, sizeof(double) * _njoints);
			memcpy (_limitsMin, peer._limitsMin, sizeof(double) * _njoints);
			memcpy (_pids, peer._pids, sizeof(Pid) * _njoints);
		}
		else
		{
			if (_highPIDs != NULL) delete [] _highPIDs;
			if (_lowPIDs != NULL) delete [] _lowPIDs;
			if (_zeros != NULL)	delete [] _zeros;
			if (_signs != NULL)	delete [] _signs;
			if (_axis_map != NULL) delete [] _axis_map;
			if (_inv_axis_map != NULL) delete [] _inv_axis_map;
			if (_encoderToAngles != NULL) delete [] _encoderToAngles;
			if (_fwdCouple != NULL) delete [] _fwdCouple;
			if (_invCouple != NULL)	delete [] _invCouple;
			if (_stiffPID != NULL) delete [] _stiffPID;
			if (_maxDAC != NULL) delete [] _maxDAC;
			if (_limitsMax != NULL) delete [] _limitsMax;
			if (_limitsMin != NULL) delete [] _limitsMin;
			if (_pids != NULL) delete [] _pids;

			_highPIDs = NULL;
			_lowPIDs = NULL;
			_zeros = NULL;
			_signs = NULL;
			_axis_map = NULL;
			_inv_axis_map = NULL;
			_encoderToAngles = NULL;
			_fwdCouple = NULL;
			_invCouple = NULL;
			_stiffPID = NULL;
			_maxDAC = NULL;
			_limitsMax = NULL;
			_limitsMin = NULL;
			_pids = NULL;

		}

		return true;
	}


	/**
	 * Frees memory and reallocates arrays of the new size.
	 * @param nj is the new size (number of joints).
	 */
void MEIMotionControlParameters::_realloc(int nj)
	{
		if (_highPIDs != NULL)
			delete [] _highPIDs;
		if (_lowPIDs != NULL)
			delete [] _lowPIDs;
		if (_pids != NULL)
			delete [] _pids;
		if (_zeros != NULL)
			delete [] _zeros;
		if (_signs != NULL)
			delete [] _signs;
		if (_axis_map != NULL)
			delete [] _axis_map;
		if (_inv_axis_map != NULL)
			delete [] _inv_axis_map;
		if (_encoderToAngles != NULL)
			delete [] _encoderToAngles;
		if (_fwdCouple != NULL)
			delete [] _fwdCouple;
		if (_invCouple != NULL)
			delete [] _invCouple;
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
		if (_limitsMax != NULL) delete [] _limitsMax;
		if (_limitsMin != NULL) delete [] _limitsMin;
		
		_highPIDs = new LowLevelPID [nj];
		_lowPIDs = new LowLevelPID [nj];
		_zeros = new double [nj];
		_signs = new double [nj];
		_axis_map = new int [nj];
		_inv_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
		_fwdCouple = new double [nj];
		_invCouple = new double [nj];
		_stiffPID = new int [nj];
		_maxDAC = new double [nj];
		_limitsMax= new double [nj];
		_limitsMin= new double [nj];
		_pids = new Pid[nj];
	}



class MEIResources
{
private:
    MEIResources (const MEIResources&);
    void operator= (const MEIResources&);

public:
	MEIResources ();
	~MEIResources ();

	bool initialize (const MEIMotionControlParameters& parms);
	bool uninitialize ();
	bool read ();

	inline int getJoints (void) const { return _Rnjoints; }
	
public:
	enum { MEI_TIMEOUT = 20, MEI_POLLING_INTERVAL = 10 };

	enum {
	/*	Digital Filter Coefficients */
	COEFFICIENTS        =		10, /* elements expected by (get|set)_filter */
	DF_P  				=		0, /* proportional term */
	DF_I	   			=		1, /* integration term */
	DF_D		   		=		2, /* derivative--damping term */
	DF_ACCEL_FF         =		3, /* acceleration feed forward */
	DF_VEL_FF           =	    4, /* velocity feed forward */
	DF_I_LIMIT          =	    5, /* Integration summing limit */
	DF_OFFSET           =	    6, /* to zero DAC output */
	DF_DAC_LIMIT		=	    7, /* 0..32767->0-10v; max DAC output possible.*/
	DF_SHIFT            =	    8, /* 2^(-n) divisor */
	DF_FRICT_FF			=	    9,  /* Friction Feedforward */
    AUX_FILTER_COEFFS   =       10,
//	PCDSP_BASE			=		0x0300
	MEIPORT				=		0x0300
};


		double *_RlimitsMax;
		double *_RlimitsMin;
		double *_Rzeros;
		double *_Rsigns;
		int *_Raxis_map;
		int *_Rinv_axis_map;
		double *_RencoderToAngles;
		LowLevelPID *_RhighPIDs;
		LowLevelPID *_RlowPIDs;
		double *_RfwdCouple;
		double *_RinvCouple;
		int *_RstiffPID;
		double *_RmaxDAC;
		int _Rnjoints;
		short *_Revents;
		
		



	int _Rtimeout;								/// this is my thread timeout.
	int _Rpolling_interval;						/// thread polling interval.
	int _Rspeed;									/// speed of the bus.
	int _RnetworkN;								/// network number.
	
		//for calibration
	double RposHome[6];


	unsigned char _Rmy_address;					/// 
};

MEIResources::MEIResources ()
{

	_Rtimeout = MEI_TIMEOUT;
	_Rpolling_interval = MEI_POLLING_INTERVAL;
	_Rspeed = 0;						/// default 1Mbit/s
	_RnetworkN = 0;
	_Rmy_address = 0;
	_Rnjoints = 0;
	_RlimitsMax = NULL;
	_RlimitsMin = NULL;
	_Rzeros = 0;
	_Rsigns = 0;
	_Raxis_map = 0;
	_Rinv_axis_map = 0;
	_RencoderToAngles = 0;
	_RhighPIDs = 0;
	_RlowPIDs = 0;
	_RfwdCouple = 0;
	_RinvCouple = 0;
	_RstiffPID = 0;
	_RmaxDAC = 0;
	_Rnjoints = 0;

	RposHome[0] = 0.0;
 	RposHome[1] = 0.0;
	RposHome[2] = 0.0;
	RposHome[3] = 0.0;
	RposHome[4] = 0.0;
	RposHome[5] = 0.0;

}


MEIResources::~MEIResources () 
{ 
	uninitialize(); 
}

bool MEIResources::initialize (const MEIMotionControlParameters& parms)
{
		_Rnjoints = parms._njoints;
		
		_RlimitsMax = new double[_Rnjoints];
		_RlimitsMin = new double[_Rnjoints];
		_Rzeros = new double[_Rnjoints];
		_Rsigns = new double[_Rnjoints];
		_Raxis_map = new int[_Rnjoints];
		_Rinv_axis_map = new int[_Rnjoints];
		_RencoderToAngles = new double[_Rnjoints];
		_RhighPIDs  = new LowLevelPID[_Rnjoints];
		_RlowPIDs = new LowLevelPID[_Rnjoints];
		_RfwdCouple = new double[_Rnjoints];
		_RinvCouple = new double[_Rnjoints];
		_RstiffPID = new int[_Rnjoints];
		_RmaxDAC = new double[_Rnjoints];

	for (int i = 0; i<parms._njoints; i++)
	{
		
		_Rzeros[i] = parms._zeros[i];
		_Rsigns[i] = parms._signs[i];
		_Raxis_map[i] = parms._axis_map[i];
		_Rinv_axis_map[i] = parms._inv_axis_map[i];
		_RencoderToAngles[i] = parms._encoderToAngles[i];
		_RhighPIDs[i] = parms._highPIDs[i];
		_RlowPIDs[i] = parms._lowPIDs[i];
		_RfwdCouple[i] = parms._fwdCouple[i];
		_RinvCouple[i] = parms._invCouple[i];
		_RstiffPID[i] = parms._stiffPID[i];
		_RmaxDAC[i] = parms._maxDAC[i];

		_RlimitsMax[i] = parms._limitsMax[i];
		_RlimitsMin[i] = parms._limitsMin[i];
	}


		_Revents = new short[4];
	
		return true;
}


bool MEIResources::uninitialize ()
{
	
	delete []	_Revents;
	delete []	_RlimitsMax ;
	delete []	_RlimitsMin ;
	delete []	_Rzeros;
	delete []	_Rsigns;
	delete []	_Raxis_map;
	delete []	_Rinv_axis_map;
	delete []	_RencoderToAngles;
	delete []	_RhighPIDs;
	delete []	_RlowPIDs ;
	delete []	_RfwdCouple;
	delete []	_RinvCouple;
	delete []	_RstiffPID;
	delete []	_RmaxDAC;

	return true;
}



/*
 *
 */

inline MEIResources& RES(void *res) { return *(MEIResources *)res; }

MEIMotionControl::MEIMotionControl() : 
    ImplementPositionControl<MEIMotionControl, IPositionControl>(this),
	ImplementVelocityControl<MEIMotionControl, IVelocityControl>(this),
    ImplementPidControl<MEIMotionControl, IPidControl>(this),
    ImplementEncoders<MEIMotionControl, IEncoders>(this),
    ImplementControlCalibration<MEIMotionControl, IControlCalibration>(this),
    ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>(this),
    ImplementControlLimits<MEIMotionControl, IControlLimits>(this)//,
//    _done(0),
//    _mutex(1)
{
	system_resources = (void *) new MEIResources;
	ACE_ASSERT (system_resources != NULL);
    _opened			= false;
	_alreadyinint	= false;
	_amplifiers		= false;
	_softwareLimits = false;
	_positionmode   = false;
	_firsttime		= true;
}


MEIMotionControl::~MEIMotionControl ()
{
	if (system_resources != NULL)
		delete (MEIResources *)system_resources;
	system_resources = NULL;
}



bool MEIMotionControl::open (const MEIMotionControlParameters &p)
{

//	_mutex.wait();

    MEIResources& r = RES (system_resources);

	if (!r.initialize (p))
        {
//           _mutex.post();
			printf("\n I AM NOT ABLE TO INITIALIZE THE RESOURCES!!!\n");
            return false;
        }
		
// temporary variables used by the ddriver.
	_ref_positions = allocAndCheck<double>(p._njoints);
	_angleToEncoder = allocAndCheck<double>(p._njoints);
	_ref_speeds = allocAndCheck<double>(p._njoints);
	_ref_accs = allocAndCheck<double>(p._njoints);

    ImplementPositionControl<MEIMotionControl, IPositionControl>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);
    
    ImplementVelocityControl<MEIMotionControl, IVelocityControl>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);

    ImplementPidControl<MEIMotionControl, IPidControl>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);

    ImplementEncoders<MEIMotionControl, IEncoders>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);

    ImplementControlCalibration<MEIMotionControl, IControlCalibration>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);

    ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);

    ImplementControlLimits<MEIMotionControl, IControlLimits>::
        initialize(p._njoints, p._axis_map, _angleToEncoder, p._zeros);


	// default initialization for this device driver.
	setPids(p._pids);

	// temporarily removed
	int16 rc = 0;

	if (_alreadyinint == false)
	{
		int error;
		char buffer[MAX_ERROR_LEN];
		error = dsp_init(r.MEIPORT);	
		rc = dsp_reset();				// reset
		if (error)
			{	
				error_msg(error, buffer);
				printf("Value: %d Message: %s", error, buffer);
				return false;
			}
		_alreadyinint = true;

	}

	int i;

	_all_axes = new int16[p._njoints];
	for(i = 0; i < p._njoints; i++)
	_all_axes[i] = i;

	_filter_coeffs = new int16 * [p._njoints];
	for(i = 0; i < p._njoints; i++)
	_filter_coeffs[i] = new int16 [r.COEFFICIENTS];

	_dsp_rate = dsp_sample_rate();
	
	_winding = new int16[p._njoints];
	ACE_ASSERT (_winding != NULL);
	memset (_winding, 0, sizeof(int16) * p._njoints);

	_16bit_oldpos = new double[p._njoints];
	ACE_ASSERT (_16bit_oldpos != NULL);
	memset (_16bit_oldpos, 0, sizeof(double) * p._njoints);

	_position_zero = new double[p._njoints];
	ACE_ASSERT (_position_zero != NULL);
	memset (_position_zero, 0, sizeof(double) * p._njoints);


	//set the limits for the motion of each axis
	for(i = 0; i < p._njoints; i++)
	{
	setLimitsRaw(i,p._limitsMin[i],p._limitsMax[i]);
	}

	//set the input port and the output port
	//set_io is needed for anable the amplifiers
	//check 

	int set0 = init_io(0, IO_INPUT);
	int set1 = init_io(1, IO_OUTPUT);
	int set3 = set_io(1,0);

	printf("\n NOW it is possible to switch the ampli ON!!!\n");
	ACE_OS::sleep(ACE_Time_Value(5,0));

	_amplifiers = true;
	_opened = true;
//	_mutex.post ();

	return true;
}

bool MEIMotionControl::open(yarp::os::Searchable& config) {

	Searchable& p = config;
    MEIResources& r = RES (system_resources);

	if (_alreadyinint == false)
	{
		int error;
		char buffer[MAX_ERROR_LEN];
		error = dsp_init(r.MEIPORT);	
		int rc = dsp_reset();				// reset
		if (error)
			{	
				error_msg(error, buffer);
				printf("Value: %d Message: %s", error, buffer);
				return false;
			}
		_alreadyinint = true;
	}



	printf("\n");

    int i;
    int nj = p.find("Joints").asInt();
	MEIMotionControlParameters params(nj);
    params._njoints = nj;
	printf("params._njoints = %d",params._njoints);
	printf("\n");
	

	_filter_coeffs = new int16 * [nj];
	for(i = 0; i < nj; i++)
	_filter_coeffs[i] = new int16 [r.COEFFICIENTS];


    ////// GENERAL
    Bottle& xtmp = p.findGroup("MaxDAC");
	ACE_ASSERT (xtmp.size() == nj+1);
	printf("MaxDAC = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._maxDAC[i-1] = xtmp.get(i).asDouble();
		printf("%.1lf ",params._maxDAC[i-1]);
	}
	printf("\n");

    xtmp = p.findGroup("AxisMap");
	ACE_ASSERT (xtmp.size() == nj+1);
	printf("_axis_map = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._axis_map[i-1] = xtmp.get(i).asInt();
		printf("%d ",params._axis_map[i-1]);
	}
	printf("\n");

	xtmp = p.findGroup("FwdCouple");
	ACE_ASSERT (xtmp.size() == nj+1);
//	printf("_fwdCouple = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._fwdCouple[i-1] = xtmp.get(i).asDouble();
//		printf("%.1lf ",params._fwdCouple[i-1]);
	}
	printf("\n");

    xtmp = p.findGroup("Zeros");
	ACE_ASSERT (xtmp.size() == nj+1);
//	printf("_zeros = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._zeros[i-1] = xtmp.get(i).asDouble();
//		printf("%.1lf ",params._zeros[i-1]);
	}
	printf("\n");
		
	xtmp = p.findGroup("Signs");
	ACE_ASSERT (xtmp.size() == nj+1);
  //  printf("_signs = ");
	for (i = 1; i < xtmp.size(); i++)
	{
		params._signs[i-1] = xtmp.get(i).asDouble();
//		printf("%.1lf ",params._signs[i-1]);
	}
	printf("\n");

	xtmp = p.findGroup("Stiff");
	ACE_ASSERT (xtmp.size() == nj+1);
//	printf("_stiffPID = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._stiffPID[i-1] = xtmp.get(i).asInt();
//		printf("%d ",params._stiffPID[i-1]);
	}
	printf("\n");


    /////// LIMITS
    xtmp = p.findGroup("Max");
 	ACE_ASSERT (xtmp.size() == nj+1);
	printf("_limitsMax = ");
    for(i=1;i<xtmp.size(); i++) 
	{
		params._limitsMax[i-1]=xtmp.get(i).asDouble();
		printf("%.1lf ",params._limitsMax[i-1]);
	}
	printf("\n");

	xtmp = p.findGroup("Min");
	ACE_ASSERT (xtmp.size() == nj+1);
	printf("_limitsMin = ");
    for(i=1;i<xtmp.size(); i++)
	{
		params._limitsMin[i-1]=xtmp.get(i).asDouble();
		printf("%.1lf",params._limitsMin[i-1]);
	}
	printf("\n");
	printf("\n");


    ////// PIDS
    int j=0;
    for(j=0;j<nj;j++)
        {
            char tmp[80];
            sprintf(tmp, "LPid%d", j); 
            xtmp = p.findGroup(tmp);
/*			_filter_coeffs[j][DF_P] = params._pids[j].kp = params._lowPIDs[j].KP = xtmp.get(1).asDouble();
			_filter_coeffs[j][DF_D] = params._pids[j].kd = params._lowPIDs[j].KD = xtmp.get(2).asDouble();
			_filter_coeffs[j][DF_I] = params._pids[j].ki = params._lowPIDs[j].KI = xtmp.get(3).asDouble();
			_filter_coeffs[j][DF_ACCEL_FF]  = xtmp.get(4).asDouble();
			_filter_coeffs[j][DF_VEL_FF]	= xtmp.get(5).asDouble();
			_filter_coeffs[j][DF_I_LIMIT]	= params._pids[j].max_int = xtmp.get(6).asDouble();
			_filter_coeffs[j][DF_OFFSET]	= params._pids[j].offset = xtmp.get(7).asDouble();
			_filter_coeffs[j][DF_DAC_LIMIT] = params._pids[j].offset =xtmp.get(8).asDouble();
			_filter_coeffs[j][DF_SHIFT]		= xtmp.get(9).asDouble();
			_filter_coeffs[j][DF_FRICT_FF] = xtmp.get(10).asDouble();
*/
			_filter_coeffs[j][r.DF_P]			= params._pids[j].kp = params._lowPIDs[j].KP = xtmp.get(1).asDouble();
			_filter_coeffs[j][r.DF_D]			= params._pids[j].kd = params._lowPIDs[j].KD = xtmp.get(2).asDouble();
			_filter_coeffs[j][r.DF_I]			= params._pids[j].ki = params._lowPIDs[j].KI = xtmp.get(3).asDouble();
			_filter_coeffs[j][r.DF_ACCEL_FF]	= xtmp.get(4).asDouble();
			_filter_coeffs[j][r.DF_VEL_FF]		= xtmp.get(5).asDouble();
			_filter_coeffs[j][r.DF_I_LIMIT]		= params._pids[j].max_int = xtmp.get(6).asDouble();
			_filter_coeffs[j][r.DF_OFFSET]		= params._pids[j].offset = xtmp.get(7).asDouble();
			_filter_coeffs[j][r.DF_DAC_LIMIT]	= params._pids[j].offset =xtmp.get(8).asDouble();
			_filter_coeffs[j][r.DF_SHIFT]		= xtmp.get(9).asDouble();
			_filter_coeffs[j][r.DF_FRICT_FF]	= xtmp.get(10).asDouble();

			set_filter(j, _filter_coeffs[j] );
//			int ret1 = get_filter(j, _filter_coeffs[j] );
		
//			get_filter(j, _filter_coeffs[j] );

            params._pids[j].scale	= 1;


			

//            params._lowPIDs[j].KP = xtmp.get(1).asDouble();
//            params._lowPIDs[j].KD = xtmp.get(2).asDouble();
//            params._lowPIDs[j].KI = xtmp.get(3).asDouble();
			params._lowPIDs[j].AC_FF = xtmp.get(4).asDouble();
			params._lowPIDs[j].VEL_FF = xtmp.get(5).asDouble();
	        params._lowPIDs[j].I_LIMIT = xtmp.get(6).asDouble();
            params._lowPIDs[j].OFFSET = xtmp.get(7).asDouble();
            params._lowPIDs[j].T_LIMIT = xtmp.get(8).asDouble();
			params._lowPIDs[j].SHIFT = xtmp.get(9).asDouble();
			params._lowPIDs[j].FRICT_FF = xtmp.get(10).asDouble();


			printf("\n");
			//print
		printf("LPID %d = %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf",
			j,
			params._lowPIDs[j].KP,
            params._lowPIDs[j].KD,
            params._lowPIDs[j].KI,
			params._lowPIDs[j].AC_FF,
			params._lowPIDs[j].VEL_FF,
	        params._lowPIDs[j].I_LIMIT,
            params._lowPIDs[j].OFFSET,
            params._lowPIDs[j].T_LIMIT,
			params._lowPIDs[j].SHIFT,
			params._lowPIDs[j].FRICT_FF);
			
			printf("\n");

			//endprint
/*			char tmp0[80];
			sprintf(tmp0, "HPid%d", j);
            Bottle& xtmp0 = p.findGroup(tmp0);
            params._highPIDs[j].KP = xtmp0.get(1).asDouble();
            params._highPIDs[j].KD = xtmp0.get(2).asDouble();
            params._highPIDs[j].KI = xtmp0.get(3).asDouble();
			params._highPIDs[j].AC_FF = xtmp0.get(4).asDouble();
			params._highPIDs[j].VEL_FF = xtmp0.get(5).asDouble();
	        params._highPIDs[j].I_LIMIT = xtmp0.get(6).asDouble();
            params._highPIDs[j].OFFSET = xtmp0.get(7).asDouble();
            params._highPIDs[j].T_LIMIT = xtmp0.get(8).asDouble();
			params._highPIDs[j].SHIFT = xtmp0.get(9).asDouble();
			params._highPIDs[j].FRICT_FF = xtmp0.get(10).asDouble();
			
			//print
			printf("HPID %d = %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf",
			j,
			params._highPIDs[j].KP,
            params._highPIDs[j].KD,
            params._highPIDs[j].KI,
			params._highPIDs[j].AC_FF,
			params._highPIDs[j].VEL_FF,
	        params._highPIDs[j].I_LIMIT,
            params._highPIDs[j].OFFSET,
            params._highPIDs[j].T_LIMIT,
			params._highPIDs[j].SHIFT,
			params._highPIDs[j].FRICT_FF);
			
			
			printf("\n");
*/



			/*
			_filter_coeffs[j][r.DF_P] ,
			_filter_coeffs[j][r.DF_I] ,
			_filter_coeffs[j][r.DF_D] ,
			_filter_coeffs[j][r.DF_ACCEL_FF] ,
			_filter_coeffs[j][r.DF_VEL_FF] ,
			_filter_coeffs[j][r.DF_I_LIMIT] ,
			_filter_coeffs[j][r.DF_OFFSET] ,
			_filter_coeffs[j][r.DF_DAC_LIMIT] ,
			_filter_coeffs[j][r.DF_SHIFT] ,
			_filter_coeffs[j][r.DF_FRICT_FF] ;
*/
				// these are stored to be used later in the setOffsets/setOffset functions
				//remember that these values have to be setted manually if you are going not to use 
				//the arm of Babybot.

	
			printf("\n");
			//Pid *pids = new Pid[6];

			printf("PID manually inserted %d = %d %d %d %d %d %d %d %d %d %d",
			j,
			_filter_coeffs[j][r.DF_P] ,
			 _filter_coeffs[j][r.DF_I] ,
			 _filter_coeffs[j][r.DF_D] ,
			 _filter_coeffs[j][r.DF_ACCEL_FF] ,
			 _filter_coeffs[j][r.DF_VEL_FF] ,
			 _filter_coeffs[j][r.DF_I_LIMIT] ,
			 _filter_coeffs[j][r.DF_OFFSET] ,
			 _filter_coeffs[j][r.DF_DAC_LIMIT] ,
			 _filter_coeffs[j][r.DF_SHIFT] ,
			 _filter_coeffs[j][r.DF_FRICT_FF] );
			
			printf("\n");

			
	}


		// build encoder to angles
	double *encoders = new double [params._njoints];
	double *encWheels = new double [params._njoints];

    xtmp = p.findGroup("EncWheel");
	ACE_ASSERT (xtmp.size() == nj+1);
//	printf("encWheels = ");
    for (i = 1; i < xtmp.size(); i++) 
	{
		encWheels[i-1] = xtmp.get(i).asDouble();
//		printf("%.1lf ",encWheels[i-1]);
	}
//	printf("\n");

    xtmp = p.findGroup("Encoder");						
	ACE_ASSERT (xtmp.size() == nj+1);
	
    for (i = 1; i < xtmp.size(); i++) 
	{
		encoders[i-1] = xtmp.get(i).asDouble();
//		printf("encoders = ");
//		printf("%.1lf ",encoders[i-1]);
		params._encoderToAngles[i-1] = encoders[i-1]*encWheels[i-1];
//		printf("_encoderToAngles = ");
//		printf("%.1lf ",params._encoderToAngles[i-1]);
//		printf("\n");
	}
//	printf("\n");

//	for (i = 1; i < xtmp.size(); i++) params._encoderToAngles[i-1] = encoders[i-1]*encWheels[i-1];
		
	params._fwdCouple[3] = params._fwdCouple[3]*encWheels[3];
	params._fwdCouple[4] = params._fwdCouple[4]*encWheels[4];
	params._fwdCouple[5] = params._fwdCouple[5]*encWheels[5];
/*
		//for debugging
		printf("_fwdCouple 3 = ");
		printf("%.1lf ",params._fwdCouple[3]);
		printf("\n");
		printf("_fwdCouple 4 = ");
		printf("%.1lf ",params._fwdCouple[4]);
		printf("\n");
		printf("_fwdCouple 5 = ");
		printf("%.1lf ",params._fwdCouple[5]);
		printf("\n");
*/
	delete [] encoders;
	delete [] encWheels;
    		///////////////////////////////////////////////////


		// build _invCouple
		for (i = 0; i < 3; i++)
			params._invCouple[i] = 0.0;	// first 3 joints are not coupled

		params._invCouple[3] = -params._fwdCouple[3] / (params._encoderToAngles[3] * params._encoderToAngles[4]);
		params._invCouple[4] = -params._fwdCouple[4] / (params._encoderToAngles[3] * params._encoderToAngles[5]) +
						(params._fwdCouple[3] * params._fwdCouple[5]) / (params._encoderToAngles[3] * params._encoderToAngles[4] * params._encoderToAngles[5]);
		params._invCouple[5] = -params._fwdCouple[5] / (params._encoderToAngles[4] * params._encoderToAngles[5]);
/*
		//for debugging
		printf("_invCouple 3 = ");
		printf("%.1lf ",params._invCouple[3]);
		printf("\n");
		printf("_invCouple 4 = ");
		printf("%.1lf ",params._invCouple[4]);
		printf("\n");
		printf("_invCouple 5 = ");
		printf("%.1lf ",params._invCouple[5]);
		printf("\n");*/

return open(params);
}


bool MEIMotionControl::close (void)
{
	MEIResources& r = RES(system_resources);
    ImplementPidControl<MEIMotionControl, IPidControl>::uninitialize();
    ImplementEncoders<MEIMotionControl, IEncoders>::uninitialize();
    ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>::uninitialize();
    ImplementControlLimits<MEIMotionControl, IControlLimits>::uninitialize();

    checkAndDestroy<double> (_ref_positions);
    checkAndDestroy<double> (_angleToEncoder);
    checkAndDestroy<double> (_ref_speeds);
    checkAndDestroy<double> (_ref_accs);
//	checkAndDestroy<double> (_pids);

    _opened = false;
	_njoints = 0;

	bool ret = r.uninitialize ();
	if (_winding != NULL) delete[] _winding;
	if (_16bit_oldpos != NULL) delete[] _16bit_oldpos;
	return ret;
}


///
///
///
void MEIMotionControl::run ()
{
/*	MEIResources& r = RES (system_resources);

	/// init part.
	bool messagePending = false;
	int i = 0;
	int remainingMsgs = 0;
	bool noreply = false;
	double now = 0;
	double before = 0;
	int counter = 0;

	_writerequested = false;
	_noreply = false;
	
	r._error_status = true;

	/// ok, init completed. 
	_done.post ();

	while (!isStopping() || messagePending)
        {
            before = Time::now();

            //_mutex.wait ();
            if (r.read () != true)
                if (r._p) 
                    (*r._p) ("CAN: read failed\n");

            // handle broadcast messages.
            // (class 1, 8 bits of the ID used to define the message type and source address).
            //
            for (i = 0; i < r._readMessages; i++)
                {
                    CMSG& m = r._readBuffer[i];
                    if (m.len & NTCAN_NO_DATA)
                        if (r._p)
                            {
                                (*r._p) ("CAN: error in message %x len: %d type: %x: %x\n",
                                         m.id, m.len, m.data[0], m.msg_lost);
                                continue;
                            }

                    if ((m.id & 0x700) == 0x100) // class = 1.
                        {
                            // 4 next bits = source address, next 4 bits = msg type
                            // this allows sending two 32-bit numbers is a single CAN message.
                            //
                            // need an array here for storing the messages on a per-joint basis.

                            const int addr = ((m.id & 0x0f0) >> 4);
                            int j;
                            for (j = 0; j < MEI_MAX_CARDS; j++)
                                {
                                    if (r._destinations[j] == addr)
                                        break;
                                }

                            j *= 2;
*/
                            /* less sign nibble specifies msg type */
/*                            switch (m.id & 0x00f)
                                {
                                case CAN_BCAST_POSITION:
                                    r._bcastRecvBuffer[j]._position = *((int *)(m.data));
                                    r._bcastRecvBuffer[j]._update_p = before;
                                    j++;
                                    if (j < r.getJoints())
                                        {
                                            r._bcastRecvBuffer[j]._position = *((int *)(m.data+4));
                                            r._bcastRecvBuffer[j]._update_p = before;
                                        }
                                    break;

                                case CAN_BCAST_PID_VAL:
                                    r._bcastRecvBuffer[j]._pid_value = *((short *)(m.data));
                                    r._bcastRecvBuffer[j]._update_v = before;

                                    j++;
                                    if (j < r.getJoints())
                                        {
                                            r._bcastRecvBuffer[j]._pid_value = *((short *)(m.data+2));
                                            r._bcastRecvBuffer[j]._update_v = before;
                                        }
                                    break;

                                case CAN_BCAST_FAULT:
                                    // fault signals.
                                    r._bcastRecvBuffer[j]._fault = *((short *)(m.data));
                                    r._bcastRecvBuffer[j]._update_e = before;
                                    j++;

                                    if (j < r.getJoints())
                                        {
                                            r._bcastRecvBuffer[j]._fault = *((short *)(m.data+2));
                                            r._bcastRecvBuffer[j]._update_e = before;
                                        }
                                    break;

                                case CAN_BCAST_CURRENT:
                                    // also receives the control values.
                                    r._bcastRecvBuffer[j]._current = *((short *)(m.data));

                                    r._bcastRecvBuffer[j]._position_error = *((short *)(m.data+4));
                                    r._bcastRecvBuffer[j]._update_c = before;
                                    j++;
                                    if (j < r.getJoints())
                                        {
                                            r._bcastRecvBuffer[j]._current = *((short *)(m.data+2));

                                            r._bcastRecvBuffer[j]._position_error = *((short *)(m.data+6));
                                            r._bcastRecvBuffer[j]._update_c = before;
                                        }
                                    break;

                                default:
                                    break;
                                }
                        }
                }

            //
            // handle class 0 messages - polling messages.
            // (class 0, 8 bits of the ID used to represent the source and destination).
            // the first byte of the message is the message type and motor number (0 or 1).
            //
            if (messagePending)
                {
                    for (i = 0; i < r._readMessages; i++)
                        {
                            CMSG& m = r._readBuffer[i];
                            if (m.len & NTCAN_NO_DATA)
                                if (r._p) 
                                    {
                                        (*r._p) ("CAN: error in message %x len: %d type: %x: %x\n", 
                                                 m.id, m.len, m.data[0], m.msg_lost);
						
                                        continue;
                                    }

                            if (((m.id &0x700) == 0) && 
                                ((m.data[0] & 0x7f) != _filter) &&
                                (m.data[0] & 0x7f) < NUM_OF_MESSAGES)
                                r.printMessage (m);

                            if (!noreply) /// this requires a reply.
                                {
                                    if (((m.id & 0x700) == 0) &&				/// class 0 msg.
                                        ((m.id & 0x0f) == r._my_address))
                                        {
                                            /// legitimate message directed here, checks whether replies to any message.
                                            int j;
                                            for (j = 0; j < r._writeMessages; j++)
                                                {
                                                    if (((r._writeBuffer[j].id & 0x0f) == ((m.id & 0xf0) >> 4)) &&
                                                        (m.data[0] == r._writeBuffer[j].data[0]))
                                                        {
                                                            if (r._replyBuffer[j].id != 0)
                                                                {
                                                                    if (r._p)
                                                                        {
                                                                            (*r._p) ("CAN: message %x was already replied\n", m.id);
                                                                            r.printMessage (m);
                                                                        }
                                                                }
                                                            else
                                                                {
                                                                    ACE_OS::memcpy (&r._replyBuffer[j], &m, sizeof(CMSG));
                                                                    remainingMsgs --;
                                                                    if (remainingMsgs < 1)
                                                                        {
                                                                            messagePending = false;
                                                                            r._error_status = true;
                                                                            goto AckMessageLoop;
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }

                    /// the pending msg doesn't require a reply.
                    if (noreply)
                        {
                            remainingMsgs = 0;
                            messagePending = false;
                            r._error_status = true;
                            goto AckMessageLoop;
                        }

                    /// timeout
                    counter ++;
                    if (counter > r._timeout)
                        {	
                            /// complains.
                            if (r._p)
                                {
                                    (*r._p) ("CAN: timeout - still %d messages unacknowledged\n", remainingMsgs);
                                    r.dumpBuffers ();
                                }

                            messagePending = false;
                            r._error_status = false;
                            goto AckMessageLoop;
                        }

                AckMessageLoop:
                    if (!messagePending)
                        {
                            /// tell the caller it can continue.
                            _done.post();
                        }
                }
            else
                {
                    /// any write?
                    if (_writerequested)
                        {
                            if (r._writeMessages > 0)
                                {
                                    if (r.writePacket () != true)
                                        {
                                            if (r._p)
                                                {
                                                    (*r._p) ("CAN: write message of %d elments failed\n", r._writeMessages);
                                                }
                                        }
                                    else
                                        {
                                            messagePending = true;
                                            _writerequested = false;
                                            remainingMsgs = r._writeMessages;
                                            noreply = _noreply;
                                            r._error_status = true;
                                            counter = 0;
                                            ACE_OS::memset (r._replyBuffer, 0, sizeof(CMSG) * r._writeMessages);

                                            if (r._p)
                                                {
                                                    int j;
                                                    for (j = 0; j < r._writeMessages; j++)
                                                        {
                                                            if ((r._writeBuffer[j].data[0] & 0x7f) != _filter)
                                                                r.printMessage (r._writeBuffer[j]);
                                                        }
                                                }
                                        }
                                }
                        }
                }

            //_mutex.post ();

            /// wait.
            now = Time::now();
            if ((now - before)*1000 < r._polling_interval)
                {

                    double k = double(r._polling_interval)/1000.0-(now-before);
                    Time::delay(k);

                    //before = now + k;
                }
            else 
                {
                    if (r._p) (*r._p)("CAN: thread can't poll fast enough (time: %f)\n", now-before);
                    //before = now;

                }
        }
*/
}

// return the number of controlled axes.
bool MEIMotionControl::getAxes(int *ax)				
{
	MEIResources& r = RES(system_resources);
    *ax = r.getJoints();

    return true;
}

// LATER: can be optimized.
bool MEIMotionControl::setPidRaw (int axis, const Pid &pid)
{
		MEIResources& r = RES(system_resources);
	    
			_filter_coeffs[axis][r.DF_P]			= int (pid.kp) ; 
			_filter_coeffs[axis][r.DF_D]			= int (pid.kd) ;
			_filter_coeffs[axis][r.DF_I]			= int (pid.ki) ;
			_filter_coeffs[axis][r.DF_I_LIMIT]		= int (pid.max_int) ;
			_filter_coeffs[axis][r.DF_OFFSET]		= int (pid.offset);
			_filter_coeffs[axis][r.DF_DAC_LIMIT]	= int (pid.max_output);
			
			set_filter(axis, _filter_coeffs[axis]);

	return true;
}

bool MEIMotionControl::getPidRaw (int axis, Pid *pids)
{
		MEIResources& r = RES(system_resources);
		ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

		get_filter(axis, _filter_coeffs[axis]);
		pids[axis].kp = _filter_coeffs[axis][r.DF_P];
		pids[axis].ki = _filter_coeffs[axis][r.DF_I];
		pids[axis].kd = _filter_coeffs[axis][r.DF_D];
		pids[axis].max_int = _filter_coeffs[axis][r.DF_I_LIMIT];
		pids[axis].offset = _filter_coeffs[axis][r.DF_OFFSET];
		pids[axis].max_output = _filter_coeffs[axis][r.DF_DAC_LIMIT];
/*	
			//for debugging
			printf("\n");
			printf("getPidRaw axis:%d = %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf",
			axis,
			pids[axis].kp ,
			pids[axis].ki ,
			pids[axis].kd ,
			pids[axis].max_int	,
			pids[axis].offset,
			pids[axis].max_output)	;
*/
	return true;
}

bool MEIMotionControl::getPidsRaw (Pid *pids)
{
    MEIResources& r = RES(system_resources);


	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
			
		get_filter(i, _filter_coeffs[i]);

		pids[i].kp			= double(_filter_coeffs[i][r.DF_P] );
		pids[i].ki			= double(_filter_coeffs[i][r.DF_I]);
		pids[i].kd			= double(_filter_coeffs[i][r.DF_D]);
		pids[i].max_int		= double(_filter_coeffs[i][r.DF_I_LIMIT]);
		pids[i].offset		= double(_filter_coeffs[i][r.DF_OFFSET]);
		pids[i].max_output	= double(_filter_coeffs[i][r.DF_DAC_LIMIT]);
/*
			//for debugging
			printf("\n");
			printf("getPidsRaw i:%d = %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf ",
			i,
			pids[i].kp ,
			pids[i].ki ,
			pids[i].kd ,
			pids[i].max_int	,
			pids[i].offset,
			pids[i].max_output)	;
			
			printf("\n");
*/
        }

	return true;
}


bool MEIMotionControl::setPidsRaw(const Pid *pids)
{
    MEIResources& r = RES(system_resources);

	int i;
    for (i = 0; i < r.getJoints(); i++) {
				
			_filter_coeffs[i][r.DF_P]			=	int(pids[i].kp) ; 
			_filter_coeffs[i][r.DF_D]			=	int(pids[i].kd) ;
			_filter_coeffs[i][r.DF_I]			=	int(pids[i].ki) ;
			_filter_coeffs[i][r.DF_I_LIMIT]		=	int(pids[i].max_int);
			_filter_coeffs[i][r.DF_OFFSET]		=	int(pids[i].offset);
			_filter_coeffs[i][r.DF_DAC_LIMIT]	=	int(pids[i].max_output);
    }

	return true;
}



bool MEIMotionControl::setReferenceRaw (int axis, double ref)		//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	_ref_positions[axis] = ref;
//		int deb = set_command(axis,ref);
	return true;
}


bool MEIMotionControl::setReferencesRaw (const double *refs)		//revisionata Mattia
{
		ACE_ASSERT(refs!=0);
	int i;
	MEIResources& r = RES(system_resources);

		for (i = 0; i < r.getJoints(); i++)
        {
            int deb = setReferenceRaw(i,refs[i]);
        }

	return true;
}

bool MEIMotionControl::setErrorLimitRaw(int j, double limit)
{
    return NOT_IMPLEMENTED_YET("setErrorLimit");
}

bool MEIMotionControl::setErrorLimitsRaw(const double *limit)
{
    return NOT_IMPLEMENTED_YET("setErrorLimits");
}


/*get the current position error*/
bool MEIMotionControl::getErrorRaw(int axis, double *err)				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	long rc = 0;
	double *out = (double *) err;

//	_mutex.wait();
	rc = get_error(axis, &out[axis]);
	printf("\n error on axis %d is %.1lf",axis,out[axis]);
//	_mutex.post();

	return true;
}


bool MEIMotionControl::getErrorsRaw(double *errs)						//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	double *out = (double *) errs;
	int i;
	long rc = 0;
//	_mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
			rc = getErrorRaw(i, &out[i]);
        }
//	_mutex.post();
	return true;
}

bool MEIMotionControl::getOutputRaw(int axis, double *out)
{
return NOT_IMPLEMENTED_YET("getOutputRaw");

}



bool MEIMotionControl::getOutputsRaw(double *outs)
{
return NOT_IMPLEMENTED_YET("getOutputsRaw");
}


//function to get the value of the positions setted asd references
bool MEIMotionControl::getReferenceRaw(int axis, double *ref)				
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	ref[axis] = _ref_positions[axis];

	return true;
}


bool MEIMotionControl::getReferencesRaw(double *ref)			
{
	int i;
	MEIResources& r = RES(system_resources);

		for (i = 0; i < r.getJoints(); i++)
        {
            getReferenceRaw(i, ref);
        }

	return true;
}

bool MEIMotionControl::getErrorLimitRaw(int j, double *err)
{
    return NOT_IMPLEMENTED_YET("getErrorLimit");
}

bool MEIMotionControl::getErrorLimitsRaw(double *errs)
{
    return NOT_IMPLEMENTED_YET("getErrorLimits");
}

bool MEIMotionControl::resetPidRaw(int j)
{
    return NOT_IMPLEMENTED_YET("resetPid");
}

bool MEIMotionControl::enablePidRaw(int axis)
{

    return NOT_IMPLEMENTED_YET("enablePidRaw");

}

bool MEIMotionControl::setOffsetRaw(int axis, double v)
{
	
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
	_filter_coeffs[axis][r.DF_OFFSET] = v;
	set_filter(axis, _filter_coeffs[axis]);
	return true;
}


bool MEIMotionControl::disablePidRaw(int axis)
{
	controller_idle(axis);
	return true;
}



/*this function check the value of the flag _positionmode
	it is just a way to safety manage the control
	if _positionmode is true means you can control in position */
bool MEIMotionControl::setPositionMode()		
{
	if (_positionmode == false)
	{
    _positionmode = true;
	return true;
	}

	else return _positionmode;
}

/*this function check the value of the flag _positionmode
	it is just a way to safety manage the control
	if _positionmode is false means you can control in velocity */
bool MEIMotionControl::setVelocityMode()
{
	if (_positionmode == true)
	{
    _positionmode = false;
	return true;
	}

	else return false;
}


/*it sets the reference postion and gives the command to move*/
bool MEIMotionControl::positionMoveRaw(int axis, double ref)		
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	if(_positionmode == true)
	{
		if (!ENABLED(axis))
			{
				// still fills the _ref_position structure.
				printf("\nAxis you want to move can not work!..check positionMoveRaw function!");
				return false;
			}

//	_mutex.wait();
	_ref_positions[axis] = ref;
	int  check = start_move(axis, _ref_positions[axis], _ref_speeds[axis], _ref_accs[axis]);
	if (check> 0)
	{
		printf("\n axis %d is already doing something....the code of the action is: %d",axis,check);
	}
	}
//	_mutex.post();

	return true;
}

/*it sets the reference postion withou giving the command to move
if you want to move you have to call the setPositionMode function*/
bool MEIMotionControl::positionMoveRaw(const double *refs)				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	int i;
//	_mutex.wait();
		for (i = 0; i < r.getJoints (); i++)
        {
            if (ENABLED(i))
			{
					positionMoveRaw(i, refs[i]);
			}
		}
//	_mutex.post();
	return true;   
}

bool MEIMotionControl::relativeMoveRaw(int j, double delta)
{
    return NOT_IMPLEMENTED_YET("positionRelative");
}

bool MEIMotionControl::relativeMoveRaw(const double *deltas)
{
    return NOT_IMPLEMENTED_YET("positionRelative");
}

/// check motion done, single axis.
//it returns true if the motion is terminated
bool MEIMotionControl::checkMotionDoneRaw(int axis, bool *ret)			
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	if (!axis_done(axis))
        {
			printf("\nMotion on axis %d not terminated yet!!", axis);
			ret[axis] = false;
           return true;
        }
	else 
	{
		printf("\nMotion on axis %d IS terminated!!", axis);
		ret[axis] = true;
		return false;
	}

}

/// check motion done, single axis.
//it returns true if the motion is terminated
bool MEIMotionControl::checkMotionDoneRaw (bool *ret)
{
	MEIResources& r = RES(system_resources);
	int i;

//	_mutex.wait();

		for (i = 0; i < r.getJoints(); i++)
        {
			checkMotionDoneRaw(i, ret);
        }
//	_mutex.post();
	return true;
		
}
	

bool MEIMotionControl::setRefSpeedRaw(int axis, double sp)		
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	_ref_speeds[axis] = sp;
	return true;
}

bool MEIMotionControl::setRefSpeedsRaw(const double *spds)					
{
	MEIResources& r = RES(system_resources);
      int i;

      for (i = 0; i < r.getJoints(); i++)
      {
      _ref_speeds[i] = spds[i];
      }

	return true;
}

bool MEIMotionControl::setRefAccelerationRaw(int axis, double acc)			
{
	MEIResources& r = RES(system_resources);
     _ref_accs[axis] = acc;

	 return true;
}

bool MEIMotionControl::setRefAccelerationsRaw(const double *accs)	
{
    MEIResources& r = RES(system_resources);
	int i;

	for (i = 0; i < r.getJoints(); i++)
        {
			_ref_accs[i] = accs[i]; 
        }

	return true;
}


bool MEIMotionControl::getRefSpeedsRaw (double *spds)	
{
	MEIResources& r = RES(system_resources);
      int i;

      for(i = 0; i < r.getJoints(); i++)
      {
	      spds[i] = _ref_speeds[i];
      }

	return true;
}

bool MEIMotionControl::getRefSpeedRaw (int axis, double *spd)	//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	spd[axis] = _ref_speeds[axis];
	
	return true;
}


bool MEIMotionControl::getRefAccelerationsRaw (double *accs)				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
        {
			getRefAccelerationRaw(i,accs);
        }

	return true;
}


bool MEIMotionControl::getRefAccelerationRaw (int axis, double *accs)				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	accs[axis] = _ref_accs[axis];

	return true;
}

bool MEIMotionControl::stopRaw(int axis)
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	int stop = set_stop(axis);
	printf("\nAxis %d has been stopped!!!",axis);
    return true;
}

bool MEIMotionControl::stopRaw()
{
	MEIResources& r = RES(system_resources);

	for(int i = 0; i < r.getJoints(); i++)
        {
			int stop = set_stop(i);
			printf("\nAxis %d has been stopped!!!",i);
        }
    return true;
}


bool MEIMotionControl::velocityMoveRaw (int axis, double sp) 				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
//	_mutex.wait();
	  if(_positionmode == false)
	{
		if (ENABLED (axis))
        {
			v_move(axis, sp , _ref_accs[axis]);
		}
	}
//	_mutex.post();

	return true;
}


bool MEIMotionControl::velocityMoveRaw (const double *sp)
{
	MEIResources& r = RES(system_resources);

      int i;
	  if(_positionmode == false)
	  {
      for(i = 0; i < r.getJoints(); i++)
		{
			if (ENABLED (i))
                {
					v_move(i, sp[i] , _ref_accs[i]);
				}
		}
	  }	

	return true;
}

bool MEIMotionControl::setEncoderRaw(int axis, double val)
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

//	int comm = set_command(axis, val);
	int posi = set_position(axis, val);

	_position_zero[axis] = double (dsp_encoder(axis)) + val;
	_winding[axis] = 0;

	return true;
}

bool MEIMotionControl::setEncodersRaw(const double *vals)
{
	MEIResources& r = RES(system_resources);
		
	int rc;
		
		/// this is to reset the encoder ref value.
	for (int i = 0; i < r.getJoints(); i++)
        {
			rc = set_position(i, vals[i]);
			_position_zero[i] = double(dsp_encoder (i)) + vals[i];
		}

	return true;
}

bool MEIMotionControl::resetEncoderRaw(int j)
{
	_position_zero[j] = double(dsp_encoder (j));
    return true;
}

bool MEIMotionControl::resetEncodersRaw()		
{
	MEIResources& r = RES(system_resources);

    int i;
	bool ret;
    for(i = 0; i < r.getJoints(); i++)
      {
        ret= resetEncoderRaw(i);
	}
    return ret;
}

bool MEIMotionControl::getEncodersRaw(double *v)		//revisionata Mattia
{														
	MEIResources& r = RES(system_resources);
	int i,ret;
//	_mutex.wait();
	for (i = 0; i < r.getJoints(); i++) 
	{
		ret = getEncoderRaw(i, v);
	}
//	_mutex.post();
	return true;
}

bool MEIMotionControl::getEncoderRaw(int axis, double *v) //revisionata Mattia
{
	int16 rc = 0;

	///rc = get_position(axis, (double *)tmp->parameters);
	double tmpd;
	tmpd = double(dsp_encoder (axis));

	/// computing the actual encoder value.
	if (_sgn(tmpd) < 0 && _16bit_oldpos[axis] > 16384.0 && _winding[axis] == 0)
	{
		_winding[axis] = 1;
	}
	else
	if (_sgn(tmpd) > 0 && _16bit_oldpos[axis] < -16384.0 && _winding[axis] == 1)
	{
		_winding[axis] = 0;
	}
	else
	if (_sgn(tmpd) > 0 && _16bit_oldpos[axis] < -16384.0 && _winding[axis] == 0)
	{
		_winding[axis] = -1;
	}
	else
	if (_sgn(tmpd) < 0 && _16bit_oldpos[axis] > 16384.0 && _winding[axis] == -1)
	{
		_winding[axis] = 0;
	}

	_16bit_oldpos[axis] = tmpd;

	switch (_winding[axis])
	{
		case 1:
			tmpd = 65535.0 + tmpd - _position_zero[axis];
			break;

		case -1:
			tmpd = -65536.0 + tmpd - _position_zero[axis];
			break;

		case 0:
			tmpd -= _position_zero[axis];
			break;
	}

	v[axis] = tmpd;
	return true;
}

bool MEIMotionControl::getEncoderSpeedsRaw(double *v)
{
  	return NOT_IMPLEMENTED_YET("getEncoderSpeeds");
}

bool MEIMotionControl::getEncoderSpeedRaw(int j, double *v)
{
    return NOT_IMPLEMENTED_YET("getEncoderSpeed");
}

bool MEIMotionControl::getEncoderAccelerationsRaw(double *v)
{
  	return NOT_IMPLEMENTED_YET("getEncoderAccs");
}

bool MEIMotionControl::getEncoderAccelerationRaw(int j, double *v)
{
    return NOT_IMPLEMENTED_YET("getEncoderAcc");
}

bool MEIMotionControl::disableAmpRaw(int axis)			
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
	int amp = disable_amplifier(axis);

	_amplifiers = false;
	return true;
}

bool MEIMotionControl::enableAmpRaw(int axis)				
{
	MEIResources& r = RES(system_resources);
//	_mutex.wait();
	int level_amp	= set_amp_enable_level(axis, true);		//set the working ampli level at TRUE = HIGH
	int fault_level = set_amp_fault_level(axis, false);

	while(!motion_done(axis));								// required otherwise controller_run may fail
	int cs = clear_status(axis);
	int rc = controller_run(axis);

	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	

	int amp = enable_amplifier(axis);						//set the state of the amp to the 
	amp = set_amp_enable(axis, true);						//enable-state(setted by set_amp_enable_level)
//	short value;
//	amp = get_amp_enable(axis, &value);

	short ampli;
	int level = get_amp_enable_level(axis, &ampli);
//	printf("\nAmp%d enable  state : %d",axis,ampli);

	_amplifiers = true;

//	_mutex.post();

	return true;
}


bool MEIMotionControl::getCurrentsRaw(double *cs)	
{
	return NOT_IMPLEMENTED_YET("getCurrentRaw");
	/*
    MEIResources& r = RES(system_resources);
	int i;
	
	_mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
 //           cs[i] = double(r._bcastRecvBuffer[i]._current);
				get_position(i,cs);	
        }

	_mutex.post();
	return true;
	*/
}


bool MEIMotionControl::getCurrentRaw(int axis, double *c)
{
	    return NOT_IMPLEMENTED_YET("getCurrentRaw");
}

bool MEIMotionControl::setMaxCurrentRaw(int axis, double v)
{
	    return NOT_IMPLEMENTED_YET("getEncoderSpeed");
}



//before starting the calibration you have to move each axis of the arm 
//they have to stay as much as possible far away from their limit..
//this function looks for a median point between two special encoder-steps
//the problem is that if the initial position is between the special encoder-steps and the
//hardware limit of the axis you will see an error in calibration.

bool MEIMotionControl::calibrateRaw(int axis, double p)
{
    MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	int check;


//	check = set_amp_fault(axis,ABORT_EVENT);
//	check = set_amp_fault(axis,E_STOP_EVENT);
//	check = set_amp_fault(axis,NO_EVENT);
	check = clear_status(axis);
//	check = controller_idle(axis);
	while (!motion_done(axis))
	check = controller_run(axis);
	check = enable_amplifier(axis);

	
	printf(("\nCalibration routine started.\n"));
		if (! (_alreadyinint && _amplifiers) )
		{
			printf(("\nArm is not initialized or power is down.\n"));
			return false;
		}

		setLimitsRaw(axis,-100000, 100000);
		printf("\n The joint-limits of axis %i are now disabled for calibration...pay attention on arm movement!!",axis);


		bool ret[6] =		  {		0,		0,		0,		0,		0,		0};
		double speeds1[6]	= { 500.0,  500.0,  500.0,  500.0,  500.0,  500.0};
		double speeds2[6]	= {-500.0, -500.0, -500.0, -500.0, -500.0, -500.0};
		double acc[6]		= {5000.0, 5000.0, 5000.0, 5000.0, 5000.0, 5000.0};
		double pos1[6]		= {   0.0,    0.0,    0.0,    0.0,    0.0,	  0.0};
		double pos2[6]		= {   0.0,    0.0,    0.0,    0.0,    0.0,    0.0};
		double newHome[6]	= {   0.0,    0.0,    0.0,    0.0,    0.0,    0.0};

		for (int i = 0; i < r.getJoints(); i++)
        {
			if(i != axis) 
			{
				speeds1[i]	= 0.0;
				speeds2[i]	= 0.0;
				acc[i]		= 0.0;
			}
		}

		// gather my current position. this is the HOME position from now on
		check = setEncoderRaw(axis,r.RposHome[axis]);

		printf(("\nGathering first index...\n"));

		// find first index
		check = set_home_index_config(axis,INDEX_ONLY);
		check = set_home_level(axis, FALSE);
		check = set_home(axis, NO_EVENT);
		check = setRefAccelerationRaw (axis,acc[axis]);

		check = setVelocityMode();
		check = velocityMoveRaw(axis, speeds1[axis]);
		while(checkMotionDoneRaw(axis,ret))
		printf("\n some axis still continue to work!");

		check = getEncoderRaw(axis,pos1);
		check = clear_status(axis);

		printf(("done.\n"));

		// go back to HOME position

		check = setRefSpeedsRaw(speeds1);
		printf("\nGoing back home...\n");

		check = setPositionMode();
		check = positionMoveRaw(axis,r.RposHome[axis]);

		while(checkMotionDoneRaw(axis,ret))
		printf("done.\n");
		check = clear_status(axis);

		printf(("\nGathering second index...\n"));

		// find second index

		check = set_home_index_config(axis,INDEX_ONLY);
		check = set_home_level(axis, FALSE);
		check = set_home(axis, NO_EVENT);
		check = setRefAccelerationRaw (axis,acc[axis]);

		check = setVelocityMode();
		check = velocityMoveRaw(axis,speeds2[axis]);
		while(checkMotionDoneRaw(axis,ret))
		printf("\n some axis still continue to work!");

		check = getEncoderRaw(axis,pos2);
		check = clear_status(axis);

		printf(("done.\n"));

		printf(("\n"));
		// compute new HOME position

			newHome[axis] = (pos1[axis]+pos2[axis])/2;
			printf("newHome%d=%.1lf, ", newHome[axis]);

		printf(("\n"));


		// go back to HOME CALIBRATED position

		check = setRefSpeedsRaw(speeds1);
		printf("\nGoing back home...\n");

		check = setPositionMode();
		check = positionMoveRaw(axis,newHome[axis]);
		
		while(checkMotionDoneRaw(axis,ret))
		printf("done.\n");

		// reset encoders here

		check = set_home_index_config(axis,INDEX_ONLY);
		check = set_home_level(axis, FALSE);
		check = set_home(axis, NO_EVENT);
		check = getEncoderRaw(axis,r.RposHome);
		check = clear_status(axis);

		printf(("\nEncoder reset.\n"));

		setLimitsRaw(axis,r._RlimitsMin[axis], r._RlimitsMax[axis]);
		printf("\n The joint-limits of axis %i are now enabled again!\ntheir values are min:%.1lf max:%.1lf!!",
			axis, r._RlimitsMin[axis], r._RlimitsMax[axis]);

return true;

}

bool MEIMotionControl::doneRaw(int axis)
{
    return NOT_IMPLEMENTED_YET("doneRaw");
}

bool MEIMotionControl::setPrintFunction(int (*f) (const char *fmt, ...))
{
	return NOT_IMPLEMENTED_YET("setPrintFunction");
}

bool MEIMotionControl::getAmpStatusRaw(int *st)				
{

	MEIResources& r = RES(system_resources);
	int i;
	short value;
//    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            st[i] = get_amp_enable(i,  &value);
			printf("\n Amp %d enable state = %d",i,value);
        }
//    _mutex.post();

	return true;
	
}

bool MEIMotionControl::setLimitsRaw(int axis, double min, double max)	
{
		MEIResources& r = RES(system_resources);

		//trick to re-initialize limits 
			if(axis == 99)
			{
				for(int i=0; i<r.getJoints(); i++)
				{
					set_positive_sw_limit(axis,r._RlimitsMax[i],STOP_EVENT);
					set_negative_sw_limit(axis,r._RlimitsMin[i],STOP_EVENT);
				}
			
				_softwareLimits = true;
				return true;

			}

			//regular limit setting 
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

			set_positive_sw_limit(axis,max,STOP_EVENT);
			set_negative_sw_limit(axis,min,STOP_EVENT);
			_softwareLimits = true;



	return true;
}

bool MEIMotionControl::getLimitsRaw(int axis, double *min, double *max)	
{
	int iMin, iMax;

	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	double tmpmax,tmpmin;
	
	iMax =	get_positive_sw_limit(axis,&tmpmax,r._Revents);
	iMin =	get_negative_sw_limit(axis,&tmpmin,r._Revents);

	max[axis]=tmpmax;
	min[axis]=tmpmin;

	return true;
}

bool MEIMotionControl::loadBootMemory()
{

    MEIResources& r = RES(system_resources);
	
    int ret = 0;
    for(int j=0; j<r.getJoints(); j++)
        {
			ret=get_boot_filter(j, _filter_coeffs[j]);
            if (ret>0)
			{
				printf("\nSome error occurred in loadBootMemory");
               return false;
			}
        }
	return true;
}

bool MEIMotionControl::saveBootMemory ()
{
    MEIResources& r = RES(system_resources);
	
    int ret = 0;
    for(int j=0; j<r.getJoints(); j++)
        {
			ret=set_boot_filter(j, _filter_coeffs[j]);
            if (ret>0)
			{
				printf("\nSome error occurred in saveBootMemory");
               return false;
			}
        }
	return true;

}

/// sets the broadcast policy for a given board (don't need to be called twice).
/// the parameter is a 32-bit integer: bit X = 1 -> message X = active
/// e.g. 0x02 activates the broadcast of position information
///		 0x04 activates the broadcast of velocity ...
///
bool MEIMotionControl::setBCastMessages (int axis, double v)
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

//	return _writeDWord (CAN_SET_BCAST_POLICY, axis, S_32(v));
	return true;
}

inline bool MEIMotionControl::ENABLED (int axis)
{
	MEIResources& r = RES(system_resources);

	int value;
	if(axis_state(axis)==0)
		return true;
	else value = axis_state(axis);
	if (value > 0)
	{
		int value2 = axis_source(axis);
		printf("\n axis %d is already doing something....the code of the action is: %d",axis,value);
		printf("\n axis_sources exit with code : %d",value2);
		return false;
	}
	else return true;
	
}
/*
///
/// helper functions.
///
///
///
*/
/// sends a message without parameters
bool MEIMotionControl::_writeNone (int msg, int axis)
{
	return NOT_IMPLEMENTED_YET("_writeNone");
}

bool MEIMotionControl::_readWord16 (int msg, int axis, short& value)
{
	return NOT_IMPLEMENTED_YET("_readWord16");
}

/// reads an array.
bool MEIMotionControl::_readWord16Array (int msg, double *out)
{
	return NOT_IMPLEMENTED_YET("_readWord16Array");
	
}

/// to send a Word16.
bool MEIMotionControl::_writeWord16 (int msg, int axis, short s)
{	
	return NOT_IMPLEMENTED_YET("_writeWord16");
}

/// write a DWord
bool MEIMotionControl::_writeDWord (int msg, int axis, int value)
{		
	return NOT_IMPLEMENTED_YET("_writeDWord");

}

/// two shorts in a single Can message (both must belong to the same control card).
bool MEIMotionControl::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
		return NOT_IMPLEMENTED_YET("_writeWord16Ex");
}

///
/// sends a message and gets a dword back.
/// 
bool MEIMotionControl::_readDWord (int msg, int axis, int& value)
{    
	return NOT_IMPLEMENTED_YET("_readDWord");
	
}

/// reads an array of double words.
bool MEIMotionControl::_readDWordArray (int msg, double *out)
{
    return NOT_IMPLEMENTED_YET("_readDWordArray");


}