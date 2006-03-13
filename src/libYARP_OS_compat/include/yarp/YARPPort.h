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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///


///
/// $Id: YARPPort.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

/*
	paulfitz Sat May 26 22:34:44 EDT 2001
*/

/**
 * \file YARPPort.h contains the definition of the YARPPort type hierarchy.
 * 
 */

#ifndef YARPPort_INC
#define YARPPort_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>

#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPPortContent.h>
#include <yarp/YARPNameID.h>

/**
 * A class for streaming communication.  Objects of this class ("ports") can
 * be registered with a name server and connected to each other across
 * the network or via shared memory.  Ports should be instantiated using
 * classes derived from YARPInputPort or YARPOutputPort, rather than
 * this base class.
 */

class YARPPort
{
protected:
	/**
	 * Internal helper object.
	 */
	void *system_resource;  

	/**
	 * Current content associated with port.
	 */
	YARPPortContent *content;

public:
  /**
   * Constructor.
   *  Creates a port.  Before using the port, you will need to Register() it.
   */
	YARPPort();


  /**
   * Destructor.
   * Although the destructor tries to close the connection and unregister the port, 
   * it is good practice to Unregister() before destroying a port.
   */
	virtual ~YARPPort();

  /**
    * Gives the port a name, and registers it.
    * The name should be unique, the only constraint on name is that they 
	* must start with a leading / which is interpreted internally in a special
	* way.
    * The name server needs to be told about the port ("registration"). It knows
	* about the specific network environment from the configuration file
	* $YARP_ROOT/conf/namer.conf
    *
    * @param name the name for the port
    * @param net_name the name of the network the port is created on
    * @return YARP_OK if registration was successful.
   */
	virtual int Register (const char *name, const char *net_name = YARP_DEFAULT_NET);

  /**
   * Asks the name server to unregister the port.
   * Other ports will no longer be able to connect to it.
   *
   * @result YARP_OK if successful.
   */
	virtual int Unregister (void);

  /**
   * Direct output from this port to a specified target port.  @param
   * name the name of the target port, as registered with the name
   * server.
   *
   * @result YARP_OK if successful.
   */
	int Connect(const char *name);

  /**
   * Allocate some content for the port.  Content is an instance of
   * the data structure that a port can read and write.  This method
   * must be defined by the user.
   *
   * @return Pointer to the content, or NULL on failure.
   */
	virtual YARPPortContent *CreateContent() = 0;

  /**
   * Get the current content associated with the port.  There is
   * guaranteed to always be such content, but the actual content
   * object may change after any call to the port's methods (this is
   * to allow for communication to multiple targets at different
   * rates, which requires content objects to have flexible
   * lifetimes).
   *
   * @return the content associated with the port.
   */
	YARPPortContent& Content();

  /**
   * Check whether any ports have the current port as their target.
   *
   * @return non-zero if any ports have the current port as their target.
   */
	int IsReceiving();

  /**
   * Check whether this port has any target ports.
   *
   * @return non-zero if this port has any target ports.
   */
	int IsSending();

  /**
   * Waits for any current communication with a target to complete.
   */
	void FinishSend();

  /**
   * Shut down this port.
   */
	void Deactivate();

  /**
   * Shut down all ports.
   */
	static void DeactivateAll();

  /**
   * Connect one port to another by name.
   *
   * @return YARP_OK on success.
   */
	static int Connect(const char *src_name, const char *dest_name);

	/**
	 * Erm.  Shrug.
	 */
	virtual void End();

	/**
	 * Sets the require acknowledge flag.
	 * @param require_ack is the flag value. If 1 it means that the
	 * reply messages aren't required for communicating with the remote peer.
	 */
	void SetRequireAck(int require_ack = 1);

	/**
	 * Gets the require acknowledge flag.
	 * @return the require acknowledge flag value. If 1 it means that the
	 * reply messages aren't required for communicating with the remote peer.
	 */
	int GetRequireAck();
};


/**
 * An input port.
 * Objects in this class can receive messages from YARPOutputPort objects
 * ("output ports").  Messages can be received either in an event-driven
 * way, by overriding the OnRead() method, or by blocking/polling, using
 * Read().  While one message is being read, other messages may be arriving,
 * depending on the number of buffers the port is configured with.
 */

class YARPInputPort : public YARPPort
{
public:

  /**
   * Number of buffers available to port to store incoming messages.
   */

	enum
	{
	  /**
	   * No buffering:
	   * While the owner of the input port is accessing a
	   * previously read message, no other messages may be
	   * received.
	   */
		NO_BUFFERS,

		/**
		 * Two buffers:
		 * while the owner of the input port is accessing a
		 * previously read message A, the next message B may
		 * be arriving - but once it has arrived, no further
		 * messages may arrive until the owner stops accessing
		 * A and starts accessing B.
		 */
		DOUBLE_BUFFERS,

		/**
		 * Three buffers:
		 * while the owner of the input port is accessing a
		 * previously read message A, arbitrary numbers of
		 * messages may arrive;  When the owner stops
		 * accessing A, and tries to access the next message,
		 * that message will be the most recently completely
		 * received message (previous completely received
		 * messages, if any, are discarded).
		 */
		TRIPLE_BUFFERS,


		/**
		 * By default, three buffers are made available to the port,
		 * so that the most recent fully-received message is always
		 * available to the owner of the port with minimal delay.
		 */
		DEFAULT_BUFFERS = TRIPLE_BUFFERS
	};

  /**
   * Constructor.
   *
   * @param n_service_type type of buffering used to receive messages.
   * @param n_protocol_type which network protocol to use.
   */
	YARPInputPort(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL);

  /**
   * Destructor.
   */
	virtual ~YARPInputPort();

	virtual int Register(const char *name, const char *net_name = YARP_DEFAULT_NET);

  /**
   * Read message from port.
   * This method is used to check if a message is currently available
   * and optionally to wait for one to arrive.  Once this method reports
   * that a message is available, the message can be accessed using the
   * Content() method.
   *
   * @param wait if true, block and wait for a message to arrive, otherwise
   * return immediately.
   * @return true if a message is available, otherwise false.
   */
	bool Read(bool wait=true);

  /**
   * Handle an incoming message.
   * This is an event handler which is called when a message is available.
   * You may override it to take some action.  Note that to access the
   * message, it is still essential to call Read().
   */
	virtual void OnRead() { /*ACE_DEBUG ((LM_DEBUG, "silly OnRead\n"));*/ }

  /**
   * Access a message.
   * Once Read() reports that a message is available, Content() will
   * return that message.  The object returned is valid until the next
   * time that Read() is called.
   *
   * @return the received message.
   */
	YARPPortContent& Content() { return YARPPort::Content(); }
};


/**
 * An output port.
 * Objects in this class can send messages to YARPInputPort objects
 * ("input ports").  
 * @see ::YARPProtocols for the allowed protocols.
 */

class YARPOutputPort : public YARPPort
{
public:
  /**
   * The number of targets the port may send output to.
   */
	enum
	{
	  /**
	   * The port may send output to an arbitrary number of other ports.
	   */
		MANY_OUTPUTS,

		/**
		 * The port may send output to at most one other port.
		 */
		SINGLE_OUTPUT,

		/**
		 * By default, a port may send output to an arbitrary number of other ports.
		 */
		DEFAULT_OUTPUTS = MANY_OUTPUTS
	};

	/**
	 * Constructor.  
	 * @param n_service_type the number of targets a
	 * port may send output to @param n_protocol_type the network
	 * protocol to use
	 */
	YARPOutputPort(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL);

	/**
	 * Destructor.
	 */
	virtual ~YARPOutputPort();

	virtual int Register(const char *name, const char *net_name = YARP_DEFAULT_NET);

  /**
   * Access the current message under preparation.
   * The object returned is valid until the next time that Write() is called.
   * @return the message under preparation.
   */
	YARPPortContent& Content();

	/**
	 * Transmit the current message under preparation to the
	 * port's targets.  
	 *
	 * @param wait if true, wait for any transmission already in
	 * progress to complete (if supported by network protocol)
	 * before trying to send the current message.  If you want to
	 * wait until the *current* message gets transmitted before
	 * continuing, call FinishSend().
	 */
	void Write(bool wait=false);

	/**
	 * Handle a write event.  This method is called when transmission occurs.
	 */
	virtual void OnWrite() {}

	/**
	 * Sets the allow shared memory communication flag.
	 * @param flag is the flag value. If 0 disables shared memory communication. While
	 * shared memory is generally more efficient than socket communication, sometimes
	 * for debugging, it migth be useful to run ports with different protocols locally.
	 */
	void SetAllowShmem (int flag = 1);

	/**
	 * Gets the allow shared memory communication flag.
	 * @return the flag value. If 0 it means that shared memory is disabled.
	 */
	int GetAllowShmem (void);
};


/**
 * Input port for complex messages.
 * A complex message is transmitted by define a "content" object which
 * knows how to read and write it.
 *
 * @param T the content class used to read and write individual messages.
 *
 * @see YARPPortContent
 */
template <class T>
class YARPInputPortOf : public YARPInputPort
{
public:
  /**
   * Constructor.
   * @see YARPInputPort
   */
	YARPInputPortOf(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) 
		: YARPInputPort(n_service_type, n_protocol_type) { ACE_ASSERT (n_protocol_type != YARP_SHMEM && n_protocol_type != YARP_MULTI); }


	/**
	 * Destructor.
	 */
	virtual ~YARPInputPortOf() { YARPPort::End(); }

	/**
	 * Create a blank message.
	 * Message creation is delegated to the associated YARPPortContent
	 * class
	 *
	 * @return Pointer to the content, or NULL on failure.
	 */
	virtual YARPPortContent *CreateContent() { return new YARPPortContentOf<T>; }

	/**
	 * Access a message.
	 * Once Read() reports that a message is available, Content() will
	 * return that message.  The object returned is valid until the next
	 * time that Read() is called.
	 *
	 * @return the received message.
	 */
	T& Content()
	{
		ACE_ASSERT(content!=NULL);
		return ((YARPPortContentOf<T>*)content)->Content();
	}
};

/**
 * Output port for complex messages.
 * A complex message is transmitted by define a "content" object which
 * knows how to read and write it.
 *
 * @param T the content class used to read and write individual messages.
 *
 * @see YARPPortContent
 */
template <class T>
class YARPOutputPortOf : public YARPOutputPort
{
public:
  /**
   * Constructor.
   * @see YARPOutputPort
   */
	YARPOutputPortOf(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) 
		: YARPOutputPort(n_service_type, n_protocol_type) { ACE_ASSERT (n_protocol_type != YARP_SHMEM && n_protocol_type != YARP_MULTI); }

	/**
	 * Destructor.
	 */
	virtual ~YARPOutputPortOf() {  YARPPort::End(); }

	/**
	 * Create a blank message.
	 * Message creation is delegated to the associated YARPPortContent
	 * class
	 *
	 * @return Pointer to the content, or NULL on failure.
	 */
	virtual YARPPortContent *CreateContent() { return new YARPPortContentOf<T>; }

  /**
   * Access the current message under preparation.
   * The object returned is valid until the next time that Write() is called.
   * @return the message under preparation.
   */
	T& Content()
    {
		YARPOutputPort::Content();
		ACE_ASSERT (content!=NULL);
		return ((YARPPortContentOf<T>*)content)->Content();
    }
};

/**
 * Input port for "simple messages".
 * Simple messages are classes which can safely be transmitted byte-for-byte
 * across the network.  They should contain no pointers!
 *
 * @param T the simple class which will be transmitted byte-for-byte
 * across the network
 */
template <class T>
class YARPBasicInputPort : public YARPInputPort
{
public:
  /**
   * Constructor.
   * @see YARPInputPort
   */
	YARPBasicInputPort(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) 
		: YARPInputPort(n_service_type, n_protocol_type) { ACE_ASSERT (n_protocol_type != YARP_SHMEM && n_protocol_type != YARP_MULTI); }

	/**
	 * Destructor.
	 */
	virtual ~YARPBasicInputPort() {  YARPPort::End(); }

	/**
	 * Create a blank message.
	 *
	 * @return Pointer to the content, or NULL on failure.
	 */
	virtual YARPPortContent *CreateContent() { return new T; }

  /**
   * Access a message.
   * Once Read() reports that a message is available, Content() will
   * return that message.  The object returned is valid until the next
   * time that Read() is called.
   *
   * @return the received message.
   */
	T& Content() { return *((T*)(&YARPPort::Content())); }
};


/**
 * Output port for "simple messages".
 * Simple messages are classes which can safely be transmitted byte-for-byte
 * across the network.  They should contain no pointers!
 *
 * @param T the simple class which will be transmitted byte-for-byte
 * across the network
 */
template <class T>
class YARPBasicOutputPort : public YARPOutputPort
{
public:
  /**
   * Constructor.
   * @see YARPOutputPort
   */
	YARPBasicOutputPort(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) 
		: YARPOutputPort(n_service_type, n_protocol_type) { ACE_ASSERT (n_protocol_type != YARP_SHMEM && n_protocol_type != YARP_MULTI); }

	/**
	 * Destructor.
	 */
	virtual ~YARPBasicOutputPort() {  YARPPort::End(); }

	/**
	 * Create a blank message.
	 *
	 * @return Pointer to the content, or NULL on failure.
	 */
	virtual YARPPortContent *CreateContent() { return new T; }

  /**
   * Access the current message under preparation.
   * The object returned is valid until the next time that Write() is called.
   * @return the message under preparation.
   */
	T& Content() { return *((T*)(&YARPOutputPort::Content())); }
};

#endif
