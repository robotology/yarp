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
/// $Id: YARPNetworkTypes.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#ifndef YARPNetworkTypes_INC
#define YARPNetworkTypes_INC

/**
 * \file YARPNetworkTypes.h These are a few definitions for data types
 * to be sent across a network.
 */

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#ifdef __LINUX__
#	include <stdint.h>
#endif

#ifdef __LINUX__
#	define PACKED_FOR_NET __attribute__ ((packed))
#else
#	define PACKED_FOR_NET
#endif

/*
  Need to specify number and order of bytes
*/

#ifdef __LINUX__
/**
 * Definition of the NetInt32 type for Linux/gcc.
 */
typedef int32_t NetInt32;
#endif

#ifdef __DARWIN__
/**
 * Definition of the NetInt32 type for Darwin/gcc.
 * We need to be careful about our Endian mode.
 * YARP integers are defined to be little-endian.
 */
#ifdef __BIG_ENDIAN__
typedef int32_t RawNetInt32;
class NetInt32 {
private:
    int32_t raw_value;
    int32_t swap(uint32_t x) const {
        return (x>>24) | ((x>>8) & 0xff00) | ((x<<8) & 0xff0000) | (x<<24);
    }
    RawNetInt32 get() const {
        return (int32_t)swap((uint32_t)raw_value);
    }
    void set(RawNetInt32 v) {
        raw_value = (int32_t)swap((uint32_t)v);
    }
public:
    NetInt32() {
    }
    NetInt32(RawNetInt32 val) {
        set(val);
    }
    operator RawNetInt32() const {
        return get();
    }
    RawNetInt32 operator+(RawNetInt32 v) const {
        return get()+v;
    }
    RawNetInt32 operator-(RawNetInt32 v) const {
        return get()-v;
    }
    RawNetInt32 operator*(RawNetInt32 v) const {
        return get()*v;
    }
    RawNetInt32 operator/(RawNetInt32 v) const {
        return get()/v;
    }
    void operator+=(RawNetInt32 v) {
        set(get()+v);
    }
    void operator-=(RawNetInt32 v) {
        set(get()-v);
    }
    void operator*=(RawNetInt32 v) {
        set(get()*v);
    }
    void operator/=(RawNetInt32 v) {
        set(get()/v);
    }
};
#else
typedef int32_t NetInt32;
#endif

#endif

#ifdef __QNX__
/**
 * Definition of the NetInt32 type for qnx6/gcc.
 */
typedef long int NetInt32;
#endif

#ifdef __WIN__
#	ifdef __WIN_MSVC__
/**
 * Definition of the NetInt32 type for Windows/msvc.
 */
typedef __int32 NetInt32;
#	else
#		include <sys/config.h>
typedef __int32_t NetInt32;
#	endif
#endif


#endif
