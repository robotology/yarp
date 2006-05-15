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
///                    #pasa, paulfitz, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: wide_nameloc.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#ifndef wide_nameloc_INC
#define wide_nameloc_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#include <yarp/YARPNetworkTypes.h>
#include <yarp/YARPNameID_defs.h>

#include <yarp/YARPString.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file wide_nameloc.h This file contains structures and classes used when
 * talking to the name server.
 */

#define __YARP_NAMESERVICE_STRING_LEN YARP_STRING_LEN
#define __YARP_NAMESERVICE_UDP_MAX_PORTS 21

const int __portNotFound = 0;
const char __ipNotFound[] = {"0.0.0.0"};


/**
 * These are the allowed commands when talking to the name server.
 */
enum __YARP_NameSeverCommands
    {
        YARPNSRegister = 0,
        YARPNSRelease = 1,
        YARPNSQuery = 2,
        YARPNSRpl = 3,
        YARPNSDumpRqs = 4,
        YARPNSDumpRpl = 5,
        YARPNSNicQuery = 6
    };


#include <yarp/begin_pack_for_net.h>
/**
 * YARPNameServiceCmd is a short header used to send commands to the
 * name server.
 */
struct YARPNameServiceCmd
{
	NetInt32 cmd;
	NetInt32 length;
	NetInt32 type;
} PACKED_FOR_NET;

/**
 * YARPNameQnx contains the elements that are used when registering a qnx
 * connection on the name server. This is a simple container with a few convenient
 * methods.
 */
class YARPNameQnx
{
public:
	/** 
	 * Sets the value of the internal data (which are public, by the way). A qnx
	 * connection is uniquely identified by the pid of the process that owns the channel,
	 * the channel ID of the connection (returned when creating the channel), and
	 * the name of the node in the QNET network.
	 * @param str is the symbolic name.
	 * @param node is the qnx node name.
	 * @param pid is the pid of the qnx connection.
	 * @param ch is the channel ID of the qnx connection.
	 */
	void set(const YARPString &str, const YARPString &node, NetInt32 pid, NetInt32 ch);

	/**
	 * Sets the symbolic name information only.
	 * @param str is the symbolic name following YARP standards for names.
	 */
	void setName(const YARPString &str);

	/**
	 * Sets the qnx channel information.
	 * @param node is the node name.
	 * @param pid is the pid of the process owning the connection.
	 * @param ch is the channel ID of the connection.
	 */
	void setAddr(const YARPString &node, NetInt32 pid, NetInt32 ch);

	/**
	 * Gets the qnx channel information.
	 * @param node is the node name.
	 * @param pid is the pid of the process owning the connection.
	 * @param ch is the channel ID of the connection.
	 */
	void getAddr(YARPString &node, NetInt32 *pid, NetInt32 *ch);

	/**
	 * Gets the symbolic name.
	 * @return the symbolic name that is registered or to be registered on
	 * the name server.
	 */
	const char *getName() const { return _name; }

	/**
	 * Gets the qnx node name.
	 * @return the node name.
	 */
	const char *getNode() const { return _node; }

	/**
	 * Gets the pid number of the process owning the connection.
	 * @return the pid of the process.
	 */
	NetInt32 getPid() const { return _pid; }

	/**
	 * Gets the channel ID of the connection.
	 * @return the channel number.
	 */
	NetInt32 getChan() const { return _chan; }

	/**
	 * Gets the size of this class.
	 * @return the size of the class YARPNameQnx in bytes.
	 */
	int length() const { return sizeof(YARPNameQnx); }

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _node[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _pid;
	NetInt32 _chan;
} PACKED_FOR_NET;


/** 
 * YARPNSNic contains information used when asking information about
 * NIC's to the name server.
 */
class YARPNSNic
{
public:
	/**
	 * Sets the IP address and network name.
	 * @param ip is the IP address of the caller.
	 * @param netId is a symbolic network name (see namer.conf file).
	 */
	void set(const YARPString &ip, const YARPString &netId);

public:
	char _ip[__YARP_NAMESERVICE_STRING_LEN];
	char _netId[__YARP_NAMESERVICE_STRING_LEN];
} PACKED_FOR_NET;


/** 
 * YARPNameTCP is used when registering TCP channels. This class is probably
 * no longer in use by the YARP code. Ports are now registered as UDP anyway
 * that allows requesting to reserve several IP port numbers in a single call.
 */
class YARPNameTCP
{
public:
	/**
	 * Sets the internal variables.
	 * @param str is the symbolic name of the port.
	 * @param addr is the tcp address specification (ip and port number).
	 */
	void set(const YARPString &str, const ACE_INET_Addr &addr);

	/**
	 * Sets the symbolic name only.
	 * @param str is the symbolic name of the port.
	 */
	void setName(const YARPString &str);

	/**
	 * Sets the tcp address.
	 * @param addr is the tcp address specification.
	 */
	void setAddr(const ACE_INET_Addr &addr);

	/**
	 * Sets the IP address only.
	 * @param ip is the IP address of the caller.
	 */
	void setIp(const YARPString &ip);

	/**
	 * Sets the port number only.
	 * @param p is the port number.
	 */
	void setPort(NetInt32 p);

	/**
	 * Gets the tcp address information.
	 * @param addr is the returned address (ip and port number).
	 */
	void getAddr(ACE_INET_Addr &addr);

	/**
	 * Gets the symbolic name.
	 * @return the name of the connection to be registered.
	 */
	const char *getName() const { return _name; }

	/**
	 * Gets the IP address.
	 * @return the IP address of the connection.
	 */
	const char *getIp() const { return _ip; }

	/**
	 * Gets the port number.
	 * @return the port number of the connection.
	 */
	NetInt32 getPort() { return _port; }

	/**
	 * Gets the size of the class.
	 * @return the size of YARPNameTCP in bytes.
	 */
	int length() { return sizeof(YARPNameTCP); }

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _ip[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _port;
} PACKED_FOR_NET;


/** 
 * YARPNameUDP is used when communicating with the name server about
 * udp connections. The typical command allows reserving a pool of port 
 * numbers for future use (e.g. in the order of 10).
 */
class YARPNameUDP
{
public:
	/**
	 * Sets the internal variables.
	 * @param str is the symbolic name of the port.
	 * @param addr is the address specification (ip and port number).
	 */
	void set(const YARPString &str, const ACE_INET_Addr &addr);

	/**
	 * Sets the symbolic name only.
	 * @param str is the symbolic name of the port.
	 */
	void setName(const YARPString &str);

	/**
	 * Sets the tcp address.
	 * @param addr is the udp address specification.
	 */
	void setAddr(const ACE_INET_Addr &addr);

	/**
	 * Sets the IP address only.
	 * @param ip is the IP address of the caller.
	 */
	void setIp(const YARPString &ip);

	/**
	 * Sets one of the port into the array.
	 * @param index is the entry in the array.
	 * @param p is the port number.
	 */
	void setPorts(NetInt32 index, NetInt32 p);

	/**
	 * Sets the size of the array to contain port numbers, this
	 * is also the number of ports requested to the name server.
	 * @param n is the number of ports.
	 */
	void setNPorts(NetInt32 n);

	/**
	 * Gets the udp address of this connection.
	 * @param addr is the address represented as ACE_INET_Addr.
	 */
	void getAddr(ACE_INET_Addr &addr);

	/**
	 * Gets a port number from the array.
	 * @param index is the entry of the array.
	 * @return the port number stored at the @a index position into the array.
	 */
	NetInt32 getPorts(NetInt32 index); 

	/**
	 * Gets the symbolic name.
	 * @return the name of the connection to be registered.
	 */
	const char *getName() const { return _name; }

	/**
	 * Gets the IP address.
	 * @return the IP address of the connection.
	 */
	const char *getIp() const { return _ip; }

	/**
	 * Gets the size of the class.
	 * @return the size of YARPNameTCP in bytes.
	 */
	int length() { return sizeof(YARPNameUDP); }

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _ip[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _ports[__YARP_NAMESERVICE_UDP_MAX_PORTS];
	NetInt32 _nPorts;
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>


#endif
