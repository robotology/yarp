// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
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
/// $Id: EsdMotionControl.cpp,v 1.4 2006-06-14 08:39:44 eshuy Exp $
///
///

/// general purpose stuff.
#include <yarp/os/Time.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_msg.h>
#include <ace/Sched_Params.h>

/// specific to this device driver.
#include "EsdMotionControl.h"
#include "../dd_orig/include/ntcan.h"

/// get the message types from the DSP code.
#define __ONLY_DEF
#include "messages.h" //this can easily go to the common part

#define BUF_SIZE 2047

typedef int (*PV) (const char *fmt, ...);

using namespace yarp::os;
using namespace yarp::dev;

bool NOT_YET_IMPLEMENTED(const char *txt)
{
   fprintf(stderr, "%s not yet implemented for EsdMotionControl\n", txt);

   return false;
}

/**
 * Max number of addressable cards in this implementation.
 */
const int ESD_MAX_CARDS		= 16;

EsdMotionControlParameters::EsdMotionControlParameters()
{
    _networkN = 0;
    _destinations = new unsigned char [ESD_MAX_CARDS];
	memset (_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);
	_my_address = 0;
	_polling_interval = 10;
	_timeout = 20;
	_njoints = 0;
	_p = NULL;

	_txQueueSize = 2047;					/** max len of the buffer for the esd driver */
	_rxQueueSize = 2047;
	_txTimeout = 20;						/** 20ms timeout */
	_rxTimeout = 20;
}

EsdMotionControlParameters::~EsdMotionControlParameters()
{
    delete [] _destinations;
}

///
///
///
class BCastBufferElement
{
public:
	// msg 1
	int _position;
	double _update_p;

	// msg 2
	short _pid_value;
	double _update_v;

	// msg 3
	short _fault;
	double _update_e;

	// msg 4
	short _current;
	short _position_error;
	double _update_c;

	BCastBufferElement () { zero (); }

	void zero (void)
	{
		_position = 0;
		_pid_value = 0;
		_current = 0;
		_fault = 0;
		_position_error = 0;

		_update_p = .0;
		_update_v = .0;
		_update_e = .0;
		_update_c = .0;
	}
};

///
///
///
class EsdCanResources
{
public:
	EsdCanResources ();
	~EsdCanResources ();

	bool initialize (const EsdMotionControlParameters& parms);
	bool uninitialize ();
	bool read ();
	bool error (const CMSG& m);

	bool startPacket ();
	bool addMessage (int msg_id, int joint);
	bool writePacket ();

	bool printMessage (const CMSG& m);
	bool dumpBuffers (void);
	inline int getJoints (void) const { return _njoints; }
	inline bool getErrorStatus (void) const { return _error_status; }
	
public:
	enum { ESD_TIMEOUT = 20, ESD_POLLING_INTERVAL = 10 };

	HANDLE _handle;								/// the actual ddriver handle.
	int _timeout;								/// this is my thread timeout.
	
	int _polling_interval;						/// thread polling interval.
	int _speed;									/// speed of the bus.
	int _networkN;								/// network number.
	
	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _readMessages;							/// size of the last read buffer.
	int _msg_lost;								/// # of lost messages.
	CMSG _readBuffer[BUF_SIZE];					/// read buffer.
	int _writeMessages;							/// size of the write packet.
	CMSG _writeBuffer[BUF_SIZE];				/// write buffer.
	CMSG _replyBuffer[BUF_SIZE];				/// reply buffer.

	BCastBufferElement *_bcastRecvBuffer;		/// local storage for bcast messages.

	unsigned char _my_address;					/// 
	unsigned char _destinations[ESD_MAX_CARDS];	/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).

	bool _error_status;							/// error status of the last packet.

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	int _filter;								/// don't print filtered messages.
	
	char _printBuffer[16384];
};

EsdCanResources::EsdCanResources ()
{
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;						/// default 1Mbit/s
	_networkN = 0;

	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	memset (_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);

	_my_address = 0;
	_njoints = 0;
	_p = NULL;

	_readMessages = 0;
	_msg_lost = 0;
	_writeMessages = 0;
	_bcastRecvBuffer = NULL;

	_error_status = true;
}

EsdCanResources::~EsdCanResources () 
{ 
	uninitialize(); 
}

bool EsdCanResources::initialize (const EsdMotionControlParameters& parms)
{
	if (_handle != ACE_INVALID_HANDLE)
		return false;

	/// general variable init.
	_handle = ACE_INVALID_HANDLE;
	_speed = 0;						/// default 1Mbit/s
	_networkN = parms._networkN;

	_readMessages = 0;
	_writeMessages = 0;
	_msg_lost = 0;
	_error_status = true;

	memcpy (_destinations, parms._destinations, sizeof(unsigned char)*ESD_MAX_CARDS);
	_my_address = parms._my_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_njoints = parms._njoints;	
	_p = parms._p;
	_filter = -1;

	_txQueueSize = parms._txQueueSize;
	_rxQueueSize = parms._rxQueueSize;
	_txTimeout = parms._txTimeout;
	_rxTimeout = parms._rxTimeout;

	_bcastRecvBuffer = new BCastBufferElement[_njoints];
	ACE_ASSERT (_bcastRecvBuffer != NULL);
    /// suspect an issue with allocation of array of classes.

    /// it requires further investigation.


	/// clean up buffers.
	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	/// open the device.
	int res = canOpen (_networkN, 0, _txQueueSize, _rxQueueSize, _txTimeout, _rxTimeout, &_handle);
	if (res != NTCAN_SUCCESS)
		return false;

	res = canSetBaudrate (_handle, _speed);
	if (res != NTCAN_SUCCESS)
	{
		canClose (_handle);
		return false;
	}

	/// sets all message ID's for class 0 and 1.
	int i;
	for (i = 0; i < 0xff; i++)
		canIdAdd (_handle, i);
	
	for (i = 0x100; i < 0x1ff; i++)
		canIdAdd (_handle, i);

	return true;
}


bool EsdCanResources::uninitialize ()
{
	if (_bcastRecvBuffer != NULL) delete[] _bcastRecvBuffer;

	if (_handle != ACE_INVALID_HANDLE)
	{
		int res = canClose (_handle);
		if (res != NTCAN_SUCCESS)
			return false;
		_handle = ACE_INVALID_HANDLE;
	}
	
	return true;
}


bool EsdCanResources::read ()
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _readBuffer, &messages); 
	if (res != NTCAN_SUCCESS)
		return false;

	_readMessages = messages;
	return true;
}

bool EsdCanResources::startPacket ()
{
	_writeMessages = 0;
	return true;
}

bool EsdCanResources::addMessage (int msg_id, int joint)
{
	CMSG x;
	memset (&x, 0, sizeof(CMSG));

	x.id = _my_address << 4;
	x.id = _destinations[joint/2] & 0x0f;

	x.len = 1;
	x.data[0] = msg_id;
	if ((joint % 2) == 1)
		x.data[0] |= 0x80;

	_writeBuffer[_writeMessages] = x;
	_writeMessages ++;

	return true;
}

bool EsdCanResources::writePacket ()
{
	if (_writeMessages < 1)
		return false;

	long len = _writeMessages;

	int res = canSend (_handle, _writeBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _writeMessages)
		return false;

	return true;
}

bool EsdCanResources::printMessage (const CMSG& m)
{
	if (!_p)
		return false;

	int ret = ACE_OS::sprintf (_printBuffer, "class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
		  (m.id & 0x700) >> 8,
		  (m.id & 0xf0) >> 4, 
		  (m.id & 0x0f), 
		  ((m.data[0] & 0x80)==0)?0:1,
		  (m.data[0] & 0x7f),
		  (m.data[0] & 0x7f));

	if (m.len > 1)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "x: "); 
	}

	int j;
	for (j = 1; j < m.len; j++)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "%x ", m.data[j]);
	}

	ret += ACE_OS::sprintf(_printBuffer+ret, "st: %x\n", m.msg_lost);


	(*_p) 
		("%s", _printBuffer);

	return true;
}

bool EsdCanResources::dumpBuffers (void)
{
	if (!_p) return false;

	int j;

	/// dump the error.
	(*_p) ("CAN: write buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_writeBuffer[j]);

	(*_p) ("CAN: reply buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_replyBuffer[j]);

	(*_p) ("CAN: read buffer\n");
	for (j = 0; j < _readMessages; j++)
		printMessage (_readBuffer[j]);
	(*_p) ("CAN: -------------\n");

	return true;
}

bool EsdCanResources::error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

///
///
///
inline EsdCanResources& RES(void *res) { return *(EsdCanResources *)res; }


EsdMotionControl::EsdMotionControl(const EsdMotionControlParameters &par) :
IPositionControl2(this, par.nj, par.axisMap, par.angleToEncoder, par.zeros)
{
	system_resources = (void *) new EsdCanResources;
	ACE_ASSERT (system_resources != NULL);
}


EsdMotionControl::~EsdMotionControl ()
{
	if (system_resources != NULL)
		delete (EsdCanResources *)system_resources;
	system_resources = NULL;
}


bool EsdMotionControl::open (const EsdMotionControlParameters &p)
{
	_mutex.wait();

    EsdCanResources& r = RES (system_resources);
	
	if (!r.initialize (p))
	{
		_mutex.post();
		return false;
	}

    Thread::start();
	_done.wait ();

	/// used for printing debug messages.
	_p = p._p;
	_filter = -1;
	_writerequested = false;
	_noreply = false;

	/// temporary variables used by the ddriver.
	_ref_positions = new double [r.getJoints()];		
	_ref_speeds = new double [r.getJoints()];
	_ref_accs = new double [r.getJoints()];
	ACE_ASSERT (_ref_positions != NULL && _ref_speeds != NULL && _ref_accs != NULL);
	_mutex.post ();

	/// default initialization for this device driver.
	int i;
	for(i = 0; i < r.getJoints(); i++)
		setBCastMessages(i, double (0x1E)); /// 0x1A activates position and current consumption broadcast + fault events
	
	return true;
}

bool EsdMotionControl::close (void)
{
	EsdCanResources& d = RES(system_resources);

    if (Thread::isRunning())
    {
	    /// default initialization for this device driver.
	    int i;
	    for(i = 0; i < d.getJoints(); i++)
		    setBCastMessages(i, double(0x00));
    }


    Thread::stop ();	/// stops the thread first (joins too).

	if (_ref_positions != NULL) delete[] _ref_positions;
	if (_ref_speeds != NULL) delete[] _ref_speeds;
	if (_ref_accs != NULL) delete[] _ref_accs;

	int ret = d.uninitialize ();

	return ret;
}


///
///
///
void EsdMotionControl::run ()
{
	EsdCanResources& r = RES (system_resources);

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

		_mutex.wait ();
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
				for (j = 0; j < ESD_MAX_CARDS; j++)
				{
					if (r._destinations[j] == addr)
						break;
				}

				j *= 2;

				/* less sign nibble specifies msg type */
				switch (m.id & 0x00f)
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
									memcpy (&r._replyBuffer[j], &m, sizeof(CMSG));
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
						memset (r._replyBuffer, 0, sizeof(CMSG) * r._writeMessages);

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

		_mutex.post ();

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

}

/// LATER: can be optimized.
bool EsdMotionControl::setPid (int axis, const yarp::dev::Pid &pid)
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

bool EsdMotionControl::getPid (int axis, Pid *out)
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

bool EsdMotionControl::getPids (Pid *out)
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


bool EsdMotionControl::setPids(const Pid *pids)
{
    return NOT_YET_IMPLEMENTED("setPids");
}

/// cmd is a SingleAxis poitner with 1 double arg
bool EsdMotionControl::setReference (int j, double ref)
{
	const int axis = j;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_COMMAND_POSITION, axis, S_32(ref));
}

/// cmd is an array of double (LATER: to be optimized).
bool EsdMotionControl::setReferences (const double *refs)
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

bool EsdMotionControl::setErrorLimit(int j, double limit)
{
    return NOT_YET_IMPLEMENTED("setErrorLimit");
}

bool EsdMotionControl::setErrorLimits(const double *limit)
{
    return NOT_YET_IMPLEMENTED("setErrorLimits");
}

bool EsdMotionControl::getError(int axis, double *err)
{
	EsdCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

    _mutex.wait();

	*(err) = double(r._bcastRecvBuffer[axis]._position_error);

    _mutex.post();

	return true;
}

bool EsdMotionControl::getErrors(double *errs)
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

bool EsdMotionControl::getOutput(int axis, double *out)
{
   	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
    _mutex.wait();
	*(out) = double(r._bcastRecvBuffer[axis]._pid_value);
    _mutex.post();
	return true;
}

bool EsdMotionControl::getOutputs(double *outs)
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

bool EsdMotionControl::getReference(int j, double *ref)
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

bool EsdMotionControl::getReferences(double *ref)
{
   return _readDWordArray(CAN_GET_DESIRED_POSITION, ref);
}

bool EsdMotionControl::getErrorLimit(int j, double *err)
{
    return NOT_YET_IMPLEMENTED("getErrorLimit");
}

bool EsdMotionControl::getErrorLimits(double *errs)
{
    return NOT_YET_IMPLEMENTED("getErrorLimits");
}

bool EsdMotionControl::resetPid(int j)
{
    return NOT_YET_IMPLEMENTED("resetPid");
}

bool EsdMotionControl::enablePid(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);

}

bool EsdMotionControl::disablePid(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

bool EsdMotionControl::setPositionMode()
{
    return NOT_YET_IMPLEMENTED("setVelocityMode");
}

bool EsdMotionControl::setVelocityMode()
{
    return NOT_YET_IMPLEMENTED("setVelocityMode");
}

bool EsdMotionControl::positionMove(int axis, double ref)
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

bool EsdMotionControl::positionMove(const double *refs)
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

bool EsdMotionControl::relativeMove(int j, double delta)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

bool EsdMotionControl::relativeMove(const double *deltas)
{
    return NOT_YET_IMPLEMENTED("positionRelative");
}

/// check motion done, single axis.
bool EsdMotionControl::checkMotionDone(int axis, bool *ret)
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
bool EsdMotionControl::checkMotionDone (bool *ret)
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

bool EsdMotionControl::setRefSpeed(int axis, double sp)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	_ref_speeds[axis] = sp;
	return true;
}

bool EsdMotionControl::setRefSpeeds(const double *spds)
{
	EsdCanResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r.getJoints(); i++)
		_ref_speeds[i] = spds[i];

	return true;
}

bool EsdMotionControl::setRefAcceleration(int axis, double acc)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	_ref_accs[axis] = acc;
	const short s = S_16(_ref_accs[axis]);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}

bool EsdMotionControl::setRefAccelerations(const double *accs)
{
    EsdCanResources& r = RES(system_resources);

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_accs[i] = accs[i];
		if (!_writeWord16 (CAN_SET_DESIRED_ACCELER, i, S_16(_ref_accs[i])))
			return false;
	}

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool EsdMotionControl::getRefSpeeds (double *spds)
{
	EsdCanResources& r = RES(system_resources);
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

	return true;
}

bool EsdMotionControl::getRefSpeed (int axis, double *spd)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	EsdCanResources& r = RES(system_resources);
	short value = 0;

    if (_readWord16 (CAN_GET_DESIRED_VELOCITY, axis, value))
    {
    	_ref_speeds[axis] = double (value);
         *spd = _ref_speeds[axis];
    } 
	else
		return false;
	
	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool EsdMotionControl::getRefAccelerations (double *accs)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_DESIRED_ACCELER, i, value) == true)
			_ref_accs[i] = accs[i] = double (value);
		else
			return false;
	}

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
bool EsdMotionControl::getRefAcceleration (int axis, double *accs)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	short value = 0;

    if (_readWord16 (CAN_GET_DESIRED_ACCELER, axis, value) == true)
    {
		_ref_accs[axis] = double (value);
        *accs = double(value);
    }
	else
		return false;

	return true;
}

bool EsdMotionControl::stop(int j)
{
    return NOT_YET_IMPLEMENTED("stop");
}

bool EsdMotionControl::stop()
{
    return NOT_YET_IMPLEMENTED("stop");
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool EsdMotionControl::velocityMove (int axis, double sp)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	
	_mutex.wait();
	r.startPacket();

	if (ENABLED (axis))
		{
			r.addMessage (CAN_VELOCITY_MOVE, axis);
			const int j = r._writeMessages - 1;
			_ref_speeds[axis] = sp;
			*((short*)(r._writeBuffer[j].data+1)) = S_16(_ref_speeds[axis]);	/// speed
			*((short*)(r._writeBuffer[j].data+3)) = S_16(_ref_accs[axis]);		/// accel
			r._writeBuffer[j].len = 5;
		}
		else
		{
			_ref_speeds[axis] = sp;
		}

	_writerequested = true;
	_noreply = true;

	_mutex.post();

	_done.wait();

	return true;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
bool EsdMotionControl::velocityMove (const double *sp)
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
			_ref_speeds[i] = sp[i];
			*((short*)(r._writeBuffer[j].data+1)) = S_16(_ref_speeds[i]);	/// speed
			*((short*)(r._writeBuffer[j].data+3)) = S_16(_ref_accs[i]);		/// accel
			r._writeBuffer[j].len = 5;
		}
		else
		{
			_ref_speeds[i] = sp[i];
		}
	}

	_writerequested = true;
	_noreply = true;

	_mutex.post();

	_done.wait();

	return true;
}

bool EsdMotionControl::setEncoder(int j, double val)
{
	const int axis = j;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(val));
}

bool EsdMotionControl::setEncoders(const double *vals)
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

bool EsdMotionControl::resetEncoder(int j)
{
    return setEncoder(j, 0);
}

bool EsdMotionControl::resetEncoders()
{
    int n=RES(system_resources).getJoints();
    double *tmp=new double [n];
    for(int i=0;i<n;i++)
        tmp[i]=0;

    bool ret= setEncoders(tmp);

    delete [] tmp;

    return ret;
}

bool EsdMotionControl::getEncoders(double *v)
{
    return _readDWordArray (CAN_GET_ENCODER_POSITION, v);
}

bool EsdMotionControl::getEncoder(int axis, double *v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	int value;
	if (_readDWord (CAN_GET_ENCODER_POSITION, axis, value) == true)
	{
		*v = double(value);
		return true;
	}
	
	*v= 0;
	return false;
}

bool EsdMotionControl::getEncoderSpeeds(double *v)
{
  	return NOT_YET_IMPLEMENTED("getEncoderSpeeds");
}

bool EsdMotionControl::getEncoderSpeed(int j, double *v)
{
    return NOT_YET_IMPLEMENTED("getEncoderSpeed");
}

bool EsdMotionControl::getEncoderAccelerations(double *v)
{
  	return NOT_YET_IMPLEMENTED("getEncoderAccs");
}

bool EsdMotionControl::getEncoderAcceleration(int j, double *v)
{
    return NOT_YET_IMPLEMENTED("getEncoderAcc");
}

bool EsdMotionControl::disableAmp(int axis)
{
    ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

bool EsdMotionControl::enableAmp(int axis)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

// bcast
bool EsdMotionControl::getCurrents(double *cs)
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
bool EsdMotionControl::getCurrent(int axis, double *c)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.wait();
	*c = double(r._bcastRecvBuffer[axis]._current);
    _mutex.post();

	return true;
}

bool EsdMotionControl::setMaxCurrent(int axis, double v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_CURRENT_LIMIT, axis, S_32(v));
}

bool EsdMotionControl::calibrate(int axis)
{
    return NOT_YET_IMPLEMENTED("calibrate");
}

bool EsdMotionControl::setPrintFunction(int (*f) (const char *fmt, ...))
{
	_mutex.wait();
	EsdCanResources& r = RES(system_resources);
	_p = f;
	_mutex.post();

	return true;
}

bool EsdMotionControl::getAmpStatus(int *st)
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

bool EsdMotionControl::setLimits(int axis, double min, double max)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

    bool ret=true;

    ret = ret && _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(min));
	ret = ret && _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(max));

    return ret;
}

bool EsdMotionControl::getLimits(int axis, double *min, double *max)
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

bool EsdMotionControl::loadBootMemory()
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

bool EsdMotionControl::saveBootMemory ()
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
bool EsdMotionControl::setBCastMessages (int axis, double v)
{
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	return _writeDWord (CAN_SET_BCAST_POLICY, axis, S_32(v));
}

inline bool EsdMotionControl::ENABLED (int axis)
{
	EsdCanResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & ESD_CAN_SKIP_ADDR) == 0) ? true : false;
}

#if 0
///
///
/// specific messages that change the driver behavior.
int EsdMotionControl::setDebugMessageFilter (void *cmd)
{
	_mutex.wait();
	EsdCanResources& r = RES(system_resources);
	_filter = *(int *)cmd;
	_mutex.post();

	return true;
}

int EsdMotionControl::setDebugPrintFunction (void *cmd)
{
	_mutex.wait();
	EsdCanResources& r = RES(system_resources);
	_p = (int (*) (const char *fmt, ...))cmd;
	_mutex.post();

	return true;
}


///
/// 
/// control card commands.
///

/// cmd is an array of double
int EsdMotionControl::getPositions (void *cmd)
{
    EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._position);
	}

    _mutex.post();
	return true;
}

/// cmd is a SingleAxisParameters pointer with double arg
int EsdMotionControl::getPosition (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position);
    _mutex.post();
    return true;
}

/// cmd is an array of double (njoints long)
int EsdMotionControl::setPositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints (); i++)
	{
		if (ENABLED(i))
		{
			//SingleAxisParameters x;
			//x.axis = i;
			//x.parameters = tmp+i;	

			r.addMessage (CAN_POSITION_MOVE, i);
			const int j = r._writeMessages - 1;
			_ref_positions[i] = tmp[i];
			*((int*)(r._writeBuffer[j].data+1)) = S_32(_ref_positions[i]);		/// pos
			*((short*)(r._writeBuffer[j].data+5)) = S_16(_ref_speeds[i]);		/// speed
			r._writeBuffer[j].len = 7;
		}
		else
		{
			_ref_positions[i] = tmp[i];
		}
	}

	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	return true;
}

///
///
/// cmd is a SingleAxisParameters pointer to a double argument
int EsdMotionControl::setPosition (void *cmd)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED (axis))
	{
		// still fills the _ref_position structure.
		_ref_positions[axis] = *((double *)tmp->parameters);
		return true;
	}

	_mutex.wait();

	r.startPacket();
	r.addMessage (CAN_POSITION_MOVE, axis);

	_ref_positions[axis] = *((double *)tmp->parameters);
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


///

/// starts the homing procedure for a given joint.

int EsdMotionControl::startCalibration (void *cmd)

{

	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;

	const int axis = tmp->axis;

	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	

	return _writeWord16 (CAN_CALIBRATE_ENCODER, axis, S_16(*((double *)tmp->parameters)));   

}



///

/// gets the control mode according to some controller specific coding (if any).

int EsdMotionControl::getControlMode (void *cmd)

{

    short value = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;

	const int axis = tmp->axis;

	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);



   	if (_readWord16 (CAN_GET_CONTROL_MODE, axis, value) != true)

	{

		*((double *)tmp->parameters) = 0;

		return false;

	}



	*((double *)tmp->parameters) = double(value); 

    return true;

}


/// cmd is an array of double of length njoints specifying speed 
/// for each axis
int EsdMotionControl::velocityMove (void *cmd)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED (i))
		{
			r.addMessage (CAN_VELOCITY_MOVE, i);
			const int j = r._writeMessages - 1;
			_ref_speeds[i] = tmp[i];
			*((short*)(r._writeBuffer[j].data+1)) = S_16(_ref_speeds[i]);	/// speed
			*((short*)(r._writeBuffer[j].data+3)) = S_16(_ref_accs[i]);		/// accel
			r._writeBuffer[j].len = 5;
		}
		else
		{
			_ref_speeds[i] = tmp[i];
		}
	}

	_writerequested = true;
	_noreply = true;

	_mutex.post();

	_done.wait();

	return true;
}

/// cmd is a SingleAxis poitner with 1 double arg
int EsdMotionControl::definePosition (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int EsdMotionControl::definePositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeDWord (CAN_SET_ENCODER_POSITION, i, S_32(tmp[i])) != true)
			return false;
	}

	return true;
}


/// cmd is an array of double
int EsdMotionControl::getRefPositions (void *cmd)
{
	return _readDWordArray (CAN_GET_DESIRED_POSITION, (double *)cmd);
}

/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::getRefPosition (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	int value = 0;
	if (_readDWord (CAN_GET_DESIRED_POSITION, axis, value) == true)
		*((double *)(tmp->parameters)) = double (value);
	else
		return false;

	return true;
}

/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::setSpeed (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	_ref_speeds[axis] = *((double *)(tmp->parameters));
	//const short s = S_16(_ref_speeds[axis]);
	//return _writeWord16 (CAN_SET_DESIRED_VELOCITY, axis, s);
	return true;
}

/// cmd is an array of double
int EsdMotionControl::setSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_speeds[i] = tmp[i];
		//if (_writeWord16 (CAN_SET_DESIRED_VELOCITY, i, S_16(tmp[i])) != true)
		//	return false;
	}

	return true;
}


/// cmd is an array of double
int EsdMotionControl::geSpeeds (void *cmd)
{
	return _readWord16Array (CAN_GET_DESIRED_VELOCITY, (double *)cmd);
}

/// cmd is an array of double (LATER: to be optimized).
int EsdMotionControl::getRefSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_DESIRED_VELOCITY, i, value) == true)
			_ref_speeds[i] = out[i] = double (value);
		else
			return false;
	}

	return true;
}

/// cmd is a SingleAxis pointer with a double arg
int EsdMotionControl::setAcceleration (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	_ref_accs[axis] = *((double *)tmp->parameters);
	const short s = S_16(_ref_accs[axis]);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}

/// cmd is an array of double (LATER: to be optimized, WARNING: doesn't skip disabled joints).
int EsdMotionControl::setAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_accs[i] = tmp[i];
		if (_writeWord16 (CAN_SET_DESIRED_ACCELER, i, S_16(_ref_accs[i])) != true)
			return false;
	}

	return true;
}

/// cmd is an array of double (LATER: to be optimized).
int EsdMotionControl::getRefAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_DESIRED_ACCELER, i, value) == true)
			_ref_accs[i] = out[i] = double (value);
		else
			return false;
	}

	return true;
}

/// cmd is a SigleAxis pointer with 1 double arg
int EsdMotionControl::setOffset (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_OFFSET, axis, s);
}

/// cmd is an array of double (LATER: to be optimized).
int EsdMotionControl::setOffsets (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_OFFSET, i, S_16(tmp[i])) != true)
			return false;
	}

	return true;
}

/// cmd is a SingleAxis pointer with 1 double arg
int EsdMotionControl::setIntegratorLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_ILIM_GAIN, axis, s);
}

/// cmd is an array of double (LATER: to be optimized, WARNING: doesn't check disabled cards).
int EsdMotionControl::setIntegratorLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_ILIM_GAIN, i, S_16(tmp[i])) != true)
			return false;
	}

	return true;
}


/// cmd is an int *
int EsdMotionControl::enableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

/// cmd is an int *
int EsdMotionControl::disableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

/// cmd is an int *
int EsdMotionControl::controllerRun (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);
}

/// cmd is an int *
int EsdMotionControl::controllerIdle (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

/// cmd is a SingleAxisParameters struct with a double argument.
int EsdMotionControl::getPidError (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	short value;

	if (_readWord16 (CAN_GET_PID_ERROR, axis, value) != true)
	{
		*((double *)tmp->parameters) = 0;
		return false;
	}

	*((double *)tmp->parameters) = double(value);
	return true;
}

/// cmd is a SingleAxis poitner with 1 double arg
int EsdMotionControl::setCurrentLimit (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_CURRENT_LIMIT, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
/// LATER: can be optimized.
int EsdMotionControl::setCurrentLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeDWord (CAN_SET_CURRENT_LIMIT, i, S_32(tmp[i])) != true)
			return false;
	}

	return true;
}

/// cmd is a pointer to an integer (axis number).
int EsdMotionControl::setSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a pointer to SingleAxisParameters struct with a single double arg.
int EsdMotionControl::setSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::getSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MIN_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return true;
}

/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::getSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MAX_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return true;
}


/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::setTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_TLIM, axis, s);
}

/// cmd is an array of double
/// LATER: can be optimized.
int EsdMotionControl::setTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_TLIM, i, S_16(tmp[i])) != true)
			return false;
	}

	return true;
}

/// cmd is a SingleAxis pointer with double arg
int EsdMotionControl::getTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	short value;

	_readWord16 (CAN_GET_TLIM, axis, value);
	*((double *)tmp->parameters) = double(value);

	return true;
}

/// cmd is an array of double
/// LATER: can be optimized.
int EsdMotionControl::getTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_TLIM, i, value) == true)
			out[i] = double (value);
		else
			return false;
	}

	return true;
}


///
/// reads an array of double from the broadcast message position buffer.
/// LATER: add a check of timing/error message.
///
int EsdMotionControl::getBCastPosition (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	

    _mutex.wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position);
    _mutex.post();


	return true;
}

int EsdMotionControl::getBCastPositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._position);
	}

    _mutex.post();
	return true;
}


int EsdMotionControl::getBCastPIDOutput (void *cmd)

{

	EsdCanResources& r = RES(system_resources);

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;

	const int axis = tmp->axis;

	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	

    _mutex.wait();

	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._pid_value);

    _mutex.post();



	return true;

}


int EsdMotionControl::getBCastPIDOutputs (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._pid_value);
	}

    _mutex.post();
	return true;
}

int EsdMotionControl::getBCastCurrent (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	

    _mutex.wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._current);
    _mutex.post();


	return true;
}

int EsdMotionControl::getBCastCurrents (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._current);
	}

    _mutex.post();
	return true;
}

int EsdMotionControl::getBCastFaults (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short *tmp = (short *)cmd;


    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = short(r._bcastRecvBuffer[i]._fault);
	}
    _mutex.post();


	return true;
}

int EsdMotionControl::getBCastPositionError (void *cmd)

{

	EsdCanResources& r = RES(system_resources);

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;

	const int axis = tmp->axis;

	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

	

    _mutex.wait();

	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position_error);

    _mutex.post();



	return true;

}



#endif

///
/// helper functions.
///
///
///

/// sends a message without parameters
bool EsdMotionControl::_writeNone (int msg, int axis)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		return true;
	}

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);

	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	return true;
}

bool EsdMotionControl::_readWord16 (int msg, int axis, short& value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		value = short(0);
		return true;
	}

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_writerequested = true;
	_noreply = false;

	_mutex.post();

	/// reads back position info.
	_done.wait();

	if (!r.getErrorStatus())
	{
		value = 0;
		return false;
	}

	value = *((short *)(r._replyBuffer[0].data+1));
	return true;
}

/// reads an array.
bool EsdMotionControl::_readWord16Array (int msg, double *out)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value = 0;

	_mutex.wait();
	r.startPacket ();

	for(i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (msg, i);
		}
		else
			out[i] = 0;
	}

	if (r._writeMessages < 1)

	{

		_mutex.post();
		return false;
	}


	_writerequested = true;
	_noreply = false;
	_mutex.post();

	_done.wait();

	if (!r.getErrorStatus())
	{
		memset (out, 0, sizeof(double) * r.getJoints());
		return false;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			CMSG& m = r._replyBuffer[j];
			if (m.id == 0xffff)
				out[i] = 0;
			else
				out[i] = *((short *)(m.data+1));
			j++;
		}
	}

	return true;
}

/// to send a Word16.
bool EsdMotionControl::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	if (!ENABLED(axis))
		return true;

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].data+1)) = s;
	r._writeBuffer[0].len = 3;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	/// hopefully ok...
	return true;
}

/// write a DWord
bool EsdMotionControl::_writeDWord (int msg, int axis, int value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
		return true;

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((int*)(r._writeBuffer[0].data+1)) = value;
	r._writeBuffer[0].len = 5;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	return true;
}

/// two shorts in a single Can message (both must belong to the same control card).
bool EsdMotionControl::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	EsdCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	ACE_ASSERT ((axis % 2) == 0);	/// axis is even.

	if (!ENABLED(axis))
		return true;

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].data+1)) = s1;
	*((short *)(r._writeBuffer[0].data+3)) = s2;
	r._writeBuffer[0].len = 5;

	_writerequested = true;
	_noreply = true;
	
	_mutex.post();

	/// syncing.
	_done.wait();

	/// hopefully ok...
	return true;
}

///
/// sends a message and gets a dword back.
/// 
bool EsdMotionControl::_readDWord (int msg, int axis, int& value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		value = 0;
		return true;
	}

	_mutex.wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_writerequested = true;
	_noreply = false;
	
	_mutex.post();

	_done.wait();

	if (!r.getErrorStatus())
	{
		value = 0;
		return false;
	}

	value = *((int *)(r._replyBuffer[0].data+1));
	return true;
}

/// reads an array of double words.
bool EsdMotionControl::_readDWordArray (int msg, double *out)
{
	EsdCanResources& r = RES(system_resources);
	int i, value = 0;

	_mutex.wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (msg, i);
		}
		else
			out[i] = 0;
	}

	if (r._writeMessages < 1)
	{
		_mutex.post();
		return false;
	}

	_writerequested = true;
	_noreply = false;
	_mutex.post();

	_done.wait();

	if (!r.getErrorStatus())
	{
		memset (out, 0, sizeof(double) * r.getJoints());
		return false;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			CMSG& m = r._replyBuffer[j];
			if (m.id == 0xffff)
				out[i] = 0;
			else
				out[i] = *((int *)(m.data+1));
			j++;
		}
	}

	return true;
}
