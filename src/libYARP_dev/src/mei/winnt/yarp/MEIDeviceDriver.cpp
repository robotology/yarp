/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///        - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of                            ///
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
/// $Id: MEIDeviceDriver.cpp,v 1.1 2007-03-22 14:08:25 claudio72 Exp $
///


/// general purpose stuff.
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>




#include <yarp/os/Searchable.h>
#include "yarp/MEIDeviceDriver.h"
/// get the message types from the DSP code.
//#include "messages.h"
using namespace yarp::dev;



// MEI
#define MEI_WINNT
#include "../dd_orig/include/pcdsp.h"
#define MEI_MSVC40		
#include "../dd_orig/include/medexp.h"		
#include "../dd_orig/include/idsp.h"
//////


/*
bool NOT_YET_IMPLEMENTED(const char *txt)
{
    ACE_OS::fprintf(stderr, "%s not yet implemented for MEIDeviceDriver\n", txt);

    return false;
}
*/
MEIDeviceDriver::MEIDeviceDriver()/* :
    ImplementPositionControl<MEIDeviceDriver, IPositionControl>(this),
    ImplementVelocityControl<MEIDeviceDriver, IVelocityControl>(this),
    ImplementPidControl<MEIDeviceDriver, IPidControl>(this),
    ImplementEncoders<MEIDeviceDriver, IEncoders>(this),
    ImplementControlCalibration<MEIDeviceDriver, IControlCalibration>(this),
    ImplementAmplifierControl<MEIDeviceDriver, IAmplifierControl>(this),
    ImplementControlLimits<MEIDeviceDriver, IControlLimits>(this)*/
{

/*	// fill function pointer table
	m_cmds[CMDSetSpeed] = &YARPMEIDeviceDriver::setSpeed;
	m_cmds[CMDSetAcceleration] = &YARPMEIDeviceDriver::setAcceleration;
	m_cmds[CMDSetPosition] = &YARPMEIDeviceDriver::setPosition;
	m_cmds[CMDSetPID] = &YARPMEIDeviceDriver::setPid;
	m_cmds[CMDGetPosition] = &YARPMEIDeviceDriver::getPosition;
	m_cmds[CMDSetPortValue] = &YARPMEIDeviceDriver::setOutputPort;
	m_cmds[CMDGetPortValue] = &YARPMEIDeviceDriver::getOutputPort;
	m_cmds[CMDSetOutputBit] = &YARPMEIDeviceDriver::setOutputBit;
	m_cmds[CMDClearOutputBit] = &YARPMEIDeviceDriver::clearOutputBit;
	m_cmds[CMDSetOffset] = &YARPMEIDeviceDriver::setOffset;
	m_cmds[CMDSetOffsets] = &YARPMEIDeviceDriver::setOffsets;
	m_cmds[CMDSetSpeeds] = &YARPMEIDeviceDriver::setSpeeds;
	m_cmds[CMDSetAccelerations] = &YARPMEIDeviceDriver::setAccelerations;
	m_cmds[CMDSetPositions] = &YARPMEIDeviceDriver::setPositions;
	m_cmds[CMDGetPositions] = &YARPMEIDeviceDriver::getPositions;
	m_cmds[CMDDefinePositions] = &YARPMEIDeviceDriver::definePositions;
	m_cmds[CMDDefinePosition] = &YARPMEIDeviceDriver::definePosition;
	m_cmds[CMDStopAxes] = &YARPMEIDeviceDriver::stopAxes;
	//m_cmds[CMDReadSwitches] = &YARPMEIDeviceDriver::readSwitches;
	m_cmds[CMDGetSpeeds] = &YARPMEIDeviceDriver::getSpeeds;
	m_cmds[CMDGetRefSpeeds] = &YARPMEIDeviceDriver::getRefSpeeds;
	m_cmds[CMDGetRefAccelerations] = &YARPMEIDeviceDriver::getRefAccelerations;
	m_cmds[CMDGetRefPositions] = &YARPMEIDeviceDriver::getRefPositions;
	m_cmds[CMDGetPID] = &YARPMEIDeviceDriver::getPid;
	m_cmds[CMDGetTorques] = &YARPMEIDeviceDriver::getTorques;
	m_cmds[CMDSetIntegratorLimits] = &YARPMEIDeviceDriver::setIntLimits;
	m_cmds[CMDSetTorqueLimits] = &YARPMEIDeviceDriver::setTorqueLimits;
	m_cmds[CMDSetTorqueLimit] = &YARPMEIDeviceDriver::setTorqueLimit;
	m_cmds[CMDSetIntegratorLimit] = &YARPMEIDeviceDriver::setIntLimit;
	m_cmds[CMDGetTorqueLimit] = &YARPMEIDeviceDriver::getTorqueLimit;
	m_cmds[CMDGetTorqueLimits] = &YARPMEIDeviceDriver::getTorqueLimits;
	m_cmds[CMDGetPIDErrors] = &YARPMEIDeviceDriver::getErrors;
	//m_cmds[CMDReadInput] = &YARPMEIDeviceDriver::readInput;
	m_cmds[CMDInitPortAsInput] = &YARPMEIDeviceDriver::initPortAsInput;
	m_cmds[CMDInitPortAsOutput] = &YARPMEIDeviceDriver::initPortAsOutput;
	m_cmds[CMDSetAmpEnableLevel] = &YARPMEIDeviceDriver::setAmpEnableLevel;
	m_cmds[CMDSetAmpEnable] = &YARPMEIDeviceDriver::setAmpEnable;
	m_cmds[CMDDisableAmp] = &YARPMEIDeviceDriver::disableAmp; 
	m_cmds[CMDEnableAmp] = &YARPMEIDeviceDriver::enableAmp;
	m_cmds[CMDControllerIdle] = &YARPMEIDeviceDriver::controllerIdle;
	m_cmds[CMDControllerRun] = &YARPMEIDeviceDriver::controllerRun;
	m_cmds[CMDClearStop] = &YARPMEIDeviceDriver::clearStop;
	m_cmds[CMDSetPositiveLevel] = &YARPMEIDeviceDriver::setPositiveLevel;
	m_cmds[CMDSetNegativeLevel] = &YARPMEIDeviceDriver::setNegativeLevel;
	m_cmds[CMDSetPositiveLimit] = &YARPMEIDeviceDriver::setPositiveLimit;
	m_cmds[CMDSetNegativeLimit] = &YARPMEIDeviceDriver::setNegativeLimit;
	m_cmds[CMDVMove] = &YARPMEIDeviceDriver::safeVMove;
	m_cmds[CMDSetCommands] = &YARPMEIDeviceDriver::setCommands;
	m_cmds[CMDSetCommand] = &YARPMEIDeviceDriver::setCommand;
	m_cmds[CMDCheckMotionDone] = &YARPMEIDeviceDriver::checkMotionDone;
	m_cmds[CMDWaitForMotionDone] = &YARPMEIDeviceDriver::waitForMotionDone;
	m_cmds[CMDSetHomingBehavior] = &YARPMEIDeviceDriver::setHomeProcedure;
	m_cmds[CMDGetAnalogChannel] = &YARPMEIDeviceDriver::readAnalog;
	m_cmds[CMDSetAxisAnalog] = &YARPMEIDeviceDriver::setAxisAnalog;
*/
	
	_events = new int[CBNEvents];
	 for(int i = 0; i<CBNEvents; i++)
		_events[i] = NO_EVENT;
	
	_events[CBNoEvent] = NO_EVENT;
	_events[CBStopEvent] = STOP_EVENT;
	_events[CBEStopEvent] = E_STOP_EVENT;
	_events[CBAbortEvent] = ABORT_EVENT;
	
}

MEIDeviceDriver::~MEIDeviceDriver()
{
	delete [] _events;
}

bool MEIDeviceDriver::open(yarp::os::Searchable& config)
{

	_njoints = config.find("Joints").asInt();
    MEIOpenParameters params;
    params.nj = config.find("Joints").asInt();

	
	if (dsp_init(PCDSP_BASE))
		return dsp_error;


	// temporarily removed
	int16 rc = 0;

//	MEIOpenParameters *p = (MEIOpenParameters *)d;

//	_njoints = p->nj;
	_njoints =params.nj;
//	rc = dsp_init(p->meiPortAddr);	// init
	rc = dsp_init(params.meiPortAddr);	// init
	rc = dsp_reset();				// reset

	

	_ref_speeds = new double [_njoints];
	ACE_ASSERT (_ref_speeds != NULL);
	_ref_accs = new double [_njoints];
	ACE_ASSERT (_ref_accs != NULL);
	_ref_positions = new double [_njoints];
	ACE_ASSERT (_ref_positions != NULL);
	_all_axes = new int16[_njoints];
	ACE_ASSERT (_all_axes != NULL);

	int i;
	for(i = 0; i < _njoints; i++)
		_all_axes[i] = i;

	_filter_coeffs = new int16* [_njoints];
	ACE_ASSERT (_filter_coeffs != NULL);
	for(i = 0; i < _njoints; i++)
	{
		_filter_coeffs[i] = new int16 [COEFFICIENTS];
		ACE_ASSERT (_filter_coeffs[i] != NULL);
	}

	_dsp_rate = dsp_sample_rate();
	
	_winding = new int16[_njoints];
	ACE_ASSERT (_winding != NULL);
	memset (_winding, 0, sizeof(int16) * _njoints);

	_16bit_oldpos = new double[_njoints];
	ACE_ASSERT (_16bit_oldpos != NULL);
	memset (_16bit_oldpos, 0, sizeof(double) * _njoints);

	_position_zero = new double[_njoints];
	ACE_ASSERT (_position_zero != NULL);
	memset (_position_zero, 0, sizeof(double) * _njoints);

//	int mask = p->ioPorts;
	int mask = params.ioPorts;
	for (i = 0; i < MAX_PORTS; i++)
	{
		if (mask & 0x1)
			init_io (i, IO_OUTPUT);
		else
			init_io (i, IO_INPUT);

		mask >>= 1;
	}

	return rc;
}

bool MEIDeviceDriver::close() 
{
	int16 rc = 0;

	if (_ref_speeds != NULL) delete [] _ref_speeds;
	if (_ref_accs != NULL) delete [] _ref_accs;
	if (_ref_positions != NULL) delete [] _ref_positions;
	
	for(int i = 0; i < _njoints; i++)
		if (_filter_coeffs[i] != NULL) delete [] _filter_coeffs[i];

	if (_filter_coeffs != NULL) delete [] _filter_coeffs;
	if (_all_axes != NULL) delete [] _all_axes;

	if (_winding != NULL) delete[] _winding;
	if (_16bit_oldpos != NULL) delete[] _16bit_oldpos;

	_njoints = 0;
	
	return rc;
}
/*
///////////// from ControlBoardInterfaces
    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
// return the number of controlled axes.
//bool MEIDeviceDriver::getAxes(int *ax)
//{
//	EsdCanResources& r = RES(system_resources);
//    *ax = r.getJoints();
//
//    return true;
//}

    /**
     * Reset encoder, single joint. Set the encoder value to zero 
     * @param j encoder number
     * @return true/false
     */
//    bool MEIDeviceDriver:: resetEncoderRaw(int j){return true;}

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
//    bool MEIDeviceDriver:: resetEncodersRaw(){return true;}

    /**
     * Set the value of the encoder for a given joint. 
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
//    bool MEIDeviceDriver:: setEncoderRaw(int j, double val){return true;}

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
//    bool MEIDeviceDriver:: setEncodersRaw(const double *vals){return true;}

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
//    bool MEIDeviceDriver:: getEncoderRaw(int j, double *v){return true;}

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
//    bool MEIDeviceDriver:: getEncodersRaw(double *encs){return true;}

    /**
     * Read the instantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
//    bool MEIDeviceDriver:: getEncoderSpeedRaw(int j, double *sp){return true;}

    /**
     * Read the instantaneous acceleration of an axis.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
//    bool MEIDeviceDriver:: getEncoderSpeedsRaw(double *spds){return true;}
    
    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
//    bool MEIDeviceDriver:: getEncoderAccelerationRaw(int j, double *spds){return true;}

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens. 
     */
//    bool MEIDeviceDriver:: getEncoderAccelerationsRaw(double *accs){return true;}



///////////////

/*
// LATER: can be optimized.
bool MEIDeviceDriver::setPidRaw (int axis, const Pid &pid)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	_writeWord16 (CAN_SET_P_GAIN, axis, S_16(pid.kp));
	_writeWord16 (CAN_SET_D_GAIN, axis, S_16(pid.kd));
	_writeWord16 (CAN_SET_I_GAIN, axis, S_16(pid.ki));
	_writeWord16 (CAN_SET_ILIM_GAIN, axis, S_16(pid.max_int));
	_writeWord16 (CAN_SET_OFFSET, axis, S_16(pid.offset));
	_writeWord16 (CAN_SET_SCALE, axis, S_16(pid.scale));
	_writeWord16 (CAN_SET_TLIM, axis, S_16(pid.max_output));

	return true;
}

bool MEIDeviceDriver::getPidRaw (int axis, Pid *out)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	short s;

	_readWord16 (CAN_GET_P_GAIN, axis, s); out->kp = double(s);
	_readWord16 (CAN_GET_D_GAIN, axis, s); out->kd = double(s);
	_readWord16 (CAN_GET_I_GAIN, axis, s); out->ki = double(s);
	_readWord16 (CAN_GET_ILIM_GAIN, axis, s); out->max_int = double(s);
	_readWord16 (CAN_GET_OFFSET, axis, s); out->offset= double(s);
	_readWord16 (CAN_GET_SCALE, axis, s); out->scale = double(s);
	_readWord16 (CAN_GET_TLIM, axis, s); out->max_output = double(s);

	return true;
}

bool MEIDeviceDriver::getPidsRaw (Pid *out)
{
    EsdCanResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
            short s;
            _readWord16 (CAN_GET_P_GAIN, i, s); out[i].kp = double(s);
            _readWord16 (CAN_GET_D_GAIN, i, s); out[i].kd = double(s);
            _readWord16 (CAN_GET_I_GAIN, i, s); out[i].ki = double(s);
            _readWord16 (CAN_GET_ILIM_GAIN, i, s); out[i].max_int = double(s);
            _readWord16 (CAN_GET_OFFSET, i, s); out[i].offset= double(s);
            _readWord16 (CAN_GET_SCALE, i, s); out[i].scale = double(s);
            _readWord16 (CAN_GET_TLIM, i, s); out[i].max_output = double(s);
        }

	return true;
}


bool MEIDeviceDriver::setPidsRaw(const Pid *pids)
{
    EsdCanResources& r = RES(system_resources);

	int i;
    for (i = 0; i < r.getJoints(); i++) {
	    _writeWord16 (CAN_SET_P_GAIN, i, S_16(pids[i].kp));
	    _writeWord16 (CAN_SET_D_GAIN, i, S_16(pids[i].kd));
	    _writeWord16 (CAN_SET_I_GAIN, i, S_16(pids[i].ki));
	    _writeWord16 (CAN_SET_ILIM_GAIN, i, S_16(pids[i].max_int));
	    _writeWord16 (CAN_SET_OFFSET, i, S_16(pids[i].offset));
	    _writeWord16 (CAN_SET_SCALE, i, S_16(pids[i].scale));
	    _writeWord16 (CAN_SET_TLIM, i, S_16(pids[i].max_output));
    }

	return true;
}

/// cmd is a SingleAxis poitner with 1 double arg
bool MEIDeviceDriver::setReferenceRaw (int j, double ref)
{
	const int axis = j;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_COMMAND_POSITION, axis, S_32(ref));
}

/// cmd is an array of double (LATER: to be optimized).
bool MEIDeviceDriver::setReferencesRaw (const double *refs)
{
	EsdCanResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
            if (_writeDWord (CAN_SET_COMMAND_POSITION, i, S_32(refs[i])) != true)
                return false;
        }

	return true;
}

bool MEIDeviceDriver::setErrorLimitRaw(int j, double limit)
{
    return NOT_YET_IMPLEMENTED("setErrorLimit");
}

bool MEIDeviceDriver::setErrorLimitsRaw(const double *limit)
{
    return NOT_YET_IMPLEMENTED("setErrorLimits");
}

bool MEIDeviceDriver::getErrorRaw(int axis, double *err)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
    _mutex.wait();
	*(err) = double(r._bcastRecvBuffer[axis]._position_error);
    _mutex.post();
	return true;
}

bool MEIDeviceDriver::getErrorsRaw(double *errs)
{
	EsdCanResources& r = RES(system_resources);
	int i;
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            errs[i] = double(r._bcastRecvBuffer[i]._position_error);
        }
    _mutex.post();
	return true;
}

bool MEIDeviceDriver::getOutputRaw(int axis, double *out)
{
   	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
    _mutex.wait();
	*(out) = double(r._bcastRecvBuffer[axis]._pid_value);
    _mutex.post();
	return true;
}

bool MEIDeviceDriver::getOutputsRaw(double *outs)
{
	EsdCanResources& r = RES(system_resources);
	int i;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            outs[i] = double(r._bcastRecvBuffer[i]._pid_value);
        }

    _mutex.post();
	return true;
}

bool MEIDeviceDriver::getReferenceRaw(int j, double *ref)
{
	const int axis = j;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	int value = 0;
	if (_readDWord (CAN_GET_DESIRED_POSITION, axis, value) == true)
		*ref = double (value);
	else
		return false;

	return true;
}

bool MEIDeviceDriver::getReferencesRaw(double *ref)
{
    return _readDWordArray(CAN_GET_DESIRED_POSITION, ref);
}

bool MEIDeviceDriver::getErrorLimitRaw(int j, double *err)
{
    return NOT_YET_IMPLEMENTED("getErrorLimit");
}

bool MEIDeviceDriver::getErrorLimitsRaw(double *errs)
{
    return NOT_YET_IMPLEMENTED("getErrorLimits");
}

bool MEIDeviceDriver::resetPidRaw(int j)
{
    return NOT_YET_IMPLEMENTED("resetPid");
}

bool MEIDeviceDriver::enablePidRaw(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);

}

bool MEIDeviceDriver::setOffsetRaw(int axis, double v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeWord16 (CAN_SET_OFFSET, axis, S_16(v));

}


bool MEIDeviceDriver::disablePidRaw(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

bool MEIDeviceDriver::setPositionMode()
{
    return NOT_YET_IMPLEMENTED("setPositionModeRaw");
}

bool MEIDeviceDriver::setVelocityMode()
{
    return NOT_YET_IMPLEMENTED("setVelocityModeRaw");
}

bool MEIDeviceDriver::positionMoveRaw(int axis, double ref)
{
    /// prepare can message.
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED (axis))
        {
            // still fills the _ref_position structure.
            _ref_positions[axis] = ref;
            return true;
        }

	_mutex.wait();

	r.startPacket();
	r.addMessage (CAN_POSITION_MOVE, axis);

	_ref_positions[axis] = ref;
	*((int*)(r._writeBuffer[0].data+1)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r._writeBuffer[0].data+5)) = S_16(_ref_speeds[axis]);			/// speed
	r._writeBuffer[0].len = 7;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	return true;
}

bool MEIDeviceDriver::positionMoveRaw(const double *refs)
{
 	EsdCanResources& r = RES(system_resources);
	int i;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints (); i++)
        {
            if (ENABLED(i))
                {
                    r.addMessage (CAN_POSITION_MOVE, i);
                    const int j = r._writeMessages - 1;
                    _ref_positions[i] = refs[i];
                    *((int*)(r._writeBuffer[j].data+1)) = S_32(_ref_positions[i]);		/// pos
                    *((short*)(r._writeBuffer[j].data+5)) = S_16(_ref_speeds[i]);		/// speed
                    r._writeBuffer[j].len = 7;
                }
            else
                {
                    _ref_positions[i] = refs[i];
                }
        }

	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	return true;   
}

bool MEIDeviceDriver::relativeMoveRaw(int j, double delta)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

bool MEIDeviceDriver::relativeMoveRaw(const double *deltas)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

/// check motion done, single axis.
bool MEIDeviceDriver::checkMotionDoneRaw(int axis, bool *ret)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	short value;

    if (!_readWord16 (CAN_MOTION_DONE, axis, value))
        {
            *ret=false;
            return false;
        }
 
    *ret= (value!=0);

    return true;
}

/// cmd is a pointer to a bool
bool MEIDeviceDriver::checkMotionDoneRaw (bool *ret)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
        {
            if (ENABLED(i))
                {
                    r.addMessage (CAN_MOTION_DONE, i);
                }
        }

	if (r._writeMessages < 1)
		return false;

	_writerequested = true;
	_noreply = false;
	_mutex.post();

	_done.wait();

	if (!r.getErrorStatus())
    	return false;
	
	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
        {
            if (ENABLED(i))
                {
                    CMSG& m = r._replyBuffer[j];
                    if (m.id != 0xffff)
                        {
                            value = *((short *)(m.data+1));
                            if (!value)
                                {
                                    *ret=false;
                                    return true;
                                }
                        }
                    j++;
                }
        }

    *ret=true;
	return true;
}

bool MEIDeviceDriver::setRefSpeedRaw(int axis, double sp)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	sp /= 10.0; // encoder ticks per ms
	_ref_speeds[axis] = sp;
	return true;
   
	//const short s = S_16(_ref_speeds[axis]);
	//return _writeWord16(CAN_SET_DESIRED_VELOCITY, axis, s);
}

bool MEIDeviceDriver::setRefSpeedsRaw(const double *spds)
{
	EsdCanResources& r = RES(system_resources);
	ACE_OS::memcpy(_ref_speeds, spds, sizeof(double) * r.getJoints());
	int i;
	for (i = 0; i < r.getJoints(); i++)
		_ref_speeds[i] /= 10.0;

	/*
      int i;
      for (i = 0; i < r.getJoints(); i++)
      {
      _ref_speeds[i] = spds[i];
      if (!_writeWord16 (CAN_SET_DESIRED_VELOCITY, i, S_16(_ref_speeds[i])))
      return false;
      }
	*/
/*	return true;
}

bool MEIDeviceDriver::setRefAccelerationRaw(int axis, double acc)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

*/	/*
	 *Acceleration is expressed in imp/s^2. 
	 *We divided it by 1000^2 to express 
	 *it in imp/ms^2
	 */
/*	acc /= 1000.0;
	acc /= 1000.0;
	_ref_accs[axis] = acc;
	const short s = S_16(_ref_accs[axis]);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}

bool MEIDeviceDriver::setRefAccelerationsRaw(const double *accs)
{
    EsdCanResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
*/			/*
			 *Acceleration is expressed in imp/s^2. 
			 *We divided it by 1000^2 to express 
		     *it in imp/ms^2
			 */
/*			double acc;
			acc = accs[i]/1000.0;
			acc /= 1000.0;
            _ref_accs[i] = acc / 1000.0;

            if (!_writeWord16 (CAN_SET_DESIRED_ACCELER, i, S_16(_ref_accs[i])))
                return false;
        }

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool MEIDeviceDriver::getRefSpeedsRaw (double *spds)
{
	EsdCanResources& r = RES(system_resources);
*/	/*
      int i;
      short value = 0;
      for(i = 0; i < r.getJoints(); i++)
      {
      if (_readWord16 (CAN_GET_DESIRED_VELOCITY, i, value))
      {
      _ref_speeds[i] = double (value);
      spds[i] = _ref_speeds[i];
      } 
      else
      return false;
      }
	*/
/*	ACE_OS::memcpy(spds, _ref_speeds, sizeof(double) * r.getJoints());
	int i;
	for (i = 0; i < r.getJoints(); i++)
		spds[i] *= 10.0;

	return true;
}

bool MEIDeviceDriver::getRefSpeedRaw (int axis, double *spd)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	EsdCanResources& r = RES(system_resources);
	*spd = _ref_speeds[axis] * 10.0;
	
*/	/*
      short value = 0;
      if (_readWord16 (CAN_GET_DESIRED_VELOCITY, axis, value))
      {
      _ref_speeds[axis] = double (value);
      *spd = _ref_speeds[axis];
      } 
      else
      return false;
	*/
/*	
	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool MEIDeviceDriver::getRefAccelerationsRaw (double *accs)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
        {
            if (_readWord16 (CAN_GET_DESIRED_ACCELER, i, value) == true) {
                _ref_accs[i] = accs[i] = double (value);
                accs[i] *= 1000.0;
				accs[i] *= 1000.0;
            }
            else
                return false;
        }

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool MEIDeviceDriver::getRefAccelerationRaw (int axis, double *accs)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	short value = 0;

    if (_readWord16 (CAN_GET_DESIRED_ACCELER, axis, value) == true)
        {
            _ref_accs[axis] = double (value);
            *accs = double(value) * 1000.0 * 1000.0;
        }
	else
		return false;

	return true;
}

bool MEIDeviceDriver::stopRaw(int j)
{
    return NOT_YET_IMPLEMENTED("stop");
}

bool MEIDeviceDriver::stopRaw()
{
    return NOT_YET_IMPLEMENTED("stop");
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool MEIDeviceDriver::velocityMoveRaw (int axis, double sp)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	
	_mutex.wait();
	r.startPacket();

	if (ENABLED (axis))
        {
            r.addMessage (CAN_VELOCITY_MOVE, axis);
            const int j = r._writeMessages - 1;
            _command_speeds[axis] = sp / 1000.0;

			*((short*)(r._writeBuffer[j].data+1)) = S_16(16*_command_speeds[axis]);	/// speed
			
			if (16*_ref_accs[axis]>1)
				*((short*)(r._writeBuffer[j].data+3)) = S_16(16*_ref_accs[axis]);		/// accel
			else
				*((short*)(r._writeBuffer[j].data+3)) = S_16(1);

            r._writeBuffer[j].len = 5;
        }
	else
        {
            _command_speeds[axis] = sp / 1000.0;
        }

	_writerequested = true;
	_noreply = true;

	_mutex.post();

	_done.wait();

	return true;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool MEIDeviceDriver::velocityMoveRaw (const double *sp)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	int i;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
        {
            if (ENABLED (i))
                {
                    r.addMessage (CAN_VELOCITY_MOVE, i);
                    const int j = r._writeMessages - 1;
                    _command_speeds[i] = sp[i] / 1000.0;

					*((short*)(r._writeBuffer[j].data+1)) = S_16(16*_command_speeds[i]);	/// speed
			
					if (16*_ref_accs[i]>1)
						*((short*)(r._writeBuffer[j].data+3)) = S_16(16*_ref_accs[i]);		/// accel
					else
						*((short*)(r._writeBuffer[j].data+3)) = S_16(1);

                    r._writeBuffer[j].len = 5;
                }
            else
                {
                    _command_speeds[i] = sp[i] / 1000.0;
                }
        }

	_writerequested = true;
	_noreply = true;

	_mutex.post();

	_done.wait();

	return true;
}

bool MEIDeviceDriver::setEncoderRaw(int j, double val)
{
	const int axis = j;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(val));
}

bool MEIDeviceDriver::setEncodersRaw(const double *vals)
{
	EsdCanResources& r = RES(system_resources);
	
	int i;
	for (i = 0; i < r.getJoints(); i++)
        {
            if (_writeDWord (CAN_SET_ENCODER_POSITION, i, S_32(vals[i])) != true)
                return false;
        }

	return true;
}

bool MEIDeviceDriver::resetEncoderRaw(int j)
{
    return setEncoderRaw(j, 0);
}

bool MEIDeviceDriver::resetEncodersRaw()
{
    int n=RES(system_resources).getJoints();
    double *tmp = new double [n];
	ACE_ASSERT (tmp != NULL);

    for(int i=0;i<n;i++)
        tmp[i]=0;

    bool ret= setEncodersRaw(tmp);

    delete [] tmp;

    return ret;
}

bool MEIDeviceDriver::getEncodersRaw(double *v)
{
	EsdCanResources& r = RES(system_resources);
	int i;
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++) {
		v[i] = double(r._bcastRecvBuffer[i]._position);
	}
    _mutex.post();
	return true;
}

bool MEIDeviceDriver::getEncoderRaw(int axis, double *v)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

    _mutex.wait();
	*v = double(r._bcastRecvBuffer[axis]._position);
    _mutex.post();

	return true;
}

bool MEIDeviceDriver::getEncoderSpeedsRaw(double *v)
{
  	return NOT_YET_IMPLEMENTED("getEncoderSpeeds");
}

bool MEIDeviceDriver::getEncoderSpeedRaw(int j, double *v)
{
    return NOT_YET_IMPLEMENTED("getEncoderSpeed");
}

bool MEIDeviceDriver::getEncoderAccelerationsRaw(double *v)
{
  	return NOT_YET_IMPLEMENTED("getEncoderAccs");
}

bool MEIDeviceDriver::getEncoderAccelerationRaw(int j, double *v)
{
    return NOT_YET_IMPLEMENTED("getEncoderAcc");
}

bool MEIDeviceDriver::disableAmpRaw(int axis)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

bool MEIDeviceDriver::enableAmpRaw(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

// bcast
bool MEIDeviceDriver::getCurrentsRaw(double *cs)
{
    EsdCanResources& r = RES(system_resources);
	int i;
	
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            cs[i] = double(r._bcastRecvBuffer[i]._current);
        }

    _mutex.post();
	return true;
}

// bcast currents
bool MEIDeviceDriver::getCurrentRaw(int axis, double *c)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.wait();
	*c = double(r._bcastRecvBuffer[axis]._current);
    _mutex.post();

	return true;
}

bool MEIDeviceDriver::setMaxCurrentRaw(int axis, double v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_CURRENT_LIMIT, axis, S_32(v));
}

bool MEIDeviceDriver::calibrateRaw(int axis, double p)
{
    return _writeWord16 (CAN_CALIBRATE_ENCODER, axis, S_16(p));
}

bool MEIDeviceDriver::doneRaw(int axis)
{
    short value = 0;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

   	if (!_readWord16 (CAN_GET_CONTROL_MODE, axis, value))
		return false;

    if (!(value & 0xf0))
		return true;

	return false;
}

bool MEIDeviceDriver::setPrintFunction(int (*f) (const char *fmt, ...))
{
	_mutex.wait();
	EsdCanResources& r = RES(system_resources);
	_p = f;
	_mutex.post();

	return true;
}

bool MEIDeviceDriver::getAmpStatusRaw(int *st)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            st[i] = short(r._bcastRecvBuffer[i]._fault);
        }
    _mutex.post();

	return true;
}

bool MEIDeviceDriver::setLimitsRaw(int axis, double min, double max)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

    bool ret=true;

    ret = ret && _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(min));
	ret = ret && _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(max));

    return ret;
}

bool MEIDeviceDriver::getLimitsRaw(int axis, double *min, double *max)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	int iMin, iMax;
    bool ret=true;

    ret = ret && _readDWord (CAN_GET_MIN_POSITION, axis, iMin);
	ret = ret && _readDWord (CAN_GET_MAX_POSITION, axis, iMax);

    *min=iMin;
    *max=iMax;
    
	return ret;
}

bool MEIDeviceDriver::loadBootMemory()
{
    EsdCanResources& r = RES(system_resources);
	
    bool ret=true;
    for(int j=0; j<r.getJoints(); j++)
        {
            ret=_writeNone(CAN_READ_FLASH_MEM, j);
            if (!ret)
                return false;
        }

	return true;
}

bool MEIDeviceDriver::saveBootMemory ()
{
    EsdCanResources& r = RES(system_resources);
	
    bool ret=true;
    for(int j=0; j<r.getJoints(); j++)
        {
            ret=_writeNone(CAN_WRITE_FLASH_MEM, j);
            if (!ret)
                return false;
        }

    return true;
}

/// sets the broadcast policy for a given board (don't need to be called twice).
/// the parameter is a 32-bit integer: bit X = 1 -> message X = active
/// e.g. 0x02 activates the broadcast of position information
///		 0x04 activates the broadcast of velocity ...
///
bool MEIDeviceDriver::setBCastMessages (int axis, double v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	return _writeDWord (CAN_SET_BCAST_POLICY, axis, S_32(v));
}

inline bool MEIDeviceDriver::ENABLED (int axis)
{
	EsdCanResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & ESD_CAN_SKIP_ADDR) == 0) ? true : false;
}

///
/// helper functions.
///
///
///
*/
//////////////////////////////Old MEI Part

int MEIDeviceDriver::setPosition(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_positions[tmp->axis] = *((double *)(tmp->parameters));
	
	int axis = tmp->axis;
	rc = start_move(axis, _ref_positions[axis], _ref_speeds[axis], _ref_accs[axis]);
	
	return rc;
}

int MEIDeviceDriver::setSpeed(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_speeds[tmp->axis] = *((double *)(tmp->parameters));

	return rc;
}

int MEIDeviceDriver::setAcceleration(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_accs[tmp->axis] = *((double *)(tmp->parameters));

	return rc;
}

int MEIDeviceDriver::definePosition (void *cmd) 
{
	long rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *pos = (double *)(tmp->parameters);

	rc = set_position(tmp->axis, *pos);

	/// this is to reset the encoder ref value.
	/// LATER: need to verify whether summing pos is the right thing to do.
	_position_zero[tmp->axis] = double(dsp_encoder (tmp->axis)) + *pos;
	_winding[tmp->axis] = 0;

	return rc;
}

int MEIDeviceDriver::getPositions(void *j)
{
	///long rc = 0;
	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
	{
		/// direct reading from the DSP (fast!).
		out[i] = double(dsp_encoder (i));

		/// computing the actual encoder value.
		if (_sgn(out[i]) < 0 && _16bit_oldpos[i] > 16384.0 && _winding[i] == 0)
		{
			_winding[i] = 1;
		}
		else
		if (_sgn(out[i]) > 0 && _16bit_oldpos[i] < -16384.0 && _winding[i] == 1)
		{
			_winding[i] = 0;
		}
		else
		if (_sgn(out[i]) > 0 && _16bit_oldpos[i] < -16384.0 && _winding[i] == 0)
		{
			_winding[i] = -1;
		}
		else
		if (_sgn(out[i]) < 0 && _16bit_oldpos[i] > 16384.0 && _winding[i] == -1)
		{
			_winding[i] = 0;
		}

		_16bit_oldpos[i] = out[i];

		switch (_winding[i])
		{
			case 1:
				out[i] = 65535.0 + out[i] - _position_zero[i];
				break;

			case -1:
				out[i] = -65536.0 + out[i] - _position_zero[i];
				break;

			case 0:
				out[i] -= _position_zero[i];
				break;
		}
	}

	return 0;
}

int MEIDeviceDriver::getRefPositions(void *j)
{
	long rc = 0;

	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
		rc = get_command(i, &out[i]);

	return rc;
}

// returns the position error.
int MEIDeviceDriver::getErrors(void *errs)
{
	long rc = 0;

	double *out = (double *) errs;

	for(int i = 0; i < _njoints; i++)
		rc = get_error(i, &out[i]);

	return rc;
}

int MEIDeviceDriver::getTorques(void *trqs)
{
	long rc = 0;

	double *out = (double *) trqs;

	for(int i = 0; i < _njoints; i++)
	{	
		int16 tmp;
		get_dac_output(i, &tmp);
		out[i] = (double) (tmp);
	}

	return rc;
}

int MEIDeviceDriver::getSpeeds(void *spds)
{
	long rc = 0;
	double *out = (double *) spds;
	
	DSP_DM vel;
	for(int i = 0; i < _njoints; i++) {
		P_DSP_DM current_v_addr = dspPtr->data_struct + DS_CURRENT_VEL + DS(i);
		pcdsp_transfer_block(dspPtr, TRUE, FALSE, current_v_addr, 1, &vel);
		out[i] =  vel*_dsp_rate;
	}
	return rc;
}

int MEIDeviceDriver::getPosition(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;

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

	*((double *)tmp->parameters) = tmpd;
	return rc;
}

int MEIDeviceDriver::readAnalog(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	rc = read_axis_analog(axis, (int16 *)tmp->parameters);

	return rc;
}

int MEIDeviceDriver::setAxisAnalog(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *( (int16*) tmp->parameters);

	rc = set_axis_analog(axis, value);

	return rc;
}

int MEIDeviceDriver::setPositions (void *param) 
{
	long rc = 0;

	memcpy(_ref_positions, param, sizeof(double)*_njoints);
	
	for(int i = 0; i < _njoints; i++)
		rc = start_move(i, _ref_positions[i], _ref_speeds[i], _ref_accs[i]);
	
	// this call is buggy
	// rc = start_move_all(_njoints, _all_axes, _ref_positions, _ref_speeds, _ref_accs);
	// wait_for_all(_njoints, _all_axes);

	return rc;
}

int MEIDeviceDriver::setSpeeds (void *spds) 
{
	long rc = 0;

	memcpy(_ref_speeds, spds, sizeof(double)*_njoints);
	
	return rc;
}

int MEIDeviceDriver::setAccelerations (void *param) 
{
	long rc = 0;

	memcpy(_ref_accs, param, sizeof(double)*_njoints);

	return rc;
}
	
int MEIDeviceDriver::definePositions (void *param) 
{
	int16 rc = 0;

	double *cmds = (double *) param;
	for(int i = 0; i < _njoints; i++)
	{
		rc = set_position(i, cmds[i]);

		/// this is to reset the encoder ref value.
		/// LATER: need to verify whether summing cmds[i] is the right thing to do.
		_position_zero[i] = double(dsp_encoder (i)) + cmds[i];
		_winding[i] = 0;
	}

	return rc;
}

int MEIDeviceDriver::setOutputPort(void *cmd)
{
	int16 rc = 0;
	IOParameters *par = (IOParameters *) cmd;
	rc = set_io(par->port, (short) par->value);
	
	return rc;
}

int MEIDeviceDriver::getOutputPort(void *cmd)
{
	int16 rc = 0;
	IOParameters *par = (IOParameters *) cmd;
	rc = get_io(par->port, (short *) &par->value);
	
	return rc;
}

int MEIDeviceDriver::setAmpEnableLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_amp_enable_level(axis, value);
	
	return rc;
}

int MEIDeviceDriver::setAmpEnable(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_amp_enable(axis, value);

	return rc;
}

int MEIDeviceDriver::setPositiveLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	rc = set_positive_limit(axis, _events[*event]);

	return rc;
}

int MEIDeviceDriver::setNegativeLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	rc = set_negative_limit(axis, _events[*event]);
	
	return rc;
}

int MEIDeviceDriver::setPositiveLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_positive_level(axis, value);

	return rc;
}

int MEIDeviceDriver::setNegativeLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_negative_level(axis, value);
	
	return rc;
}

int MEIDeviceDriver::enableAmp(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = enable_amplifier(*ax);

	return rc;
}

int MEIDeviceDriver::disableAmp(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = disable_amplifier(*ax);

	return rc;
}

int MEIDeviceDriver::controllerRun(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	while(!motion_done(*ax));	// required otherwise controller_run may fail
	rc = controller_run(*ax);
	while(!motion_done(*ax));	//LATER: check this
	
	return rc;
}

int MEIDeviceDriver::clearStop(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = clear_status(*ax);
	// while(!motion_done(*ax));	//LATER: check this
	
	return rc;
}

int MEIDeviceDriver::setStopRate(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double rate = *((double *)(tmp->parameters));
	rc = set_stop_rate(tmp->axis, rate);

	return rc;
}

int MEIDeviceDriver::controllerIdle(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = controller_idle(*ax);
	while(!motion_done(*ax));	//LATER: check this

	return rc;
}

int MEIDeviceDriver::setOutputBit(void *n)
{
	ACE_ASSERT (!implemented);
	return 0;
}

int MEIDeviceDriver::clearOutputBit(void *n)
{
	ACE_ASSERT (!implemented);
	return 0;
}

int MEIDeviceDriver::setOffset(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_OFFSET] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

int MEIDeviceDriver::setOffsets(void *offs)
{
	int16 rc = 0;

	double *cmd = (double *) offs;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_OFFSET] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int MEIDeviceDriver::setTorqueLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_DAC_LIMIT] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

int MEIDeviceDriver::getTorqueLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double *val = ((double *) tmp->parameters);

	*val = _filter_coeffs[axis][DF_DAC_LIMIT];
		
	return rc;
}

int MEIDeviceDriver::getTorqueLimits(void *trqs)
{
	int16 rc = 0;
	double *out = (double *) trqs;

	for (int i = 0; i<_njoints; i++)
		out[i] = _filter_coeffs[i][DF_DAC_LIMIT];
	
	return rc;
}

int MEIDeviceDriver::setTorqueLimits(void *trqs)
{
	int16 rc = 0;

	double *cmd = (double *) trqs;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_DAC_LIMIT] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int MEIDeviceDriver::setIntLimits(void *lmts)
{
	int16 rc = 0;

	double *cmd = (double *) lmts;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_I_LIMIT] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int MEIDeviceDriver::setIntLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_I_LIMIT] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

MEIDeviceDriver::readInput(void *input)
{
	int16 rc = 0;	
	ACE_ASSERT (!implemented);
	return rc;
}

int MEIDeviceDriver::dummy(void *d)
{
	int16 rc = 0;
	return rc;
}

int MEIDeviceDriver::stopAxes(void *par)
{
	int16 rc = 0;
	ACE_ASSERT (!implemented);
	return rc;
}

int MEIDeviceDriver::readSwitches(void *switches)
{
	int16 rc = 0;
	ACE_ASSERT (!implemented);
	return rc;
}

int MEIDeviceDriver::getRefSpeeds(void *spds)
{
	int16 rc = 0;

	memcpy(spds, _ref_speeds, sizeof(double) * _njoints);

	return rc;
}

int MEIDeviceDriver::getRefAccelerations(void *accs)
{
	int16 rc = 0;

	memcpy(accs, _ref_accs, sizeof(double) * _njoints);

	return rc;
}

int MEIDeviceDriver::initPortAsInput(void *p)
{
	int16 rc = 0;
	int *port = (int *) p;
	rc = init_io(*port, IO_INPUT);
	return rc;
}

int MEIDeviceDriver::initPortAsOutput(void *p)
{
	int16 rc = 0;
	int *port = (int *) p;
	rc = init_io(*port, IO_OUTPUT);
	return rc;
}

int MEIDeviceDriver::vMove(void *spds)
{
	int16 rc = 0;
	double *cmds = (double *) spds;
	for(int i = 0; i < _njoints; i++)
		rc = v_move(i, cmds[i], _ref_accs[i]);

	return rc;
}

int MEIDeviceDriver::safeVMove(void *spds)
{
	int16 rc = 0;
	double *cmds = (double *) spds;
	for(int i = 0; i < _njoints; i++)
	{
		if (!frames_left(i))
			rc = v_move(i, cmds[i], _ref_accs[i]);
	}

	return rc;
}

int MEIDeviceDriver::setCommands(void *pos)
{
	int16 rc = 0;
	double *tmpPos = (double *) pos;
	for(int i = 0; i < _njoints; i++)
		rc = set_command(i, tmpPos[i]);

	return rc;
}

int MEIDeviceDriver::setCommand(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	double *tmpPos = (double *) tmp->parameters;
	
	rc = set_command(axis, *tmpPos);

	return rc;
}

int MEIDeviceDriver::checkMotionDone(void *flag)
{
	int16 rc = 0;
	
	bool *tmp = (bool *) flag;
	*tmp = true;

	for(int i = 0; i < _njoints; i++)
		 *tmp = *tmp && motion_done(i);
	
	return rc;
}

int MEIDeviceDriver::waitForMotionDone(void *cmd)
{
	int16 rc = 0;

	int time = *((int *) cmd);

	if (time != 0) {
		for(int i = 0; i < _njoints; i++)
			while(!motion_done(i)) ACE_OS::sleep(ACE_Time_Value(0,time));
	}
	else {
		for(int i = 0; i < _njoints; i++)
		while(!motion_done(i));	// busy waiting
	}

	return rc;
}


int MEIDeviceDriver::waitForFramesLeft(void *cmd)
{
	int16 rc = 0;

	int time = *((int *) cmd);

	if (time != 0) {
		for(int i = 0; i < _njoints; i++)
			while(frames_left(i)) ACE_OS::sleep(ACE_Time_Value(0,time));
	}
	else {
		for(int i = 0; i < _njoints; i++)
		while(frames_left(i));	// busy waiting
	}

	return rc;
}

int MEIDeviceDriver::checkFramesLeft(void *flag)
{
	int16 rc = 0;
	
	bool *tmp = (bool *) flag;
	*tmp = true;

	for(int i = 0; i < _njoints; i++)
		 *tmp = *tmp && frames_left(i);
	
	return rc;
}

int MEIDeviceDriver::setHomeIndexConfig(void *cmd)
{
	int16 rc;
	SingleAxisParameters *par = (SingleAxisParameters *) cmd;
	int conf = *(int *)(par->parameters);
	int16 config;
	switch (conf)
	{
	case CBHomeOnly:
		config = HOME_ONLY;
		break;
	case CBIndexOnly:
		config = INDEX_ONLY;
		break;
	case CBHighHomeAndIndex:
		config = HIGH_HOME_AND_INDEX;
		break;
	case CBLowHomeAndIndex:
	default:
		config = LOW_HOME_AND_INDEX;
	}
	rc = set_home_index_config(par->axis, config);
	return rc;
}

int MEIDeviceDriver::setHomeLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_home_level(tmp->axis, value);
	return rc;
}

int MEIDeviceDriver::setHome(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	int16 ev = _events[*event];
	rc = set_home(axis, ev);
	return rc;
}

int MEIDeviceDriver::setHomeProcedure(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int16 ev = _events[*(ControlBoardEvents *)tmp->parameters];

	SingleAxisParameters x;
	int ipar;
	double dpar;

	x.axis = tmp->axis;
	x.parameters = &ipar;
	ipar = CBIndexOnly;			// index_only
	setHomeIndexConfig(&cmd);
	ipar = 0;					// (active low)
	setHomeLevel(&cmd);

	rc = set_home(tmp->axis, ev);

	x.parameters = &dpar;
	dpar = 50000.0;				// stop rate (acc)
	setStopRate(&cmd);

	return rc;
}

int MEIDeviceDriver::setPid(void *cmd)
{
	int16 rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	// these are stored to be used later in the setOffsets/setOffset functions
	_filter_coeffs[tmp->axis][DF_P] = (int16) round(pid->KP);
	_filter_coeffs[tmp->axis][DF_I] = (int16) round(pid->KI);
	_filter_coeffs[tmp->axis][DF_D] = (int16) round(pid->KD);
	_filter_coeffs[tmp->axis][DF_ACCEL_FF] = (int16) round (pid->AC_FF);
	_filter_coeffs[tmp->axis][DF_VEL_FF] = (int16) round(pid->VEL_FF);
	_filter_coeffs[tmp->axis][DF_I_LIMIT] = (int16) round(pid->I_LIMIT);
	_filter_coeffs[tmp->axis][DF_OFFSET] = (int16) round(pid->OFFSET);
	_filter_coeffs[tmp->axis][DF_DAC_LIMIT] = (int16) round(pid->T_LIMIT);
	_filter_coeffs[tmp->axis][DF_SHIFT] = (int16) round(pid->SHIFT);
	_filter_coeffs[tmp->axis][DF_FRICT_FF] = (int16) round(pid->FRICT_FF);
	
	rc = set_filter(tmp->axis, _filter_coeffs[tmp->axis]);

	return rc;
}

int MEIDeviceDriver::getPid(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
	int axis = tmp->axis;
	
	get_filter(axis, _filter_coeffs[axis]);

	pid->KP = _filter_coeffs[axis][DF_P];
	pid->KI = _filter_coeffs[axis][DF_I];
	pid->KD = _filter_coeffs[axis][DF_D];
	pid->AC_FF = _filter_coeffs[axis][DF_ACCEL_FF];
	pid->VEL_FF = _filter_coeffs[axis][DF_VEL_FF];
	pid->I_LIMIT = _filter_coeffs[axis][DF_I_LIMIT];
	pid->OFFSET = _filter_coeffs[axis][DF_OFFSET];
	pid->T_LIMIT = _filter_coeffs[axis][DF_DAC_LIMIT];
	pid->SHIFT = _filter_coeffs[axis][DF_SHIFT];
	pid->FRICT_FF = _filter_coeffs[axis][DF_FRICT_FF];
	
	return rc;
}