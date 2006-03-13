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
///                    #paulfitz, pasa, carlos#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNativeSyncComm.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNativeSyncComm_INC
#define YARPNativeSyncComm_INC

/**
 * \file YARPNativeSyncComm.h It contains the definition of the synchronous communication
 * model for the QNX protocol.
 */

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPNameService.h>
#include <yarp/YARPMultipartMessage.h>


#ifdef __QNX__

/**
 * This class implements the synchronous communication over QNX channels. This
 * comes naturally on QNX and, actually socket communication is modeled after the
 * QNX model. This is, differently than sockets, always synchronous.
 */
class YARPNativeSyncComm
{
public:
	/**
	 * Sends a buffer to a remote peer.
	 * @param dest is the destination of the message identified by a YARPNameID class.
	 * @param buffer is the buffer to be sent.
	 * @param buffer_length is the length of the buffer.
	 * @param return_buffer is the buffer that will contain the reply to the message.
	 * @param return_buffer_length is the length of the reply message.
	 * @return YARP_OK on success.
	 */
	static int Send(YARPNameID dest, char *buffer, int buffer_length, char *return_buffer, int return_buffer_length);

	/**
	 * Blocks to receive a message from a remote peer. This is the first function called
	 * when receiving a message.
	 * @param src is the local address (this is not related with the remote peer address).
	 * @param buffer is the buffer for receiving the message.
	 * @param buffer_length is the length of the receiving buffer.
	 * @return the ID of the remote peer (protocol and id for replying).
	 */
	static YARPNameID BlockingReceive(YARPNameID src, char *buffer, int buffer_length);

	/**
	 * Polls to see whether there's any pending message. This can be also the first function
	 * called when receiving as an alternative to the blocking receive.
	 * @param src is the local address (this is not related with the remote peer address).
	 * @param buffer is the buffer for receiving the message.
	 * @param buffer_length is the length of the receiving buffer.
	 * @return the ID of the remote peer (protocol and id for replying).
	 */
	static YARPNameID PollingReceive(YARPNameID src, char *buffer, int buffer_length);

	/**
	 * Continues the reception of a message. A polling or blocking receive should have been
	 * called and the return id stored somewhere.
	 * @param src is the address of the remote peer (the one returned by polling or blocking recv).
	 * @param buffer is the buffer for receiving the message piece.
	 * @param buffer_length is the length of the receiving buffer.
	 * @return YARP_OK on success.
	 */
	static int ContinuedReceive(YARPNameID src, char *buffer, int buffer_length);

	/**
	 * Replies to the sender.
	 * @param src is the address of the remote peer (the one returned by polling or blocking recv).
	 * @param buffer is the buffer containing the reply message.
	 * @param buffer_length is the length of the buffer.
	 * @return YARP_OK on success.
	 */
	static int Reply(YARPNameID src, char *buffer, int buffer_length);

	/**
	 * Sends a message to a remote peer. This method sends a multi-part message and
	 * waits for a multi-part response from the peer.
	 * @param dest is the destination of the message identified by a YARPNameID class.
	 * @param msg is the multipart message to be sent.
	 * @param return_msg is the reply to the message.
	 * @return YARP_OK on success.
	 */
	static int Send(YARPNameID dest, YARPMultipartMessage& msg, YARPMultipartMessage& return_msg);

	/**
	 * Blocks to receive a message from a remote peer. This is the first function called
	 * when receiving a message.
	 * @param src is the local address (this is not related with the remote peer address).
	 * @param msg is the multipart message.
	 * @return the ID of the remote peer (protocol and id for replying).
	 */
	static YARPNameID BlockingReceive(YARPNameID src, YARPMultipartMessage& msg);

	/**
	 * Polls to see whether there's any pending message. This can be also the first function
	 * called when receiving as an alternative to the blocking receive.
	 * @param src is the local address (this is not related with the remote peer address).
	 * @param msg is the multipart message.
	 * @return the ID of the remote peer (protocol and id for replying).
	 */
	static YARPNameID PollingReceive(YARPNameID src, YARPMultipartMessage& msg);

	/**
	 * Replies a multipart message to the sender.
	 * @param src is the address of the remote peer (the one returned by polling or blocking recv).
	 * @param msg is a multipart message.
	 * @return YARP_OK on success.
	 */
	static int Reply(YARPNameID src, YARPMultipartMessage& msg);
};

#else

/// redefined type in order to use Socket as native under NT/Linux.
#include <yarp/YARPSocketSyncComm.h>
#define YARPNativeSyncComm YARPSocketSyncComm

#endif

#endif

