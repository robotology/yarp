/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMSTDIO_H
#define YARP_OS_IMPL_PLATFORMSTDIO_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_stdio.h>
#else
#  include <cstdio>
#  ifndef ACE_OS
#    define ACE_OS
#  endif
#endif

#endif // YARP_OS_IMPL_PLATFORMSTDIO_H
