// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

///
/// $Id: begin_pack_for_net.h,v 1.7 2007-11-26 16:31:25 eshuy Exp $
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

#ifdef CYGWIN
#ifndef YARP2_CYGWIN
#define YARP2_CYGWIN
#endif
#endif

#ifdef WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef _WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WIN__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WINDOWS__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOWS
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif


#ifdef YARP2_CYGWIN
#pragma pack(1)
#define YARP_PACKING_CONSIDERED
#else
#ifdef YARP2_WINDOWS
// use packing and make no apologies about it
#pragma warning (disable:4103)
#pragma pack(push, 1)
#define YARP_PACKING_CONSIDERED
#endif
#endif

#ifdef __linux__
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

#ifdef __APPLE__
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
