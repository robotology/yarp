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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocketMulti.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef __YARPSocketMultih__
#define __YARPSocketMultih__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/MEM_Acceptor.h>
#include <ace/MEM_Connector.h>
#include <ace/Synch.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPSocket.h>

/**
 * \file YARPSocketMulti.h contains the definition of the YARP input socket.
 * This is implemented by various classes and managed by various threads defined
 * in YARPSocketMulti.cpp.
 */

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPNetworkTypes.h> // not strictly necessary here

/// SocketTypes
///{
///	YARP_NO_SOCKET = 0,
///	YARP_I_SOCKET = 1,
///	YARP_O_SOCKET = 2,
///};

/**
 * YARPInputSocketMulti represents an input socket that can read from
 * multiple targets using a set of predefined protocols (tcp, udp, etc.).
 * This is implemented by having multiple threads listening on various
 * connections and a manager thread accepting new connections and 
 * managing a list of these threads.
 */
class YARPInputSocketMulti : public YARPNetworkInputObject
{
protected:
	/** The internal implementation */
	void *system_resources;

public:
	/**
	 * Default constructor. It initializes the object to a suitable default.
	 */
	YARPInputSocketMulti();

	/**
	 * Destructor. Frees memory too.
	 */
	virtual ~YARPInputSocketMulti();

	/**
	 * Closes a connection identified by its socket handle.
	 * @param reply_id is the socket handle that identifies the connection.
	 * @return YARP_OK on success.
	 */
	int Close(ACE_HANDLE reply_id);

	/** 
	 * Closes a connection identified by its symbolic name (the one registered
	 * on the name server).
	 * @param name is the name of the connection.
	 * @return YARP_OK on success.
	 */
	int CloseByName(const YARPString& name);

	/**
	 * Closes all connections.
	 * @return YARP_OK if successful.
	 */
	int CloseAll(void);

	/**
	 * Starts receiving a new message by polling all available connections.
	 * @param buffer is the input buffer to be filled with the message.
	 * @param buffer_length is the length of the @a buffer.
	 * @param reply_id is the handle of the socket that received the message and
	 * it could be used to receive more buffers from the same connection.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);

	/**
	 * Starts receiving a new message but blocks and waits for a new message
	 * to be available.
	 * @param buffer is the input buffer to be filled with the message.
	 * @param buffer_length is the length of the @a buffer.
	 * @param reply_id is the handle of the socket that received the message and
	 * it could be used to receive more buffers from the same connection.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);

	/**
	 * Continues receiving from the same socket a PollingReceiveBegin() or a 
	 * ReceiveBegin() already received a buffer successfully.
	 * @param reply_id identifies the socket that received correctly.
	 * @param buffer is the pointer to the buffer to be filled with the message.
	 * @param buffer_length is the size of the buffer/message.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length);

	/**
	 * Starts replying to a message that was already received.
	 * @param reply_id identifies the socket that received correctly.
	 * @param reply_buffer is the pointer to the buffer that contains the reply.
	 * @param buffer_reply_length is the size of the buffer/message.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);

	/**
	 * Completes the reply and terminates reception of the message.
	 * @param reply_id identifies the socket that received correctly.
	 * @param reply_buffer is the pointer to the buffer that contains the reply.
	 * @param buffer_reply_length is the size of the buffer/message.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);

	/**
	 * Gets the identifier of the socket. For the input socket it returns
	 * the handle of the acceptor socket.
	 * @return the handle of the acceptor socket associated to the
	 * input socket class.
	 */
	ACE_HANDLE GetIdentifier(void) const;

	/**
	 * Gets the type of service provided by this class.
	 * @return always YARP_MULTI.
	 */
	int GetServiceType (void) { return YARP_MULTI; }

	/**
	 * Prepares the socket for receiving messages. This includes allocating certain
	 * structures.
	 * @param name is the ID of the local enpoint receiving connections.
	 * @param ports is an array of IP port numbers.
	 * @param number_o_ports is the size of the @a ports array.
	 * @return YARP_OK if successful.
	 */
	int Prepare (const YARPUniqueNameID& name, int *ports, int number_o_ports);

	/**
	 * Gets the port assigned when accepting connections.
	 * @return the IP port used for accepting connections (tcp).
	 */
	int GetAssignedPort(void) const;

	/**
	 * Prints information about the list of threads managed by this class
	 * internally.
	 * @return YARP_OK on success.
	 */
	int PrintThreadList(void);
};

/**
 * YARPOutputSocketMulti in spite of its name only manages an output socket
 * of type YARP_SHMEM. The name is misleading for historical reasons but since
 * the user is not concerned with this class directly it hasn't been changed
 * yet. An instance of this class is used by ports whenever creating a new
 * SHMEM connection.
 */
class YARPOutputSocketMulti : public YARPNetworkOutputObject
{
protected:
	/** the intenal implementation of this class */
	void *system_resources;

	/** the handle of the stream connection */
	ACE_HANDLE identifier;

public:
	/** 
	 * Default constructor.
	 */
	YARPOutputSocketMulti();

	/**
	 * Destructor.
	 */
	virtual ~YARPOutputSocketMulti();

	/**
	 * Closes the connection to the remote peer.
	 * @param name is the YARPUniqueNameID that identifies the remote peer.
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int Close(const YARPUniqueNameID& name);

	/**
	 * Connects to the remote peer.
	 * @param name is the ID of the remote endpoint.
	 * @param own_name is the symbolic name of the local endpoint (which is sent
	 * to the remote to identify ourselves).
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int Connect(const YARPUniqueNameID& name, const YARPString& own_name);
	
	/**
	 * Starts sending a new message.
	 * @param buffer is the buffer containing the message.
	 * @param buffer_length is the size of the buffer.
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int SendBegin(char *buffer, int buffer_length);

	/**
	 * Continues sending a message (adds a new piece).
	 * @param buffer is the buffer with the new chunk.
	 * @param buffer_length is the size of the buffer.
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int SendContinue(char *buffer, int buffer_length);

	/**
	 * Starts receiving the reply to the message.
	 * @param reply_buffer is the pointer to the buffer receiving the reply.
	 * @param reply_buffer_length is the size of the reply buffer.
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);

	/**
	 * Completes the reception of the reply and the message.
	 * @param reply_buffer is the pointer to the buffer receiving the reply.
	 * @param reply_buffer_length is the size of the reply buffer.
	 * @return YARP_OK if successful, YARP_FAIL otherwise.
	 */
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	/**
	 * Gets the identifier of the socket.
	 * @return the ACE_HANDLE of the underlying socket.
	 */
	ACE_HANDLE GetIdentifier(void) const;

	/**
	 * Gets the type of service provided by this class.
	 * @return always YARP_SHMEM.
	 */
	int GetServiceType (void) { return YARP_SHMEM; }

	/**
	 * Prepares the socket for connecting. This is to be called
	 * after contruction.
	 * @param name is the ID of the local endpoint.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int Prepare (const YARPUniqueNameID& name);
};

#endif

