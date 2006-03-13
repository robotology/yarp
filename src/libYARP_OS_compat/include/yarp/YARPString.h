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
///	YARPString wrapper by pasa.
///

///
/// $Id: YARPString.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef YARPString_INC
#define YARPString_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <iostream>

#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <ace/OS.h>
#include <ace/String_Base.h>

/**
 * \file YARPString.h
 * Definition of a simple stl-like string class.
 */

/**
 * A simple stl-like string class.
 * This is an stl-like string class built on ACE string that assures a 
 * better portability across compiler if compared with stl string.
 */
class YARPString : public ACE_String_Base<char>
{
public:
	/**
	 * Constructor.
	 */
	YARPString () : ACE_String_Base<char> () {}

	/**
	 * Constructor.
	 * @param s a pointer to a null terminated string.
	 */
	YARPString (const char *s) : ACE_String_Base<char> (s) {}

	/**
	 * Constructor.
	 * @param s a pointer to an array of a certain maximum len.
	 * @param len the length of the array.
	 */
	YARPString (const char *s, size_t len) : ACE_String_Base<char> (s, len) {}

	/**
	 * Constructor.
	 * @param s an ACE array of <char>.
	 */
	YARPString (const ACE_String_Base<char>& s) : ACE_String_Base<char> (s) {}

	/**
	 * Constructor: this is the copy constructor of YARPString.
	 * @param s the source string to copy from.
	 */
	YARPString (const YARPString& s) : ACE_String_Base<char> (s) {}

	/**
	 * Constructor: fills the string with the char in <c>.
	 * @param c char to fill the string with.
	 */
	YARPString (char c) : ACE_String_Base<char> (c) {}

	/**
	 * Constructor.
	 * @param len length of the string.
	 * @param c initial value.
	 */
	YARPString (size_t len, char c = 0) : ACE_String_Base<char> (len, c) {}

	/**
	 * Destructor.
	 * Destroys and deallocates the memory associated with the string.
	 */
	~YARPString () { clear(1); }

	/**
	 * Copies a YARPString into another.
	 * @param s the source string.
	 * @return a reference to the destination string.
	 */
	YARPString& operator= (const YARPString& s) { ACE_String_Base<char>::operator= (s); return *this; }
	
	/**
	 * Appends an char array (null terminated) to a string.
	 * @param s the source string.
	 * @return the new string.
	 */
	YARPString& append(const char *s) { ACE_String_Base<char>::operator+=(s); return *this; }

	/**
	 * Appends a string to another.
	 * @param s the string to be appended.
	 * @return the new string.
	 */
	YARPString& append(const YARPString& s) { ACE_String_Base<char>::operator+=(s); return *this; }

	/**
	 * Checks whether the string is empty.
	 * @return true or false.
	 */
	bool empty (void) { return (length() == 0) ? true : false; }
};

/**
 * Prints a string.
 * @param os the iostream to print into.
 * @param s the string to be printed.
 * @return the printed string (to allow chaining).
 */
inline std::ostream& operator<< (std::ostream& os, YARPString& s) { os << s.c_str(); return os; }

/**
 * Prints a string.
 * @param os the iostream to print into.
 * @param s the string to be printed (const).
 * @return the printed string (to allow chaining).
 */
inline std::ostream& operator<< (std::ostream& os, const YARPString& s) { os << s.c_str(); return os; }

/**
 * Accepts (input) a string.
 * @param is the iostream to receive from.
 * @param s the string to be filled with the incoming data.
 * @return the destination string (to allow chaining).
 */
inline std::istream& operator>> (std::istream& is, YARPString& s) 
{
	char _buf[1024]; ACE_OS::memset (_buf, 0, 1024);
	is.getline(_buf, 1024, '\n');
	s = _buf; 
	return is; 
}

#endif
