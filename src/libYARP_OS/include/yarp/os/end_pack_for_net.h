// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: end_pack_for_net.h,v 1.5 2006-10-24 16:43:50 eshuy Exp $
///
///

/**
 * \file yarp/os/end_pack_for_net.h Ends 1 byte packing for structs/classes.
 * Instructs the compiler that the default packing can be reinstated.
 * @see yarp/os/begin_pack_for_net.h
 */

#include <yarp/os/NetInt32.h>

#ifdef YARP2_WINDOWS
#pragma pack(pop)
#endif

#ifdef __LINUX__
#pragma pack()
#endif

#ifdef __linux__
#pragma pack()
#endif

#ifdef __DARWIN__
#pragma pack()
#endif

#ifdef __QNX4__
#pragma  pack (pop) ;
#endif

#ifdef __QNX6__
///#pragma align 0 
#pragma pack()
#endif
