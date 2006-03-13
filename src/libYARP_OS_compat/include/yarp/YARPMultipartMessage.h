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
/// $Id: YARPMultipartMessage.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPMultipartMessage_INC
#define YARPMultipartMessage_INC

/**
 * \file YARPMultipartMessage.h This class contains the YARP high level interface to
 * multipart messages (akin to the multipart QNX messages).
 */

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * YARPMultipartMessage represents a packetized message. This would allow to implement
 * generic mechanisms for sending arbitrary sized messages. There are limitations at the
 * moment which the authors didn't take time to remove.
 */
class YARPMultipartMessage
{
protected:
	void *system_resource;
	int top_index, length, owned;

	// topindex is the last part that has been set (allocated).
	// length is the number of parts.
	// owned, whether the memory is owned by the class and needs to be freed.
public:
	/**
	 * Default constructor.
	 */
	YARPMultipartMessage() 
	{ 
		system_resource = NULL; 
		top_index = -1; 
		length = -1; 
		owned = 1; 
	}

	/**
	 * Constructor.
	 * @param n_length is the number of parts (Block) of the message.
	 */
	YARPMultipartMessage(int n_length)
	{ 
		system_resource = NULL; 
		top_index = -1; 
		length = -1; 
		Resize(n_length);  
		owned = 1; 
	}

	/**
	 * Creates a message starting from an existing buffer composed of a 
	 * certain number of @a entries.
	 * @param buffer is the pointer to the buffer.
	 * @param entries is the size of the buffer.
	 */
	YARPMultipartMessage(void *buffer, int entries)
	{ 
		owned = 0;  
		system_resource = buffer;  
		top_index = entries-1;
		length = entries; 
	}

	/**
	 * Destructor.
	 */
	virtual ~YARPMultipartMessage();

	/**
	 * Resizes the message to a new size of @a n_length.
	 * @param n_length is the size of the message in blocks.
	 */
	void Resize(int n_length);

	/**
	 * Attaches a buffer to a certain part of the multipart message.
	 * @param index is the position where to add the new buffer.
	 * @param buffer is the pointer to the buffer.
	 * @param buffer_length is the length of the new buffer.
	 */
	void Set(int index, char *buffer, int buffer_length);

	/**
	 * Gets the index-th element of the buffer.
	 * @param index is the index into the multipart message.
	 * @return the pointer to the buffer at position @a index.
	 */
	char *GetBuffer(int index);

	/**
	 * Gets the size of one of the parts.
	 * @param index is the index into the multipart message.
	 * @return the size of the buffer at position @a index.
	 */
	int GetBufferLength(int index);

	/**
	 * Resets the array without changing the memory structure.
	 */
	void Reset();

	/**
	 * Gets the number of parts in the message.
	 * @return the number of blocks in the message.
	 */
	int GetParts()       { return top_index+1; }

	/**
	 * Gets the raw representation of the multipart message.
	 * @return the pointer to the internal buffer.
	 */
	void *GetRawBuffer() { return system_resource; }
};


#endif
