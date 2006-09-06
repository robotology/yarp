// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// $Id: EsdMessageSniffer.cpp,v 1.3 2006-09-06 21:30:55 eshuy Exp $
//

// general purpose stuff.
#include <yarp/os/Time.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_msg.h>
#include <ace/Sched_Params.h>

// specific to this device driver.
#include "EsdMessageSniffer.h"
#include "../dd_orig/include/ntcan.h"

#include "../../../ControlBoardInterfacesImpl.inl"

// get the message types from the DSP code.
#include "messages.h"

#define BUF_SIZE 2047

typedef int (*PV) (const char *fmt, ...);

using namespace yarp::os;
using namespace yarp::dev;

/**
 * Max number of addressable cards in this implementation.
 */
const int ESD_MAX_CARDS	= 16;

EsdMessageSnifferParameters::EsdMessageSnifferParameters(int nj)
{
    _destinations = allocAndCheck<unsigned char> (ESD_MAX_CARDS);
    _axisMap = allocAndCheck<int>(nj);
    _angleToEncoder = allocAndCheck<double>(nj);
    _zeros = allocAndCheck<double>(nj);

	_networkN = 0;
    _my_address = 0;
	_polling_interval = 10;
	_timeout = 20;
	_njoints = 0;
	_p = NULL;

	_txQueueSize = 2047;
	_rxQueueSize = 2047;
	_txTimeout = 20;
	_rxTimeout = 20;
}

EsdMessageSnifferParameters::~EsdMessageSnifferParameters()
{
    checkAndDestroy<double>(_zeros);
    checkAndDestroy<double>(_angleToEncoder);
    checkAndDestroy<int>(_axisMap);
    checkAndDestroy<unsigned char>(_destinations);
}

/*
 * The brodacast-type message container class.
 */
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

/*
 * A container class to deal with the CAN bus API.
 */
class EsdResources
{
public:
	EsdResources ();
	~EsdResources ();

	bool initialize (const EsdMessageSnifferParameters& parms);
	bool uninitialize (void);
	bool read (void);
	bool error (const CMSG& m);
	bool startPacket (void);
	bool addMessage (int msg_id, int joint);
	bool writePacket (void);
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

	bool _error_status;							/// error status of the last packet (true = error).

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	
	char _printBuffer[16384];                   /// old-style static print buffer (should be used only for debugging).
};

EsdResources::EsdResources ()
{
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;						/// default 1Mbit/s
	_networkN = 0;

    ACE_OS::memset(_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	ACE_OS::memset(_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	ACE_OS::memset(_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	ACE_OS::memset(_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);

	_my_address = 0;
	_njoints = 0;
	_p = NULL;

	_readMessages = 0;
	_msg_lost = 0;
	_writeMessages = 0;
	_bcastRecvBuffer = NULL;

	_error_status = false;
}

EsdResources::~EsdResources () 
{ 
	uninitialize(); 
}

bool EsdResources::initialize (const EsdMessageSnifferParameters& parms)
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
	_error_status = false;

    ACE_OS::memcpy (_destinations, parms._destinations, sizeof(unsigned char)*ESD_MAX_CARDS);
	_my_address = parms._my_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_njoints = parms._njoints;	
	_p = parms._p;

	_txQueueSize = parms._txQueueSize;
	_rxQueueSize = parms._rxQueueSize;
	_txTimeout = parms._txTimeout;
	_rxTimeout = parms._rxTimeout;

	_bcastRecvBuffer = new BCastBufferElement[_njoints];
	ACE_ASSERT (_bcastRecvBuffer != 0);

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


bool EsdResources::uninitialize ()
{
	if (_bcastRecvBuffer != NULL) 
        {
            delete[] _bcastRecvBuffer;
            _bcastRecvBuffer = NULL;
        }

	if (_handle != ACE_INVALID_HANDLE)
        {
            int res = canClose (_handle);
            if (res != NTCAN_SUCCESS)
                return false;
            _handle = ACE_INVALID_HANDLE;
        }
	
	return true;
}


bool EsdResources::read (void)
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _readBuffer, &messages); 
	if (res != NTCAN_SUCCESS)
		return false;

	_readMessages = messages;
	return true;
}

bool EsdResources::startPacket (void)
{
	_writeMessages = 0;
	return true;
}

bool EsdResources::addMessage (int msg_id, int joint)
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

bool EsdResources::writePacket ()
{
	if (_writeMessages < 1)
		return false;

	long len = _writeMessages;

	int res = canSend (_handle, _writeBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _writeMessages)
		return false;

	return true;
}

bool EsdResources::printMessage (const CMSG& m)
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


bool EsdResources::dumpBuffers (void)
{
	if (!_p) return false;

	int j;

	/// dump the error.
	(*_p) ("Sniffer: write buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_writeBuffer[j]);

	(*_p) ("Sniffer: reply buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_replyBuffer[j]);

	(*_p) ("Sniffer: read buffer\n");
	for (j = 0; j < _readMessages; j++)
		printMessage (_readBuffer[j]);
	(*_p) ("Sniffer: -------------\n");

	return true;
}

bool EsdResources::error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

///
///
///
inline EsdResources& RES(void *res) { return *(EsdResources *)res; }


EsdMessageSniffer::EsdMessageSniffer(void) :
    ImplementPidControl<EsdMessageSniffer, IPidControl>(this),
    ImplementAmplifierControl<EsdMessageSniffer, IAmplifierControl>(this),
    ImplementEncoders<EsdMessageSniffer, IEncoders>(this),
	_mutex(1), 
    _done(0)
{
	system_resources = (void *) new EsdResources;
	ACE_ASSERT (system_resources != NULL);
}


EsdMessageSniffer::~EsdMessageSniffer ()
{
	if (system_resources != NULL)
		delete (EsdResources *)system_resources;
	system_resources = NULL;
}

bool EsdMessageSniffer::open(yarp::os::Searchable& config)
{
    Property p;
    p.fromString(config.toString());

    if (!p.check("GENERAL")) {
        ACE_OS::fprintf(stderr, "Cannot understand configuration parameters\n");
        return false;
    }

    int i;
    int nj = p.findGroup("GENERAL").find("Joints").asInt();
    EsdMessageSnifferParameters params(nj);
    params._njoints = nj;

    ///// CAN PARAMETERS
    Bottle &xtmp = p.findGroup("CAN").findGroup("CanDeviceNum");
    params._networkN=xtmp.get(1).asInt();

    xtmp = p.findGroup("CAN").findGroup("CanMyAddress");
    params._my_address=xtmp.get(1).asInt();

    xtmp = p.findGroup("CAN").findGroup("CanPollingInterval");
    params._polling_interval=xtmp.get(1).asInt();

    xtmp = p.findGroup("CAN").findGroup("CanTimeout");
    params._timeout=xtmp.get(1).asInt();

    xtmp = p.findGroup("CAN").findGroup("CanAddresses");
    for (i = 1; i < xtmp.size(); i++) params._destinations[i-1] = (unsigned char)(xtmp.get(i).asInt());
   
    ////// GENERAL
    xtmp = p.findGroup("GENERAL").findGroup("AxisMap");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._axisMap[i-1] = xtmp.get(i).asInt();
    xtmp = p.findGroup("GENERAL").findGroup("Encoder");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._angleToEncoder[i-1] = xtmp.get(i).asDouble();
    xtmp = p.findGroup("GENERAL").findGroup("Zeros");
	ACE_ASSERT (xtmp.size() == nj+1);
    for (i = 1; i < xtmp.size(); i++) params._zeros[i-1] = xtmp.get(i).asDouble();

	if (p.findGroup("GENERAL").find("Verbose").asInt() == 1)
	    params._p = ACE_OS::printf;
	else
		params._p = NULL;

    return open(params);
}

bool EsdMessageSniffer::open (EsdMessageSnifferParameters& parms)
{
	_mutex.wait();
	EsdResources& r = RES (system_resources);
	if (!r.initialize (parms))
        {
            _mutex.post();
            return false;
        }

    // initialize the implementation.
    ImplementPidControl<EsdMessageSniffer, IPidControl>::
        initialize(parms._njoints, parms._axisMap, parms._angleToEncoder, parms._zeros);
    ImplementAmplifierControl<EsdMessageSniffer, IAmplifierControl>::
        initialize(parms._njoints, parms._axisMap, parms._angleToEncoder, parms._zeros);
    ImplementEncoders<EsdMessageSniffer, IEncoders>::
        initialize(parms._njoints, parms._axisMap, parms._angleToEncoder, parms._zeros);

	start();
	_done.wait();

	// used for printing debug messages.
	_p = parms._p;
	_writerequested = false;
	_noreply = false;

	_mutex.post ();

	return true;
}

bool EsdMessageSniffer::close (void)
{
	EsdResources& d = RES(system_resources);
    Thread::stop ();
    ImplementPidControl<EsdMessageSniffer, IPidControl>::uninitialize();
    ImplementAmplifierControl<EsdMessageSniffer, IAmplifierControl>::uninitialize();
    ImplementEncoders<EsdMessageSniffer, IEncoders>::uninitialize();
	return d.uninitialize ();
}

//
//
//
void EsdMessageSniffer::run(void)
{
	EsdResources& r = RES (system_resources);

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
	
	r._error_status = false;

	/// ok, init completed. 
	_done.post ();

	while (!isStopping() || messagePending)
        {
            before = Time::now();

            _mutex.wait ();
            if (r.read () != true)
                if (r._p) 
                    (*r._p) ("Sniffer: read failed\n");

            // handle broadcast messages.
            // (class 1, 8 bits of the ID used to define the message type and source address).
            //
            for (i = 0; i < r._readMessages; i++)
                {
                    CMSG& m = r._readBuffer[i];
                    if (m.len & NTCAN_NO_DATA)
                        if (r._p)
                            {
                                (*r._p) ("Sniffer: error in message %x len: %d type: %x: %x\n",
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
                                        (*r._p) ("Sniffer: error in message %x len: %d type: %x: %x\n", 
                                                 m.id, m.len, m.data[0], m.msg_lost);
						
                                        continue;
                                    }

                            if (((m.id &0x700) == 0) && 
                                ((m.data[0] & 0x7f) < NUM_OF_MESSAGES))
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
                                                                            (*r._p) ("Sniffer: message %x was already replied\n", m.id);
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
                                                                            r._error_status = false;
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
                            r._error_status = false;
                            goto AckMessageLoop;
                        }

                    /// timeout
                    counter ++;
                    if (counter > r._timeout)
                        {	
                            /// complains.
                            if (r._p)
                                {
                                    (*r._p) ("Sniffer: timeout - still %d messages unacknowledged\n", remainingMsgs);
                                    r.dumpBuffers ();
                                }

                            messagePending = false;
                            r._error_status = true;
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
                                                    (*r._p) ("Sniffer: write message of %d elments failed\n", r._writeMessages);
                                                }
                                        }
                                    else
                                        {
                                            messagePending = true;
                                            _writerequested = false;
                                            remainingMsgs = r._writeMessages;
                                            noreply = _noreply;
                                            r._error_status = false;
                                            counter = 0;
                                            memset (r._replyBuffer, 0, sizeof(CMSG) * r._writeMessages);

                                            if (r._p)
                                                {
                                                    int j;
                                                    for (j = 0; j < r._writeMessages; j++)
                                                        {
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
                    if (r._p) (*r._p)("Sniffer: thread can't poll fast enough (time: %f)\n", now-before);
                    //before = now;
                }
        }

	///
}


bool EsdMessageSniffer::setDebugPrintFunction (void *cmd)
{
	_mutex.wait();
	EsdResources& r = RES(system_resources);
	_p = (int (*) (const char *fmt, ...))cmd;
	_mutex.post();
	return true;
}


///
/// 
/// control card commands.
///
inline bool EsdMessageSniffer::ENABLED (int axis)
{
	EsdResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & ESD_CAN_SKIP_ADDR) == 0) ? true : false;
}

// return the number of controlled axes.
bool EsdMessageSniffer::getAxes(int *ax)
{
	EsdResources& r = RES(system_resources);
    *ax = r.getJoints();
    return true;
}

/// sets the broadcast policy for a given board (don't need to be called twice).
/// the parameter is a 32-bit integer: bit X = 1 -> message X = active
/// e.g. 0x02 activates the broadcast of position information
///		 0x04 activates the broadcast of velocity ...
///
bool EsdMessageSniffer::setBCastMessages (int i, int value)
{
	ACE_ASSERT (i >= 0 && i <= (ESD_MAX_CARDS-1)*2);
	return _writeDWord (CAN_SET_BCAST_POLICY, i, S_32(value));
}

///
/// reads an array of double from the broadcast message position buffer.
/// LATER: add a check of timing/error message.
///
bool EsdMessageSniffer::getBCastPosition (int i, double *value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (i >= 0 && i <= r.getJoints());

    _mutex.wait();
	*value = double(r._bcastRecvBuffer[i]._position);
    _mutex.post();

	return true;
}

bool EsdMessageSniffer::getBCastPositions (double *p)
{
	EsdResources& r = RES(system_resources);
	int i;
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            p[i] = double(r._bcastRecvBuffer[i]._position);
        }
    _mutex.post();
	return true;
}

bool EsdMessageSniffer::getBCastPIDOutput (int i, double *value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (i >= 0 && i <= r.getJoints());
	
    _mutex.wait();
	*value = double(r._bcastRecvBuffer[i]._pid_value);
    _mutex.post();

	return true;
}

bool EsdMessageSniffer::getBCastPIDOutputs (double *p)
{
	EsdResources& r = RES(system_resources);
	int i;
    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            p[i] = double(r._bcastRecvBuffer[i]._pid_value);
        }
    _mutex.post();
	return true;
}

bool EsdMessageSniffer::getBCastCurrent (int i, double *value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (i >= 0 && i <= r.getJoints());
	
    _mutex.wait();
	*value = double(r._bcastRecvBuffer[i]._current);
    _mutex.post();

	return true;
}

bool EsdMessageSniffer::getBCastCurrents (double *p)
{
	EsdResources& r = RES(system_resources);
	int i;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            p[i] = double(r._bcastRecvBuffer[i]._current);
        }
    _mutex.post();

	return true;
}

bool EsdMessageSniffer::getBCastFaults (int *p)
{
	EsdResources& r = RES(system_resources);
	int i;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            p[i] = short(r._bcastRecvBuffer[i]._fault);
        }
    _mutex.post();

	return true;
}

bool EsdMessageSniffer::getBCastPositionError (int i, double *value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (i >= 0 && i <= r.getJoints());
	
    _mutex.wait();
	*value = double(r._bcastRecvBuffer[i]._position_error);
    _mutex.post();

	return true;
}


bool EsdMessageSniffer::getBCastPositionErrors (double *p)
{
	EsdResources& r = RES(system_resources);
	int i;

    _mutex.wait();
	for (i = 0; i < r.getJoints(); i++)
        {
            p[i] = double(r._bcastRecvBuffer[i]._position_error);
        }
    _mutex.post();

	return true;
}


///
/// helper functions.
///
///
///

/// sends a message without parameters
bool EsdMessageSniffer::_writeNone (int msg, int axis)
{
	EsdResources& r = RES(system_resources);
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

bool EsdMessageSniffer::_readWord16 (int msg, int axis, short& value)
{
	EsdResources& r = RES(system_resources);
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

	if (r.getErrorStatus() != true)
        {
            value = 0;
            return false;
        }

	value = *((short *)(r._replyBuffer[0].data+1));
	return true;
}

/// reads an array.
bool EsdMessageSniffer::_readWord16Array (int msg, double *out)
{
	EsdResources& r = RES(system_resources);
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

	if (r.getErrorStatus() != true)
        {
            ACE_OS::memset (out, 0, sizeof(double) * r.getJoints());
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
bool EsdMessageSniffer::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	EsdResources& r = RES(system_resources);
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
bool EsdMessageSniffer::_writeDWord (int msg, int axis, int value)
{
	EsdResources& r = RES(system_resources);
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
bool EsdMessageSniffer::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	EsdResources& r = RES(system_resources);

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
bool EsdMessageSniffer::_readDWord (int msg, int axis, int& value)
{
	EsdResources& r = RES(system_resources);
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

	if (r.getErrorStatus() != true)
        {
            value = 0;
            return false;
        }

	value = *((int *)(r._replyBuffer[0].data+1));
	return true;
}

/// reads an array of double words.
bool EsdMessageSniffer::_readDWordArray (int msg, double *out)
{
	EsdResources& r = RES(system_resources);
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

	if (r.getErrorStatus() != true)
        {
            ACE_OS::memset (out, 0, sizeof(double) * r.getJoints());
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

