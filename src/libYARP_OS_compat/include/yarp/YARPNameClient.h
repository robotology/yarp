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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNameClient.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

// YARPNameClient.h: interface for the YARPNameClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __NAME_CLIENT__
#define __NAME_CLIENT__

/**
 * \file YARPNameClient.h This file contains definitions of classes and methods
 * to access the name server. Ports must register to the name server to allow
 * publicizing their existence to the network of processes and other ports objects
 * that might require to connect or receive message from.
 */
#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>
#include <ace/config.h>

#include <yarp/wide_nameloc.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPNameClient2.h>

#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPString.h>


#ifndef SIZE_BUF
#define SIZE_BUF 4096
#endif

/**
 * YARPNameClient is a container of methods that talk to the name server via TCP
 * to request the name services. This is just the interface while the actual object
 * is global to a process.
 */
class YARPNameClient  
{
public:
	/**
	 * Constructor.
	 * @param server is the IP address or name where the server resides.
	 * @param port is the port number the server listens to (e.g. 10000).
	 */
	YARPNameClient(const YARPString& server, int port);

	/**
	 * Constructor.
	 * @param addr is the ACE_INET_Addr that contains the name and port number
	 * of the server.
	 */
	YARPNameClient(const ACE_INET_Addr &addr);

	/**
	 * Destructor.
	 */
	virtual ~YARPNameClient();

	/**
	 * Registers an mcast address to a certain symbolic name.
	 * @param s is the symbolic name to register.
	 * @param addr is the returned address (IP and port number).
	 * @return YARP_OK on success.
	 */
	int check_in_mcast(const YARPString &s, ACE_INET_Addr &addr);

	/**
	 * Registers an address to a certain symbolic name.
	 * @param s is the symbolic name to register.
	 * @param reg_addr is the address of the caller.
	 * @param addr is the return address.
	 * @return YARP_OK on success.
	 */
	int check_in (const YARPString &s, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr);

	/**
	 * Registers an address to a certain symbolic name.
	 * @param s is the symbolic name to register.
	 * @param addr is both the address of the caller and the return address of the registration.
	 * @return YARP_OK on success.
	 */
	int check_in (const YARPString &s, ACE_INET_Addr &addr);

	/**
	 * Registers an address to a certain symbolic name.
	 * @param s is the symbolic name to register.
	 * @param ip is both the address of the caller and the returned IP address.
	 * @param port is the returned IP port number.
	 * @return YARP_OK on success.
	 */
	int check_in (const YARPString &s, YARPString &ip, NetInt32 *port);

	/**
	 * Registers an udp address to a certain symbolic name.
	 * @param name is the symbolic name to register.
	 * @param addr is the IP address.
	 * @param ports is an array of ports returned by the name server.
	 * @param n is the size of the array requested.
	 * @return YARP_OK on success.
	 */
	int check_in_udp(const YARPString &name, YARPString &addr, NetInt32 *ports, NetInt32 n);

	/**
	 * Registers an udp address to a certain symbolic name.
	 * @param name is the symbolic name to register.
	 * @param reg_addr is the address of the caller.
	 * @param addr is the IP address.
	 * @param ports is an array of ports returned by the name server.
	 * @param n is the size of the array requested.
	 * @return YARP_OK on success.
	 */
	int check_in_udp(const YARPString &name, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr, NetInt32 *ports, NetInt32 n);

	/**
	 * Registers a qnx address to a symbolic name.
	 * @param entry is the qnx name structure that contain the pid, channel number
	 * and node name of the caller.
	 * @return YARP_OK on success.
	 */
	int check_in_qnx(const YARPNameQnx &entry);

	/**
	 * Queries the name sever for a name.
	 * @param s is the name we're requesting information about.
	 * @param addr is the returned address (both ip and port).
	 * @param type is a pointer to the return type of the name (mcast, tcp, qnx, etc.).
	 * @return YARP_OK on success.
	 */
	int query (const YARPString &s, ACE_INET_Addr &addr, int *type);

	/**
	 * Queries the name sever for a qnx name.
	 * @param s is the name we're requesting information about.
	 * @param entry is the qnx name structure with the requested information.
	 * @param type is the type of the name (qnx on success).
	 * @return YARP_OK on success.
	 */
	int query_qnx (const YARPString &s, YARPNameQnx &entry, int *type);

	/**
	 * Queries network information about a specific NIC.
	 * @param inIp is the IP number of the caller.
	 * @param netId is the name of the network we're asking information about.
	 * @param outNic is the return name of the NIC (network name).
	 * @param outIp is the return IP address on the @a outNic.
	 * @return YARP_OK on success.
	 */
	int query_nic(const YARPString &inIp, const YARPString &netId, YARPString &outNic, YARPString &outIp);

	/**
	 * Unregisters a name.
	 * @param s the name to be removed from the name server tables.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int check_out (const YARPString &s);

	/** 
	 * Unregisters a qnx name.
	 * @param s the name to be removed from the name server tables.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int check_out_qnx (const YARPString &s);

	/**
	 * Dumps the name server tables to stdout.
	 * @param i determines the behavior of the call: 
	 * 0 for short dump, 1 for getting extended information.
	 */
	YARPString dump(int i = 0);	// 0 short, 1 extended

	int _handle_reply(YARPString &out);

    int check(const char *name, const char *key, 
              const char *value);

private:
	// connect to server
	int connect_to_server ();
	// close down the connection properly
	int close();
	int _checkIn(const YARPString &s, ACE_INET_Addr &addr);
	int _checkInUdp(const YARPString &name, const YARPString &ip, NetInt32 *ports, NetInt32 n);
	int _checkInMcast(const YARPString &s, ACE_INET_Addr &addr);
	int _checkInQnx(const YARPNameQnx &entry);
	int _query(const YARPString &s, ACE_INET_Addr &addr, int *type);
	int _queryQnx(const YARPString &s, YARPNameQnx &entry, int *type);
	int _query_nic(const YARPNSNic &in, YARPString &outNic, YARPString &outIp);


private:
	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
	YARPSemaphore mutex_;
    YARPNameClient2 alt_client;
	
	char *data_buf_;
	char *reply_buf_;
};

#endif // .h
