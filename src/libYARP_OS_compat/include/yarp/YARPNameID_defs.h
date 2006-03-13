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
/// $Id: YARPNameID_defs.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef YARPNameID_defs_INC
#define YARPNameID_defs_INC

#include <yarp/YARPConfig.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPNameID_defs.h Definition of the protocols used by the port.
 * Certain protocols are not selected by the user but rather managed internally by ports.
 * Also, an imput port can have multiple input connections with different protocols.
 * Valid user choices are limited to: UDP, TCP, MCAST, QNET.
 */

/**
 * All possible protocols in a port.
 */
enum YARPProtocols
{
	/**
	* The port is not connected or it reported a serious network error.
	*/
	YARP_NO_SERVICE_AVAILABLE = -1,

	/**
	* The UDP protocol, a connectionless efficient protocol, The maximum
	* message size is limited to 64Kbytes.
	*/
	YARP_UDP = 0,

	/**
	* TCP protocol, 
	* Reliable connection-oriented, There's no limit on the 
	* message size.
	*/
	YARP_TCP = 1,

	/**
	* MCAST protocol,
	* Like UDP but the same message is automatically dispatched
	* to multiple targets (network efficient).
	*/
	YARP_MCAST = 2,

	/**
	* QNET is the proprietary QNX version 6 network protocol, Very efficient but
	* doesn't mix with the IP based protocols.
	*/
	YARP_QNET = 3,

	/**
	* QNX4 protocol is deprecated and no longer supported/implemented.
	*/
	YARP_QNX4 = 4,

	/**
	* SHMEM protocol will activate automatically if both ends of a connection
	* are on the same machine and network, It is connection oriented and
	* doesn't have any limit on the size of the messages.
	*/
	YARP_SHMEM = 5,

	/**
	* The port is an input port with multiple protocol abilities.
	*/
	YARP_MULTI = 6,

	YARP_PROTOCOL_MASK = 15,

	/**
	 * Protocols TCP and SHMEM may or may not request acknowledgements.
	 */
	YARP_REQUIRE_ACK = 128,
};

const char __udpString[] = "UDP";
const char __tcpString[] = "TCP";
const char __mcastString[] = "MCAST";
const char __qnxString[] = "QNX";
const char __noServiceString[] = "NO_SVC";

/**
 * Converts the protocol type enumerator into a string.
 * @param type the protocol type.
 * @return the converted string.
 */
inline const char *servicetypeConverter(int type)
{
	if (type == YARP_UDP)
		return __udpString;
	if (type == YARP_TCP)
		return __tcpString;
	if (type == YARP_MCAST)
		return __mcastString;
	if ((type == YARP_QNET) || (type == YARP_QNX4))
		return __qnxString;
	
	return __noServiceString;
}

#endif
