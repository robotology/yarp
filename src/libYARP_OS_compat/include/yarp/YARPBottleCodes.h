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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPBottleCodes.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#ifndef __YARPBOTTLECODESINC__
#define __YARPBOTTLECODESINC__

/**
 * \file YARPBottleCodes.h This file contains definitions for the
 * vocabulary type, a sort of string used to define the types of YARPBottle.
 *
 */

/**
 * Bottle types, these are the types that can be added to bottles.
 */
enum __YBTypeCodes 
    {
        YBTypeChar = 0,
        YBTypeInt = 1,
        YBTypeDouble = 2,
        YBTypeYVector = 3,
        YBTypeVocab = 4,
        YBTypeString = 5,
        YBTypeDoubleVector = 6,
        YBTypeIntVector = 7,
    };

// who's including from C anyway? Matlab libraries (mex) are including from C!
#ifdef __cplusplus

#include <yarp/YARPString.h>

// yikes! bad idea...
const char * const YBLabelNULL	= "Null";
const char * const YBVIsAlive	= "IsAlive";
const int __maxBottleID = 80;

/**
 * The YBVocab class is a sort of string of char which is
 * used to name bottles and messages within bottles. It's equivalent (almost)
 * to a YARPString but don't rely too much on it.
 */
class YBVocab : public ACE_String_Base<char>
{
public:
	/**
	 * Default contructor.
	 */
	YBVocab () : ACE_String_Base<char> () {}

	/**
	 * Constructor.
	 * @param s is a string to copy into this object, s must be zero terminated.
	 */
	YBVocab (const char *s) : ACE_String_Base<char> (s) {}

	/**
	 * Constructor.
	 * @param s is a char buffer to copy into this object.
	 * @param len is the length of the buffer.
	 */
	YBVocab (const char *s, size_t len) : ACE_String_Base<char> (s, len) {}

	/**
	 * Constructor.
	 * @param s is an ACE string to copy from.
	 */
	YBVocab (const ACE_String_Base<char>& s) : ACE_String_Base<char> (s) {}

	/**
	 * Copy constructor.
	 * @param s is the object to copy from.
	 */
	YBVocab (const YBVocab& s) : ACE_String_Base<char> (s) {}

	/**
	 * Constructor.
	 * @param c is a char to fill the string with.
	 */
	YBVocab (char c) : ACE_String_Base<char> (c) {}

	/**
	 * Constructor.
	 * @param len is the length of the newly created object.
	 * @param c is a char value to fill the buffer with.
	 */
	YBVocab (size_t len, char c = 0) : ACE_String_Base<char> (len, c) {}

	/**
	 * Destructor.
	 */
	~YBVocab () { clear(1); }

	/**
	 * Copy operator.
	 * @param s is the object reference to copy from.
	 * @return a reference to this object.
	 */
	YBVocab& operator= (const YBVocab& s) { ACE_String_Base<char>::operator= (s); return *this; }

	/**
	 * Adds something to a YBVocab.
	 * @param s the string to add to the current object.
	 * @return a reference to this object.
	 */
	YBVocab& append(const char *s) { ACE_String_Base<char>::operator+=(s); return *this; }

	/**
	 * Adds a YBVocab to the current YBVocab.
	 * @param s is the object reference to add to the current object.
	 * @return a reference to this object.
	 */
	YBVocab& append(const YBVocab& s) { ACE_String_Base<char>::operator+=(s); return *this; }

	/**
	 * Checks whether the vocab is empty.
	 * @return true if it's an empty string, false otherwise.
	 */
	bool empty (void) { return (length() == 0) ? true : false; }
};

#endif /* cplusplus */

#endif
