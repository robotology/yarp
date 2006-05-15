// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
///
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
/// $Id: YARPSocketNameService.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

/*
  paulfitz Tue May 22 15:34:43 EDT 2001
*/

#ifndef YARPSocketNameService_INC
#define YARPSocketNameService_INC

///
/// Socket name service/server interface and managin communication endpoints
///		is now implemented by two separate classes.
///
///	NOW means Apr 2003.
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

/**
 * \file YARPSocketNameService.h This is the interface to the name server and the
 * creation and management of socket based endpoints.
 */

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPAll.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPNameClient.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPSocket.h>


/**
 * YARPSocketNameService is the socket based interface (using a tcp channel) to the
 * name server. It allows conveniently registering a name, locating it, or unregistering it.
 * Also, it allows checking whether a certain IP address belongs to a network name and
 * verifying whether two IP's are on the same machine.
 */
class YARPSocketNameService
{
public:
	/**
	 * Registers a name on the name server and gets the parameters of the connection
	 * back.
	 * @param namer is a reference to a namer object (typically a global instance will
	 * be created by the library).
	 * @param name is the symbolic name to register.
	 * @param network_name is the symbolic name of the network (see namer.conf file).
	 * @param reg_type is the protocol to register. This is one of the YARPProtocols enumeration.
	 * @param extra_param is an integer which has various uses depending on the protocol. For udp
	 * names contains the number of ports requested to the name server.
	 * @return a pointer to a name ID class (YARPUniqueNameID) containing the parameters and 
	 * result of the registration. Returns always a valid object, the user must take care
	 * of deleting it using delete.
	 */
	static YARPUniqueNameID* RegisterName (YARPNameClient& namer, const char *name, const char *network_name, int reg_type = YARP_DEFAULT_PROTOCOL, int extra_param = YARP_PROTOCOL_REGPORTS);

	/**
	 * Locates a name on the name server and returns the parameters required to form a
	 * connection to the port identified by the symbolic name.
	 * @param namer is a reference to a namer object (typically a global instance will
	 * be created by the library).
	 * @param name is the symbolic name to locate.
	 * @param network_name is the symbolic name of the network (see namer.conf file).
	 * @param reg_type is the protocol to register. This is one of the YARPProtocols enumeration.
	 * @return a pointer to a name ID class (YARPUniqueNameID) containing the parameters and 
	 * result of the registration. Returns always a valid object, the user must take care
	 * of deleting it using delete.
	 */
	static YARPUniqueNameID* LocateName (YARPNameClient& namer, const char *name, const char *network_name, int reg_type = YARP_DEFAULT_PROTOCOL);

	/**
	 * Deletes a name from the name server.
	 * @param namer is a reference to a namer object (typically a global instance will
	 * be created by the library).
	 * @param name is the name to delete.
	 * @param reg_type is the protocol the name belongs to.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int UnregisterName (YARPNameClient& namer, const char *name, int reg_type);

	/**
	 * Verifies whether @a ip and @a netname belong to the same network.
	 * @param namer is a reference to a namer object (typically a global instance will
	 * be created by the library).
	 * @param ip is the IP address, typically the caller.
	 * @param netname is the name of the network.
	 * @param if_name is the interface name, this is returned by the name server.
	 * @return true if @a ip and @a netname belong to the same network.
	 */
	static bool VerifySame (YARPNameClient& namer, const char *ip, const char *netname, YARPString& if_name);

	/**
	 * Verifies whether @a rem_ip and @a loc_ip are on the same machine.
	 * @param namer is a reference to a namer object (typically a global instance will
	 * be created by the library).
	 * @param rem_ip is one of the IP numbers to compare.
	 * @param loc_ip is the second argument of the comparison.
	 * @param netname is the name of the network the IP have been registered on.
	 * @return true if  @a rem_ip and @a loc_ip are on the same machine, false otherwise.
	 */
	static bool VerifyLocal (YARPNameClient& namer, const char *rem_ip, const char *loc_ip, const char *netname);
};


/**
 * YARPSocketEndpointManager manages socket communication endpoints. It contains methods
 * to instantiate, manage, and close endpoints. Enpoints contain sockets with various
 * properties, this is just a convenience class for dealing with creation and setting
 * parameters.
 */
class YARPSocketEndpointManager
{
public:
	/**
	 * Gets the socket associated with the caller thread. It queries the global list
	 * of associacions (thead, socket) and returns the socket object of the current
	 * thread. In YARP there's one socket per thread.
	 * @return a pointer to the socket object associated with the caller thread.
	 */
	static YARPNetworkObject *GetThreadSocket(void);

	/**
	 * Creates an input endpoint and associates it with the caller.
	 * @param name is the specification of the local endpoint 
	 * (e.g. port to listen for incoming connections).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int CreateInputEndpoint(YARPUniqueNameID& name);

	/**
	 * Creates an output endpoint and associates it with the caller.
	 * @param name is the specification of the local endpoint (e.g. the local address). 
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int CreateOutputEndpoint(YARPUniqueNameID& name);

	/**
	 * Connects the local output endpoint to a remote input endpoint.
	 * @param dest is the specification of the remote peer.
	 * @param own_name is the name of the port issuing the call (this is passed
	 * to the remote for identifying the caller).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int ConnectEndpoints(YARPUniqueNameID& dest, const YARPString& own_name);

	/**
	 * Closes the endpoint and frees memory.
	 * @param dest is the specification of the endpoint to close (e.g. protocol).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int Close(YARPUniqueNameID& dest);

	/**
	 * Closes all MCAST connections. This is a special method that talks to an
	 * output socket to terminate all MCAST connections. It tries to close all
	 * connections by talking to the listeners.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int CloseMcastAll(void);

	/**
	 * Gets the number of clients to an output MCAST connection. This is useful
	 * for deciding when to terminate multicasting packets on the network.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int GetNumberOfClients(void);

	/**
	 * For TCP sockets it disables the Nagle's algorithm. This can have an 
	 * impact on the network performance, use with care.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int SetTCPNoDelay(void);

	/**
	 * Prints existing connection parameters to stdout.
	 * @return YARP_OK on success.
	 */
	static int PrintConnections(void);
};

#endif


