/*
* Copyright (C) 2007 Mattia Castelnovi
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


/// general purpose stuff.
#include <yarp/os/Time.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <ace/Sched_Params.h>


/// specific to this device driver.
#include "MEIMotionControl.h"

#include <yarp/dev/ControlBoardInterfaces.h>


#include "X:\Mattia2\PumaControlInterface\ProvaNuovoDriver\Puma260ControlBoardInterfacesImpl.inl"


// MEI
#define MEI_WINNT
#define PCDSP_LIB
#include "include/pcdsp.h"
#include "include/idsp.h"

char __filename[256] = "arm.ini";


#define M_PI 3.14159265358979323846
#define CAN_GET_CONTROL_MODE 7
#define BUF_SIZE 2047

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
	

		_zeros = new double[nj];
		_signs = new int[nj];
		_axis_map = new int[nj];
		_inv_axis_map = new int[nj];
		_encoderToAngles = new double[nj];
		_fwdCouple = new double[nj];
		_invCouple = new double[nj];
		_stiffPID = new int[nj];
		_limitsMax = new int[nj];
		_limitsMin = new int[nj];
		_maxDAC = new double[nj];
		_pids	= new Pid[nj];


		_njoints= _BabybotArm::_nj;
		nj= _BabybotArm::_nj;

		int i;
		for(i = 0; i<_njoints; i++) 
		{
			_zeros[i]			= _BabybotArm::_zeros[i];
			_axis_map[i]		= _BabybotArm::_axis_map[i];
			_signs[i]			= _BabybotArm::_signs[i];
			_encoderToAngles[i] = _BabybotArm::_encoders[i]*_BabybotArm::_encWheels[i];		
			_fwdCouple[i]		= _BabybotArm::_fwdCouple[i];
			_stiffPID[i]		= _BabybotArm::_stiffPID[i];
			_maxDAC[i]			= _BabybotArm::_maxDAC[i];
		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) + (_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
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
 		if (_encoderToAngles != NULL)			delete [] _encoderToAngles;
		if (_zeros != NULL)						delete [] _zeros;
		if (_signs != NULL)						delete [] _signs;
		if (_axis_map != NULL)					delete [] _axis_map;
		if (_invCouple != NULL)					delete [] _invCouple;
		if (_stiffPID != NULL)					delete [] _stiffPID;
		if (_maxDAC != NULL)					delete [] _maxDAC;
		if (_limitsMax != NULL)					delete [] _limitsMax;
		if (_limitsMin != NULL)					delete [] _limitsMin;
		if (_inv_axis_map != NULL)				delete [] _inv_axis_map;
		if (_fwdCouple != NULL)					delete [] _fwdCouple;
		if (_pids != NULL)						delete [] _pids;

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
		if (_pids != NULL)				delete [] _pids;
		if (_zeros != NULL)				delete [] _zeros;
		if (_signs != NULL)				delete [] _signs;
		if (_axis_map != NULL)			delete [] _axis_map;
		if (_inv_axis_map != NULL)		delete [] _inv_axis_map;
		if (_encoderToAngles != NULL)	delete [] _encoderToAngles;
		if (_fwdCouple != NULL)			delete [] _fwdCouple;
		if (_invCouple != NULL)			delete [] _invCouple;
		if (_stiffPID != NULL)			delete [] _stiffPID;
		if (_maxDAC != NULL)			delete [] _maxDAC;
		if (_limitsMax != NULL)			delete [] _limitsMax;
		if (_limitsMin != NULL)			delete [] _limitsMin;
		
		_zeros				= new double [nj];
		_signs				= new int [nj];
		_axis_map			= new int [nj];
		_inv_axis_map		= new int [nj];
		_encoderToAngles	= new double [nj];
		_fwdCouple			= new double [nj];

		_invCouple			= new double [nj];
		_stiffPID			= new int [nj];
		_maxDAC				= new double [nj];
		_limitsMax			= new int [nj];
		_limitsMin			= new int [nj];
		_pids				= new Pid[nj];
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
//    AUX_FILTER_COEFFS   =       10,
	MEIPORT				=		0x0300
};


		double *_RlimitsMax;
		double *_RlimitsMin;
		double *_Rzeros;
		double *_Rsigns;
		double *_RencoderToAngles;
		double *_RfwdCouple;
		double *_RfwdCoupleC;
		double *_RinvCouple;
		double *_RmaxDAC;		
		double *_R16bit_oldpos;
		double *_RposHome;			//10 double

		double *_Rref_acc;
		double *_Rref_speeds;
		double *_Rref_positions;
		
		short *_Revents;

		int16 *_Rwinding;			/// counting how many times the encoder wraps up.

		int *_RstiffPID;
		int *_Raxis_map;
		int *_Rinv_axis_map;		//3int

		int _Rnjoints;
		int _Rtimeout;								/// this is my thread timeout.
		int _Rpolling_interval;						/// thread polling interval.
		int _Rspeed;								/// speed of the bus.
		int _RnetworkN;								/// network number.
	




	unsigned char _Rmy_address;					/// 
};

MEIResources::MEIResources ()
{
	_RlimitsMax = NULL;
	_RlimitsMin = NULL;
	_Rzeros = NULL;
	_Rsigns = NULL;
	_Raxis_map = NULL;
	_Rinv_axis_map = NULL;
	_RencoderToAngles = NULL;
	_RfwdCouple = NULL;
	_RinvCouple = NULL;
	_RstiffPID = NULL;
	_RmaxDAC = NULL;
	_RposHome = NULL;
	_Rwinding=NULL;
	_R16bit_oldpos=NULL;
	_Revents=NULL;


	_Rref_acc = NULL;
	_Rref_speeds = NULL;
	_Rref_positions = NULL;



	//here it is better to put a value which can change depending on the number of robot-joints- 
	MEIMotionControlParameters param(6);

	initialize(param);
		for (int i = 0; i<6; i++)
	{
			_R16bit_oldpos[i]=0.0;
		}

}


MEIResources::~MEIResources () 
{ 
//	uninitialize(); 
}

bool MEIResources::initialize (const MEIMotionControlParameters& parms)
{
		_Rnjoints = parms._njoints;
		

		_RlimitsMax			= new double[_Rnjoints];
		_RlimitsMin			= new double[_Rnjoints];
		_Rzeros				= new double[_Rnjoints];
		_Rsigns				= new double[_Rnjoints];
		_RencoderToAngles	= new double[_Rnjoints];
		_RfwdCouple			= new double[_Rnjoints];
		_RfwdCoupleC		= new double[_Rnjoints];
		_RinvCouple			= new double[_Rnjoints];
		_RmaxDAC			= new double[_Rnjoints];
		_R16bit_oldpos		= new double[_Rnjoints];
		_RposHome			= new double[_Rnjoints];	//10 double

		_RstiffPID			= new int[_Rnjoints];
		_Raxis_map			= new int[_Rnjoints];
		_Rinv_axis_map		= new int[_Rnjoints];		//3 int

		_Rwinding			= new int16[_Rnjoints];		
		_Revents			= new short[4];				//2short

		_Rref_acc           = new double[_Rnjoints];
		_Rref_speeds		= new double[_Rnjoints];
		_Rref_positions		= new double[_Rnjoints];

	
	for (int i = 0; i<parms._njoints; i++)
	{
		
		_Rzeros[i] = parms._zeros[i];
		_Rsigns[i] = parms._signs[i];
		_Raxis_map[i] = parms._axis_map[i];
		_Rinv_axis_map[i] = parms._inv_axis_map[i];
		_RencoderToAngles[i] = parms._encoderToAngles[i];
		_RfwdCouple[i] = parms._fwdCouple[i];
		_RinvCouple[i] = parms._invCouple[i];
		_RstiffPID[i] = parms._stiffPID[i];
		_RmaxDAC[i] = parms._maxDAC[i];
		_RposHome[i] = 0.0;
		_RlimitsMax[i] = parms._limitsMax[i];
		_RlimitsMin[i] = parms._limitsMin[i];
		_Rref_acc[i] = 0;
		_Rref_speeds[i] = 0;	
		_Rref_positions[i] = 0;	
	}


		return true;
}


bool MEIResources::uninitialize ()
{

	if (_RlimitsMax != NULL)		delete []	_RlimitsMax ;
	if (_RlimitsMin != NULL)		delete []	_RlimitsMin ;
	if (_Rzeros != NULL)			delete []	_Rzeros;
	if (_Rsigns != NULL)			delete []	_Rsigns;
	if (_RencoderToAngles != NULL)	delete []	_RencoderToAngles;
	if (_RfwdCouple != NULL)		delete []	_RfwdCouple;
	if (_RinvCouple != NULL)		delete []	_RinvCouple;
	if (_RmaxDAC != NULL)			delete []	_RmaxDAC;
	if (_RposHome != NULL)			delete []	_RposHome;
	if (_R16bit_oldpos != NULL)		delete []	_R16bit_oldpos;
	if (_RstiffPID != NULL)			delete []	_RstiffPID;
	if (_Raxis_map != NULL)			delete []	_Raxis_map;
	if (_Rinv_axis_map != NULL)		delete []	_Rinv_axis_map;
	if (_Revents != NULL)			delete []	_Revents;
	if (_Rwinding != NULL)			delete []	_Rwinding;
	if (_Rref_acc != NULL)			delete []	_Rref_acc;
	if (_Rref_speeds != NULL)		delete []	_Rref_speeds;
	if (_Rref_positions != NULL)	delete []	_Rref_positions;

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
{
	system_resources = NULL;
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



bool MEIMotionControl::open (const MEIMotionControlParameters &params)
{

	MEIResources& r = RES (system_resources);


	if (!r.initialize (params))
        {
			printf("\n I AM NOT ABLE TO INITIALIZE THE RESOURCES!!!\n");
            return false;
        }
		

    ImplementPositionControl<MEIMotionControl, IPositionControl>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles , params._zeros);
    
    ImplementVelocityControl<MEIMotionControl, IVelocityControl>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);

    ImplementPidControl<MEIMotionControl, IPidControl>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);

    ImplementEncoders<MEIMotionControl, IEncoders>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);

    ImplementControlCalibration<MEIMotionControl, IControlCalibration>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);

    ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);

    ImplementControlLimits<MEIMotionControl, IControlLimits>::
        initialize(params._njoints, params._axis_map, params._encoderToAngles, params._zeros);




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



	// default initialization for this device driver.
	setPids(params._pids);
	_dsp_rate = dsp_sample_rate();

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

	return true;
}



bool MEIMotionControl::open(yarp::os::Searchable& config) {
	
	bool fromfileflag=false;
	Searchable& p = config;	
	MEIResources& r = RES (system_resources);

    if (!p.check("GENERAL","section for general motor control parameters")) 
	{
        fprintf(stderr, "Cannot understand configuration parameters\n");
		printf("\nI am going to load parameters from file\n");
        fromfileflag=true;
	}

	
	if (fromfileflag==true)
	{
		Property robot;

	
		//take data from file
		robot.fromConfigFile("MEIconfig.txt");
		if (!robot.check("GENERAL")) 
		{
			printf("Cannot understand config file\n");
			return false;
		}


    int nj = robot.findGroup("GENERAL").check("Joints", Value(1), "Number of degrees of freedom").asInt();
    MEIMotionControlParameters params(nj);
    params._njoints = nj;

	int i;

	_filter_coeffs = new int16* [params._njoints];
	ACE_ASSERT (_filter_coeffs != NULL);
	
	for(i = 0; i < params._njoints; i++)
	{
		_filter_coeffs[i] = new int16 [r.COEFFICIENTS];
		ACE_ASSERT (_filter_coeffs[i] != NULL);
	}


    ////// GENERAL
    Bottle& xtmp = robot.findGroup("GENERAL").findGroup("AxisMap","a list of reordered indices for the axes");
	if (params._njoints != 6) {
        printf("AxisMap does not have the right number of entries\n");
        return false;
    }
    for (i = 1; i < params._njoints+1; i++) params._axis_map[i-1] = xtmp.get(i).asInt();

	xtmp = robot.findGroup("GENERAL").findGroup("Signs","a list of Signs for the axes");
	if (params._njoints != 6) {
        printf("Signs does not have the right number of entries\n");
        return false;
    }
    for (i = 1; i < params._njoints+1; i++) params._signs[i-1] = xtmp.get(i).asInt();

	xtmp = robot.findGroup("GENERAL").findGroup("Stiff","a list of Stiff for the axes");
	if (params._njoints != 6) {
        printf("Signs does not have the right number of entries\n");
        return false;
    }
    
	for (i = 1; i < params._njoints+1; i++) params._stiffPID[i-1] = xtmp.get(i).asInt();

	xtmp = robot.findGroup("GENERAL").findGroup("MaxDAC","a list of MaxDAC values");
	if (params._njoints != 6) {
        printf("MaxDAC does not have the right number of entries\n");
        return false;
    }
    for (i = 1; i < params._njoints+1; i++) params._maxDAC[i-1] = xtmp.get(i).asDouble();


    xtmp = robot.findGroup("GENERAL").findGroup("Encoder","a list of scales for the encoders");
	if (params._njoints != 6) {
        printf("Encoder does not have the right number of entries\n");
        return false;
    }
	for (i = 1; i < params._njoints+1; i++) params._encoderToAngles[i-1] = xtmp.get(i).asDouble();

    xtmp = robot.findGroup("GENERAL").findGroup("Zeros","a list of offsets for the zero point");
	if (params._njoints != 6) {
        printf("Zeros does not have the right number of entries\n");
        return false;
    }
    for (i = 1; i < params._njoints+1; i++) params._zeros[i-1] = xtmp.get(i).asDouble();


    ////// PIDS
    int j;
    for(j=0;j<params._njoints;j++)
        {
            char tmp[80];
            sprintf(tmp, "Pid%d", j); 
            xtmp = robot.findGroup("PIDS").findGroup(tmp);
			
			_filter_coeffs[j][r.DF_P]			= params._pids[j].kp = xtmp.get(1).asInt();
			_filter_coeffs[j][r.DF_I]			= params._pids[j].ki = xtmp.get(2).asInt();
			_filter_coeffs[j][r.DF_D]			= params._pids[j].kd = xtmp.get(3).asInt();
			_filter_coeffs[j][r.DF_ACCEL_FF]	= xtmp.get(4).asInt();
			_filter_coeffs[j][r.DF_VEL_FF]		= xtmp.get(5).asInt();
			_filter_coeffs[j][r.DF_I_LIMIT]		= params._pids[j].max_int = xtmp.get(6).asInt();
			_filter_coeffs[j][r.DF_OFFSET]		= params._pids[j].offset = xtmp.get(7).asInt();
			_filter_coeffs[j][r.DF_DAC_LIMIT]	= params._pids[j].max_output =xtmp.get(8).asInt();
			_filter_coeffs[j][r.DF_SHIFT]		= xtmp.get(9).asInt();
			_filter_coeffs[j][r.DF_FRICT_FF]	= xtmp.get(10).asInt();

			params._pids[j].kp = (double)_filter_coeffs[j][r.DF_P];
            params._pids[j].kd = (double)_filter_coeffs[j][r.DF_D];
            params._pids[j].ki = (double)_filter_coeffs[j][r.DF_I];
    
            params._pids[j].max_int		= (double)_filter_coeffs[j][r.DF_I_LIMIT]	;
			params._pids[j].offset		= (double)_filter_coeffs[j][r.DF_OFFSET];
            params._pids[j].max_output	= (double)_filter_coeffs[j][r.DF_DAC_LIMIT];
            params._pids[j].scale		= (double)_filter_coeffs[j][r.DF_SHIFT];
        }



    /////// LIMITS

    xtmp = robot.findGroup("LIMITS").findGroup("Max","a list of maximum angles (in degrees)");
	if (params._njoints != 6) {
        printf("Max does not have the right number of entries\n");
        return false;
    }
    for(i=1;i<params._njoints+1; i++) params._limitsMax[i-1]=xtmp.get(i).asDouble();

    xtmp = robot.findGroup("LIMITS").findGroup("Min","a list of minimum angles (in degrees)");
	if (params._njoints != 6) {
        printf("Min does not have the right number of entries\n");
        return false;
    }
    for(i=1;i<params._njoints+1; i++) params._limitsMin[i-1]=xtmp.get(i).asDouble();

	fromfileflag=false;

	
////////////////////////////////////////////////////end new
return open(params);
}
else
{
	int i;

	int nj = p.findGroup("GENERAL").check("Joints",Value(1),
                                          "Number of degrees of freedom").asInt();
	MEIMotionControlParameters params(nj);
    params._njoints = nj;

    ////// GENERAL
    Bottle& xtmp = p.findGroup("MaxDAC");
	ACE_ASSERT (xtmp.size() == nj+1);

    for (i = 1; i < xtmp.size(); i++) 
	{
		params._maxDAC[i-1] = xtmp.get(i).asDouble();
	}


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
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._fwdCouple[i-1] = xtmp.get(i).asDouble();
	}
	printf("\n");


    xtmp = p.findGroup("Zeros");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._zeros[i-1] = xtmp.get(i).asDouble();
	}
		
	xtmp = p.findGroup("Signs");
	ACE_ASSERT (xtmp.size() == nj+1);
	for (i = 1; i < xtmp.size(); i++)
	{
		params._signs[i-1] = xtmp.get(i).asDouble();
	}

	xtmp = p.findGroup("Stiff");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) 
	{
		params._stiffPID[i-1] = xtmp.get(i).asInt();
	}

    /////// LIMITS
    xtmp = p.findGroup("Max");
 	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++) 
	{
		params._limitsMax[i-1]=xtmp.get(i).asDouble();
	}

	xtmp = p.findGroup("Min");
	ACE_ASSERT (xtmp.size() == nj+1);
    for(i=1;i<xtmp.size(); i++)
	{
		params._limitsMin[i-1]=xtmp.get(i).asDouble();
	}



    ////// PIDS
    int j=0;
    for(j=0;j<nj;j++)
        {
            char tmp[80];
            sprintf(tmp, "LPid%d", j); 
            xtmp = p.findGroup(tmp);

			_filter_coeffs[j][r.DF_P]			= params._pids[j].kp = xtmp.get(1).asInt();
			_filter_coeffs[j][r.DF_I]			= params._pids[j].ki = xtmp.get(2).asInt();
			_filter_coeffs[j][r.DF_D]			= params._pids[j].kd = xtmp.get(3).asInt();
			_filter_coeffs[j][r.DF_ACCEL_FF]	= xtmp.get(4).asInt();
			_filter_coeffs[j][r.DF_VEL_FF]		= xtmp.get(5).asInt();
			_filter_coeffs[j][r.DF_I_LIMIT]		= params._pids[j].max_int = xtmp.get(6).asInt();
			_filter_coeffs[j][r.DF_OFFSET]		= params._pids[j].offset = xtmp.get(7).asInt();
			_filter_coeffs[j][r.DF_DAC_LIMIT]	= params._pids[j].max_output =xtmp.get(8).asInt();
			_filter_coeffs[j][r.DF_SHIFT]		= xtmp.get(9).asInt();
			_filter_coeffs[j][r.DF_FRICT_FF]	= xtmp.get(10).asInt();

			params._pids[j].kp = (double)_filter_coeffs[j][r.DF_P];
            params._pids[j].ki = (double)_filter_coeffs[j][r.DF_I];
			params._pids[j].kd = (double)_filter_coeffs[j][r.DF_D];
            params._pids[j].max_int		= (double)_filter_coeffs[j][r.DF_I_LIMIT]	;
			params._pids[j].offset		= (double)_filter_coeffs[j][r.DF_OFFSET];
            params._pids[j].max_output	= (double)_filter_coeffs[j][r.DF_DAC_LIMIT];
            params._pids[j].scale		= (double)_filter_coeffs[j][r.DF_SHIFT];

			set_filter(j, _filter_coeffs[j] );
	}


		// build encoder to angles
	double *encoders = new double [params._njoints];
	double *encWheels = new double [params._njoints];

    xtmp = p.findGroup("EncWheel");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) 
	{
		encWheels[i-1] = xtmp.get(i).asDouble();
	}

    xtmp = p.findGroup("Encoder");						
	ACE_ASSERT (xtmp.size() == nj+1);
	
    for (i = 1; i < xtmp.size(); i++) 
	{
		encoders[i-1] = xtmp.get(i).asDouble();
		params._encoderToAngles[i-1] = encoders[i-1]*encWheels[i-1];
	}
		
	params._fwdCouple[3] = params._fwdCouple[3]*encWheels[3];
	params._fwdCouple[4] = params._fwdCouple[4]*encWheels[4];
	params._fwdCouple[5] = params._fwdCouple[5]*encWheels[5];

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

		return open(params);
		}
		
		


}

 
bool MEIMotionControl::close(void)
{
	MEIResources& r = RES(system_resources);

	    if (_opened) {
        // disable the controller, pid controller & pwm off
        int i;
        for (i = 0; i < r._Rnjoints; i++) {
            disablePid(i);
            disableAmp(i);
        }

    ImplementPositionControl<MEIMotionControl, IPositionControl>::uninitialize();
	ImplementVelocityControl<MEIMotionControl, IVelocityControl>::uninitialize();
    ImplementPidControl<MEIMotionControl, IPidControl>::uninitialize();
    ImplementEncoders<MEIMotionControl, IEncoders>::uninitialize();
    ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>::uninitialize();
    ImplementControlLimits<MEIMotionControl, IControlLimits>::uninitialize();

    }




    _opened = false;

	bool ret = r.uninitialize ();

	if (_filter_coeffs != NULL) delete[] _filter_coeffs;

	return ret;

}

void MEIMotionControl::run ()
{
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
	
	int j = axis;
	
	
	printf("\n filter_coeffs j:%d = %d ", j ,_filter_coeffs[j][0]);
		printf(" %d",_filter_coeffs[j][1]);
		printf(" %d",_filter_coeffs[j][2]);
		printf(" %d",_filter_coeffs[j][3]);
		printf(" %d",_filter_coeffs[j][4]);
		printf(" %d",_filter_coeffs[j][5]);
		printf(" %d",_filter_coeffs[j][6]);
		printf(" %d",_filter_coeffs[j][7]);
		printf(" %d",_filter_coeffs[j][8]);
		printf(" %d",_filter_coeffs[j][9]);
		
		printf("\n");

		pids[axis].kp = (double)_filter_coeffs[axis][r.DF_P];
		pids[axis].ki = (double)_filter_coeffs[axis][r.DF_I];
		pids[axis].kd = (double)_filter_coeffs[axis][r.DF_D];
		pids[axis].max_int = (double)_filter_coeffs[axis][r.DF_I_LIMIT];
		pids[axis].offset = (double)_filter_coeffs[axis][r.DF_OFFSET];
		pids[axis].max_output = (double)_filter_coeffs[axis][r.DF_DAC_LIMIT];

		printf("\n");
		printf("getPidRaw axis:%d = %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf",
		axis,
		pids[axis].kp ,
		pids[axis].ki ,
		pids[axis].kd ,
		pids[axis].max_int	,
		pids[axis].offset,
		pids[axis].max_output)	;

		return true;
}

bool MEIMotionControl::getPidsRaw (Pid *pids)
{
    MEIResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r._Rnjoints; i++)
        {
			
		get_filter(i, _filter_coeffs[i]);

		pids[i].kp			= (double)(_filter_coeffs[i][r.DF_P] );
		pids[i].ki			= (double)(_filter_coeffs[i][r.DF_I]);
		pids[i].kd			= (double)(_filter_coeffs[i][r.DF_D]);
		pids[i].max_int		= (double)(_filter_coeffs[i][r.DF_I_LIMIT]);
		pids[i].offset		= (double)(_filter_coeffs[i][r.DF_OFFSET]);
		pids[i].max_output	= (double)(_filter_coeffs[i][r.DF_DAC_LIMIT]);

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
		printf("\n filter_coeffs i:%d = %d ",
		i,_filter_coeffs[i][0]);
		printf(" %d",_filter_coeffs[i][1]);
		printf(" %d",_filter_coeffs[i][2]);
		printf(" %d",_filter_coeffs[i][3]);
		printf(" %d",_filter_coeffs[i][4]);
		printf(" %d",_filter_coeffs[i][5]);
		printf(" %d",_filter_coeffs[i][6]);
		printf(" %d",_filter_coeffs[i][7]);
		printf(" %d",_filter_coeffs[i][8]);
		printf(" %d",_filter_coeffs[i][9]);

		printf("\n");
 }
	return true;
}


bool MEIMotionControl::setPidsRaw(const Pid *pids)
{
    MEIResources& r = RES(system_resources);
    for (int i = 0; i < r.getJoints(); i++) 
	{
		_filter_coeffs[i][r.DF_P]			=	int(pids[i].kp) ; 
		_filter_coeffs[i][r.DF_D]			=	int(pids[i].kd) ;
		_filter_coeffs[i][r.DF_I]			=	int(pids[i].ki) ;
		_filter_coeffs[i][r.DF_I_LIMIT]		=	int(pids[i].max_int);
		_filter_coeffs[i][r.DF_OFFSET]		=	int(pids[i].offset);
		_filter_coeffs[i][r.DF_DAC_LIMIT]	=	int(pids[i].max_output);

		set_filter(i, _filter_coeffs[i]);
    }
	
	return true;
}



bool MEIMotionControl::setReferenceRaw (int axis, double ref)		//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	r._Rref_positions[axis] = ref;

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

	rc = get_error(axis, &out[axis]);
	printf("\n error on axis %d is %.1lf",axis,out[axis]);

	return true;
}


bool MEIMotionControl::getErrorsRaw(double *errs)						//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	double *out = (double *) errs;
	int i;
	long rc = 0;

	for (i = 0; i < r.getJoints(); i++)
        {
			rc = getErrorRaw(i, &out[i]);
        }

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


//function to get the value of the positions setted as references
bool MEIMotionControl::getReferenceRaw(int axis, double *ref)				
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	ref[axis] = r._Rref_positions[axis];

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
	MEIResources& r = RES(system_resources);
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
	MEIResources& r = RES(system_resources);
	if (_positionmode == true)
	{
/*		//this is needed just for velocity command
		//remember to set is back to  NO_EVENT before using position command
		//otherwise it is not going to work!
				for (int i = 0; i < r.getJoints (); i++)
        {
			set_home(i, STOP_EVENT);
		}
*/    _positionmode = false;
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
				printf("\nAxis you want to move can not work!..check positionMoveRaw function!");
				printf("\n I am going to clear_status.. if you do not agree check the code!");
				clear_status(axis);
			}

	r._Rref_positions[axis] = ref;
	int  check = move(axis, r._Rref_positions[axis], r._Rref_speeds[axis], r._Rref_acc[axis]);
//	if (check> 0)printf("\n axis %d is already doing something....the code of the action is: %d",axis,check);
	}
	else 	printf("\n _positionmode is  false, you need to set it true before starting positionMove!!");

	return true;
}

/*it sets the reference postion withou giving the command to move
if you want to move you have to call the setPositionMode function*/
bool MEIMotionControl::positionMoveRaw(const double *refs)				//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	int i;
	for (i = 0; i < r.getJoints (); i++)
	{
		int	check = axis_state(i);		
        if (ENABLED(i))
			{
				positionMoveRaw(i, refs[i]);
			}
		if (!ENABLED(i))
			{
//				printf("\nAxis you want to move can not work!..check positionMoveRaw function!");
//				printf("\n I am going to clear_status.. if you do not agree check the code!");
				clear_status(i);
				positionMoveRaw(i, refs[i]);
			}
		}
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
			//printf("\nMotion on axis %d not terminated yet!!", axis);
			ret[axis] = false;
           return true;
        }
	else 
	{
		printf("\nMotion on axis %d IS terminated!!", axis);
		ret[axis] = true;
		if((axis_state(axis) == 10)||(axis_state(axis) == 10))//if STOP EVENT or E STOP EVENT
			clear_status(axis);
		return false;
	}

}

/// check motion done, single axis.
//it returns true if the motion is terminated
bool MEIMotionControl::checkMotionDoneRaw (bool *ret)
{
	MEIResources& r = RES(system_resources);
	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
			checkMotionDoneRaw(i, ret);
        }
	return true;
}
	

bool MEIMotionControl::setRefSpeedRaw(int axis, double sp)		
{
	MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());


	r._Rref_speeds[axis] = sp;
	//da togliere
	if(r._Rref_speeds[axis]>10000)
	{
		printf("\nACHTUNG! I am slowing DOWN something is going wrong!\n check setRefSpeedRaw function!your speed was %lf",_ref_speeds[axis]);
		r._Rref_speeds[axis]=10000;
	}
		if(r._Rref_speeds[axis]<-10000)
	{
		printf("\nACHTUNG! I am slowing DOWN something is going wrong!\n check setRefSpeedRaw function!your speed was %lf",_ref_speeds[axis]);
		r._Rref_speeds[axis]=-10000;
	}
	return true;
}

bool MEIMotionControl::setRefSpeedsRaw(const double *spds)					
{
	MEIResources& r = RES(system_resources);
      int i;
	  for (i = 0; i < r.getJoints(); i++)
      {
        r._Rref_speeds[i] = spds[i];
	  	//da togliere
		if(r._Rref_speeds[i]>10000)
		{
			printf("\nACHTUNG! I am slowing DOWN something is going wrong!\n check setRefSpeedRaw function!your speed was %lf",r._Rref_speeds[i]);
			r._Rref_speeds[i]=10000;
		}
		if(r._Rref_speeds[i]<-10000)
		{
			printf("\nACHTUNG! I am slowing DOWN something is going wrong!\n check setRefSpeedRaw function!your speed was %lf",r._Rref_speeds[i]);
			r._Rref_speeds[i]=-10000;
		}
      }
	return true;
}

bool MEIMotionControl::setRefAccelerationRaw(int axis, double acc)			
{

	MEIResources& r = RES(system_resources);
	r._Rref_acc[axis] = acc;
	 return true;
}

bool MEIMotionControl::setRefAccelerationsRaw(const double *accs)	
{
    MEIResources& r = RES(system_resources);
	for (int i = 0; i < r.getJoints(); i++)
		r._Rref_acc[i] = accs[i]; 

	return true;
}



bool MEIMotionControl::getRefSpeedsRaw (double *spds)	
{
	MEIResources& r = RES(system_resources);
    for(int i = 0; i < r.getJoints(); i++)
      {
	      spds[i] = r._Rref_speeds[i];
      }

	return true;
}

bool MEIMotionControl::getRefSpeedRaw (int axis, double *spd)	//revisionata Mattia
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	spd[axis] = r._Rref_speeds[axis];
	
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

	accs[axis] = r._Rref_acc[axis];

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
	
	int check;
	if(dsp_init(PCDSP_BASE))
		return dsp_error;

	//check if the axis is ready to worl or not
	//	int check = axis_state(axis);		
	if(_positionmode == false)
		{
		 if (!ENABLED(axis))
			{
				printf("\nAxis you want to move can not work!..check positionMoveRaw function!");
				printf("\n I am going to clear_status.. if you do not agree check the code!");
				clear_status(axis);
				check = v_move(axis, sp , r._Rref_acc[axis]);
				if(check!=0)
					printf("\n v_move exit with value %d, check the manual!!", check);
			}
		 if (ENABLED (axis))
			{
			check = v_move(axis, sp , r._Rref_acc[axis]);
			if(check!=0)
			printf("\n v_move exit with value %d, check the manual!!", check);
			}

		}
	  else 	printf("\n _positionmode is not false, you need to set it false before starting velocitymove!!");

	return true;
}


inline bool MEIMotionControl::velocityMoveRaw (const double *sp)
{
	MEIResources& r = RES(system_resources);

	int i;
	for(i = 0; i < r.getJoints(); i++)
		{		
			int check	= v_move(i, sp[i] , r._Rref_acc[i]);
			if(check!=0)
					printf("\n v_move exit with value %d, check the manual!!", check);
		}
	return true;
}



int MEIMotionControl::safeVMove (double *spds, double *accs)
{
	int rc = 0;
	MEIResources& r = RES(system_resources);

	for(int i = 0; i <  r.getJoints(); i++)
	{
		if (!frames_left(i))
			rc = v_move(i, spds[i], accs[i]);
	}

	return rc;
}

bool MEIMotionControl::setEncoderRaw(int axis, double val)
{
	MEIResources& r = RES(system_resources);
    ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	r._Rzeros[axis] = double (dsp_encoder(axis)) + val;
	r._Rwinding[axis] = 0;

	return true;
}

bool MEIMotionControl::setEncodersRaw(const double *vals)
{
	MEIResources& r = RES(system_resources);
		
	int rc;

	/// this is to (re)set the encoder ref value.
	for (int i = 0; i < r.getJoints(); i++)
        {
			rc = set_position(i, vals[i]);
			r._Rzeros[i] = double(dsp_encoder(i)) + vals[i];
			r._Rwinding[i] = 0;
		}

	return true;
}

bool MEIMotionControl::resetEncoderRaw(int j)
{
	MEIResources& r = RES(system_resources);
	r._Rzeros[j] = double(dsp_encoder(j));
	printf("\n r._Rzeros[%d]=%lf",j, r._Rzeros[j]);
	r._Rwinding[j] = 0;
    return true;
}




bool MEIMotionControl::resetEncodersRaw()		
{
	MEIResources& r = RES(system_resources);

	bool  ret;
    for(int i = 0; i < r.getJoints(); i++)
		ret= resetEncoderRaw(i);
	
    return ret;
}

bool MEIMotionControl::getEncodersRaw(double *v)		//revisionata Mattia
{														
	MEIResources& r = RES(system_resources);
	bool ret;
	for (int i = 0; i < r.getJoints(); i++) 
		ret = getEncoderRaw(i, v);
	
	return true;
}

bool MEIMotionControl::getEncoderRaw(int axis, double *v) //revisionata Mattia
{
	MEIResources& r = RES(system_resources);
	int16 rc = 0;

	double tmpd;
	tmpd = double(dsp_encoder(axis));
	/// computing the actual encoder value.
	if (_sgn(tmpd) < 0 && r._R16bit_oldpos[axis] > 16384.0 && r._Rwinding[axis] == 0)
	{
		r._Rwinding[axis] = 1;
	}
	else
	if (_sgn(tmpd) > 0 && r._R16bit_oldpos[axis] < -16384.0 && r._Rwinding[axis] == 1)
	{
		r._Rwinding[axis] = 0;
	}
	else
	if (_sgn(tmpd) > 0 && r._R16bit_oldpos[axis] < -16384.0 && r._Rwinding[axis] == 0)
	{
		r._Rwinding[axis] = -1;
	}
	else
	if (_sgn(tmpd) < 0 && r._R16bit_oldpos[axis] > 16384.0 && r._Rwinding[axis] == -1)
	{
		r._Rwinding[axis] = 0;
	}

	r._R16bit_oldpos[axis] = tmpd;

	switch (r._Rwinding[axis])
	{
		case 1:
 			tmpd = 65535.0 + tmpd ;//variazione per il wrapper
			break;

		case -1:
			tmpd = -65536.0 + tmpd ;//variazione per il wrapper
			break;

		case 0:
			tmpd = tmpd;//variazione per il wrapper
			break;
	}

	v[axis] =tmpd;

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

	int level_amp	= set_amp_enable_level(axis, true);		//set the working ampli level at TRUE = HIGH
	printf("\nset the working ampli level at TRUE = HIGH");
	int fault_level = set_amp_fault_level(axis, false);

	while(!motion_done(axis));								// required otherwise controller_run may fail
	int cs = clear_status(axis);
	int rc = controller_run(axis);

	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	

	int amp = enable_amplifier(axis);						//set the state of the amp to the 
	short ampli;
	int level = get_amp_enable_level(axis, &ampli);
	printf("\nAmp%d enable  state : %d",axis,ampli);

	if(level == 1)
	{
		_amplifiers = true;
		printf("\nAmpli: %d is ON at level %d",axis,level);
	}

	return true;
}


bool MEIMotionControl::getCurrentsRaw(double *cs)	
{
	return NOT_IMPLEMENTED_YET("getCurrentRaw");
}


bool MEIMotionControl::getCurrentRaw(int axis, double *c)
{
	    return NOT_IMPLEMENTED_YET("getCurrentRaw");
}

bool MEIMotionControl::setMaxCurrentRaw(int axis, double v)
{
	    return NOT_IMPLEMENTED_YET("setMaxCurrentRaw");
}

bool MEIMotionControl::getMaxCurrentRaw(int axis, double v)
{
	    return NOT_IMPLEMENTED_YET("getMaxCurrentRaw");
}

//before starting the calibration you have to move each axis of the arm 
//they have to stay as much as possible far away from their limit..
//this function looks for a median point between two special encoder-steps
//the problem is that if the initial position is between home-index and the
//hardware limit of the axis you will see an error in calibration.

bool MEIMotionControl::calibrateRaw(int axis, double zero)
{
    MEIResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	int check;


//check if everything is OK
	check = clear_status(axis);

	while (!motion_done(axis))
	check = controller_run(axis);
	check = enable_amplifier(axis);

	
	printf(("\nCalibration routine started.\n"));
	if (! (_alreadyinint && _amplifiers) )
		{
			printf(("\nArm is not initialized or power is down.\n"));
			return false;
		}
//disable the joint limits of this axis
	setLimitsRaw(axis,-100000, 100000);
	printf("\n The joint-limits of axis %i are now disabled for calibration...pay attention on arm movement!!",axis);


	bool ret[6] =		  {		0,		0,		0,		0,		0,		0};
	double speeds1[6]	= { 1000.0,  1000.0,  1000.0,  1000.0,  1000.0,  1000.0};
	double speeds2[6]	= {-1000.0, -1000.0, -1000.0, -1000.0, -1000.0, -1000.0};
	double acc[6]		= {10000.0, 10000.0, 10000.0, 10000.0, 10000.0, 10000.0};
	double pos1[6]		= {   0.0,    0.0,    0.0,    0.0,    0.0,	  0.0};
	double pos2[6]		= {   0.0,    0.0,    0.0,    0.0,    0.0,    0.0};
	double newHome[6]	= {   0.0,    0.0,    0.0,    0.0,    0.0,    0.0};
	double debug[6]		= {   0.0,    0.0,    0.0,    0.0,    0.0,    0.0};


	for (int i = 0; i < r.getJoints(); i++)
        {
			clear_status(i);
			controller_run(i);
			if(i != axis) 
				{
					speeds1[i]	= 0.0;
					speeds2[i]	= 0.0;
					acc[i]		= 0.0;
				}
		}

		// gather my current position. this is the HOME position from now on
		check = getEncoderRaw(axis,r._RposHome);

		printf(("\nGathering first index...\n"));
		// find first index
		check = set_home_index_config(axis,INDEX_ONLY);
		//the level of HOMe index is LOW
		check = set_home_level(axis, FALSE);
		
		//this is needed just for velocity command
		//remember to set is back to  NO_EVENT before using position command
		//otherwise it is not going to work!
		check = set_home(axis, STOP_EVENT);

		//check if the axis is ready to work or not
		check = axis_state(axis);		
		if(check!=0)
		{
			printf("\n some action did not exit in the right way for the %d axis \nI am going to reset the STOP",axis);
			check = controller_run(axis);
			check = clear_status(axis);
			if(check!=0)
			printf("\n some action did not exit AGAIN in the right way for the %d axis \nplease TAKE a LOOK!!!",axis);
		}

		check = setVelocityMode();
		//select the acceleration and the velocity you prefer
		check = setRefAccelerationRaw (axis,acc[axis]);
		check = velocityMoveRaw(axis, speeds1[axis]);

		while(checkMotionDoneRaw(axis,ret))
		{
//			printf("\n some axis still continue to work!!!!");
//			printf(".");
								}

		//load the first enc value in pos1
		check = getEncoderRaw(axis,pos1);
//		printf("\nfirst home index is in %lf",pos1[axis]);
		check = clear_status(axis);
		if(check>1)
		{
			("\nclear_status(%d) exit with %d I am going to call axis_source",axis,check);
			check = axis_source(axis);
			("\naxis_source(%d) exit with %d ",axis,check);

		}

		printf(("done.\n"));

		// go back to HOME position
		printf("\nGoing back home...\n");
		check = axis_state(axis);
		
		if(check!=0)
		{
			printf("\n some action did not exit in the right way for the %d axis \nI am going to reset the STOP",axis);
			check = controller_run(axis);
			check = clear_status(axis);
			if(check!=0)
			printf("\n some action did not exit AGAIN in the right way for the %d axis \nplease TAKE a LOOK!!!",axis);
		}

		//REMEMBER to set NO_EVENT otherwise it is not going to move!!
		check = set_home(axis, NO_EVENT);

		check = setPositionMode();
		check = setRefSpeedRaw(axis,speeds2[axis]);
		check = setRefAccelerationRaw(axis,acc[axis]);
		check = positionMoveRaw(axis,r._RposHome[axis]);
		while (!motion_done(axis));
		check = getEncoderRaw(axis,debug);
		

		printf(("\nGathering second index...\n"));
		check = axis_state(axis);
		if(check!=0)
		{
			printf("\n some action did not exit in the right way for the %d axis \nI am going to reset the STOP",axis);
			check = controller_run(axis);
			check = clear_status(axis);
			if(check!=0)
			printf("\n some action did not exit AGAIN in the right way for the %d axis \nplease TAKE a LOOK!!!",axis);
		}


		// find second index
		check = set_home_index_config(axis,INDEX_ONLY);
		check = set_home_level(axis, FALSE);
		check = set_home(axis, STOP_EVENT);
		check = setRefAccelerationRaw (axis,acc[axis]);
		check = setVelocityMode();
		check = velocityMoveRaw(axis,speeds2[axis]);
		while(checkMotionDoneRaw(axis,ret))

		//save the second value in pos2
		check = getEncoderRaw(axis,pos2);

		check = clear_status(axis);
		printf(("done.\n"));

		// compute new HOME position
		newHome[axis] = (pos1[axis]+pos2[axis])/2;
		r._RposHome[axis]=newHome[axis];

		// go back to HOME CALIBRATED position
		printf("\nGoing back home...\n");

		//REMEMBER to set NO_EVENT otherwise it is not going to move!!
		check = set_home(axis, NO_EVENT);
		check = setPositionMode();
		check = setRefSpeedRaw(axis,speeds1[axis]);
		check = setRefAccelerationRaw(axis,acc[axis]);
		check = positionMoveRaw(axis,newHome[axis]);

		while(checkMotionDoneRaw(axis,ret))

		mei_checksum();
		check = getEncoderRaw(axis,r._Rzeros);
		r._RposHome[axis]=r._Rzeros[axis];

		return true;
}

bool MEIMotionControl::doneRaw(int axis)
{
	MEIResources& r = RES(system_resources);
	
	bool ret[6] =		  {		0,		0,		0,		0,		0,		0};

	checkMotionDoneRaw(axis, ret);
	
	if (ret[axis]==1)
	return true;
	else
		return false;

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

	for (i = 0; i < r.getJoints(); i++)
        {
            st[i] = get_amp_enable(i,  &value);
//			printf("\n Amp %d enable state = %d",i,value);
        }

	return true;
	
}

bool MEIMotionControl::getAmpStatusRaw(int i, int *st)				
{

	MEIResources& r = RES(system_resources);
	short value;

    if (i<r.getJoints())
        st[i]=get_amp_enable(i, &value);

    *st=value;

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
		printf("\n axis_source on axis %d esxited with code %d",axis,value2);
		return false;
	}
	else return true;
	
}


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
