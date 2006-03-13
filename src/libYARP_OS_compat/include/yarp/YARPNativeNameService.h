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
/// $Id: YARPNativeNameService.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNativeNameService_INC
#define YARPNativeNameService_INC

/**
 * \file YARPNativeNameService.h Classes and methods for the management of QNX endpoints.
 */
#include <yarp/YARPConfig.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID.h>

/**
 * YARPNativeNameService used to implement the native name registration on
 * QNX4.25. It's no longer used on QNX6. This is still here for no specific reason.
 */
class YARPNativeNameService
{
public:
	/** 
	 * Registers a name on the QNX4 protocol. This doesn't require the 
	 * name server since QNX4 has its naming services.
	 * @param name is the symbolic name to register.
	 * @return NULL always.
	 */
	static YARPUniqueNameID *RegisterName (const char *name);

	/** 
	 * Locates a name on the QNX4 protocol. This doesn't require the 
	 * name server since QNX4 has its naming services.
	 * @param name is the symbolic name to look for.
	 * @return NULL always.
	 */
	static YARPUniqueNameID *LocateName (const char *name);

	/**
	 * Checks whether the name can be registered on QNX4.
	 * @return always 0 on QNX6, always 1 on QNX4.
	 */
	static int IsNonTrivial (void);
};


/**
 * YARPNativeEndpointManager deals with creation, use, and deletion of
 * QNX6 endpoints. If the QNET protocol is selected the port calls these
 * methods to create and manage the communication channel.
 */
class YARPNativeEndpointManager
{
public:
	/**
	 * Creates a QNX6 channel and assign it to the current process.
	 * @return the channel ID on success, YARP_FAIL on failure.
	 */
	static int CreateQnetChannel(void); 

	/**
	 * This isn't needed on QNX6. It's provided for aestetic reasons.
	 * @param name is unused.
	 * @return always YARP_OK on QNX6, YARP_FAIL if called on other systems.
	 */
	static int CreateInputEndpoint(YARPUniqueNameID& name);

	/**
	 * Creates and connects an output endpoint. This method creates a channel 
	 * that attaches to an existing channel created with CreateQnetChannel().
	 * @param name contains the specification of the remote peer according to the QNET
	 * standard: pid, channel ID, and node name.
	 * @return YARP_OK always, this is probably a mistake.
	 */
	static int CreateOutputEndpoint(YARPUniqueNameID& name);

	/**
	 * Connects two endpoints. This is also a unused method.
	 * @param dest unused.
	 * @param own_name unused.
	 * @return YARP_OK on QNX6, YARP_FAIL on other systems.
	 */
	static int ConnectEndpoints(YARPUniqueNameID& dest, const YARPString& own_name);

	/**
	 * Closes a QNX endpoint/channel.
	 * @param id is the name ID of the channel to be closed.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	static int Close(YARPUniqueNameID& id);
};

#endif


