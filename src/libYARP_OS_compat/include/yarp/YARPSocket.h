/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
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
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocket.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef YARPSOCKET_INC
#define YARPSOCKET_INC

/**
 * \file YARPSocket.h YARP incapsulation of a communication channel.
 * These are private classes used internally by ports, nothing the user
 * should be concerned with.
 *
 * YARP incapsulates some socket (or any other OS communication features) 
 * into two externally visible classes YARPInputSocket and YARPOutputSocket.
 *
 */


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Synch.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPNetworkTypes.h> // not strictly necessary here

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/** 
 * Identifies the socket type.
 */
enum
{
	/**
	 * An invalid socket class.
	 */
	YARP_NO_SOCKET = 0,

	/**
	 * The socket class incapsulates an input channel.
	 */
	YARP_I_SOCKET = 1,

	/**
	 * The socket class incapsulates an output channel.
	 */
	YARP_O_SOCKET = 2,

	/**
	 * The masks to &'ing when reading the socket type bits.
	 */
	YARP_X_SOCKET_MASK = 3,

	/**
	 * Require acknowledge flag for messages (YARP reply messages flag).
	 */
	YARP_SOCK_REQUIRE_ACK = 128,
};

/**
 * Base class of all socket communication classes. 
 *	Methods are not pure virtual because this is base of both In and Out sockets.
 */
class YARPNetworkObject
{
private:
	YARPNetworkObject (const YARPNetworkObject&);
	YARPNetworkObject& operator= (const YARPNetworkObject&);

protected:
	int _socktype;
	enum { NOT_IMPLEMENTED = 0 };

public:
	/**
	 * Constructor.
	 */
	YARPNetworkObject () { _socktype = YARP_NO_SOCKET; }

	/**
	 * Destructor.
	 */
	virtual ~YARPNetworkObject() {}

	/**
	 * Sets the low-level socket object buffer size (in the ip stack).
	 * @param sock is the ACE_SOCK object to set the buffer.
	 * @param size is the new size.
	 * @return YARP_OK on success.
	 */
	static int setSocketBufSize(ACE_SOCK& sock, int size);

	/**
	 * Gets the socket type.
	 * @return the socket type as in the enum.
	 */
	virtual int getSocketType (void) const { return _socktype & YARP_X_SOCKET_MASK; }

	/**
	 * Gets the socket low-level identifier.
	 * @return the low-level ACE_HANDLE associated with the socket.
	 */
	virtual ACE_HANDLE GetIdentifier(void) const = 0;

	/**
	 * Gets the service type.
	 * @return the protocol type or any other service descriptor.
	 */
	virtual int GetServiceType (void) = 0;

	/**
	 * Gets the RequireAck flag that tells whether reply messages are enabled
	 * for this connection.
	 * @return the require ack flag.
	 */
	int getRequireAck (void) const { return (_socktype & YARP_SOCK_REQUIRE_ACK)!=0; }

	/**
	 * Sets the RequireAck flag for this connection.
	 * @param flag 1 means that reply messages are required, 0 that they're not.
	 */
	void setRequireAck(int flag) { _socktype = (_socktype&YARP_X_SOCKET_MASK)|(flag?YARP_SOCK_REQUIRE_ACK:0); }
};

/**
 * A class that encapsulates the concept of an input socket (in some
 * generalized sense).
 * Sockets are used by the communication code by associating them
 * to a specific thread (the owner). When the thread has to perform
 * a read/write, it recover its associated sockets and asks
 * for the operation. Typically this allows calling a generic
 * read/write that the thread then specializes.
 * Classes derived from this class must implement a certain number of
 * methods that allow reading from certain communication channels. In
 * particular, for sockets, there's going to be a call to accept()
 * somewhere and the creation of a new receiver socket. Subsequently 
 * the code should select() on the sockets or spawn new threads to
 * handle all the connections.
 * For QNET this same base class is used but clearly the socket
 * is not a real socket.
 * @see YARPInputSocketMulti
 */
class YARPNetworkInputObject : public YARPNetworkObject
{
private:
	YARPNetworkInputObject (const YARPNetworkInputObject&);
	YARPNetworkInputObject& operator=(const YARPNetworkInputObject&);

public:
	/**
	 * Constructor.
	 */
	YARPNetworkInputObject() : YARPNetworkObject() { _socktype = YARP_I_SOCKET; }

	/**
	 * Destructor.
	 */
	virtual ~YARPNetworkInputObject() {}

	/**
	 * Should close the input socket identified by the argument.
	 * @param reply_id is the socket to be closed.
	 * @return YARP_OK on success.
	 */
	virtual int Close(ACE_HANDLE reply_id) = 0;

	/**
	 * Should close the input socket identified by the argument.
	 * @param name is the symbolic name of the socket to be closed.
	 * @return YARP_OK on success.
	 */
	virtual int CloseByName(const YARPString& name) = 0;

	/**
	 * Should close all the input channels associated with the class.
	 * @return YARP_OK on success.
	 */
	virtual int CloseAll(void) = 0;

	/**
	 * Polls on the socket channels to see whether there's anything to be read.
	 * @param buffer is the receive buffer.
	 * @param buffer_length is the buffer length in bytes.
	 * @param reply_id is the handle of the socket, useful to reply to the remote endpoint.
	 * @return YARP_OK on a successful read.
	 */
	virtual int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) = 0;

	/**
	 * Start a blocking receive (a message might be composed of multiple parts.
	 * @param buffer is the receive buffer.
	 * @param buffer_length is the buffer length in bytes.
	 * @param reply_id is the handle of the socket, useful to reply to the remote endpoint.
	 * @return YARP_OK on a successful read.
	 */
	virtual int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) = 0;

	/**
	 * Continue receiving, this function can be called multiple times for
	 * receiving parts of the same long message.
	 * @param buffer is the receive buffer.
	 * @param buffer_length is the buffer length in bytes.
	 * @return YARP_OK on a successful read.
	 */
	virtual int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length) = 0;

	/**
	 * Replies to the remote endpoint using the stored reply_id.
	 * @param reply_id is the socket handle received from PollingReceiveBegin() or ReceiveBegin().
	 * @param reply_buffer is the buffer of the reply message.
	 * @param reply_buffer_length is the buffer length in bytes.
	 * @return YARP_OK on a successful read.
	 */
	virtual int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) = 0;

	/**
	 * Replies the last part of a message and closes the read procedure.
	 * @param reply_id is the socket handle received from PollingReceiveBegin() or ReceiveBegin().
	 * @param reply_buffer is the buffer of the reply message.
	 * @param reply_buffer_length is the buffer length in bytes.
	 * @return YARP_OK on a successful read.
	 */
	virtual int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) = 0;
};

/**
 * An output object for communicating. It encapsulates the
 * low-level communication object. This is a much simpler structure
 * than input sockets. There's a single connection per class.
 * Note the strong asimmetry between input and output objects.
 * @see YARPNetworkInputObject.
 */
class YARPNetworkOutputObject : public YARPNetworkObject
{
private:
	// to prevent copies.
	YARPNetworkOutputObject (const YARPNetworkOutputObject&);
	YARPNetworkOutputObject& operator=(const YARPNetworkOutputObject&);

public:
	/**
	 * Constructor.
	 */
	YARPNetworkOutputObject (void) : YARPNetworkObject() { _socktype = YARP_O_SOCKET; }

	/**
	 * Destructor.
	 */
	virtual ~YARPNetworkOutputObject () {}

	/**
	 * Closes the socket.
	 * @param name is the YARPUniqueNameID which contains the information
	 * about the channel to be closed.
	 * @return YARP_OK on success.
	 */
	virtual int Close (const YARPUniqueNameID& name) = 0;

	/**
	 * Connects to the remote endpoint (an input object).
	 * @param name is the YARPUniqueNameID which contains the information
	 * on the remote endpoint of the channel.
	 * @param own_name is the symbolic name (as registered in the name server) of 
	 * the port owning the connection, it is sent during connection to the remote
	 * to allow identifying the specific connection being created.
	 */
	virtual int Connect (const YARPUniqueNameID& name, const YARPString& own_name) = 0;

	/**
	 * Begins sending a message.
	 * @param buffer is the message buffer.
	 * @param buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	virtual int SendBegin(char *buffer, int buffer_length) = 0;

	/**
	 * Continue by sending another buffer as part of the previous message.
	 * @param buffer is the message buffer.
	 * @param buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	virtual int SendContinue(char *buffer, int buffer_length) = 0;

	/**
	 * Depending on the protocol starts receiving a reply from the remote
	 * endpoint.
	 * @param reply_buffer is the message buffer.
	 * @param reply_buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	virtual int SendReceivingReply(char *reply_buffer, int reply_buffer_length) = 0;

	/**
	 * Completes the reception of the message reply.
	 * @param reply_buffer is the message buffer.
	 * @param reply_buffer_length is the length of the buffer in byte.
	 * @return YARP_OK on success.
	 */
	virtual int SendEnd(char *reply_buffer, int reply_buffer_length) = 0;
};

/**
 * An actual output socket class. The socket now is a real
 * unix/windows socket using the TCP protocol.
 * @see YARPOutputSocketDgram, YARPOutputSocketMcast for more information.
 */
class YARPOutputSocket : public YARPNetworkOutputObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;

public:
	/**
	 * Constructor.
	 */
	YARPOutputSocket();

	/**
	 * Destructor.
	 */
	virtual ~YARPOutputSocket();

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
	 * endpoint.
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
	 * @return TCP always.
	 */
	int GetServiceType (void) { return YARP_TCP; }

	/**
	 * Prepares the socket for communicating.
	 * @param name is the id of the remote end point of the channel.
	 * @return YARP_OK on success.
	 */
	int Prepare (const YARPUniqueNameID& name);

	/**
	 * Disable the Nagle's algorithm in the TCP protocol.
	 * @return YARP_OK on success.
	 */
	int SetTCPNoDelay (void);
};

#endif
