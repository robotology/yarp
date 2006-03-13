/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNameID.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNameID_INC
#define YARPNameID_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/MEM_Addr.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPString.h>
#include <yarp/YARPNetworkTypes.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPNameID.h Defines the endpoint name id. Encapsulates the socket id and 
 * network mode flag.
 */

/**
 * Encapsulates an identifier of a specific port. This may include a socket id and 
 * network mode flag. Generally, the user should not care about this at all. 
 * Objects of this type and of derived types are used as ID's for endpoints. 
 * Generally keeping an instance of one of this classes allows calling endpoint 
 * (e.g. socket) creation/deletion routines. The only constraint is that the 
 * information about the real socket is only accessible by the thread that created
 * the socket (this is probably an unnecessary limitation that might disappear
 * in future releases). Variables contained in name ID's are used to provide
 * parameters for socket creation and operation.
 * @see YARPProtocols enumeration for permitted modes.
 */
class YARPNameID
{
protected:
	/**
	 * Type of connection.
	 */
	int _mode;			// mode socket, Qnet, etc.

	/**
	 * Connection-specific identifier.  Typically a socket handle.
	 */
	ACE_HANDLE _raw_id;	// OS id, e.g. socket id.

public:
	/**
	 * Default constructor.
	 */
	YARPNameID () { _raw_id = ACE_INVALID_HANDLE;  _mode = YARP_NO_SERVICE_AVAILABLE; }

	/**
	 * Copy contructor.
	 * @param other is the YARPNameID to copy from.
	 */
	YARPNameID (const YARPNameID& other)
	{
		_mode = other._mode;
		_raw_id = other._raw_id;
	}

	/**
	 * Constructor.
	 *
	 * @param n_mode Type of connection.
	 * @param n_raw_id Connection-specific handle.
	 */
	YARPNameID (int n_mode, ACE_HANDLE n_raw_id) { _mode = n_mode;  _raw_id = n_raw_id; }

	/**
	 * Destructor.
	 */
	virtual ~YARPNameID() {}

	/**
	 * Checks whether two ID's are the same.
	 * @param other is the peer to compare to.
	 * @return true if the two objects are the same (internally), false otherwise.
	 */
	int operator == (const YARPNameID& other) { return (_raw_id == other._raw_id) && (_mode == other._mode); }

	/**
	 * Checks whether two ID's are the same.
	 * @param other is the peer to compare to.
	 * @return true if the two objects are different (internally).
	 */
	int operator != (const YARPNameID& other) { return !(operator == (other)); }

	/**
	 * Gets the service type.
	 * @return the type of service identified by this object.
	 */
	inline int getServiceType (void) const { return _mode&YARP_PROTOCOL_MASK; }

	/**
	 * Tests the required replies flag.
	 * @return 1 if the messages sent through endpoints identified by
	 * this object require replies (ack).
	 */
	inline int getRequireAck (void) const { return (_mode&YARP_REQUIRE_ACK)!=0; }

	/**
	 * Fills the service type variable.
	 * @param type is the service type (upd, tcp, etc.).
	 */
	inline void setServiceType (int type) { _mode = type; }

	/**
	 * Sets the require acknowledge flag. This influences the behavior of communication
	 * enpoints created using this ID.
	 * @param flag 1 means replies are used, 0 they're not.
	 */
	inline void setRequireAck (int flag) { _mode = (_mode&YARP_PROTOCOL_MASK) | (flag?YARP_REQUIRE_ACK:0); }

	/**
	 * Gets the ID of the socket.
	 * @return the ACE_HANDLE of the socket associated with the endpoint.
	 */
	inline ACE_HANDLE getRawIdentifier (void) const { return _raw_id; }

	/**
	 * Fills the socket ID variable.
	 * @param id is the ACE_HANDLE of the socket. This has different meanings depending
	 * on the type of protocol and input vs output connection.
	 */
	inline void setRawIdentifier (ACE_HANDLE id) { _raw_id = id; }

	/**
	 * Tests whether this ID refers to a valid socket.
	 * @return 1 if the socket is a valid handle, 0 otherwise.
	 */
	inline int isValid(void) const { return (_raw_id != ACE_INVALID_HANDLE) ? 1 : 0; }

	/**
	 * Tests whether the ID is valid.
	 * @return 1 if the socket in NOT valid.
	 */
	inline int isError(void) const { return (_raw_id == ACE_INVALID_HANDLE); }

	/**
	 * Tests whether the ID is consistent with a given protocol.
	 * @param n_mode is the service type (tcp, udp, etc.).
	 * @return 1 if the ID is consistent with @a n_mode.
	 */
	inline int isConsistent(int n_mode) const { return (getServiceType() == n_mode); }

	/**
	 * Invalidates this name ID by cleaning the socket ID.
	 */
	void invalidate() { _raw_id = ACE_INVALID_HANDLE;  _mode = YARP_NO_SERVICE_AVAILABLE; }
};

/**
 * A NULL name ID.
 */
#define YARP_NAMEID_NULL YARPNameID()

/**
 * YARPUniqueNameID is an extension to the simple name ID (YARPNameID).
 */
class YARPUniqueNameID : public YARPNameID
{
protected:
	/** the symbolic name of the connection as registered on the name server */
	YARPString _symbolic_name;

public:
	/**
	 * Default constructor.
	 */
	YARPUniqueNameID (void) : YARPNameID() { _symbolic_name = "__null"; }

	/**
	 * A simple constructor.
	 * @param service is the service type. It only sets the type of service
	 * provided by the endpoint.
	 */
	YARPUniqueNameID (int service) : YARPNameID(service, ACE_INVALID_HANDLE) { _symbolic_name = "__null"; }

	/**
	 * A copy constructor.
	 * @param other is the ID to copy from.
	 */
	YARPUniqueNameID (const YARPUniqueNameID& other) : YARPNameID(other) { _symbolic_name = other._symbolic_name; }

	/**
	 * Destructor.
	 */
	virtual ~YARPUniqueNameID() {}

	/**
	 * Gets the YARPNameID part of this object.
	 * @return a YARPNameID reference of this object.
	 */
	inline YARPNameID& getNameID(void) { return (YARPNameID &)(*this); }

	/**
	 * Explicitly sets the symbolic name of this ID.
	 * @param name is the name of the connection.
	 */
	inline void setName (const char *name) { _symbolic_name = name; }

	/**
	 * Explicitly sets the symbolic name of this ID.
	 * @param name is the name of the connection as a YARPString.
	 */
	inline void setName (const YARPString& name) { _symbolic_name = name; }

	/**
	 * Gets the name of the connection.
	 * @return a YARPString containing the name of the connection.
	 */
	inline YARPString getName(void) const { return _symbolic_name; }

	/**
	 * The copy operator for this object.
	 * @param other is the object to copy from.
	 * @return a reference to this object.
	 */
	YARPUniqueNameID& operator= (const YARPUniqueNameID& other) 
	{
		_symbolic_name = other._symbolic_name;
		YARPNameID::operator= (other);
		return *this;
	}
};



/**
 * YARPUniqueNameQnx is a specialized version of YARPUniqueNameID for
 * managing YARP_QNET connections. Every QNET connection is identified by 
 * three variables: the pid (of the process), the channel ID (associated
 * with the created channel) and the name of the node (a string) where
 * the connection is going to. These three parameters allow connecting
 * to an already enstablished connection for reading/writing into the
 * channel.
 */
class YARPUniqueNameQnx : public YARPUniqueNameID
{
protected:
	/** the pid of the connection */
	int _pid;
	/** the channel id of the connection */
	int _channel_id;
	/** the node ID of the QNET connection */
	YARPString _node;

public:
	/**
	 * Constructor.
	 * @param service is the type of service, generally YARP_QNET but sometimes
	 * YARP_NO_SERVICE_AVAILABLE is found here.
	 */
	YARPUniqueNameQnx (int service = YARP_QNET) : YARPUniqueNameID(service)
	{
		ACE_ASSERT (service == YARP_QNET || service == YARP_NO_SERVICE_AVAILABLE);
		_pid = -1;
		_channel_id = -1;
	}

	/**
	 * Constructor.
	 * @param service is the type of service, generally YARP_QNET but sometimes
	 * YARP_NO_SERVICE_AVAILABLE is found here.
	 * @param pid is the pid of the QNET connection.
	 * @param channel is the channel of the connection.
	 * @param node is the name of the node of the connection.
	 */
	YARPUniqueNameQnx (int service, int pid, int channel, YARPString node) : YARPUniqueNameID(service)
	{
		_pid = pid;
		_channel_id = channel;
		_node = node;
	}

	/**
	 * Copy constructor.
	 * @param other is the name ID object to copy from.
	 */
	YARPUniqueNameQnx (const YARPUniqueNameQnx& other) : YARPUniqueNameID(other) 
	{
		_pid = other._pid;
		_channel_id = other._channel_id;
		_node = other._node;
	}

	/** 
	 * Destructor.
	 */
	virtual ~YARPUniqueNameQnx() {}

	/**
	 * Gets the pid of the connection.
	 * @return the pid of the connection.
	 */
	int getPid() const { return _pid; }

	/**
	 * Gets the channel ID associated with the name object.
	 * @return the channel ID.
	 */
	int getChannelID() const { return _channel_id; }

	/**
	 * Sets the pid of the connection.
	 * @param pid is the pid of the QNET connection.
	 */
	void setPid(int pid) { _pid = pid; }

	/**
	 * Sets the channel ID of the connection.
	 * @param channel is the channel ID.
	 */
	void setChannelID(int channel) 
	{ 
		_channel_id = channel; 
		setRawIdentifier ((ACE_HANDLE)channel);
	} 

	/**
	 * Gets the node name.
	 * @return a YARPString containing the name of the QNET node.
	 */
	YARPString getNode (void) const { return _node; }

	/**
	 * Sets the node name.
	 * @param node is the name of the QNET node.
	 */
	void setNode (const YARPString& node) { _node = node; }
};

/**
 * YARPUniqueNameSock is a specialized class for dealing with
 * name ID's of socket endpoints. It contains the usual details
 * of socket addressing.
 */
class YARPUniqueNameSock : public YARPUniqueNameID
{
protected:
	/** The INET address of the socket. */
	ACE_INET_Addr _address;
	/** The interface name of the NIC, this is used by MCAST to determine
	 on which network send messages to. */
	YARPString _ifname;
	/** The number of IP ports associated with this endpoint */
	int _nports;
	/** An array of IP ports associated with this endpoint */
	int *_ports;

public:
	/**
	 * Constructor.
	 * @param service it can be one of the YARPProtocols enumeration (tcp, udp, etc.).
	 */
	YARPUniqueNameSock (int service = YARP_MCAST) : YARPUniqueNameID(service), _address(1111) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}
	
	/**
	 * Constructor.
	 * @param service it can be one of the YARPProtocols enumeration (tcp, udp, etc.).
	 * @param port is the port number associated with the INET address.
	 * @param hostname is the local hostname.
	 */
	YARPUniqueNameSock (int service, int port, char *hostname) : YARPUniqueNameID(service), _address (port, hostname) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}
	
	/**
	 * Constructor.
	 * @param service it can be one of the YARPProtocols enumeration (tcp, udp, etc.).
	 * @param addr is the INET address associated with the socket endpoint.
	 */
	YARPUniqueNameSock (int service, const ACE_INET_Addr& addr) : YARPUniqueNameID(service), _address (addr) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}

	/**
	 * Copy constructor.
	 * @param other is a reference to the object to copy from.
	 */
	YARPUniqueNameSock (const YARPUniqueNameSock& other) : YARPUniqueNameID(other) ///YARPUniqueNameID (other->getServiceType()), _address (other->_address)
	{
		_address = other._address;
		_ifname = other._ifname;
		
		_nports = -1;
		_ports = NULL;

		if (other._nports > 0 && other._ports != NULL)
		{
			setPorts (other._ports, other._nports);
		}
	}

	/**
	 * Destructor. Frees allocated memory.
	 */
	virtual ~YARPUniqueNameSock() 
	{
		if (_ports != NULL)	delete[] _ports;
	}

	/**
	 * Gets the address of the socket.
	 * @return a reference to the ACE_INET_Addr stored internally.
	 */
	inline ACE_INET_Addr& getAddressRef (void) { return _address; }

	/**
	 * Gets the number of IP ports managed by this endpoint.
	 * @return a reference to the class member variable.
	 */
	inline int& getNPorts (void) { return _nports; }

	/**
	 * Gets the array of IP ports.
	 * @return a pointer to the array.
	 */
	inline int* getPorts (void) { return _ports; }

	/**
	 * Gets the name of the interface. Depending on the
	 * architecture this can be either an IP address or the network
	 * card name (e.g. lo0, eth).
	 * @return a reference to a YARPString containing the interface name.
	 */
	inline YARPString& getInterfaceName(void) { return _ifname; }

	/**
	 * Sets the interface name.
	 * @param name is a reference to a YARPString containing the name.
	 */
	inline void setInterfaceName (const YARPString& name) { _ifname = name; }

	/**
	 * Sets the IP port array. It allocates memory when needed.
	 * @param p is a pointer to an array of ports.
	 * @param size is the size of the array.
	 * @return YARP_OK or asserts.
	 */
	inline int setPorts (int *p, int size) 
	{
		ACE_ASSERT (p != NULL && size > 0);

		if (_nports != size)
			delete[] _ports;
		_nports = size;
		_ports = new int[_nports];
		ACE_ASSERT (_ports != NULL);

		memcpy (_ports, p, sizeof(int) * _nports); 
		return YARP_OK; 
	}
	
#if defined(__DARWIN__) || defined(__WIN32__) || defined(__QNX__)
	// Minor variant on other setPorts method
	// Deals with NetInt32's, which are not ints on Darwin
	inline int setPorts (NetInt32 *p, int size) 
	{
		ACE_ASSERT (p != NULL && size > 0);

		if (_nports != size)
			delete[] _ports;
		_nports = size;
		_ports = new int[_nports];
		ACE_ASSERT (_ports != NULL);

		for (int i=0; i<_nports; i++) {
			_ports[i] = (int)p[i];
		}
		return YARP_OK; 
	}
#endif

	
	/**
	 * Frees memory associated with ports.
	 * @return YARP_OK always.
	 */
	inline int releasePorts (void)
	{
		if (_ports != NULL)
			delete[] _ports;
		_ports = NULL;
		_nports = -1;

		return YARP_OK;
	}

	/**
	 * Copy operator. It does a deep copy of the class.
	 * @param other is a reference to a class to copy from.
	 * @return a reference to this.
	 */
	YARPUniqueNameSock& operator= (const YARPUniqueNameSock& other)
	{
		if (_nports != other._nports)
		{
			delete[] _ports;
			_nports = other._nports;
			if (_nports > 0)
			{
				_ports = new int[_nports];
				ACE_ASSERT (_ports != NULL);
			}
			else
				_ports = NULL;
		}

		if (_nports > 0)
		{
			memcpy (_ports, other._ports, sizeof(int) * _nports);
		}

		_address = other._address;
		_ifname = other._ifname;

		YARPUniqueNameID::operator= (other);
		return *this;
	}
};

/**
 * YARPUniqueNameMem is a specialized class for dealing with
 * SHMEM connection endpoints.
 */
class YARPUniqueNameMem : public YARPUniqueNameID
{
protected:
	/** The shmem address type. */
	ACE_MEM_Addr _address;

public:
	/** 
	 * Constructor. 
	 * @param service is the service type. This is either YARP_SHMEM or YARP_NO_SERVICE_AVAILABLE.
	 */
	YARPUniqueNameMem (int service = YARP_SHMEM) : YARPUniqueNameID(service), _address() {}

	/**
	 * Constructor.
	 * @param service is the service type.
	 * @param port is the IP port number associated with the shmem address.
	 */
	YARPUniqueNameMem (int service, int port) : YARPUniqueNameID(service), _address (port) {  }

	/**
	 * Constructor.
	 * @param service is the service type.
	 * @param addr is the address represented by an ACE_MEM_Addr.
	 */
	YARPUniqueNameMem (int service, const ACE_MEM_Addr& addr) : YARPUniqueNameID(service), _address (addr) {}

	/**
	 * Copy contructor.
	 * @param other is the reference to the class to copy from.
	 */
	YARPUniqueNameMem (const YARPUniqueNameMem& other) 
		///: YARPUniqueNameID(YARP_SHMEM), _address(other->_address) {}
		: YARPUniqueNameID (other)
	{
		/// make sure is SHMEM.
		setServiceType (YARP_SHMEM);
		_address = other._address;
	}
	
	/**
	 * Destructor.
	 */
	virtual ~YARPUniqueNameMem() {}

	/**
	 * Gets the address.
	 * @return a reference to the internal ACE_MEM_Addr variable.
	 */
	inline ACE_MEM_Addr& getAddressRef (void) { return _address; }
};


/// service types.
///
///	YARP_NO_SERVICE_AVAILABLE	= -1,
///	YARP_UDP					= 0,
///	YARP_TCP					= 1,
///	YARP_MCAST					= 2,
///	YARP_QNET					= 3,
/// YARP_SHMEM					= 4,
///


///
/// contains the assigned port/IP address.
///	a bit more generic than the address/port pair only.
///
///

#endif
