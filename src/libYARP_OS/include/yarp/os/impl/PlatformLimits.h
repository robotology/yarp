/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMLIMITS_H
#define YARP_OS_IMPL_PLATFORMLIMITS_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/os_include/os_limits.h>
#else
# include <limits.h>
#endif

#ifndef HOST_NAME_MAX
  YARP_COMPILER_ERROR("HOST_NAME_MAX not defined on this platform")
#endif

#endif // YARP_OS_IMPL_PLATFORMLIMITS_H
