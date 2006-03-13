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
/// $Id: YARPNameService.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNameService_INC
#define YARPNameService_INC

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPNameID.h>

/**
 * \file YARPNameService.h interface to the name server (yarp-service) and
 * high level interface to the creation of communication endpoints.
 * Creation of an endpoint involves calling the name server.
 */

#define NAMER_CONFIG_FILE "conf/namer.conf"

/// don't optimize constructor is actually for troubles I encountered under gcc QNX6.
#define DONT_OPTIMIZE_CONSTRUCTOR if (0) { int i; i++; }

/**
 * High level interface to the name server. It connects using tcp to the
 * name server and send certain messages to access the name server services.
 * This high level interface can serve both socket communication and QNET.
 */
class YARPNameService
{
public:
	/**
	 * Constructor.
	 */
	YARPNameService () { DONT_OPTIMIZE_CONSTRUCTOR; Initialize (); }

	/**
	 * Destructor.
	 */
	~YARPNameService () { Finalize (); }

	/**
	 * Does nothing.
	 * @param name is ignored.
	 * @return always YARP_OK.
	 */ 
	static int ConnectNameServer(const char *name = NULL);

	/**
	 * Initializes the global object (singleton) by reading the configuration file.
	 * @return YARP_OK on success.
	 */
	static int Initialize (void);

	/**
	 * Finilizes the use of the class by cleaning memory.
	 * @return YARP_OK on success.
	 */
	static int Finalize (void);

	/**
	 * Registers a new name into the name server.
	 * @param name is the name to be registered (a symbolic name contained in the string).
	 * @param network_name is the name of the network to be used (YARP supports multiple nic's on the same machine).
	 * @param reg_type is one of the enumeration in YARP_NameID_defs.h, typically the protocol type (UDP is generally used for registration).
	 * @param num_ports_needed is the number of ip ports to reserve (i.e. in practice the max number of incoming connections).
	 * @return a pointer to a YARPUniqueNameID which as been allocated on the fly (DeleteName() must be called to free memory).
	 */
	static YARPUniqueNameID* RegisterName (const char *name, const char *network_name, int reg_type = YARP_DEFAULT_PROTOCOL, int num_ports_needed = YARP_PROTOCOL_REGPORTS);

	/**
	 * Locates a name (if exists) on the name server.
	 * @param name is the name to be searched.
	 * @param network_name is the name of the network to be used (YARP supports multiple nic's on the same machine).
	 * @param name_type is one of the enumeration in YARP_NameID_defs.h, typically the protocol type.
	 * @return a pointer to a YARPUniqueNameID which as been allocated on the fly (DeleteName() must be called to free memory).
	 */
	static YARPUniqueNameID* LocateName (const char *name, const char *network_name = NULL, int name_type = YARP_NO_SERVICE_AVAILABLE);

	/**
	 * Just deallocates memory, call UnregisterName() to actually remove the
	 * name from the name server (call this after un-registration).
	 * @param the pointer to the YARPUniqueNameID to be freed.
	 * @return YARP_OK on success.
	 */
	static int DeleteName (YARPUniqueNameID* pid);

	/**
	 * Unregisters the name from the name server.
	 * @param the pointer to the YARPUniqueNameID to be unregistered.
	 * @return YARP_OK on success.
	 */
	static int UnregisterName (YARPUniqueNameID* pid);

	/**
	 * Queries the name server for an IP and netname association. Returns the 
	 * interface name (IP or symbolic) and true if IP and netname belong to the same subnet.
	 * @param ip is the ip address to be searched.
	 * @param network name is the network name to compare with.
	 * @param ifname is the name of the interface (ip address on Windows, device name on unix).
	 * @return true if the ip belongs to the subnet identified by the network name.
	 */
	static bool VerifySame (const char *ip, const char *network_name, YARPString& ifname);

	/**
	 * Verifies whether two ip's are actually on the same machine. This is needed to
	 * decide whether to start a SHMEM connection.
	 * Also checks whether the two ip's are on the same network.
	 * @param rem_ip is the address of the remote connection.
	 * @param loc_ip is the address of the local endpoint.
	 * @param network_name is the network of the local endpoint.
	 * @return true if a shared memory connection is possible.
	 */
	static bool VerifyLocal (const char *rem_ip, const char *loc_ip, const char *network_name);

  static int CheckProperty(const char *name,
			   const char *key,
			   const char *value);

};


/**
 * YARPEndpointManage is the manager of the communication endpoints. 
 * This class can create both socket and QNET endpoints.
 */
class YARPEndpointManager
{
public:
	/**
	 * Creates an input endpoint. Depending on the protocol creates a socket or a qnet endpoint.
	 * Some more details can be found on the documentation of YARPSocket.
	 * @param name is the unique name id obtained by registering the symbolic name to the name
	 * server.
	 * @return YARP_OK on success.
	 */
	static int CreateInputEndpoint(YARPUniqueNameID& name);

	/**
	 * Creates an output endpoint. Depending on the protocol creates a socket or a qnet endpoint.
	 * Some more details can be found on the documentation of YARPSocket.
	 * @param name is the unique name id obtained by registering the symbolic name to the name
	 * server.
	 * @return YARP_OK on success.
	 */
	static int CreateOutputEndpoint(YARPUniqueNameID& name);

	/**
	 * Connects the local endpoint with the destination. The local endpoint is acquired
	 * by looking to the socket associated with the current thread.
	 * Some more details can be found on the documentation of YARPSocket.
	 * @param dest is the unique name of the destination endpoint (this is queried from the 
	 * name server).
	 * @param own_name is the symbolic name of the owner of the connection. The name is sent 
	 * to the remote to allow identifying the specific connection being created.
	 * @return YARP_OK on success.
	 */
	static int ConnectEndpoints(YARPUniqueNameID& dest, const YARPString& own_name);

	/**
	 * Closes an endpoint.
	 * @param endp is the unique name of the endpoint to be closed. The calling thread
	 * must be the owner.
	 * @return YARP_OK on success.
	 */
	static int Close(YARPUniqueNameID& endp);

	/**
	 * Closes all MCAST connections. It only works for output sockets.
	 * @return YARP_OK on success.
	 */
	static int CloseMcastAll(void);

	/**
	 * Returns the number of clients. Called by MCAST sockets only.
	 * The behavior is undefined (e.g. segmentation violation) if called by a non-MCAST socket.
	 * @return the number of connected MCAST listeners on success or YARP_FAIL on failure.
	 */
	static int GetNumberOfClients(void);

	/**
	 * Disable the Nagle's algorithm for the endpoint. Beware that this might affect 
	 * the performance of communication. Make sure you know the effects of it.
	 * @param endp is the endpoint.
	 * @return YARP_OK on success.
	 */
	static int SetTCPNoDelay (const YARPUniqueNameID& endp);

	/**
	 * Print existing connection parameters to stdout.
	 * @param endp is the endpoint to print data for.
	 * @return YARP_OK on success.
	 */
	static int PrintConnections (const YARPUniqueNameID& endp);

};

#endif

