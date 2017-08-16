/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMLIMITS_H
#define YARP_OS_IMPL_PLATFORMLIMITS_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
# include <ace/os_include/os_limits.h>
#else
# include <climits>
#if !defined(HOST_NAME_MAX) && defined(__APPLE__)
# define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif
#endif


#endif // YARP_OS_IMPL_PLATFORMLIMITS_H
