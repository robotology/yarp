// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

///
/// $Id: end_pack_for_net.h,v 1.7 2007-02-02 15:45:20 eshuy Exp $
///
///

/**
 * \file yarp/os/end_pack_for_net.h Ends 1 byte packing for structs/classes.
 * Instructs the compiler that the default packing can be reinstated.
 * @see yarp/os/begin_pack_for_net.h
 */

#include <yarp/os/NetInt32.h>

#ifdef YARP2_CYGWIN
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#else
#ifdef YARP2_WINDOWS
#pragma pack(pop)
#define YARP_UNPACKING_CONSIDERED
#endif
#endif

#ifdef __linux__
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif

#ifdef __linux__
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif

#ifdef __DARWIN__
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif

#ifdef __APPLE__
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif

#ifdef __QNX4__
#pragma  pack (pop) ;
#define YARP_UNPACKING_CONSIDERED
#endif

#ifdef __QNX6__
///#pragma align 0
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif

#ifndef YARP_UNPACKING_CONSIDERED
//#warning "Platform not known, guessing, please update end_pack_for_net.h"
#pragma pack()
#define YARP_UNPACKING_CONSIDERED
#endif
