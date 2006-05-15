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
/// $Id: YARPAll.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#ifndef YARPAll_INC
#define YARPAll_INC

#include <yarp/YARPConfig.h>
#include <yarp/YARPErrorCodes.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPAll.h a few global functions prototypes.
 */

#include <stdio.h>
#include <yarp/debug.h>

#define USE_YARP2

// thread-safe version of printf
void YARP_safe_printf(char *format,...);
void YARP_unsafe_printf(char *format,...);
void YARP_output_wait();
void YARP_output_post();

/**
 * Gets the value of the YARP_ROOT symbol from the environment.
 * @return the YARP_ROOT as a null-terminated string or NULL on failure. Note
 * that the space required by the string is allocated by the OS and this function
 * is not reentrant (although it probably won't matter much).
 */
extern char * GetYarpRoot (void);

/**
 * Gets the network host name.
 * @param the buffer where the name is to be stored.
 * @param the maximum length of the buffer.
 * @return a non-negative value if successful.
 */
extern int getHostname(char *buffer, int buffer_length);


#include <ace/config.h>
// need to fix assertion behavior...
#ifdef ACE_NDEBUG
#undef ACE_NDEBUG
#endif

#ifdef ACE_ASSERT
#undef ACE_ASSERT
#define ACE_ASSERT(X) \
  do { if(!(X)) { \
  int __ace_error = ACE_Log_Msg::last_error_adapter (); \
  ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
  ace___->set (__FILE__, __LINE__, -1, __ace_error, ace___->restart (), \
               ace___->msg_ostream (), ace___->msg_callback ()); \
  ace___->log (LM_ERROR, ACE_LIB_TEXT ("ACE_ASSERT: file %N, line %l assertion failed for '%s'.%a\n"), #X, -1); \
  } } while (0)
#endif

#endif
