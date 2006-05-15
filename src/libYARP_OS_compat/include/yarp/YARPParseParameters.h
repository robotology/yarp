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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPParseParameters.h,v 1.4 2006-05-15 15:57:58 eshuy Exp $
///
/// Simple parser facility for program arguments. July 2003 -- by nat
/// usage: -switch parameter
/// Note: "-name" behaves differently, appends "/" at the beginning of the string


#ifndef __YARPPARSEPARAMETERS__
#define __YARPPARSEPARAMETERS__

/**
 * \file YARPParseParameters.h These are methods to parse the argument
 * list for strings and subsequently for arguments associated with the string.
 */
#include <yarp/YARPString.h>

/**
 * Legacy namespace for functions for parsing parameters.
 */
namespace YARPParseParameters
{
	/**
	 * Parse the argv array for a string argument. It looks for a key and then
	 * assume the next argument is a string associated with a key.
	 * @param argc is the length of the argv array.
	 * @param argv is an array to pointer to strings.
	 * @param key is the string to search in argv.
	 * @param out is the return argument, the string read from argv.
	 * @return true if the key has been found, false otherwise.
	 */
	bool parse (int argc, char *argv[], const YARPString &key, YARPString &out);

	/**
	 * Parse the argv array for a boolean argument (on/off option). 
	 * It looks for a key and assumes the presence of the key to 
	 * specify an ON option.
	 * @param argc is the length of the argv array.
	 * @param argv is an array to pointer to strings.
	 * @param key is the string to search in argv.
	 * @return true if the key has been found, false otherwise.
	 */
	bool parse (int argc, char *argv[], const YARPString &key);

	/**
	 * Parse the argv array for an integer argument.
	 * @param argc is the length of the argv array.
	 * @param argv is an array to pointer to strings.
	 * @param key is the string to search in argv.
	 * @param out is a pointer to the integer that will contain the value.
	 * @return true if the key has been found and the value stored in out, 
	 * false otherwise.
	 */
	bool parse (int argc, char *argv[], const YARPString &key, int *out);

	/**
	 * Parse the argv array for a string argument (represented as char *).
	 * @param argc is the length of the argv array.
	 * @param argv is an array to pointer to strings.
	 * @param key is the string to search in argv.
	 * @param out is a pointer to the buffer to contain the string.
	 * @return true if the key has been found and the value stored in out, 
	 * false otherwise.
	 */
	bool parse (int argc, char *argv[], const YARPString &key, char *out);

	/**
	 * Parse the argv array for a double precision numeric value.
	 * @param argc is the length of the argv array.
	 * @param argv is an array to pointer to strings.
	 * @param key is the string to search in argv.
	 * @param out is a pointer to the numeric value.
	 * @return true if the key has been found and the value stored in out, 
	 * false otherwise.
	 */
	bool parse (int argc, char *argv[], const YARPString &key, double *out);
};

#endif


