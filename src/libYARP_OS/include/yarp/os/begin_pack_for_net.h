// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: begin_pack_for_net.h,v 1.4 2006-05-15 15:57:58 eshuy Exp $
///

/**
 * \file begin_pack_for_net.h Starts 1 byte packing for structs/classes.
 * Instructs the compiler that the following structure/class has to be
 * packed with 1 byte boundary.  This is conditionally generated depending
 * on the compiler and architecture. It assures interoperability of network 
 * communication between compilers.
 * @see end_pack_for_net.h
 */

#include <yarp/os/NetInt32.h>

#ifdef YARP2_WINDOWS
// use packing and make no apologies about it
#pragma warning (disable:4103)
#pragma pack(push, 1)
#define YARP_PACKING_CONSIDERED
#endif

#ifdef __LINUX__
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#else
#ifdef __linux__
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#endif
#endif

#ifdef __DARWIN__
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#endif

#ifdef __QNX4__
#pragma  pack (push) ;
#pragma  pack (1) ;
#define YARP_PACKING_CONSIDERED
#endif

#ifdef __QNX6__
///#pragma  align 1
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#endif

#ifndef YARP_PACKING_CONSIDERED
//#warning "Platform not known, guessing, please update begin_pack_for_net.h"
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#endif

#ifndef PACKED_FOR_NET
#define PACKED_FOR_NET
#endif
