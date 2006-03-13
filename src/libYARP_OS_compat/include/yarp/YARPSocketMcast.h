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
///                    #pasa, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocketMcast.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef __YARPSocketMcasth__
#define __YARPSocketMcasth__

/**
 * \file YARPSocketMcast.h This contains classes and methods that implement
 * the YARP output socket constructs for the MCAST protocol. MCAST is a datagram
 * protocol that allows sending to multiple targets only once. In practice,
 * packets are sent to special IP addresses and clients can subscribe to certain
 * addresses to get the packets. This allows very efficient trasmission of the
 * same information to multiple targets.
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/Synch.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPSocket.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPNetworkTypes.h> // not strictly necessary here

// SocketTypes
//{
//	YARP_NO_SOCKET = 0,
//	YARP_I_SOCKET = 1,
//	YARP_O_SOCKET = 2,
//};

/**
 * An actual output socket class for the MCAST protocol. The socket now is a real
 * unix/windows socket using the MCAST protocol.
 * @see YARPOutputSocket, YARPOutputSocketDgram for more information.
 */
class YARPOutputSocketMcast : public YARPNetworkOutputObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;

public:
	/**
	 * Constructor.
	 */
	YARPOutputSocketMcast();

	/**
	 * Destructor.
	 */
	virtual ~YARPOutputSocketMcast();

	/**
	 * Closes the socket and its connection.
	 * @param name is the name ID of the connection to terminate.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int Close(const YARPUniqueNameID& name);

	/**
	 * Connects to the remote endpoint (an input object).
	 * @param name is the YARPUniqueNameID which contains the information
	 * on the remote endpoint of the channel.
	 * @param own_name is the symbolic name (as registered in the name server) of 
	 * the port owning the connection, it is sent during connection to the remote
	 * to allow identifying the specific connection being created.
	 * @see YARPNetworkOutputObject for details.
	 */
	int Connect(const YARPUniqueNameID& name, const YARPString& own_name);
	
	/**
	 * Begins sending a message.
	 * @param buffer is the message buffer.
	 * @param buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	int SendBegin(char *buffer, int buffer_length);

	/**
	 * Continue by sending another buffer as part of the previous message.
	 * @param buffer is the message buffer.
	 * @param buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	int SendContinue(char *buffer, int buffer_length);

	/**
	 * Depending on the protocol starts receiving a reply from the remote
	 * endpoint. This method is here for uniformity with other socket types
	 * but does nothing.
	 * @param reply_buffer is the message buffer.
	 * @param reply_buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);

	/**
	 * Completes the reception of the message reply.
	 * @param reply_buffer is the message buffer.
	 * @param reply_buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	/**
	 * Returns the identifier of the undelying socket.
	 * @return the ACE_HANDLE of the socket.
	 */
	ACE_HANDLE GetIdentifier(void) const;

	/**
	 * Gets the YARP service type.
	 * @return YARP_MCAST always.
	 */
	int GetServiceType (void) { return YARP_MCAST; }

	/**
	 * Prepares the socket for communicating.
	 * @param name is the id of the remote end point of the channel.
	 * @return YARP_OK on success.
	 */
	int Prepare (const YARPUniqueNameID& name);

	/**
	 * Closes all MCAST connections, this method talks to all clients (listeners)
	 * and tells them to stop listening because the channel is about to close.
	 * @param YARP_OK if successful.
	 */
	int CloseMcastAll (void);

	/**
	 * Gets the number of MCAST clients.
	 * @return the number of MCAST clients listening to this channel.
	 */
	int GetNumberOfClients (void);
};

#endif

