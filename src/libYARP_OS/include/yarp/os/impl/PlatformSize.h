// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PLATFORMSIZE
#define YARP2_PLATFORMSIZE

// We may use ssize_t in YARP internals, not in public API.
// In public API, YARP_SSIZE_T is now available via yarp/conf/numeric.h

#include <yarp/conf/system.h>

#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_unistd.h>
#else
#  include <unistd.h>
#endif

#endif
