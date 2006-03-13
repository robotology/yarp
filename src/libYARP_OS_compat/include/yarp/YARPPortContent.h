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
/// $Id: YARPPortContent.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Sat May 26 22:27:07 EDT 2001
*/
#ifndef YARPPortContent_INC
#define YARPPortContent_INC

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPPortContent.h definition of the "port content" helper classes.
 */

/**
 * Helper for reading raw data from a port.
 */
class YARPPortReader
{
public:
  /**
   * Read a block of data from a port.
   *
   * @param buffer address of block of memory to read into.
   * @param length size of block of memory available.
   * @return true on success
   */
	virtual int Read(char *buffer, int length) = 0;
};

/**
 * Helper for writing raw data to a port.
 */
class YARPPortWriter
{
public:
  /**
   * Write a block of data to a port.
   *
   * @param buffer address of block of memory to read from.
   * @param length size of block of memory.
   * @return true on success
   */
	virtual int Write(char *buffer, int length) = 0;
};

template <class T>
struct HierarchyRoot
{
	// HierarchyId is a nested class
	struct HierarchyId {};
};


/**
 * Wrapper for any content to be transmitted between ports.  Instances
 * of this class know how to read or write themselves, and are can be
 * kept in a pool of objects that circulate from the user to the
 * communications code and back to the user.  Necessary for efficiency
 * when transmitting to multiple targets that read data at different
 * rates.
 */

class YARPPortContent : public HierarchyRoot<YARPPortContent>
{
public:
	virtual ~YARPPortContent () { /*ACE_DEBUG ((LM_DEBUG, "destroying a YARPPortContent\n"));*/ }

	/**
	 * Initialize the object from a message.  A "reader" is supplied
	 * by a port object, which expects us to make a series of 
	 * one or more calls to Read() on the reader to reconstruct
	 * an object passed across the network.  The port object then
	 * doesn't need to know anything about the size/format of the
	 * message.
	 *
	 * @param reader interface to read blocks of the message from the port.
	 * @return true if successful, false otherwise.
	 */
	virtual int Read(YARPPortReader& reader) = 0;

	/**
	 * Describe the object as a message.  A "writer" is supplied
	 * by a port object, which expects us to make a series of one
	 * or more calls to Write() on the writer to describe an
	 * object.  The port object then doesn't need to know anything
	 * about the size/format of the message.
	 *
	 * @param writer interface to write blocks of the message to the port.
	 * @return true if successful, false otherwise.
	 */
	virtual int Write(YARPPortWriter& writer) = 0;

	/**
	 * Clean up. Called when communications code is finished with
	 * the object, and it will be passed back to the user.  Often
	 * fine to do nothing here.
	 *
	 * @return true if successful, false otherwise.
	 */
	virtual int Recycle() = 0;
};


/**
 * Wrapper for simple content to be transmitted between ports.  "Simple"
 * content means a class which can safely be transmitted byte-for-byte
 * across the network.  It must not contain any pointers!
 *
 * @param T the simple class which is to be transmitted across the network.
 */

template <class T>
class YARPPortContentOf : public YARPPortContent
{
public:
  /**
   * An instance of the simple content.
   */
	T datum;

  /**
   * Access an instance of the simple content.
   *
   * @return a reference to an instance of the simple content.
   */
	T& Content() { return datum; }

	virtual ~YARPPortContentOf<T> () { /*ACE_DEBUG ((LM_DEBUG, "destroying a YARPPortContentOf\n"));*/ }

	virtual int Read(YARPPortReader& reader) { return reader.Read((char*)(&datum),sizeof(datum)); }

	virtual int Write(YARPPortWriter& writer) { return writer.Write((char*)(&datum),sizeof(datum)); }

	virtual int Recycle() { return 0; }
};

#endif
